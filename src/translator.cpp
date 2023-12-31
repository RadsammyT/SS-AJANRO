#include "translator.hpp"
#include "translator_fileio.hpp"
#include "utils.hpp"
#include <algorithm>
#include <string>
#include <variant>

namespace translator {
	//this is more of a checker than something important for translation
	//except for when concatenating strings/variables in output. do '<<' operator
	//instead of '+' operator

	bool isType(LTT type) {
		return 
			type == LTT::TypeString ||
			type == LTT::TypeChar ||
			type == LTT::TypeFloating ||
			type == LTT::TypeInteger ||
			type == LTT::TypeBool ||
			type == LTT::TypeVoid ||
			type == LTT::TypeOutputFile ||
			type == LTT::TypeInputFile;
	}

	bool isLiteral(LTT type) {
		return 
			type == LTT::IntegerLit ||
			type == LTT::FloatingLit ||
			type == LTT::StringLit || 
			type == LTT::BoolLit ||
			type == LTT::CharLit;
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
			case LTT::TypeInputFile:
				return "std::string";
			case LTT::TypeOutputFile:
				return "std::ofstream";
			case LTT::TypeVoid:
				return "void";
			default:
				printf("UH OH: Got a 'getTranslatedType()' call with an invalid"
						" type. Type is %d.\n", type);
				return "__INVALID_TYPE__";
		}
	}


	std::vector<std::string> getInputtedVars(std::vector<lexer::Token> tokens) {
		std::vector<std::string> ret;
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::Input && tokens[i+1].type == LTT::Identifier) {
				if((utils::peekAheadToEOL(tokens, i).end() - 2)->type == LTT::FromFile) {
					i += utils::peekAheadToEOL(tokens, i).size();
					continue;
				}
				ret.push_back(tokens[i+1].val);
			}
		}
		return ret;
	} 

	std::map<std::string, var> getAllVars(std::vector<lexer::Token> tokens) {
		std::map<std::string, var> vars;
		int line = 1;
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::EOL) {
				line++;
			}
			if(tokens[i].type == LTT::Identifier) {
				if(isType(tokens[i-1].type)) {
					var temp = {
						.type = tokens[i-1].type,
						.isConst = false,
						.val = "",
						.array {
							.dimension = 0,
							.allDimensionsConst = true,
						}
					};
					if(tokens[i-2].type == LTT::ConstVar) {
						temp.isConst = true;
						// if i+2 is a literal?
						if(isLiteral(tokens[i+2].type) &&
								tokens[i+2].type != LTT::OpenBrace)
							temp.val = tokens[i+2].val;
						else {
							printf(
								"getAllVars WARNING: "
								"const var declaration @ L%d does not have a literal.",
								line
							);
						} 		// prolly do nothing
								// because it was likely an array 
								// brace initalizer

					}
					
					vars[tokens[i].val] = temp;
					int original = i;
					while(true) {
						if(tokens[i+1].type == LTT::OpenBracket
								&& tokens[i+3].type == LTT::CloseBracket) {
							int len;
							if(tokens[i+2].type == LTT::IntegerLit)
								len = std::stoi(tokens[i+2].val);
							else if(tokens[i+2].type == LTT::Identifier) {
								var ident = vars[tokens[i+2].val];
								if(ident.isConst && ident.type == LTT::TypeInteger) {
									len = std::stoi(ident.val);
								} else {
									if(ident.type != LTT::TypeInteger) {
										printf(
											"getAllVars ERROR: invalid const for array size!\n"
											"offending const: %s\n"
											"used in identifier: %s\n"
											"you might have used a const that is not an integer\n",
											tokens[i+2].val.c_str(),
											tokens[original].val.c_str()
										);
										exit(1);
									} else {
										// fileIO check for later
									}
								}
							} else {
								printf(
									"getAllVars ERROR: invalid type for array size!\n"
									"offending type|value: %d|%s\n"
									"used in identifier: %s\n"
									"you might have used a literal thats not an integer\n",
									tokens[i+2].type, tokens[i+2].val.c_str(),
									tokens[original].val.c_str()
								);
							}
							vars[tokens[original].val].array.sizes.push_back(len);
							i += 3;
							vars[tokens[original].val].array.dimension += 1;
							if(tokens[i].type != LTT::Identifier) {
								continue;
							}
						} else {
							break;
						}
					}
				}
				if(vars.find(tokens[i].val) == vars.end()
						&& tokens[i-1].type != LTT::StartProgram
						&& tokens[i].type == LTT::Identifier) {
					printf(
						"WARNING: Unknown identifier \"%s\" at L%d\n",
						tokens[i].val.c_str(),
						line
					);
				}
				continue;
			}
		}
		getArrayConstStatus(vars, tokens);
