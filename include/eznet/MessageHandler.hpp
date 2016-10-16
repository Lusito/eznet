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
#include <functional>

struct _ENetEvent;
typedef _ENetEvent ENetEvent;

namespace eznet {
	class MessageHandler : public MessageHandlerBase {
	private:
		using ReaderCallback = std::function<void(BufferReader& reader, ENetEvent& event)>;
		template <typename CT, typename MDT>
		using HandlerCallback = void (CT::*)(MDT& message, ENetEvent& event);
		ReaderCallback callbacks[static_cast<uint16_t>(MessageType::NUM_TYPES)];

	public:
		template <typename CT, typename MDT>
		void setCallback(CT* const listener, HandlerCallback<CT, MDT> method) {
			MessageType type = MDT::getType();
			assert(type < MessageType::NUM_TYPES);
			callbacks[static_cast<uint16_t>(type)] = [listener, method](BufferReader& reader, ENetEvent& event) {
				MDT message;
				readMessage(reader, message);
				(listener->*method)(message, event);
			};
		}
		void handleEvent(uint16_t type, buffered_reader& reader, ENetEvent& event) const override {
			assert(type >= 0 && type < MessageType::NUM_TYPES);
			if(callbacks[type])
				callbacks[type](reader, event);
		}
	};
}
