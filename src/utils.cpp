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
	void applyBracketsToDimensionalVar(std::vector<lexer::Token> lineTokens, int i,
			std::stringstream& file) {
		while(true) {
			if(lineTokens[i+1].type == LTT::OpenBracket
					&& lineTokens[i+3].type == LTT::CloseBracket) {
				file << "["<< lineTokens[i+2].val <<"]";
				i += 3;
			} else {
				break;
			}
		}
	}
}
