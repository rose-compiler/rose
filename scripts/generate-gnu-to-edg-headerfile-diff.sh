!# /usr/bin/sh

# This is the command used to generate the patch required to fixup the GNU 3.x header files
# to work with EDG.  This patch is only tohandle the problems with using std::string.
# Otherfixups to the GNU header files are implemented in a perl script separately
# (e.g. processing include_only directives etc.).
# This line has to be run from the g++_HEADERS/hdrs1/bits directory and assumes a specific location
# of the ROSE source tree (in ~/ROSE/NEW_ROSE/).  This file is meant mostly to provide some
# documentation for the ROSE developers.

diff -b -u basic_string.h.orig basic_string.h > ~/ROSE/NEW_ROSE/config/gnu-hdrs1-bits-basic_string-h.patchfile
