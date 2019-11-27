#!/bin/bash

# This shell script logic is isolated into a separate shell script because it is a problem to call it from m4 files in autoconf.

echo `clang --version | grep -o 'version[\ ]\([1][0-1]\|[7-9]\)\.[0-9]*\.[0-9]*' | grep -o '\([1][0-1]\|[7-9]\)\.[0-9]*\.[0-9]*' | cut -d\. -f2`
