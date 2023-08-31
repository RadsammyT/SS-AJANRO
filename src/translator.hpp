#include "lexer.hpp"
#include <vector>
#include <fstream>
#include <filesystem>
#include <map>
#pragma once
#define LTT lexer::TokenType // this define is not sponsored by lttstore.com
namespace translator {
	//this is more of a checker than something important for translation
	//except for when concatenating strings/variables in output. do '<<' operator
	//instead of '+' operator
	enum class Context {
		Program,
		Function, // Be it main or a usual Function
		Flow, // if, while, etc.
		Output, // so + gets translated to << instead of +
		Expression // used in parenthesis `()` for output
	};

	bool isType(LTT type) {
		return type == LTT::TypeString ||
			type == LTT::TypeChar ||
			type == LTT::TypeFloating ||
			type == LTT::TypeInteger ||
			type == LTT::TypeBool;
	}
	std::string getTranslatedType(LTT type) {
		switch(type) {
			case LTT::TypeString:
				return "std::string";
			case LTT::TypeChar:
				return "char";
			case LTT::TypeFloating:
				return "float";
			case LTT::TypeInteger:
				return "int";
			case LTT::TypeBool:
				return "bool";
			default:
				return "__INVALID_TYPE__";
		}
	}

	void illegalTokenContext(lexer::TokenType type, Context context, int line) {

					printf("Token %d used in incorrect"
							" Context (%d), at line %d",
							type,
							context,
							line);
					exit(1);
	}

	std::string  translate(std::vector<lexer::Token> tokens, std::string outputFile) {
		std::map<std::string, lexer::TokenType> vars;
		std::vector<Context> contextStack;
		std::stringstream file;
		while(tokens.front().type != LTT::StartProgram) {
			tokens.erase(tokens.begin());
		}
		#define TKN tokens[i] // cursed
		int line = 1;
		for(int i = 0; i < tokens.size(); i++) {
			if(TKN.type == LTT::EOL) {
				line++;
				if(tokens[i-1].type != LTT::EOL) {
					if(tokens[i-1].type == LTT::OperatorAdd) {
						file << "\n";
						continue;
					} else {
						if(contextStack.back() ==
								Context::Output) {
							contextStack.pop_back();
							file << "<< std::endl";
						}
						if(contextStack.back() ==
								Context::Flow) {
							file << ") {\n";
							contextStack.pop_back();
							continue;
						}
					}
					file << ";\n";
					continue;
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
				i++;
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
				i++;
				continue;
			}
			if(TKN.type == LTT::ConstVar) {
				file << "const ";
				continue;
			}
			if(TKN.type == LTT::Identifier) {
				if(isType(tokens[i-1].type)) {
					vars[TKN.val] = tokens[i-1].type;
				}
				file << " " << TKN.val << " ";
				continue;
			}
			if(TKN.type == LTT::FlowIf) {
				file << "if(";
				contextStack.push_back(Context::Flow);
				continue;
			}

			if(TKN.type == LTT::FlowElse) {
				file << "} else {";
				continue;
			}

			if(TKN.type == LTT::FlowEndif) {
				file << "}";
				continue;
			}
			if(TKN.type == LTT::FlowWhile) {
				file << "while(";
				contextStack.push_back(Context::Flow);
				continue;
			}
			if(TKN.type == LTT::FlowEndWhile) {
				file << "}";
				continue;
			}
			if(TKN.type == LTT::Output) {
				contextStack.push_back(Context::Output);
				file << "std::cout << ";
				continue;
			}
			if(TKN.type == LTT::Input) {
				if(tokens[i+1].type == LTT::Identifier) {
					if(vars.find(tokens[i+1].val) != vars.end()) {
						if(vars[tokens[i+1].val] == LTT::TypeString) {
							file << "std::getline(std::cin,"
								<< tokens[i+1].val << ")";
						} else {
							file << "std::cin >> " << 
								tokens[i+1].val;
						}
					}
				}
				i++;
				continue;
			}
			if(TKN.type == LTT::OperatorAdd) {
				if(contextStack.back() ==
						Context::Output) {
					file << " << ";
				} else {
					file << " + ";
				}
				continue;
			}
			if(TKN.type == LTT::OperatorSub) {
				file << "-";
				continue;
			} 
			if(TKN.type == LTT::OperatorMulti) {
				file << "*";
				continue;
			}
			if(TKN.type == LTT::OperatorDivide) {
				file << "/";
			}
			if(TKN.type == LTT::OperatorGreater) {
				file << " > ";
				continue;
			}
			if(TKN.type == LTT::OperatorGreaterEqual) {
				file << " >= ";
				continue;
			}
			if(TKN.type == LTT::OperatorLess) {
				file << " < ";
				continue;
			}
			if(TKN.type == LTT::OperatorLessEqual) {
				file << " <= ";
				continue;
			}
			if(TKN.type == LTT::StringLit) {
				file << "\"" << TKN.val << "\"";
				continue;
			}
			if(TKN.type == LTT::IntegerLit) {
				file << " " << TKN.val << " ";
				continue;
			}
			if(TKN.type == LTT::FloatingLit) {
				file << " " << TKN.val << " ";
				continue;
			}
			if(TKN.type == LTT::BoolLit) {
				file << " " << TKN.val << " ";
				continue;
			}
			if(TKN.type == LTT::CharLit) {
				file << " \'" << TKN.val << "\'";
				continue;
			}
			if(TKN.type == LTT::SetSymbol) {
				if(contextStack.back() == Context::Flow) {
					file << " == ";
				}
				file << " = ";
				continue;
			}
			
			if(isType(TKN.type)) {
				file << getTranslatedType(TKN.type);
				continue;
			}
			printf("Unimplemented translation for token type %d\n"
					"Value of token: %s\n",
					TKN.type,
					TKN.val.c_str());
			exit(1);
		}
		return file.str();
	}
}
