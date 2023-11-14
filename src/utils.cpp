#include "utils.hpp"

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

	//Checks if the array access is accessing an individual element
	// instead of an array. Returns false otherwise.
	// 
	// int i must be an identifier just before the brackets.
	bool fullDimensionCoverage(std::vector<lexer::Token> lineTokens, int i,
			std::map<std::string, translator::var> vars) {
		// array[(INT LIT OR IDENT)][(INDEX)][(INDEX)]
		int originalIndex = i;
		if(lineTokens[i].type != LTT::Identifier) {
#if defined(DEBUG)
			printf("fullDimensionCoverage ERROR: starting i index points to"
					" token that is NOT an identifier!\n"
					"Index is %d with type being %d.\n"
					"Value of token is '%s'\n", i, lineTokens[i].type,
					lineTokens[i].val.c_str());
			for(auto i: lineTokens) {
				printf("'%s' ", i.val.c_str());
			}
			printf("\n");
#endif
			return false;
		}
		if(lineTokens[i+1].type != LTT::OpenBracket) {
#if defined(DEBUG)
			printf("fullDimensionCoverage ERROR: i+1 index points to"
					" token that is NOT open bracket!\n"
					"Index is %d with type being %d.\n"
					"Value of token is '%s'\n", i+1, lineTokens[i+1].type,
					lineTokens[i+1].val.c_str());
			for(auto i: lineTokens) {
				printf("'%s' ", i.val.c_str());
			}
			printf("\n");
#endif
			return false;

		}
		bool openedBracket; 
		int accessedElements = 0;
		i++; // to the bracket (or the first instance of bracket)
		for(; i < lineTokens.size(); i++) {
			lexer::Token t = lineTokens[i];
			if(t.type == LTT::OpenBracket) {
				openedBracket = true;
				continue;
			}
			if(t.type == LTT::CloseBracket && openedBracket) {
				openedBracket = false;
				accessedElements++;
				continue;
			}
			if(t.type == LTT::Comma) {
				break;
			}
		}
		if(vars[lineTokens[originalIndex].val].array.dimension == accessedElements) {
			return true;
		}
		return false;
	}
	int getDimensionCoverage(std::vector<lexer::Token> lineTokens, int i, 
			std::map<std::string, translator::var> vars) {
		if(lineTokens[i].type != LTT::Identifier) {
#if defined(DEBUG)
			printf("getDimensionCoverage ERROR: starting i index points to"
					" token that is NOT an identifier!\n"
					"Index is %d with type being %d.\n"
					"Value of token is '%s'\n", i, lineTokens[i].type,
					lineTokens[i].val.c_str());
			for(auto i: lineTokens) {
				printf("'%s' ", i.val.c_str());
			}
			printf("\n");
#endif
			return false;
		}
		if(lineTokens[i+1].type != LTT::OpenBracket) {
#if defined(DEBUG)
			printf("getDimensionCoverage ERROR: i+1 index points to"
					" token that is NOT open bracket!\n"
					"Index is %d with type being %d.\n"
					"Value of token is '%s'\n", i+1, lineTokens[i+1].type,
					lineTokens[i+1].val.c_str());
			for(auto i: lineTokens) {
				printf("'%s' ", i.val.c_str());
			}
			printf("\n");
#endif
			return false;
		}
		bool openedBracket; 
		int accessedElements = 0;
		i++; // to the bracket (or the first instance of bracket)
		for(; i < lineTokens.size(); i++) {
			lexer::Token t = lineTokens[i];
			if(t.type == LTT::OpenBracket) {
				openedBracket = true;
				continue;
			}
			if(t.type == LTT::CloseBracket && openedBracket) {
				openedBracket = false;
				accessedElements++;
				continue;
			}
			if(t.type == LTT::Comma) {
				break;
			}
		}
		return accessedElements;
	}
	bool recurseFileInputs(std::vector<lexer::Token> lineTokens,
			int i,
			std::stringstream& file,
			std::string stream,
			std::map<std::string, translator::var> vars) {
		// only recurses file inputs for arrays only!
		// should also work for cases like:
		// int array[10][10][10]
		// input array[10] from handle 
		// input array from handle
		// note: int i(ndex) must point to identifier on lineTokens
		int actualDimensions = 
			vars[lineTokens[i].val].array.dimension;
		int coveredDimensions = 
			utils::getDimensionCoverage(lineTokens, i, vars);
		
		if(actualDimensions - coveredDimensions == 1
				&& actualDimensions == 1) {
			for(int iter = 0; iter < *(vars[lineTokens[i].val].array.sizes.end() - 1); iter++) {
				file << "fileInput(";
				file << lineTokens[i].val;
				file << "[" << iter << "]";
				file << ",\"" << lineTokens[i].val;
				file << "[" << iter << "]";
				file << "\"," << stream << ");\n";
			}
			return true;
		}
		printf("recurseFileInputs ERROR: UNCOVERED CASE\n");
		return false;
	}
}
