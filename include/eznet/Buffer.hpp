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
#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <string>
#include <type_traits>

namespace eznet {
	template <class _Ty>
	struct is_bool : std::false_type {};
	template <>
	struct is_bool<bool> : std::true_type {};
	template <class _Ty>
	struct is_string : std::false_type {};
	template <>
	struct is_string<std::string> : std::true_type {};

	const int MAX_BUFFER = 4096;
	
	class BufferWriter {

	protected:
		uint8_t data[MAX_BUFFER];
		size_t position = 0;
		size_t limit;

	public:
		BufferWriter(size_t limit) : limit(limit) {}
		void reset() { position = 0; }
		uint8_t* getData() { return data; }
		size_t getPosition() { return position; }

		template <typename T>
		void write(const typename std::enable_if<!is_bool<T>::value && !is_string<T>::value, T>::type& value) {
			static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value || std::is_enum<T>::value,
						  "Get can only take integer and float types");
			size_t size = sizeof(value);
			assert(position + size <= limit);
			memcpy(data + position, &value, size);
			position += size;
		}

		template <typename T>
		void write(const typename std::enable_if<is_bool<T>::value, bool>::type& value) {
			write<uint8_t>(value ? 1 : 0);
		}

		template <typename T>
		void write(const typename std::enable_if<is_string<T>::value, std::string>::type& value) {
			size_t length = value.size();
			write<uint16_t>(uint16_t(length));
			assert(position + length <= limit);
			memcpy(data + position, value.c_str(), length);
			position += length;
		}
	};

	class BufferReader {
	protected:
		const uint8_t* data;
		size_t position = 0;
		size_t limit;

	public:
		BufferReader(const uint8_t* data, size_t limit) : data(data), limit(limit) {}

		template <typename T>
		typename std::enable_if<!is_bool<T>::value && !is_string<T>::value, T>::type read() {
			static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value || std::is_enum<T>::value,
						  "Get can only take integer and float types");
			size_t size = sizeof(T);
			assert(position + size <= limit);
			T value;
			memcpy(&value, data + position, size);
			position += size;
			return value;
		}

		template <typename T>
		typename std::enable_if<is_bool<T>::value, bool>::type read() {
			return read<uint8_t>() != 0;
		}

		template <typename T>
		typename std::enable_if<is_string<T>::value, std::string>::type read() {
			uint16_t length = read<uint16_t>();
			assert(position + length <= limit);
			const char* result = reinterpret_cast<const char*>(data + position);
			position += length;
			return std::string(result, length);
		}
	};
}
