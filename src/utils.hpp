#pragma once
#include "lexer.hpp"
#include "translator.hpp"

namespace utils {
	std::vector<lexer::Token> peekAheadToEOL(std::vector<lexer::Token> tokens, int i) {
		std::vector<lexer::Token> ret;
		for(; tokens[i].type != LTT::EOL; i++) {
			if(tokens[i].type == LTT::EOL)
				break;
			ret.push_back(tokens[i]);
		}
		return ret;
	}
}
