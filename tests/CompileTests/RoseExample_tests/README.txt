This directory tests the compilation of ROSE using ROSE.

Some of the test codes are constructed using g++ -E on the 
command line used to compile ROSE.  This generates a single 
file which is easier to process (simpler, and shorter, command line).

Generating the command line requires the use of:
   g++ -E -DUSE_ROSE -D_GLIBCXX__PTHREADS=1 

The -E option calls cpp and generates output to stdout.
The -DUSE_ROSE option defines the macro USE_ROSE, which limits a few 
parts of ROSE that don't appear to compile with EDG and which need to 
be investigated further (for example rangemap.h is not accepted as 
standard C++ independent of anything specific to ROSE).  I expect that 
there may be some code that GNU accepts that EDG does not accept.  
However, this appears to be a problem only for C++ code output from 
CPP, and is not present in the compilation of the ROSE more directly 
using ROSE.  Not sure what the issues is here.

This directory collects a number of test codes that are specific to 
compiling ROSE using ROSE.

