#!/bin/sh
printf "#define GIT_COMMIT \"%s\"\n#pragma once" $(git describe --always --dirty --match 'NOTATAG') > src/gitVersion.hpp
