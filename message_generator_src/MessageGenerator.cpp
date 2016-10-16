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
#include "MessageGenerator.hpp"
#include <fstream>

const std::string CAUTION_MESSAGE = "// THIS FILE IS GENERATED, DO NOT MODIFY IT!";

bool MessageGenerator::generateConstantsHpp(const DefinitionConfig& config, const std::string& filename) {
	std::ofstream file(filename);
	if(!file.is_open())
		return false;
	
	file << CAUTION_MESSAGE << std::endl;
	file << "#pragma once" << std::endl;
	file << "#include <stdint.h>" << std::endl;
	file << "namespace eznet {" << std::endl;
	file << "enum class MessageType : uint16_t {" << std::endl;
	for(auto& def: config.definitions) {
		if(def.message)
			file << "\t" << def.enumName << "," << std::endl;
	}
	file << "\tNUM_TYPES" << std::endl;
	file << "};" << std::endl;
	file << "}" << std::endl;
	file.close();
	return true;
}

bool MessageGenerator::generateMessagesHpp(const DefinitionConfig& config, const std::string& filename) {
	std::ofstream file(filename);
	if(!file.is_open())
		return false;
	
	file << CAUTION_MESSAGE << std::endl;
	file << "#pragma once" << std::endl;
	file << "#include <vector>" << std::endl;
	file << "#include <string>" << std::endl;
	file << "#include \"MessageConstants.hpp\"" << std::endl;
	for(auto& inc: config.messagesIncludes)
		file << "#include " << inc << std::endl;
	file << "namespace eznet {" << std::endl;
	
	for(auto& def: config.definitions) {
		file << "struct " << def.structName << " {" << std::endl;
		if(def.message) {
			file << "\t" << "static constexpr MessageType getType() { return MessageType::" << def.enumName << ";}" << std::endl;
		}
		for(auto& field: def.fields) {
			if(field.list)
				file << "\tstd::vector<" << field.type << "> " << field.name << ";" << std::endl;
			else
				file << "\t" << field.type << " " << field.name << ";" << std::endl;
		}
		file << "};" << std::endl;
	}
	file << "}" << std::endl;
	file.close();
	return true;
}

bool MessageGenerator::generateAdaptersHpp(const DefinitionConfig& config, const std::string& filename) {
	std::ofstream file(filename);
	if(!file.is_open())
		return false;
	
	file << CAUTION_MESSAGE << std::endl;
	file << "#pragma once" << std::endl;
	file << "#include \"Messages.hpp\"" << std::endl;
	file << "#include \"MessageConstants.hpp\"" << std::endl;
	file << "#include <eznet/MessageAdapters.hpp>" << std::endl;
	for(auto& inc: config.adaptersIncludes)
		file << "#include " << inc << std::endl;
	file << "namespace eznet {" << std::endl;
	
	for(auto& def: config.definitions) {
		file << "void readMessage(BufferReader& reader, " << def.structName << "& message);" << std::endl;
		file << "void writeMessage(BufferWriter& writer, const " << def.structName << "& message);" << std::endl;
		file << "size_t getMessageSize(const " << def.structName << "& message);" << std::endl;
		if(def.message) {
			file << "ENetPacket* createPacket(BufferWriter& writer, const " << def.structName << "& message);" << std::endl;
		}
	}
	file << "}" << std::endl;
	file.close();
	return true;
}

bool MessageGenerator::generateAdaptersCpp(const DefinitionConfig& config, const std::string& filename) {
	std::ofstream file(filename);
	if(!file.is_open())
		return false;

	file << CAUTION_MESSAGE << std::endl;
	file << "#include \"MessageAdapters.hpp\"" << std::endl;
	file << "#include <enet/enet.h>" << std::endl;
	file << "namespace eznet {" << std::endl;
	for(auto& def: config.definitions) {
		
		file << "void readMessage(BufferReader& reader, " << def.structName <<"& message) {" << std::endl;
		for(auto& field: def.fields) {
			if(field.list) {
				std::string sizeName = field.name + "_size";
				file << "\tuint16_t " << sizeName << " = reader.read<uint16_t>();" << std::endl;
				file << "\tfor(uint16_t i=0; i<" << sizeName << "; i++){" << std::endl;
				file << "\t\tmessage." << field.name << ".emplace_back();" << std::endl;
				file << "\t\treadMessage(reader, message." << field.name << ".back());" << std::endl;
				file << "\t}" << std::endl;
			} else {
				file << "\treadMessage(reader, message." << field.name << ");" << std::endl;
			}
		}
		file << "}" << std::endl;

		file << "void writeMessage(BufferWriter& writer, const " << def.structName <<"& message) {" << std::endl;
		for(auto& field: def.fields) {
			if(field.list) {
				std::string sizeName = field.name + "_size";
				file << "\tauto " << sizeName << " = uint16_t(message." << field.name << ".size());" << std::endl;
				file << "\twriter.write<uint16_t>(" << sizeName << ");" << std::endl;
				file << "\tfor(auto& list_entry: message." << field.name << "){" << std::endl;
				file << "\t\twriteMessage(writer,list_entry);" << std::endl;
				file << "\t}" << std::endl;
			} else {
				file << "\twriteMessage(writer, message." << field.name << ");" << std::endl;
			}
		}
		file << "}" << std::endl;
		
		file << "size_t getMessageSize(const " << def.structName <<"& message){" << std::endl;
		if(def.message)
			file << "\treturn sizeof(MessageType)";
		else
			file << "\treturn 0";
		
		for(auto& field: def.fields) {
			file << "+getMessageSize(message." << field.name << ")";
		}
		file << ";" << std::endl;
		file << "}" << std::endl;
		
		if(def.message) {
			file << "ENetPacket* createPacket(BufferWriter& writer, const " << def.structName << "& message) {" << std::endl;
			file << "\twriter.reset();" << std::endl;
			file << "\twriter.write<MessageType>(" << def.structName << "::getType());" << std::endl;
			file << "\twriteMessage(writer, message);" << std::endl;
			std::string flags;
			if(def.reliable)
				flags = "ENET_PACKET_FLAG_RELIABLE";
			else if(def.unsequenced)
				flags = "ENET_PACKET_FLAG_UNSEQUENCED";
			else
				flags = "0";
			file << "\treturn enet_packet_create(writer.getData(), writer.getPosition(), " << flags << ");" << std::endl;
			file << "}" << std::endl;
		}
	}
	file << "}" << std::endl;
	file.close();
	return true;
}


