#!/bin/bash
#it has to be ran in the makefile that premake generates
#otherwise gitVersion.hpp will get updated in the wrong place
git_version=$(git describe --abbrev=7 --always)
echo "#define GIT_VERSION \"$git_version\"" > ../src/gitVersion.hpp
