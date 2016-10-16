#pragma once

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace eznet {
	class ConnectHandler {
	public:
		virtual ~ConnectHandler();
		virtual void onConnect(ENetEvent& event) = 0;
		virtual void onDisconnect(ENetEvent& event) = 0;
	};
}