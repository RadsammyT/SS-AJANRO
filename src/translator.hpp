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
		FunctionDecl, // to differenciate between calling and declaring functions
		Flow, // if, while, etc.
		Output, // so + gets translated to << instead of +
		Expression, // used in parenthesis `()` for output
		ArrayDecl, // to put '= {}' after array initialization
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
				printf("UH OH: Got a 'getTranslatedType()' call with an invalid"
						" type. Type is %d.", type);
				return "__INVALID_TYPE__";
		}
	}

	std::vector<std::string> getInputtedVars(std::vector<lexer::Token> tokens) {
		std::vector<std::string> ret;
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::Input && tokens[i+1].type == LTT::Identifier) {
				ret.push_back(tokens[i+1].val);
			}
		}
		return ret;
	} 

	std::map<std::string, lexer::TokenType> getAllVars(std::vector<lexer::Token> tokens) {
		std::map<std::string, lexer::TokenType> vars;
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::Identifier) {
				if(isType(tokens[i-1].type)) {
					vars[tokens[i].val] = tokens[i-1].type;
				}
				if(vars.find(tokens[i].val) == vars.end()) {
					printf("WARNING: Unknown identifier \"%s\"\n", tokens[i].val.c_str());
				}
				continue;
			}
		}
		return vars;
	}

	void illegalTokenContext(lexer::TokenType type, std::vector<Context> c, int line) {

					printf("Token %d used in incorrect"
							" Context (%d), at line %d\n",
							type,
							c.back(),
							line);
					printf("Contents of stack:\n");
					for(auto con: c) {
						printf("%d ", con);

					}
					printf("\n");
					exit(1);
	}

	std::string  translate(std::vector<lexer::Token> tokens, std::string outputFile,
			std::string& programName) {
		std::map<std::string, lexer::TokenType> vars = getAllVars(tokens);
		std::vector<std::string> inputs = getInputtedVars(tokens);
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
				} else {
					programName = tokens[i].val;
				}
				file << "#include <iostream>\n#include <string>\n"
					<< "#include <limits>\n";
				contextStack.push_back(Context::Program);
				continue;
			}
			if(TKN.type == LTT::EndProgram) {
				// TODO: Continue from here
				if(contextStack.back() == Context::Program)
					contextStack.pop_back();
				else {
					illegalTokenContext(TKN.type,
							contextStack,
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
							contextStack,
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
							contextStack, line);
				}
				contextStack.pop_back();
				i++;
				continue;
			}
			if(TKN.type == LTT::EndModule) {
				if(contextStack.back() !=
						Context::Function) {
					illegalTokenContext(TKN.type,
							contextStack, line);
				}
				file << "}";
				contextStack.pop_back();
				continue;
			}
			if(TKN.type == LTT::ConstVar) {
				file << "const ";
				continue;
			}
			if(TKN.type == LTT::Identifier) {
				if(isType(tokens[i-1].type) && vars.find(TKN.val) == vars.end()) {
					vars[TKN.val] = tokens[i-1].type;
				}
				if(vars.find(TKN.val) == vars.end()) {
					printf("WARNING: Unknown identifier \"%s\"\n", TKN.val.c_str());
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
				bool wipeCin = false;
				if(tokens[i+1].type == LTT::Identifier) {
					if(inputs.front() != tokens[i+1].val) {
						printf("ERROR: Inaccurate input vars vector at line %d.\n"
								"Front of inputs is %s, the identifier here is %s\n",
								line,
								inputs.front().c_str(), tokens[i+1].val.c_str());
					}
					if(vars.find(tokens[i+1].val) != vars.end()) {
						if(inputs.size() > 1) {
							if(vars[inputs.front()] != LTT::TypeString &&
									vars[inputs[1]] == LTT::TypeString) {
								wipeCin = true;

							}
						}
						if(vars[tokens[i+1].val] == LTT::TypeString) {
							file << "std::getline(std::cin,"
								<< tokens[i+1].val << ")";
						} else {
							file << "std::cin >> " << 
								tokens[i+1].val;
						}
						if(wipeCin) {
							file << ";\n";
							file << "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n')";
						}
					}
				}
				inputs.erase(inputs.begin());
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
				continue;
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
					continue;
				}
				file << " = ";
				continue;
			}

			if(TKN.type == LTT::OpenParenthesis) {
				if(tokens[i-1].type == LTT::Identifier && 
						isType(tokens[i-2].type)) {
					contextStack.push_back(Context::FunctionDecl);
					file << "(";
					continue;
				} else {
					contextStack.push_back(Context::Expression);
					file << "(";
					continue;
				}
			}

			if(TKN.type == LTT::CloseParenthesis) {
				if(contextStack.back() == Context::FunctionDecl) {
					file << ") {";
					contextStack.pop_back();
					contextStack.push_back(Context::Function);
					continue;
				} 
				if(contextStack.back() == Context::Expression) {
					file << ")";
					contextStack.pop_back();
					continue;
				}
			}

			if(TKN.type == LTT::OpenBracket) {
				if(((tokens[i-1].type == LTT::Identifier &&
						isType(tokens[i-2].type))
						|| (tokens[i-1].type == LTT::CloseBracket
						&& contextStack.back() != Context::Output
						&& contextStack.back() != Context::Expression))) {
					file << "[";
					contextStack.push_back(Context::ArrayDecl);
					continue;
				}
				file << "[";
				continue;
			}

			if(TKN.type == LTT::CloseBracket) {
				if(contextStack.back() == Context::ArrayDecl) {
					if(tokens[i+1].type != LTT::OpenBracket &&
							tokens[i+1].type != LTT::SetSymbol &&
							contextStack.back() != Context::Output)
						file << "] = {}";
					contextStack.pop_back();
					if(tokens[i+1].type != LTT::OpenBracket &&
							tokens[i+1].type != LTT::SetSymbol &&
							contextStack.back() != Context::Output)
						continue;
				} 
				file << "]";
				continue;
			}

			if(TKN.type == LTT::Comma) {
				file << ",";
				continue;
			}
			
			if(TKN.type == LTT::Return) {
				file << "return ";
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
