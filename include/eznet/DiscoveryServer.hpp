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
#include <memory>
#include <string>

struct _ENetAddress;
typedef _ENetAddress ENetAddress;

namespace eznet {

	struct DiscoveryServerEnetData;
	
	class DiscoveryServer {
	protected:
		std::unique_ptr<DiscoveryServerEnetData> enetData;
		std::string gameName;
		std::string serverName;
		uint8_t maxSlots = 0;
		uint8_t availableSlots = 0;
		uint16_t serverPort = 0;

	public:
		DiscoveryServer(const std::string& gameName);
		~DiscoveryServer();

		void getAvailableSlots(uint8_t availableSlots) {
			availableSlots = availableSlots;
		}

		uint8_t getAvailableSlots() {
			return availableSlots;
		}

		bool start(uint16_t discoveryPort, const std::string& serverName, uint16_t serverPort, uint8_t maxSlots);
		void stop();
		bool update();

	private:
		void sendReply(const ENetAddress * address);
	};

}
