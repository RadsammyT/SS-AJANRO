#include <cstring>
#include <iostream>
#include <string>

namespace CLI {
	struct flags {
		std::string inFile;
		std::string outFile; // output of translated file
		bool translate; // should the file only be tokenized to output?
		bool compile; // should it also be compiled?
	};

	void processArguments(int argc, char** argv, flags *flags) {
		bool output = false;
		for(int i = 1; i < argc; i++) {
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