#if defined (DEBUG)
		DEBUGMARK;
#endif
		std::vector<std::string> illegals;
		for(auto i: vars) 
			if(i.second.type == (LTT) 0) 
				illegals.push_back(i.first);

		for(auto i: illegals)
			vars.erase(i);

		for(auto i: vars) {
#if defined(DEBUG)
			printf("var: %s | type: %d\n"
					"dimension: %d\n"
					"isConst: %d | arrayConst: %d\n"
					"constVal: %s\n", i.first.c_str(),
					i.second.type,
					i.second.array.dimension,
					i.second.isConst,
					i.second.array.allDimensionsConst,
					i.second.val.c_str());
			printf("sizes: ");
			for(int i: i.second.array.sizes) {
				printf("%d ", i);
			}
			printf("\n_______________\n");
#endif
		}
		return vars;
	}

	void getArrayConstStatus(std::map<std::string, var>& vars,
			std::vector<lexer::Token> tokens) {
		for(int i = 0; i < tokens.size(); i++) {
			if(tokens[i].type == LTT::Identifier) {
				if(isType(tokens[i-1].type) && tokens[i+1].type == LTT::OpenBracket) {
					int original = i;
					while(true) {
						if(tokens[i+1].type == LTT::OpenBracket && !isLiteral(tokens[i+2].type)) {
							if(!vars[tokens[i+2].val].isConst) {
								vars[tokens[original].val].array.allDimensionsConst = false;
							}
							i += 3;
						} else {
							break;
						}
					}
				} else {
					if(isType(tokens[i-1].type)) {
						vars[tokens[i].val].array.allDimensionsConst = false;
					}
				}
			}
		}
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
			std::string& programName, CLI::flags flags) {
		std::map<std::string, var> vars = getAllVars(tokens);
		std::vector<std::string> inputs = getInputtedVars(tokens);
		file::InputtedFileVars fileInputs = file::getFileInputtedVars(tokens,  vars);
		std::vector<Context> contextStack;
		std::stringstream file;
		bool wipeCin = false;
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
						if(contextStack.back() == 
								Context::FunctionDecl) {
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
					<< "#include <limits>\n#include <fstream>\n"
					<< "#include <sstream>\n";
				if(fileInputs.integer) {
					file << file::intFileInput << "\n";
				}
				if(fileInputs.floating) {
					file << file::floatFileInput << "\n";
				}
				if(fileInputs.string) {
					file << file::stringFileInput << "\n";
				}
				if(fileInputs.boolean) {
					file << file::boolFileInput << "\n";
				}

				if(fileInputs.character) {
					file << file::charFileInput << "\n";
				}
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
					var temp;
					if(tokens[i-2].type == LTT::ConstVar) {
						temp.isConst = true;
						temp.val = tokens[i+2].val;
					}
					temp.type = tokens[i-1].type;
					temp.array.dimension = -1;
					vars[TKN.val] = temp;
					//	var {
					//		tokens[i-1].type,
					//		-1
					//	};
				}
				if(vars.find(TKN.val) == vars.end()) {
					// already notified the user of the unknown identifier
					// in getAllVars() so no need to remind them again
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
				auto lineTokens = utils::peekAheadToEOL(tokens, i);
				int lineSize = lineTokens.size();
				if(lineTokens.size() > 2) {
					if(lineTokens[lineTokens.size()-2].type == LTT::ToFile) {
						lineTokens.erase(lineTokens.begin());
						std::string streamName = (lineTokens.end() - 1)->val;
						lineTokens.erase(lineTokens.end()-1);
						lineTokens.erase(lineTokens.end()-1);
						file << streamName;
						for(int j = 0; j < lineTokens.size(); j++) {
							if(lineTokens[j].type != LTT::Comma) {
								if(lineTokens[j].type == LTT::StringLit) {
									file << " << \"" << lineTokens[j].val << "\"";
								} else {
									file << " << " << lineTokens[j].val;
								}
							}
						}
						file << "<< \"\\n\";\n";
						i += lineSize;
						continue;
					}
				}
				contextStack.push_back(Context::Output);
				file << "std::cout << ";
				continue;
			}
			if(TKN.type == LTT::Input) {
				bool fileIO = false;
					if(vars[(utils::peekAheadToEOL(tokens, i).end() - 1)->val].type 
							// aka the last token of current line
							== LTT::TypeInputFile) 
						fileIO = true;
				if(tokens[i+1].type == LTT::Identifier) {
					if(!inputs.empty())
						if(inputs.front() != tokens[i+1].val && !fileIO) {
							printf("ERROR: Inaccurate input vars vector at line %d.\n"
									"Front of inputs is %s, the identifier here is %s\n",
									line,
									inputs.front().c_str(), tokens[i+1].val.c_str());
						}
					if(vars[(utils::peekAheadToEOL(tokens, i).end() - 1)->val].type 
							// aka the last token of current line
							== LTT::TypeInputFile) {
						// fileIO input
						auto lineTokens = utils::peekAheadToEOL(tokens, i);
						std::string stream = lineTokens[lineTokens.size()-1].val;
						lineTokens.erase(lineTokens.begin());
						lineTokens.erase(lineTokens.end()-2, lineTokens.end()-1);
						// erase input token, "from" token, and fileHandle identifier
						for(int j = 0; j < lineTokens.size(); j++) {
							if(lineTokens[j].type != LTT::Identifier)
								continue;
							if(vars[lineTokens[j].val].type != LTT::TypeInputFile) {
								if(vars[lineTokens[j].val].array.dimension < 1)
									file << "fileInput(" << lineTokens[j].val <<
										",\"" << lineTokens[j].val << "\","
										<< stream << ");\n";
								else {
									if(!vars[lineTokens[j].val]
											.array
											.allDimensionsConst) {
										printf(
											"TRANSLATION ERROR: "
											"array var '%s'|'%s' does not have fully const"
											" dimensions. FileIO is not supported for those "
											" arrays.\n",
											vars[lineTokens[j].val].val.c_str(),
											lineTokens[j].val.c_str()
										);
										continue;
									}
									// TODO: do this if all brackets cover
									// the amount of dimensions allocated to array
									if(utils::fullDimensionCoverage(
												lineTokens, j, vars)) {
										file << "fileInput(";
										file << lineTokens[j].val;
										utils::applyBracketsToDimensionalVar(lineTokens, j, file);
										file << ",\""<<lineTokens[j].val;
										utils::applyBracketsToDimensionalVar(lineTokens, j, file);
										file << "\"," << stream << ");\n";
									} else {
										utils::recurseFileInputs(lineTokens,
												j,
												file, 
												stream,
												vars);
									}
								}
							}
						}
						i += lineTokens.size() + 1;
						continue;
					}
					// console input
					int original = i;
					if(vars.find(tokens[i+1].val) != vars.end()) {
						if(inputs.size() > 1) {
							if(vars[inputs.front()].type != LTT::TypeString &&
									vars[inputs[1]].type == LTT::TypeString) {
								wipeCin = true;

							}
							if(vars[inputs.front()].type == LTT::TypeString &&
									vars[inputs[1]].type != LTT::TypeString) {
								wipeCin = true;

							}
						}
						if(vars[tokens[i+1].val].type == LTT::TypeString) {
							file << "std::getline(std::cin,"
								<< tokens[i+1].val << ")";
						} else {
							file << "std::cin >> " << 
								tokens[i+1].val;
						}
						if(vars[tokens[i+1].val].array.dimension != 0) {
							i += 2;
							while(true) {
								file << tokens[i].val;
								i++;
								if(tokens[i].type == LTT::EOL) {
									file << ";\n";
									break;
								}
							}
						}
						if((wipeCin || flags.constIgnore) && !flags.noConstIgnore) {
							file << ";\n";
							file << "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n')";
							wipeCin = false;
						}

						if(vars[tokens[original+1].val].array.dimension == 0) {
							i++;
						}
					}
				}
				if(!inputs.empty())
					inputs.erase(inputs.begin());
				continue;
			}
			if(TKN.type == LTT::OpenFile) {
				if(vars[tokens[i+1].val].type == LTT::TypeOutputFile) {
					if(tokens[i+2].type == LTT::StringLit)
						file << tokens[i+1].val
							<< ".open(\"" << tokens[i+2].val 
							<< "\")";
					else
						file << tokens[i+1].val
							<< ".open(" << tokens[i+2].val 
							<< ")";
				}
				if(vars[tokens[i+1].val].type == LTT::TypeInputFile) {
					file << tokens[i+1].val
						<< " = \"" << tokens[i+2].val
						<< "\"";
				}
				i += 2;
				continue;
			}
			if(TKN.type == LTT::CloseFile) {
				if(vars[tokens[i+1].val].type == LTT::TypeOutputFile)
					file << tokens[i+1].val << ".close()";
				else 
					file << tokens[i+1].val << " = \"\"";
				i += 1;
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
			if(TKN.type == LTT::OperatorNotEqual) {
				file << " != ";
				continue;
			}
			if(TKN.type == LTT::OperatorAnd) {
				file << " && ";
				continue;
			}
			if(TKN.type == LTT::OperatorOr) {
				file << " || ";
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
					// maybe iterate over the params here?
					while(true) {
						if(TKN.type == LTT::CloseParenthesis) {
							file << ") {";
							contextStack.pop_back();
							contextStack.push_back(Context::Function);
							break;
						}
						if(TKN.type == LTT::OpenBracket ||
								TKN.type == LTT::CloseBracket) {
							i++;
							continue;
						}
						if(TKN.type == LTT::Comma) {
							file << ", ";
						}
						if(isType(TKN.type)) {
							file << getTranslatedType(TKN.type) << " ";
						}
						if(TKN.type == LTT::Identifier
								&& tokens[i-1].type != LTT::OpenBracket) {
							if(vars.find(TKN.val) != vars.end()) {
								int dimensions = 0;
								int j = i;
								while(true) {
									if(tokens[j].type == LTT::Comma ||
											tokens[j].type == LTT::CloseParenthesis) {
										vars[TKN.val].array.dimension = dimensions;
										break;
									}
									if(tokens[j+1].type == LTT::OpenBracket &&
											tokens[j+2].type == LTT::CloseBracket) {
										dimensions++;
										j += 2;
									}
									if(tokens[j+1].type == LTT::OpenBracket &&
											tokens[j+2].type == LTT::Identifier &&
											tokens[j+3].type == LTT::CloseBracket) {
										dimensions++;
										j += 3;
									}
									j++;
								}
								if(vars[TKN.val].array.dimension != 0) {
									auto var = vars[TKN.val];
									for(int iter = 0; 
											iter < var.array.dimension;
											iter++ ) {
										file << "*"; // spooky pointers
									}
								}
								file << TKN.val;
							}
						}
						i++;
					}
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
						&& contextStack.back() != Context::Expression
						&& contextStack.back() != Context::FunctionDecl
						&& contextStack.back() != Context::Flow))) {
					file << "[";
					contextStack.push_back(Context::ArrayDecl);
					if(*(contextStack.end() - 2) == Context::FunctionDecl) {
						contextStack.pop_back();
						printf("OpenBracket triggered a ArrayDecl context in a FunctionDecl\n"
								"Context. This should not happen and should be fixed.\n");
					}
					if(tokens[i-1].type != LTT::Identifier
							&& !(isType(tokens[i-2].type))
							&& contextStack.back() == Context::ArrayDecl) {
						contextStack.pop_back();
						printf("OpenBracket triggered a ArrayDecl context when the tokens\n"
								"of previous do not indicate such. This should now be fixed.\n");
					}
					continue;
				}
				file << "[";
				continue;
			}

			if(TKN.type == LTT::CloseBracket) {
				if(contextStack.back() == Context::ArrayDecl) {
					if(tokens[i+1].type != LTT::OpenBracket &&
							tokens[i+1].type != LTT::SetSymbol) {
						file << "] = {}";
					contextStack.pop_back();
						continue;
					}
				} 
				file << "]";
				continue;
			}

			if(TKN.type == LTT::OpenBrace) {
				if(contextStack.back() == Context::ArrayDecl)
					contextStack.pop_back();
				file << "{";
				continue;
			}
			if(TKN.type == LTT::CloseBrace) {
				file << "}";
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
