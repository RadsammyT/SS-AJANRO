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
	struct InputtedFileVars {
		bool string;
		bool character;
		bool floating;
		bool integer;
		bool boolean;
	};
	
	InputtedFileVars getFileInputtedVars(std::vector<lexer::Token> tokens,
			std::map<std::string, translator::var> vars) {
		InputtedFileVars ret = {};
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::Input) {
				auto lineTokens = utils::peekAheadToEOL(tokens, i);
			if(lineTokens[lineTokens.size() - 2].type == LTT::FromFile) {
				if(lineTokens[0].type != LTT::Input) {
					printf("uh oh! in getFileInputtedVars:\n"
							"first index of peeked ahead is not input\n"
							"vector goes like this:\n");
					for(int j = 0; j < lineTokens.size(); j++) {
						printf("%d ", lineTokens[j].type);
					}
					exit(1);
				}
				lineTokens.erase(lineTokens.end() - 2, lineTokens.end() - 1);
				lineTokens.erase(lineTokens.begin());
				for(int j = 0; j < lineTokens.size(); j++) {
					if(vars.find(lineTokens[j].val) != vars.end()) {
						if(vars[lineTokens[j].val].type == LTT::TypeString) {
							ret.string = true;
						}
						if(vars[lineTokens[j].val].type == LTT::TypeFloating) {
							ret.floating = true;
						}
						if(vars[lineTokens[j].val].type == LTT::TypeInteger) {
							ret.integer = true;
						}
						if(vars[lineTokens[j].val].type == LTT::TypeChar) {
							ret.character = true;
						}
						if(vars[lineTokens[j].val].type == LTT::TypeBool) {
							ret.boolean = true;
						}
					}
				}
				// var1, var2
			}
		}
		}
		return ret;
	}
}
}
