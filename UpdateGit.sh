#!/bin/bash
git_version=$(git describe --abbrev=7 --tags --always)
echo "#define GIT_VERSION \"$git_version\"" > ../src/gitVersion.hpp
