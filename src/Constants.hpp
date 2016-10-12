#pragma once

#include <stdint.h>

namespace eznet {
	const std::string SERVER_MAGIC = "EZNET_SERVER";
	const std::string CLIENT_MAGIC = "EZNET_DISCOVER";
	const int MAX_HOSTNAME_LENGTH = 256;
	const int MAX_DISCOVERY_MESSAGE_SIZE = 1024;
	// how much time to wait before broadcasting another request
	const uint64_t RETRY_INTERVAL = 4000;
	// how long it takes for a server to be removed from the list if it does not reply
	const uint64_t SERVER_TIMEOUT_TIME = 30000;
}
