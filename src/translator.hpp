#pragma once
#include <vector>
#include <map>

#include "CLI.hpp"
#include "lexer.hpp"


#define LTT lexer::TokenType // this define is not sponsored by lttstore.com

namespace translator {
	//this is more of a checker than something important for translation
	//except for when concatenating strings/variables in output. do '<<' operator
	//instead of '+' operator
	enum class Context {
		Program,
		Function, // Be it main or a usual Function
		FunctionDecl, // to differenciate between calling and declaring functions
		Flow, // if, while, etc.
		Output, // so + gets translated to << instead of +
		Expression, // used in parenthesis `()` for output
		ArrayDecl, // to put '= {}' after array initialization
				   // as not doing so causes UB
	};

	struct var {
		LTT type;
		bool isConst;
		std::string val;
		struct {
			int dimension; // 0 if no array
			std::vector<int> sizes;
			bool allDimensionsConst; // true if all dimensions are const integers and 
									 // literals
									 // false if one dimension isnt a const or a literal
		} array;
	};

	bool isType(LTT type);
	bool isLiteral(LTT type);
	std::string getTranslatedType(LTT type);
	std::vector<std::string> getInputtedVars(std::vector<lexer::Token> tokens);
	std::map<std::string, var> getAllVars(std::vector<lexer::Token> tokens);
	void getArrayConstStatus(std::map<std::string, var>& vars,
			std::vector<lexer::Token> tokens);
	void illegalTokenContext(lexer::TokenType type, std::vector<Context> c, int line);
	std::string  translate(std::vector<lexer::Token> tokens, std::string outputFile,
			std::string& programName, CLI::flags flags);
}
