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
#include "MessageGenerator.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if(argc != 3) {
		std::cerr << "Usage: " << std::string(argv[0]) << " <definition-file> <output-directory>" << std::endl;
		return -1;
	}
	std::string definitionFile = argv[1];
	std::string outputDir = argv[2];
	std::string messageConstantsHpp = outputDir + "/MessageConstants.hpp";
	std::string messagesHpp = outputDir + "/Messages.hpp";
	std::string messageAdaptersHpp = outputDir + "/MessageAdapters.hpp";
	std::string messageAdaptersCpp = outputDir + "/MessageAdapters.cpp";
	
	DefinitionParser parser;
	DefinitionConfig config;
	std::string error = parser.parse(definitionFile, config);
	if(!error.empty()) {
		std::cout << error << std::endl;
		return -1;
	}
	
	MessageGenerator generator;
	if(!generator.generateConstantsHpp(config, messageConstantsHpp)) {
		std::cout << "Failed creating " << messageConstantsHpp << std::endl;
		return -1;
	}
	if(!generator.generateMessagesHpp(config, messagesHpp)) {
		std::cout << "Failed creating " << messagesHpp << std::endl;
		return -1;
	}
	if(!generator.generateAdaptersHpp(config, messageAdaptersHpp)) {
		std::cout << "Failed creating " << messageAdaptersHpp << std::endl;
		return -1;
	}
	if(!generator.generateAdaptersCpp(config, messageAdaptersCpp)) {
		std::cout << "Failed creating " << messageAdaptersCpp << std::endl;
		return -1;
	}
	
	return 0;
}

