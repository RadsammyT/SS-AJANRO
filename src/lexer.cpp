
#pragma once
#include <cctype>
#include <cwctype>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include <regex>

namespace lexer {

enum class TokenType {
	StartProgram = 0, // MUST be the starting token
	EndProgram, // MUST be the final token
	StartMain,
	EndMain,
	EndModule, // whoever thought it was a good idea to rename functions to """modules"""
	Output,		// is clinically INSANE
	Input,
	Identifier, // Default for any tokens that aren't "reserved"
	ConstVar,
	FlowWhile, // Control flow statements.
	FlowIf,
	FlowThen,
	FlowElse,
	FlowDo,
	FlowEndif,
	FlowEndWhile,
	TypeString, // Types that are usually in variable declarations.
	TypeInteger,
	TypeFloating,
	TypeBool,
	TypeChar,
	TypeInputFile,
	TypeOutputFile,
	OpenFile,
	CloseFile,
	FromFile,
	ToFile,
	StringLit, // Type literals
	IntegerLit,
	FloatingLit,
	BoolLit,
	CharLit,
	SetSymbol, // =
	OperatorMulti,
	OperatorDivide,
	OperatorAdd,
	OperatorSub,
	OperatorAnd, // Boolean Operators 
	OperatorOr,
	OperatorLess,
	OperatorGreater,
	OperatorEqual,
	OperatorNotEqual,
	OperatorLessEqual,
	OperatorGreaterEqual,
	OpenParenthesis,
	CloseParenthesis,
	OpenBracket,
	CloseBracket,
	Comma,
	Return,
	EOL, // serves as a semicolon (as in the end of a command)
	_EOF,
};

struct Token {
	TokenType type;
	std::string val;
};

bool IsSymbol(char c, std::string in, int i) {
	if(c == '*' ||
		c == '/' ||
		c == '+' ||
		c == '-' ||
		c == '=' ||
		c == '&' ||
		c == '<' ||
		c == '>' ||
		c == '_' ||
		c == '!') {
		return true;
	}
	return false;
}

Token parseBuffer(std::string buf) {
	// keyword parsing
	if(buf == "startprogram") {
		return Token {TokenType::StartProgram, buf};
	}
	if(buf == "endprogram") {
		return Token {TokenType::EndProgram, buf};
	}
	if(buf == "startmain") {
		return Token {TokenType::StartMain, buf};
	}
	if(buf == "endmodule")
		return Token {TokenType::EndModule, buf};
	if(buf == "endmain") {
		return Token {TokenType::EndMain, buf};
	}
	if(buf == "output") {
		return Token {TokenType::Output, buf};
	}

	if(buf == "input") 
		return Token{TokenType::Input, buf};

	if(buf == "const") {
		return Token {TokenType::ConstVar, buf};
	}

	if(buf == "string") {
		return Token {TokenType::TypeString, buf};
	}

	if(buf == "floating") {
		return Token {TokenType::TypeFloating, buf};
	}

	if(buf == "integer") {
		return Token{TokenType::TypeInteger, buf};
	}

	if(buf == "char") {
		return Token{TokenType::TypeChar, buf};
	}

	if(buf == "boolean") {
		return Token{TokenType::TypeBool, buf};
	}

	if(buf == "inputfile")
		return Token{TokenType::TypeInputFile, buf};
	if(buf == "outputfile")
		return Token{TokenType::TypeOutputFile, buf};
	if(buf == "open")
		return Token{TokenType::OpenFile, buf};
	if(buf == "close")
		return Token{TokenType::CloseFile, buf};
	if(buf == "to")
		return Token{TokenType::ToFile, buf};
	if(buf == "from")
		return Token{TokenType::FromFile, buf};

	if(buf == "=") {
		return Token{TokenType::SetSymbol, buf};
	}

	if(buf == "true") {
		return Token{TokenType::BoolLit, buf};
	}

	if(buf == "false") {
		return Token{TokenType::BoolLit, buf};
	}

	if(buf == "if") {
		return Token{TokenType::FlowIf, buf};
	}

	if(buf == "then") {
		return Token{TokenType::FlowThen, buf};
	}
	
	if(buf == "else") {
		return Token{TokenType::FlowElse, buf};
	}

	if(buf == "endif") {
		return Token{TokenType::FlowEndif, buf};
	}

	if(buf == "while") {
		return Token{TokenType::FlowWhile, buf};
	}

	if(buf == "do")
		return Token{TokenType::FlowDo, buf};

	if(buf == "endwhile")
		return Token{TokenType::FlowEndWhile, buf};

	if(buf == "return")
		return Token{TokenType::Return, buf};
	//literal parsing
	// 32.141
	// 0.141
	// 3141
	// -3.141
	// -3141
	std::cmatch m;
	// i have two problems now :)
	if(std::regex_match(buf.c_str(), m, std::regex("[0-9]*[.][0-9]*"))) {
		// we must consider the possibility that the matched buffer may only contain a '.'
		if(buf != ".") {
			return Token { TokenType::FloatingLit, buf};
		} else {
			printf("ERROR: Invalid Float Literal (%s), dot only\n", buf.c_str());
			exit(1);
		}
	}

	if(std::regex_match(buf.c_str(), m, std::regex("[-][0-9]*[.][0-9]*"))) {
		// we must consider the possibility that the matched buffer may only contain a '.'
		if(buf != ".") {
			return Token { TokenType::FloatingLit, buf};
		} else {
			printf("ERROR: Invalid Float Literal (%s), dot only\n", buf.c_str());
			exit(1);
		}
	}

	if(std::regex_match(buf.c_str(), m, std::regex("[0-9]*"))) {
		return Token {TokenType::IntegerLit, buf};
	}

	if(std::regex_match(buf.c_str(), m, std::regex("[-][0-9]*"))) {
		return Token {TokenType::IntegerLit, buf};
	}

	if(IsSymbol(buf[0], buf, 0)) {
			if(buf == "*")
				return Token{TokenType::OperatorMulti, buf};
			if(buf == "/")
				return Token{TokenType::OperatorDivide, buf};
			if(buf == "+")
				return Token{TokenType::OperatorAdd, buf};
			if(buf == "-")
				return Token{TokenType::OperatorSub, buf};
			if(buf == ">")
				return Token{TokenType::OperatorGreater, buf};
			if(buf == "<")
				return Token{TokenType::OperatorLess, buf};
			if(buf == "!=")
				return Token{TokenType::OperatorNotEqual, buf};
			if(buf == ">=")
				return Token{TokenType::OperatorGreaterEqual, buf};
			if(buf == "<=")
				return Token{TokenType::OperatorLessEqual, buf};
			if(buf == "==")
				return Token{TokenType::OperatorEqual, buf};
			if(buf == "&&")
				return Token{TokenType::OperatorAnd, buf};
			if(buf == "||") 
				return Token{TokenType::OperatorOr, buf};
	}		


	return Token {TokenType::Identifier, buf};
}


std::vector<Token> tokenize(std::string in) {
	std::vector<Token> ret;
	std::string buf;
	while(true) { // cuz \n\r is used for windows text
		if(in.find("\r") != std::string::npos) {
			in.replace(in.find("\r"), sizeof("\r"), "");
		} else {
			break;
		}
	}
	if(in.find("startprogram") == std::string::npos) {
		printf("ERROR: startprogram not found!\n");
		exit(1);
	}
	if(in.find("endprogram") == std::string::npos) {
		printf("ERROR: endprogram not found!\n");
		exit(1);
	}
	int line = 1;
	for(int i = 0; i < in.size(); i++) {

		if(std::isalpha(in[i]) || in[i] == '_') {
			while(std::isalpha(in[i]) || in[i] == '_') {
				buf.push_back(in[i]);
				i++;
			}
			ret.push_back(parseBuffer(buf));
			buf.clear();
			i--;
			continue;
		}

		if(std::isdigit(in[i])) {
			if(in[i-1] == '-')
				buf.push_back(in[i-1]);
			while(std::isdigit(in[i]) || in[i] == '.') {
				buf.push_back(in[i]);
				i++;
			}
			ret.push_back(parseBuffer(buf));
			buf.clear();
			i--;
			continue;
		}

		if(std::isspace(in[i])) {
			if(in[i] == '\n') {
				ret.push_back(Token {TokenType::EOL, std::to_string(in[i])});
				line++;
			}
			continue;
		}

		if(in[i] == '\"') {
			i++;
			while(in[i] != '\"') {
				if(in[i] == '\n') {
					printf("Unterminated string literal at line %d!\n", line);
					exit(1);
				}
				buf.push_back(in[i]);
				i++;
			}
			ret.push_back(Token {TokenType::StringLit, buf});
			buf.clear();
			continue;
		}

		if(in[i] == '\'') {
			buf.push_back(in[++i]);
			if(in[++i] != '\'') {
				printf("Char literal at line %d"
						" cannot contain more than one character!\n", line);
				exit(1);
			}
			ret.push_back(Token {TokenType::CharLit, buf});
			buf.clear();
		}

		if(IsSymbol(in[i], in, i)) {
			if(std::isdigit(in[i+1]))
				continue;
			//ajanro comments to be excluded from lexing
			if(in[i] == '/' && in[i+1] == '/') {
				while(in[i] != '\n') {
					i++;
				}
				i--;
				continue;
			}

			buf.push_back(in[i]);
			if(IsSymbol(in[i+1], in, i)) {
				i++;
				buf.push_back(in[i]);
			}
			ret.push_back(parseBuffer(buf));
			buf.clear();
			
		}

		if(in[i] == '(') {
			ret.push_back(Token{TokenType::OpenParenthesis, "("});
		}
		if(in[i] == ')') {
			ret.push_back(Token {TokenType::CloseParenthesis, ")"});
		}
		if(in[i] == '[') {
			ret.push_back(Token {TokenType::OpenBracket, "["});
		}
		if(in[i] == ']') {
			ret.push_back(Token {TokenType::CloseBracket, "]"});
		}
		if(in[i] == ',') {
			ret.push_back(Token {TokenType::Comma, ","});
		}
	}
	ret.push_back(Token {TokenType::_EOF, "\0"});
	return ret;
}

}
