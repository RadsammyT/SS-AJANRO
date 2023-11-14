#pragma once
#include <fstream>
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
	TypeVoid, // to be used for functions/""""modules"""" only
	TypeInputFile,
	TypeOutputFile,
	OpenFile, // FileIO keywords
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
	OpenBrace,
	CloseBrace,
	Comma,
	Return,
	EOL, // serves as a semicolon (as in the end of a command)
	_EOF,
};

struct Token {
	TokenType type;
	std::string val;
};

bool IsSymbol(char c, std::string in, int i);
Token parseBuffer(std::string buf);
std::vector<Token> tokenize(std::string in);

}
