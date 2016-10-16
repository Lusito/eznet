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
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace eznet {

	class ServerDescription {
	private:
		friend class DiscoveryClient;
		std::string hostname;
		std::string servername;
		uint8_t maxSlots;
		uint8_t availableSlots;
		uint16_t port;
		uint64_t lastSeen;

	public:
		const std::string& getHostname() const noexcept { return hostname; };
		const std::string& getServername() const noexcept { return servername; };
		uint8_t getMaxSlots() const noexcept { return maxSlots; };
		uint8_t getAvailableSlots() const noexcept { return availableSlots; };
		uint16_t getPort() const noexcept { return port; };
		uint64_t getLastSeen() const noexcept { return lastSeen; };
	};

	struct DiscoveryClientEnetData;
	
	class DiscoveryClient {
	protected:
		std::unique_ptr<DiscoveryClientEnetData> enetData;
		std::string gamename;
		uint64_t lastSend = 0;
		std::vector<ServerDescription> servers;
		bool serversChanged = false;

	public:
		DiscoveryClient(const std::string& gamename);
		~DiscoveryClient();

		const std::vector<ServerDescription>& getServers() {
			serversChanged = false;
			return servers;
		};

		bool getServersChanged() { return serversChanged; }

		bool start(uint16_t port);
		void stop();
		void send();
		bool update();
	};

}
