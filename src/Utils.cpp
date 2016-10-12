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
#include <eznet/Utils.hpp>
#include <enet/enet.h>
#include <chrono>
#include <string>

namespace eznet {
	bool init() {
		static bool initialized = false;
		static bool success = false;
		if(!initialized) {
			getTime();
			atexit(enet_deinitialize);
			success = enet_initialize() == 0;
		}
		return success;
	}

	int64_t getTime() {
		using namespace std::chrono;
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		uint64_t now = ms.count();
		static uint64_t start = now;
		return now - start;
	}
}