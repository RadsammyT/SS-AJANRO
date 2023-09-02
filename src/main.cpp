#include <cstdio>
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
	
	std::vector<lexer::Token> tokens = lexer::tokenize(stream.str());
#ifdef DEBUG
	printf("Tokens of file:\n");
	for(auto i: tokens) {
		printf("%d: %s\n", i.type, i.val.c_str());
	}
#endif

	file.close();
	std::string translation = translator::translate(tokens, flags.outFile);
	if(flags.translate) {
		printf("Translation:\n%s\n", translation.c_str());
		if(!flags.compile)
			return 0;
	}
	if(flags.compile) {
		std::FILE *proc = popen("g++ -std=c++20 -x c++ -", "w");
		if(!proc) {
			perror("Unable to open pipe.");
		}
		fwrite(translation.c_str(),
				sizeof(char),
				strlen(translation.c_str()),
				proc);
		if(ferror(proc)) {
			perror("Unable to write to pipe.");
		}
		if(pclose(proc) == -1) {
			perror("Unable to close pipe.");
		}
		return 0;
	}
	std::fstream fileOut(flags.outFile, std::ios::out);
	fileOut << translation;
	fileOut.close();
	return 0;
}
