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
#include <eznet/Connection.hpp>
#include <eznet/Utils.hpp>
#include <enet/enet.h>

namespace eznet {
	
	Connection::Connection() : host(nullptr), handler(nullptr) {
	}
	Connection::~Connection() {
		disconnectNow();
	}

	void Connection::disconnect() {
		if(host && state == ConnectionState::CONNECTED) {
			for(size_t i = 0; i < host->peerCount; i++) enet_peer_disconnect(&host->peers[i], 0);
			state = ConnectionState::DISCONNECTING;
		}
	}

	void Connection::disconnectNow() {
		if(host && state == ConnectionState::CONNECTED) {
			for(size_t i = 0; i < host->peerCount; i++) enet_peer_disconnect_now(&host->peers[i], 0);
			disconnectInternal();
		}
	}

	void Connection::disconnectLater() {
		if(host && state == ConnectionState::CONNECTED) {
			for(size_t i = 0; i < host->peerCount; i++) enet_peer_disconnect_later(&host->peers[i], 0);
			state = ConnectionState::DISCONNECTING;
		}
	}

	bool Connection::update() {
		if(state != ConnectionState::DISCONNECTED) {
			int serviceResult = 1;

			/* Keep doing host_service until no events are left */
			ENetEvent event;
			while((serviceResult = enet_host_service(host, &event, 0)) > 0) {
				switch(event.type) {
				case ENET_EVENT_TYPE_CONNECT: onConnect(event); break;
				case ENET_EVENT_TYPE_RECEIVE: onReceive(event); break;
				case ENET_EVENT_TYPE_DISCONNECT: onDisconnect(event); break;
				default: break;
				}
			}

			if(serviceResult < 0)
				return false;
		}
		return true;
	}

	void Connection::disconnectInternal() {
		enet_host_destroy(host);
		host = nullptr;
		state = ConnectionState::DISCONNECTED;
	}

	void Connection::onConnect(ENetEvent& event) {
		if(state == ConnectionState::CONNECTING) {
			state = ConnectionState::CONNECTED;
		} else if(state == ConnectionState::CONNECTED) {
		} else if(state == ConnectionState::DISCONNECTING) {
			// fixme: disconnect the connecting peer ?
		}

		// Notify listener
		if(connectCallback)
			connectCallback(event);
	}

	void Connection::onDisconnect(ENetEvent& event) {
		if(state == ConnectionState::DISCONNECTING && !host->connectedPeers)
			disconnectInternal();

		// Notify listener
		if(disconnectCallback)
			disconnectCallback(event);
	}

	void Connection::onReceive(ENetEvent& event) {
		if(state == ConnectionState::CONNECTED && handler)
			handler->handleMessage(event);
		enet_packet_destroy(event.packet);
	}

	bool ServerConnection::connect(const std::string& hostName, uint16_t port, int clients, int channels) {
		if(state != ConnectionState::DISCONNECTED)
			disconnectNow();

		ENetAddress address;
		if(hostName.empty())
			address.host = ENET_HOST_ANY;
		else
			enet_address_set_host(&address, hostName.c_str());
		address.port = port;

		host = enet_host_create(&address, clients, channels, 0, 0);
		if(host == nullptr) return false;

		state = ConnectionState::CONNECTED;
		return true;
	}

	ClientConnection::ClientConnection() {}

	bool ClientConnection::connect(const std::string& hostName, uint16_t port, int channels) {
		if(state != ConnectionState::DISCONNECTED)
			disconnectNow();

		host = enet_host_create(nullptr, 1, channels, 0, 0);
		if(host == nullptr) return false;

		ENetAddress address;
		enet_address_set_host(&address, hostName.c_str());
		address.port = port;

		peer = enet_host_connect(host, &address, channels, 0);
		if(peer == nullptr) {
			disconnectNow();
			return false;
		}

		state = ConnectionState::CONNECTING;
		connectionTime = getTime();
		return true;
	}

	void ClientConnection::disconnectInternal() {
		Connection::disconnectInternal();
		peer = nullptr;
	}

	bool ClientConnection::update() {
		bool result = Connection::update();
		if(state == ConnectionState::CONNECTING && (getTime() - connectionTime) > 5000) {
			disconnectNow();

			// Notify listener
			ENetEvent event;
			event.type = ENET_EVENT_TYPE_DISCONNECT;
			event.peer = nullptr;
			if(disconnectCallback)
				disconnectCallback(event);
		}
		return result;
	}
}