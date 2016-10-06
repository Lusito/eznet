#pragma once
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
#include <eznet/MessageHandlerBase.hpp>
#include <stdint.h>
#include <string>
#include <functional>

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

struct _ENetHost;
typedef _ENetHost ENetHost;

struct _ENetPeer;
typedef _ENetPeer ENetPeer;


namespace eznet {

	class MessageHandlerBase;
	enum class ConnectionState : uint8_t { DISCONNECTED, CONNECTING, CONNECTED, DISCONNECTING };

	class Connection {
	protected:
		using EventCallback = std::function<void(ENetEvent& event)>;
		ENetHost* host = nullptr;
		MessageHandlerBase* handler = nullptr;
		ConnectionState state = ConnectionState::DISCONNECTED;
		EventCallback connectCallback;
		EventCallback disconnectCallback;

	public:
		Connection();
		virtual ~Connection();

		bool isConnected() {
			return state == ConnectionState::CONNECTED;
		}
		bool isDisconnected() {
			return state == ConnectionState::DISCONNECTED;
		}

		void disconnect();
		void disconnectNow();
		void disconnectLater();

		ENetHost* getHost() {
			return host;
		}
		virtual void setHandler(MessageHandlerBase* handler_) {
			handler = handler_;
		}

		virtual bool update();

		virtual void setConnectCallback(EventCallback callback) {
			connectCallback = callback;
		}
		virtual void setDisconnectCallback(EventCallback callback) {
			disconnectCallback = callback;
		}

	protected:
		virtual void disconnectInternal();

		void onConnect(ENetEvent& event);

		void onDisconnect(ENetEvent& event);

		void onReceive(ENetEvent& event);
	};

	class ServerConnection : public Connection {
	public:
		bool connect(const std::string& hostName, uint16_t port, int clients, int channels);
	};

	class ClientConnection : public Connection {
	protected:
		ENetPeer* peer = nullptr;
		uint64_t connectionTime = 0;

	public:
		ClientConnection();

		ENetPeer* getPeer() {
			return peer;
		}

		bool connect(const std::string& hostName, uint16_t port, int channels);
		bool update() override;
		
	protected:
		void disconnectInternal() override;
	};
}
