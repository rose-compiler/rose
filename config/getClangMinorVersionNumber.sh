#!/bin/bash

# This shell script logic is isolated into a separate shell script because it is a problem to call it from m4 files in autoconf.

clang_minor=$(clang --version | sed -n 's/.*version[[:space:]]*[0-9][0-9]*\.\([0-9][0-9]*\)\..*/\1/p' | head -1)

echo "$clang_minor"
