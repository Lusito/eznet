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
#include <eznet/Buffer.hpp>
#include <type_traits>
#include <vector>
#include <string>
#include <numeric>

struct _ENetPacket;
typedef _ENetPacket ENetPacket;

namespace eznet {
	inline void readMessage(BufferReader& reader, std::string& message) {
		message = reader.read<std::string>();
	}
	inline void writeMessage(BufferWriter& writer,
							  const std::string& message) {
		writer.write<std::string>(message);
	}

	template <typename T, typename = std::enable_if<std::is_fundamental<T>::value>>
	void readMessage(BufferReader& reader, T& message) {
		message = reader.read<T>();
	}

	template <typename T, typename = std::enable_if<std::is_fundamental<T>::value>>
	void writeMessage(BufferWriter& writer, const T& message) {
		writer.write<T>(message);
	}

	inline size_t getMessageSize(const std::string& message){
		return sizeof(uint16_t)+message.length();
	}

	constexpr size_t getMessageSize(bool){
		return sizeof(uint8_t);
	}

	template<typename T, typename = std::enable_if<std::is_fundamental<T>::value>>
	constexpr size_t getMessageSize(const T&){
		return sizeof(T);
	}

	template<typename T>
	size_t getMessageSize(const std::vector<T>& message) {
		return std::accumulate(message.begin(), message.end(), sizeof(uint16_t),
				[](size_t s, const T& e) {return s + getMessageSize(e);});
	}

}
