#line 1227 "configure"
#include "confdefs.h"

// We want to unparse this if:
//    1) the name of the file is "confdef.c", and
//    2) if the source file info for the statements are from filename "configure"
// Then, this is the clue that this is a test file gnerated as part of 
// autoconf and so we want to process it as a special file so that ROSE
// can be used to within the autoconf build system testing.  Since any
// C or C++ file should have a proper suffix, this should not lead to 
// ambiguity in the processing of normal files.  But I do worry about this
// since it seems to be a bit of a hack.  Other build system (e.g. CMake)
// might requires some similar special processing. This is important for 
// using ROSE with some build systems (in some packages). Specifically this
// is important for w3c-libwww (one of the 100K C code application tests.
main(){return(0);}
