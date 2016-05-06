#!/bin/bash -x

# This shell script logic is isolated into a seperate shell script because it is a problem to call it from m4 files in autoconf.

BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER=`clang --version | grep -Po '(?<=version )[^;]+' | cut -d\. -f1`
# echo "     (script major version number: clang) C++ back-end compiler major version number = $BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER"
echo "$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER"
