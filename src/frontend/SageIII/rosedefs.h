#ifndef __rosedefs
#define __rosedefs

#include <Rose/BinaryAnalysis/Address.h>

#include "stdio.h"
#include <cassert>
#include <cstdio>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <sstream>

// DQ (9/25/2007): Need to move this to here so that all of ROSE will see it.
#define Rose_STL_Container std::vector

// DQ (2/5/2010): include stdint.h always.
// #ifdef _MSC_VER
// #include "stdint.h"
// //typedef __int64 uint64_t;
// #endif

#include "stdint.h"

// DQ (2/10/2010): Added assert.h (not clear where else it is included).
#include "assert.h"

#include "roseInternal.h"


//#include "rose_attributes_list.h"

// DQ (10/14/2010): We don't want to include this into our header file system
// since then users will see the defined macros in our autoconf generated 
// config.h (which we generate as rose_config.h to avoid filename conflicts).
// This fixes the problem that causes macro names to conflict (e.g. PACKAGE_BUGREPORT).
// #include "rose_config.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "virtualCFG.h" 

// DQ (10/29/2010): This must be included as a header file since the function 
// declarations in SgAsmStatement require it in the generated Cxx_Grammar.h file.
#include "virtualBinCFG.h" 

#include "staticCFG.h"
#else

// DQ (11/12/2011): We need a declaration that can be used in Cxx_Grammar.h
class VirtualCFG
   {
     public:
          typedef int CFGNode;
          typedef int CFGEdge;
   };

class VirtualBinCFG
   {
     public:
          typedef int AuxiliaryInformation;
          typedef int CFGNode;
          typedef int CFGEdge;
   };

#endif

#endif
