#include "lexer.hpp"
#include <vector>
#include <fstream>
#include <filesystem>
#pragma once
namespace translator {
	//this is more of a checker than something important for translation
	//except for when concatenating strings/variables in output. do '<<' operator
	//instead of '+' operator
	enum class Context {
		Program,
		Function, // Be it main or a usual Function
		Flow, // if, while, etc.
		Output // so + gets translated to << instead of +
	};
	
	struct Variable {
		lexer::TokenType type;
		std::string val;
	};

	void illegalTokenContext(lexer::TokenType type, Context context, int line) {

					printf("Token %d used in incorrect"
							" Context (%d), at line %d",
							type,
							context,
							line);
					exit(1);
	}

	bool translate(std::vector<lexer::Token> tokens, std::string outputFile) {
		std::vector<Context> contextStack;
		std::ofstream file_out;
		std::stringstream file;
		file_out.open(outputFile);
		if(!file_out.is_open()) {
			printf("ERROR: Cannot output translation - Unable to open output file\n");
			exit(1);
		}

		#define TKN tokens[i] // cursed
		#define LTT lexer::TokenType // this define is not sponsored by lttstore.com
		int line = 1;
		for(int i = 0; i < tokens.size(); i++) {
			if(TKN.type == LTT::EOL) {
				line++;
				if(tokens[i-1].type != LTT::EOL) {
					file << ";";
				}
				file << "\n";
				continue;
			}
			if(TKN.type == LTT::_EOF) {
				continue;
			}
			if(TKN.type == lexer::TokenType::StartProgram) {
				if(tokens[++i].type != LTT::Identifier) {
					printf("StartProgram token does not have "
							"an associating identifier!\n");
					exit(1);
				}
				file << "#include <iostream>\n#include <string>\n";
				contextStack.push_back(Context::Program);
				continue;
			}
			if(TKN.type == LTT::EndProgram) {
				// TODO: Continue from here
				if(contextStack.back() == Context::Program)
					contextStack.pop_back();
				else {
					illegalTokenContext(TKN.type,
							contextStack.back(),
							line);
				}
				continue;
			}
			if(TKN.type == LTT::StartMain) {
				file << "int main(int argc, char** argv) {";
				if(contextStack.back() != 
						Context::Program) {

					illegalTokenContext(TKN.type,
							contextStack.back(),
							line);
				}
				contextStack.push_back(Context::Function);
				continue;
			}
			if(TKN.type == LTT::EndMain) {
				file << "}\n";
				if(contextStack.back() !=
						Context::Function) {
					illegalTokenContext(TKN.type, 
							contextStack.back(), line);
				}
				contextStack.pop_back();
				continue;
			}
			if(TKN.type == LTT::Identifier) {
				file << " " << TKN.val << " ";
				continue;
			}
			printf("Unimplemented translation for token type %d\n",
					TKN.type);
			exit(1);
		}
		file_out << file.str();
		file_out.close();
		return true;
	}
}
