#!/bin/bash

# This script runs on the output of "make" within ROSE and 
# filters the output of make and then computes the number of
# files that were compiled in each directory as a measure of
# the amount of parallelism in the build step for ROSE.  

echo ""
echo "************************************************************"
echo "***** Measurement of Parallelism in ROSE Build Process *****"
echo "************************************************************"
echo ""
echo "The goal would be to have a few directories with a lot of files"
echo "so that a parallel build (using -j24) would be as efficient as "
echo "possible. However this is in conflict with good software design "
echo "so some compromise is required."
echo ""

# Note: What we really want is only the lines with "COMPILE" and "Making all in ".
# Filter output that does not represent separate compile lines (trash).
# grep -v 'Entering directory' example_make.out | grep -v 'Leaving directory' | grep -v LINK | grep -v "mkdir: cannot create directory" | grep -v "gcc" | grep -v "warning" | grep -v "In function" | grep -v "Nothing to be done for" | grep -v "In member function" | grep -v "gmake" | grep -v "In constructor" | grep -v "DONE" | grep -v "Building" > filtered_example_make.out

# Just extract the specific lines that we want.
# egrep "Making all in |COMPILE" example_make.out > filtered_example_make.out
egrep "Making all in |COMPILE" $1 > filtered_example_make.out

# Add a marker at the end so that the last directory will be included in the measure.
echo "Making all in END" >> filtered_example_make.out

# Since we have a number of directories names "src" (also from other projects included withn ROSE) 
# we rename each src directory in order of there being visited in the directory structure by make 
# (a fixed order dependent on the SUBDIRS automake variable.) Each entry is replace in only
# the first occurance in th file (sed syntax I found at: 
# http://stackoverflow.com/questions/148451/how-to-use-sed-to-replace-only-the-first-occurrence-in-a-file)
sed -i "0,/Making all in src/s//Making all in (src\/ROSETTA's) src/" filtered_example_make.out
sed -i "0,/Making all in src/s//Making all in (src\/frontend\/CxxFrontend\/EDG\/EDG_3.3's) src/" filtered_example_make.out
sed -i "0,/Making all in src/s//Making all in (src\/frontend\/CxxFrontend\/EDG\/EDG_4.0's) src/" filtered_example_make.out
sed -i "0,/Making all in src/s//Making all in (src\/3rdPartyLibraries\/libharu-x.x.x's) src/" filtered_example_make.out
sed -i "0,/Making all in src/s//Making all in (src\/roseExtensions\/roseHPCToolkit's) src/" filtered_example_make.out

# Run grep with -n (line numbering) to generate input for associated perl script.
grep -n "Making all in " filtered_example_make.out | checkMakeParallelism.pl
