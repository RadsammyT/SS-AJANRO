#pragma once
#include "lexer.hpp"
#include "translator.hpp"

namespace utils {
	std::vector<lexer::Token> peekAheadToEOL(std::vector<lexer::Token> tokens, int i);
}
