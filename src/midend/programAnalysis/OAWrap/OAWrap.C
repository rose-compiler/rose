// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 18,7 MB to 9,5MB

#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#ifndef CXX_IS_ROSE_CODE_GENERATION
#include <OAWrap.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

namespace OpenAnalysis {
#include <OpenAnalysis/Utils/BaseGraph.C>
#include <OpenAnalysis/Utils/DGraph.C>
#include <OpenAnalysis/CFG/CFG.C>
};
#endif
