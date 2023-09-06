#include <cstring>
#include <iostream>
#include <string>

#pragma once
namespace CLI {
	struct flags {
		std::string inFile;
		std::string outFile; // output of translated file
		bool translate; // should the translated code be sent to output?
		bool compile; // should it also be compiled?
		bool run; // if compiled, run executable after?
		bool help; // show help screen? overrides all flags- just exits when
				   // help output is printed
	};

	void processArguments(int argc, char** argv, flags *flags) {
		bool output = false;
		for(int i = 1; i < argc; i++) {
			if(argc == 1)
				break;
			if(strcmp(argv[i], "-o") == 0) {
				output = true;
				continue;
			}
			if(strcmp(argv[i], "-t") == 0) {
				flags->translate = true;
				continue;
			}

			if(strcmp(argv[i], "-c") == 0) {
				flags->compile = true;
				continue;
			}

			if(strcmp(argv[i], "-r") == 0) {
				flags->run= true;
				continue;
			}

			if(strcmp(argv[i], "--version") == 0) {
				flags->help = true;
				continue;
			}

			if(strcmp(argv[i], "-h") == 0 ||
					strcmp(argv[i], "--help") == 0) {
				flags->help = true;
			}

			if(output) {
				flags->outFile = argv[i];
				continue;
			} else {
				flags->inFile = argv[i];
				continue;
			}
		}
	}
}
