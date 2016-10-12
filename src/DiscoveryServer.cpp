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
#include <eznet/DiscoveryServer.hpp>
#include <eznet/Buffer.hpp>
#include <enet/enet.h>
#include <string.h>
#include "Constants.hpp"


namespace eznet {
	
	struct DiscoveryServerEnetData {
		ENetSocket socket = ENET_SOCKET_NULL;
	};

	DiscoveryServer::DiscoveryServer(const std::string& gameName)
		: enetData(std::make_unique<DiscoveryServerEnetData>()), gameName(gameName) {
	}

	DiscoveryServer::~DiscoveryServer() {
		stop();
	}

	bool DiscoveryServer::start(uint16_t discoveryPort, const std::string& serverName_, uint16_t serverPort_, uint8_t maxSlots_) {
		serverName = serverName_;
		serverPort = serverPort_;
		maxSlots = maxSlots_;
		availableSlots = maxSlots;

		ENetAddress address;
		// win32 needs different ip than linux when listening for broadcasts, don't know why.
#ifdef WIN32
		address.host = ENET_HOST_ANY;
#else
		address.host = ENET_HOST_BROADCAST;
#endif
		address.port = discoveryPort;

		auto& socket = enetData->socket;
		socket = enet_socket_create (ENET_SOCKET_TYPE_DATAGRAM);
		if (socket == ENET_SOCKET_NULL)
		   return false;

		enet_socket_set_option (socket, ENET_SOCKOPT_NONBLOCK, 1);
		enet_socket_set_option (socket, ENET_SOCKOPT_BROADCAST, 1);
		enet_socket_set_option (socket, ENET_SOCKOPT_RCVBUF, ENET_HOST_RECEIVE_BUFFER_SIZE);
		enet_socket_set_option (socket, ENET_SOCKOPT_SNDBUF, ENET_HOST_SEND_BUFFER_SIZE);

		if(enet_socket_bind (socket, &address) < 0) {
			 stop();
			 return false;
		}
		return true;
	}

	void DiscoveryServer::stop() {
		auto& socket = enetData->socket;
		if(socket != ENET_SOCKET_NULL) {
			enet_socket_destroy (socket);
			socket = ENET_SOCKET_NULL;
		}
	}

	bool DiscoveryServer::update() {
		char packetData[ENET_PROTOCOL_MAXIMUM_MTU];
		ENetBuffer buffer;
		ENetAddress receivedAddress;
		buffer.data = packetData;
		buffer.dataLength = ENET_PROTOCOL_MAXIMUM_MTU;
		int receivedLength = enet_socket_receive (enetData->socket,&receivedAddress, &buffer, 1);
		if(receivedLength > 0) {
			char hostName[MAX_HOSTNAME_LENGTH];
			int r = enet_address_get_host_ip (&receivedAddress, hostName, MAX_HOSTNAME_LENGTH);
			if(r == 0) {
				BufferReader reader(static_cast<uint8_t *>(buffer.data), buffer.dataLength);
				std::string magic = reader.read<std::string>();
				if(magic == CLIENT_MAGIC) {
					std::string gameName = reader.read<std::string>();
					if(gameName == gameName) {
						sendReply(&receivedAddress);
					}
				}
			}

		}
		return true;
	}

	void DiscoveryServer::sendReply(const ENetAddress * address) {
		ENetBuffer buffer;
		BufferWriter writer(MAX_DISCOVERY_MESSAGE_SIZE);
		writer.write<std::string>(SERVER_MAGIC);
		writer.write<std::string>(gameName);
		writer.write<std::string>(serverName);
		writer.write<uint8_t>(maxSlots);
		writer.write<uint8_t>(availableSlots);
		writer.write<uint16_t>(serverPort);

		buffer.data = writer.getData();
		buffer.dataLength = writer.getPosition();
		enet_socket_send (enetData->socket, address, &buffer, 1);
	}
}
