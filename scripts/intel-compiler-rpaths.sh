#!/bin/bash

# Prints a single line of colon-separated directory names that must be searched to find shared
# libraries when running a program created by an Intel compiler.  The Intel compilers use shell
# scripts to modify the environment, and one of the modifications is that these extra directories
# are added to LD_LIBRARY_PATH.
#
# When using Intel compilers under RMC-Spock, the compilers are wrapped so that the user doesn't
# need to concern himself with setting up the environment each time he wants to invoke a compiler.
# This approach also makes it possible to invoke different Intel compilers from the shell in a
# manner similar to how one invokes different GNU or LLVM compilers, namely by using different
# names: e.g., g++-4.8.4 vs. g++-5.3.0, or icpc-15.0.4 or icpc-16.0.3.
#
# This script assumes that appropriate links have been set up so that "c++" is the name of
# the currently selected C++ compiler. This is done by "rmc" (specifically "spock-shell") when
# the compiler is chosen.
#
# If the current c++ compiler is not an Intel compiler then no output is produced.

[ "$(c++ --spock-triplet 2>/dev/null |cut -d: -f1)" = "intel" ] && exec c++ --spock-so-paths
exit 0
