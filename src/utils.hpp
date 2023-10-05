#pragma once
#include "lexer.hpp"
#include "translator.hpp"
#include <sstream>

namespace utils {
	/// returns a vector of tokens starting at index i until the first occurence of LTT::EOL
	std::vector<lexer::Token> peekAheadToEOL(std::vector<lexer::Token> tokens, int i);

	void applyBracketsToDimensionalVar(std::vector<lexer::Token> lineTokens, int i,
			std::stringstream& file);
}
