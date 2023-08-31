#include <iostream>
#include <fstream>
#include <sstream>

#include "translator.hpp"
#include "lexer.hpp"
#include "CLI.hpp"
int main(int argc, char** argv) {
	if(argc == 1) {
		printf("require a file!\n");
		exit(1);
	}
	
	CLI::flags flags = {
		.inFile = "",
		.outFile = "./out.cpp",
		.translate = false,
		.compile = false,
	};
	CLI::processArguments(argc, argv, &flags);

	std::fstream file(flags.inFile, std::ios::in);
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
	std::string translation = translator::translate(tokens, flags.outFile);
	if(flags.translate) {
		printf("Translation:\n%s\n", translation.c_str());
		return 1;
	}
	std::fstream fileOut(flags.outFile, std::ios::out);
	fileOut << translation;
	fileOut.close();
	return 0;
}
