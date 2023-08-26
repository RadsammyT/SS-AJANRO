#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.hpp"
int main(int argc, char** argv) {
	if(argc == 1) {
		printf("require a file!\n");
	}

	std::fstream file(argv[1], std::ios::in);
	if(!file.is_open()) {
		printf("Unable to open file.\n");
		exit(1);
	}
	std::stringstream stream;
	stream << file.rdbuf();
	
	printf("Tokens of file:\n");
	std::vector<lexer::Token> tokens = lexer::tokenize(stream.str());
	for(auto i: tokens) {
		printf("%d: %s\n", i.type, i.val.c_str());
	}

	file.close();
	
	return 0;
}
