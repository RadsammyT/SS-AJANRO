AJANRO - A Pseudocode programming language designed by Professor Madras in collaboration
with Tiffany, Andrew, and Robert.

# SS-AJANRO - A (crude) translator for AJANRO written in C++ out of spite. 
			This translator makes multiple assumptions regarding I/O, being:
				- That the input comes from the console.
				- That the output goes to the console.
				- While there are features for File I/O in AJANRO, they won't be implemented
					for now.

            This translator also makes the following changes to the AJANRO language:
                - if statements REQUIRE a 'then' keyword at the end of a boolean expression. They are no longer optional
                    -ex: if true then
                            output "Hello world!"
                        endif
                - Same applies to while loops, which now REQUIRES a 'do' keyword at the end of a boolean expression.
                - Making equal comparisons in boolean expressions (a = b) now requres a `==` operator to make the comparison. (a == b)
                - 


# How to Build
	Compilation requires premake5 (premake.github.io) for build files.
	Check the premake5 documentation here (https://premake.github.io/docs/using-premake)
	to see a list of all compatable build systems.

