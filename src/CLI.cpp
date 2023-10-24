#include <cstring>
#include <iostream>
#include <string>

#include "CLI.hpp"

namespace CLI {

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

			if(strcmp(argv[i], "-i") == 0) {
				flags->constIgnore = true;
				continue;
			}

			if(!strcmp(argv[i], "-ni")) {
				flags->noConstIgnore = true;
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
