#pragma once
#include <string> 
#include <vector>
#include <map>

#include "translator.hpp"
#include "utils.hpp"

namespace translator { namespace file {


	const std::string floatFileInput =
		"void fileInput(float& var, std::string varName, std::string a) {"
		"	std::ifstream stream(a);"
		"	if(!stream.is_open()) {"
		"		printf(\"FILEIO ERROR: file '%s' NOT FOUND\","
		"				a.c_str());"
		"	}"
		"	std::string in;"
		"	while(std::getline(stream, in)) {"
		"	if(in == varName) {"
		"			std::getline(stream, in);"
		"			var = std::stof(in);"
		"			return;"
		"		}"
		"	}"
		"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
			"return;"
		"}";	
	const std::string intFileInput =
		"void fileInput(int& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
		"	if(!stream.is_open()) {"
		"		printf(\"FILEIO ERROR: file '%s' NOT FOUND\","
		"				a.c_str());"
		"	}"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = std::stoi(in);"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
			"return;"
		"}";	
	const std::string stringFileInput =
		"void fileInput(std::string& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
		"	if(!stream.is_open()) {"
		"		printf(\"FILEIO ERROR: file '%s' NOT FOUND\","
		"				a.c_str());"
		"	}"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = in;"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
			"return;"
		"}";	
	const std::string charFileInput =
		"void fileInput(char& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
		"	if(!stream.is_open()) {"
		"		printf(\"FILEIO ERROR: file '%s' NOT FOUND\","
		"				a.c_str());"
		"	}"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = in.at(0);"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
			"return;"
		"}";	

	const std::string boolFileInput =
		"void fileInput(bool& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
		"	if(!stream.is_open()) {"
		"		printf(\"FILEIO ERROR: file '%s' NOT FOUND\","
		"				a.c_str());"
		"	}"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		if(in == \"true\") "
			"			var = true;"
			"		else if(in == \"false\")"
			"			var = false;"
			"		else { "
			"			printf(\"FILEIO ERROR: varName %s is bound to non-boolean value %s\", varName.c_str(), in.c_str());"
			"			}"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
			"return;"
		"}";	

	struct InputtedFileVars {
		bool string;
		bool character;
		bool floating;
		bool integer;
		bool boolean;
	};
	
	InputtedFileVars getFileInputtedVars(std::vector<lexer::Token> tokens,
			std::map<std::string, translator::var> vars);
}
}
