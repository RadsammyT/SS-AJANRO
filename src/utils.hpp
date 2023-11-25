#pragma once
#include "lexer.hpp"
#include "translator.hpp"
#include <sstream>

#define DEBUGMARK fprintf(stderr, "[file: %s | ln: %d | fn: %s]\n", __FILE__, __LINE__, __func__)
namespace utils {
	/// returns a vector of tokens starting at index i until the first occurence of LTT::EOL
	std::vector<lexer::Token> peekAheadToEOL(std::vector<lexer::Token> tokens, int i);
	// checks if 
	bool fullDimensionCoverage(std::vector<lexer::Token> lineTokens, int i,
			std::map<std::string, translator::var> vars);
	int getDimensionCoverage(std::vector<lexer::Token> lineTokens, int i, 
			std::map<std::string, translator::var> vars); 
	void applyBracketsToDimensionalVar(std::vector<lexer::Token> lineTokens, int i,
			std::stringstream& file);
	bool recurseFileInputs(std::vector<lexer::Token> lineTokens,
			int i,
			std::stringstream& file,
			std::string stream,
			std::map<std::string, translator::var> vars);
}
