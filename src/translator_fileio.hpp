#pragma once

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "translator.hpp"
#define LTT lexer::TokenType
/*
 *	AJANRO input file:
 *	inputfile fileVar
 * // equivalent to std::ifstream fileVar; 
 *	open fileVar filePath
 * // equivalent to filePath.open(fileName)
 *	input var1, var2 from fileVar
 *	close fileVar
 * // equivalent to fileVar.close();
 *
 *	AJANRO output files:
 *	outputfile fileVar
 *	//equivalent to std::ofstream fileVar;
 *	open fileVar filePath 
 * //equivalent to fileVar.open(filePath)
 *	output "String Literal: ", variable to fileVar 
 * //equivalent to fileVar << "String Literal: " << variable;
 *	close fileVar
 * //equivalent to fileVar.close();
 *	
 *	possible layout of an AJANRO input file:
 *	varInt
 *	123
 *	varArrInt[0]
 *	12344
 *	varString
 *	This is a test message
 *
 *
 *  possible translation being:
 *  fileInput(var1, "var1", a);
 *  would need a function to be included into output
 *	oh boy time to abuse function overloading
 *
 */
/*
void fileInput(int& var, std::string varName, std::ifstream a) {
	std::stringstream stream;
	stream << a.rdbuf();
	std::string in;
	while(std::getline(stream, in)) {
		if(in == varName) {
			std::getline(stream, in);
			var = std::stoi(in);
			continue;
		}
	}
}	
void fileInput(float& var, std::string varName, std::ifstream a) {
	
}
void fileInput(std::string* var, std::string varName, std::ifstream a) {

}

void cppFileIO() {
	// input file
	std::string fileName;
	std::string line;
	std::ifstream a;
	a.open(fileName);
	std::getline(a, line);
	a.close();


	//output file
	std::ofstream b;
	b.open(fileName, std::ios::out);
	b << "TTeest";
	b.close();
}
*/
namespace translator { namespace file {


	const std::string floatFileInput =
		"void fileInput(float& var, std::string varName, std::string a) {"
		"	std::ifstream stream(a);"
		"	std::string in;"
		"	while(std::getline(stream, in)) {"
		"	if(in == varName) {"
		"			std::getline(stream, in);"
		"			var = std::stof(in);"
		"			return;"
		"		}"
		"	}"
		"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
		"}";	
	const std::string intFileInput =
		"void fileInput(int& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = std::stoi(in);"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
		"}";	
	const std::string stringFileInput =
		"void fileInput(std::string& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = in;"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
		"}";	
	const std::string charFileInput =
		"void fileInput(char& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
			"std::string in;"
			"while(std::getline(stream, in)) {"
			"if(in == varName) {"
			"		std::getline(stream, in);"
			"		var = in.at(0);"
			"		return;"
			"	}"
			"}"
			"printf(\"FILEIO ERROR: '%s' not found\\n\", varName.c_str());"
		"}";	

	const std::string boolFileInput =
		"void fileInput(bool& var, std::string varName, std::string a) {"
			"std::ifstream stream(a);"
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
