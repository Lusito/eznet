/*******************************************************************************
 * Copyright (c) 2016 Santo Pfingsten
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 ******************************************************************************/
#include <eznet/DiscoveryClient.hpp>
#include <eznet/Buffer.hpp>
#include <eznet/Utils.hpp>
#include <enet/enet.h>
#include <string.h>
#include "Constants.hpp"

namespace eznet {
	
	struct DiscoveryClientEnetData {
		ENetSocket socket = ENET_SOCKET_NULL;
		ENetAddress address;
	};

	DiscoveryClient::DiscoveryClient(const std::string &gameName)
		: enetData(std::make_unique<DiscoveryClientEnetData>()), gameName(gameName) {
	}

	DiscoveryClient::~DiscoveryClient() {
		stop();
	}

	bool DiscoveryClient::start(uint16_t port) {
		auto& address = enetData->address;
		address.host = ENET_HOST_BROADCAST;
		address.port = port;

		auto& socket = enetData->socket;
		socket = enet_socket_create (ENET_SOCKET_TYPE_DATAGRAM);
		if (socket == ENET_SOCKET_NULL)
		   return false;

		enet_socket_set_option (socket, ENET_SOCKOPT_NONBLOCK, 1);
		enet_socket_set_option (socket, ENET_SOCKOPT_BROADCAST, 1);
		enet_socket_set_option (socket, ENET_SOCKOPT_RCVBUF, ENET_HOST_RECEIVE_BUFFER_SIZE);
		enet_socket_set_option (socket, ENET_SOCKOPT_SNDBUF, ENET_HOST_SEND_BUFFER_SIZE);

		send();
		return true;
	}

	void DiscoveryClient::stop() {
		auto& socket = enetData->socket;
		if(socket != ENET_SOCKET_NULL) {
			enet_socket_destroy (socket);
			socket = ENET_SOCKET_NULL;
		}
	}

	void DiscoveryClient::send() {
		lastSend = getTime();

		ENetBuffer buffer;
		BufferWriter writer(MAX_DISCOVERY_MESSAGE_SIZE);
		writer.write<std::string>(CLIENT_MAGIC);
		writer.write<std::string>(gameName);

		buffer.data = writer.getData();
		buffer.dataLength = writer.getPosition();

		enet_socket_send (enetData->socket, &enetData->address, &buffer, 1);
	}
	bool DiscoveryClient::update() {
		if((getTime() - lastSend) > RETRY_INTERVAL)
			send();

		auto now = getTime();
		char packetData[ENET_PROTOCOL_MAXIMUM_MTU];
		ENetBuffer buffer;
		buffer.data = packetData;
		buffer.dataLength = ENET_PROTOCOL_MAXIMUM_MTU;
		ENetAddress receivedAddress;
		int receivedLength = enet_socket_receive (enetData->socket,&receivedAddress, &buffer, 1);
		if(receivedLength > 0) {
			char hostName[MAX_HOSTNAME_LENGTH];
			int r = enet_address_get_host_ip (&receivedAddress, hostName, MAX_HOSTNAME_LENGTH);
			if(r == 0) {
				BufferReader reader(static_cast<uint8_t *>(buffer.data), buffer.dataLength);
				std::string magic = reader.read<std::string>();
				if(magic == SERVER_MAGIC) {
					std::string otherGameName = reader.read<std::string>();
					if(gameName == otherGameName) {
						std::string serverName = reader.read<std::string>();
						uint8_t maxSlots = reader.read<uint8_t>();
						uint8_t availableSlots = reader.read<uint8_t>();
						uint16_t serverPort = reader.read<uint16_t>();

						bool found = false;
						for(auto& server: servers) {
							if (server.hostName == hostName && server.port == serverPort) {
								found = true;
								if(server.serverName != serverName) {
									server.serverName = serverName;
									serversChanged = true;
								}
								if(server.maxSlots != maxSlots) {
									server.maxSlots = maxSlots;
									serversChanged = true;
								}
								if(server.availableSlots != availableSlots) {
									server.availableSlots = availableSlots;
									serversChanged = true;
								}
								server.lastSeen = now;
								break;
							}
						}
						if(!found) {
							servers.emplace_back();
							auto& server = servers.back();

							server.hostName = hostName;
							server.serverName = serverName;
							server.port = serverPort;
							server.maxSlots = maxSlots;
							server.availableSlots = availableSlots;
							server.lastSeen = now;
							serversChanged = true;
						}
					}
				}
			}

		}

		auto i = servers.begin();
		while (i != servers.end()) {
			auto& server = (*i);
			if ((now - server.lastSeen) > SERVER_TIMEOUT_TIME) {
				i = servers.erase(i);
				serversChanged = true;
				continue;
			}
			i++;
		}

		return true;
	}

}
