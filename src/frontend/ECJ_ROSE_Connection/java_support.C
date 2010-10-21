#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// This file does not exist preently, but will in the future...
// #include "java_support.h"

SgSourceFile* OpenFortranParser_globalFilePointer = NULL;
