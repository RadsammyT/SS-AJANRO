#include <string>
#include <cstring>

#pragma once
namespace CLI {
	struct flags {
		std::string inFile;
		std::string outFile; // output of translated file
		bool translate; // should the translated code be sent to output?
		bool compile; // should it also be compiled?
		bool constIgnore; // ignore to newline after input?
		bool noConstIgnore; // no ignore in translation?
		bool run; // if compiled, run executable after?
		bool help; // show help screen? overrides all flags- just exits when
				   // help output is printed
	};

	void processArguments(int argc, char** argv, flags *flags);
}
