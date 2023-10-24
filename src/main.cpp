#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "gitVersion.hpp"
#include "lexer.hpp"
#include "CLI.hpp"
#include "utils.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;

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
				"Version: " GIT_VERSION "\n"
				"Usage: ajanro (file) [option(s)]\n"
				"By default, this will output the translated code to ./out.cpp\n"
				"or by what is specified in the -o flag.\n"
				"\n"
				"Flags:\n"
				"-t - Output translated C++ code to console output, then exit\n"
				"     NOTE: This will not save the C++ code to a file, however.\n"
				"-c - Compile the translated C++ code. You can use the -o flag to\n"
				"     determine the location of the executable.\n"
				"     By default the executable will be placed in the ./bin dir\n"
				"     for organization with the executable name being the\n"
				"     identifier declared after the \'startprogram\' token.\n"
				"-r - If compile flag is set, run the compiled executable\n"
				"     after a successful compilation. \n"
				"-o - Declare the output of the translated C++ code.\n"
				"     If compiling, this will instead declare the output \n"
				"     of the executable.\n"
				"     The argument after the flag will be the output file.\n"
				"-i - Always ignore 'cin' to newline after each console input.\n"
				"-ni - Never ignore 'cin' to newline after each console input.\n"
				);
		return 0;
	}

	std::fstream file(flags.inFile, std::ios::in);
	if(!file.is_open()) {
		printf("Unable to open file.\n");
		return 1;
	}
	std::stringstream stream;
	stream << file.rdbuf();
	
	std::vector<lexer::Token> tokens = lexer::tokenize(stream.str());
#ifdef DEBUG
	printf("Tokens of file:\n");
	for(auto i: tokens) {
		printf("%d: %s\n", i.type, i.val.c_str());
	}
	printf("----------\n");
#endif

	file.close();
	std::string programName;
	std::string out = "bin/";
	std::string translation = translator::translate(tokens, flags.outFile,
			programName, flags);
	if(flags.translate) {

		printf("Translation:\n%s\n", translation.c_str());
		if(!flags.compile)
			return 0;
	}
	if(flags.compile) {
		char buf[2048] = {};
		if(flags.outFile != "./out.cpp")
			sprintf(buf, "c++ -o %s -std=c++20 -x c++ -",
					flags.outFile.c_str());
		else {
			if(!fs::exists("bin")) {
				fs::create_directory("bin");
			}
			out += programName;
#if defined(_WIN32)
			programName += ".exe";
			sprintf(buf, "g++ -o %s -std=c++20 -x c++ -",
					out.c_str());
#else
			sprintf(buf, "g++ -o %s -std=c++20 -x c++ -",
					out.c_str());
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
		int pclose_ret = pclose(proc);
		if(pclose_ret == -1){
			perror("Unable to close pipe.");
		}
		if(pclose_ret != 0) {
			perror("Pipe closed in an error!\n");
			printf("Return code of closed pipe = %d\n", pclose_ret);
		}
		if(flags.run && pclose_ret == 0) {
			std::string cmd = "./";
			cmd += out;
#if defined(_WIN32)
			// cuz running via system() in windows uses command prompt for execution
			// meaning '\' instead of '/'.
			while(true) {
				if(cmd.find("/") != std::string::npos) {
					cmd.replace(cmd.find("/"), 1, "\\");
				} else {
					break;
				}
			}
#endif
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
