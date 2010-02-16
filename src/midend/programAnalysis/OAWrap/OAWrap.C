// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 18,7 MB to 9,5MB

#include "sage3basic.h"

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
