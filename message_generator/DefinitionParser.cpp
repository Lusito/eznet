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
#include "DefinitionParser.hpp"
#include <sstream>
#include <fstream>
#include <iterator>

std::string DefinitionParser::parse(const std::string& filename, DefinitionConfig& config) {
	
	std::ifstream file(filename);
	if(!file.is_open())
		return "Can't open file " + filename;
	
	Definition* lastDefinition = nullptr;
	std::vector<std::string> tokens;
	std::string line;
	for(int lineNum=0; std::getline(file, line); lineNum++) {
		// skip/remove comments
		size_t commentPos = line.find("#");
		if (commentPos == 0)
			continue;
		else if(commentPos > 0)
			line = line.substr (0, commentPos);

		// parse tokens
		std::istringstream iss(line);
		std::copy(std::istream_iterator<std::string>(iss),
			std::istream_iterator<std::string>(),
			std::back_inserter(tokens));
		auto numTokens = tokens.size();
		if(numTokens) {
			std::string& command = tokens.at(0);
			if(command == "message") {
				if(numTokens == 1)
					return "Line " + std::to_string(lineNum) + ": missing name";
				std::string& name = tokens.at(1);
				bool reliable = false, unsequenced = false;
				if(numTokens == 3) {
					std::string& flag = tokens.at(2);
					if(flag == "!reliable")
						reliable = true;
					else if(flag == "!unsequenced")
						unsequenced = true;
					else
						return "Line " + std::to_string(lineNum) + ": unknown flag '" + flag + "'";
				}
				if(numTokens > 3)
					return "Line " + std::to_string(lineNum) + ": too many arguments";
				config.definitions.emplace_back(name, reliable, unsequenced, true);
				lastDefinition = &config.definitions.back();
			} else if(command == "struct") {
				if(numTokens == 1)
					return "Line " + std::to_string(lineNum) + ": missing name";
				std::string& name = tokens.at(1);
				if(numTokens > 2)
					return "Line " + std::to_string(lineNum) + ": too many arguments";
				config.definitions.emplace_back(name);
				lastDefinition = &config.definitions.back();
			} else if(command == "field" || command == "field[]") {
				bool list = command == "field[]";
				if(!lastDefinition)
					return "Line " + std::to_string(lineNum) + ": no message or struct to add this field to";
				if(numTokens == 1)
					return "Line " + std::to_string(lineNum) + ": missing name";
				if(numTokens == 2)
					return "Line " + std::to_string(lineNum) + ": missing type";
				if(numTokens > 3)
					return "Line " + std::to_string(lineNum) + ": too many arguments";
				lastDefinition->fields.emplace_back(tokens.at(1), tokens.at(2), list);
			} else if(command == "include") {
				if(numTokens == 1)
					return "Line " + std::to_string(lineNum) + ": missing destination";
				if(numTokens == 2)
					return "Line " + std::to_string(lineNum) + ": missing include";
				if(numTokens > 3)
					return "Line " + std::to_string(lineNum) + ": too many arguments";
				std::string destination = tokens.at(1);
				if(destination == "messages")
					config.messagesIncludes.push_back(tokens.at(2));
				else if(destination == "adapters")
					config.adaptersIncludes.push_back(tokens.at(2));
				else
					return "Line " + std::to_string(lineNum) + ": unknown destination '" + destination + "'";
			} else {
				return "Unknown command: " + command;
			}
			tokens.clear();
		}
	}
	return "";
}
