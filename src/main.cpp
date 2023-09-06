#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "gitVersion.hpp"
#include "translator.hpp"
#include "lexer.hpp"
#include "CLI.hpp"
int main(int argc, char** argv) {
	CLI::flags flags = {
		.inFile = "",
		.outFile = "./out.cpp",
		.translate = false,
		.compile = false,
	};
	CLI::processArguments(argc, argv, &flags);

	if(flags.help || argc == 1) {
		printf("SS-AJANRO - The AJANRO to C++ translator\n"
				"Version: " GIT_COMMIT "\n"
				"Usage: ajanro (file) [option(s)]\n"
				"By default, this will output the translated code to ./out.cpp\n"
				"or by what is specified in the -o flag.\n"
				"\n"
				"Flags:\n"
				"-t - Output translated C++ code to console output, then exit\n"
				"     NOTE: This will not save the C++ code to a file, however.\n"
				"-c - Compile the translated C++ code. You can use the -o flag to\n"
				"     determine the location of the executable.\n"
				"-o - Declare the output of the translated C++ code.\n"
				"     If compiling, this will instead declare the output \n"
				"     of the executable.\n"
				"     The argument after the flag will be the output file.\n"
				);
		return 0;
	}

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
	std::string programName;
	std::string translation = translator::translate(tokens, flags.outFile,
			programName);
	if(flags.translate) {

		printf("Translation:\n%s\n", translation.c_str());
		if(!flags.compile)
			return 0;
	}
	if(flags.compile) {
		char buf[2048] = {};
		if(flags.outFile != "./out.cpp")
			sprintf(buf, "g++ -o %s -std=c++20 -x c++ -",
					flags.outFile.c_str());
		else {
#if defined(_WIN32)
			programName += ".exe";
			sprintf(buf, "g++ -o %s -std=c++20 -x c++ -",
					programName.c_str());
#else
			sprintf(buf, "g++ -o %s -std=c++20 -x c++ -",
					programName.c_str());
#endif

		}
		std::FILE *proc = popen(buf, "w");
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
		if(flags.run) {
			std::string cmd = "./";
			if(flags.outFile == "./out.cpp") {
				cmd += programName;
			} else 
				cmd += flags.outFile;
			printf("Running %s...\n", cmd.c_str());
			system(cmd.c_str());
		}
		return 0;
	}
	std::fstream fileOut(flags.outFile, std::ios::out);
	fileOut << translation;
	fileOut.close();
	return 0;
}
