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
#include <algorithm>
#include <string>
#include <vector> 
#include <regex>

struct DefinitionField {
	DefinitionField(const std::string &name, const std::string &type, bool list)
		: name(name), type(type), list(list) {}
	std::string name;
	std::string type;
	bool list;
};

struct Definition {
	Definition(const std::string &name, bool reliable=false, bool unsequenced=false, bool message=false)
		: reliable(reliable), unsequenced(unsequenced), message(message) {
		if(message)
			structName = name + "Message";
		else
			structName = name;
		
		// CamelCase to UPPER_SNAKE_CASE
		static std::regex re("([a-z])([A-Z])");
		enumName = std::regex_replace (name,re,"$1_$2");
		std::transform(enumName.begin(), enumName.end(),enumName.begin(), ::toupper);
	}
	std::string structName;
	std::string enumName;
	bool reliable;
	bool unsequenced;
	bool message;
	std::vector<DefinitionField> fields;
};

struct DefinitionConfig {
	std::vector<Definition> definitions;
	std::vector<std::string> messagesIncludes;
	std::vector<std::string> adaptersIncludes;
	int messageCapacity = 1024;
};
