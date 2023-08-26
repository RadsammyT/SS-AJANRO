AJANRO - A Pseudocode programming language designed by Professor Madras in collaboration
with Tiffany, Andrew, and Robert.

SS-AJANRO - A (crude) interpreter for AJANRO written in C++ out of spite. 
			This interpreter makes multiple assumptions regarding I/O, being:
				- That the input comes from the console.
				- That the output goes to the console.
				- While there are features for File I/O in AJANRO, they won't be implemented
					for now.

How to Build
	Compilation requires premake5 (premake.github.io) for build files.
	Check the premake5 documentation here (https://premake.github.io/docs/using-premake)
	to see a list of all compatable build systems.

keywords:
	startprogram [PROGRAM_NAME] : declare start of program PROGRAM_NAME

	endprogram : declare end of program PROGRAM_NAME
