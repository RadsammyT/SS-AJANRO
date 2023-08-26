#include <cctype>
#include <cwctype>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include <regex>

#pragma once
namespace lexer {

enum class TokenType {
	StartProgram = 0, // MUST be the starting token
	EndProgram, // MUST be the final token
	StartMain,
	EndMain,
	Output,
	Input,
	Identifier, // Default for any tokens that aren't "reserved"
	ConstVar,
	TypeString,
	TypeInteger,
	TypeFloating,
	TypeBool,
	TypeChar,
	StringLit,
	IntegerLit,
	FloatingLit,
	BoolLit,
	CharLit,
	SetSymbol, // =
	OperatorMulti,
	OperatorDivide,
	OperatorAdd,
	OperatorSub,
	EOL, // serves as a semicolon (as in the end of a command)
	_EOF,
};

struct Token {
	TokenType type;
	std::string val;
};

bool IsSymbol(char c) {
	if(c == '*' ||
		c == '/' ||
		c == '+' ||
		c == '-' ||
		c == '=' ||
		c == '_') {
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
	if(buf == "endmain") {
		return Token {TokenType::EndMain, buf};
	}
	if(buf == "output") {
		return Token {TokenType::Output, buf};
	}

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

	if(buf == "=") {
		return Token{TokenType::SetSymbol, buf};
	}

	//literal parsing
	// 32.141
	// 0.141
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

	if(std::regex_match(buf.c_str(), m, std::regex("[0-9]*"))) {
		return Token {TokenType::IntegerLit, buf};
	}

	if(IsSymbol(buf[0])) {
		switch(buf[0]) {
			case '*':
				return Token{TokenType::OperatorMulti, buf};
			case '/':
				return Token{TokenType::OperatorDivide, buf};
			case '+':
				return Token{TokenType::OperatorAdd, buf};
			case '-':
				return Token{TokenType::OperatorSub, buf};
		}
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

		if(IsSymbol(in[i])) {
			
			//ajanro comments to be excluded from lexing
			if(in[i] == '/' && in[i+1] == '/') {
				while(in[i] != '\n') {
					i++;
				}
				i--;
				continue;
			}

			buf.push_back(in[i]);
			ret.push_back(parseBuffer(buf));
			buf.clear();
			
		}
	}
	ret.push_back(Token {TokenType::_EOF, "\0"});
	return ret;
}

}
