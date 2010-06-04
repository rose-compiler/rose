#ifndef __rosedefs
#define __rosedefs


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
typedef uint64_t rose_addr_t; /* address and size (file and memory) */
#define ROSE_DEPRECATED_FUNCTION 

// DQ (2/10/2010): Added assert.h (not clear where else it is included).
#include "assert.h"

#include "roseInternal.h"


//#include "rose_attributes_list.h"
#include "rose_config.h"
#include "virtualCFG.h" 
#include "virtualBinCFG.h" 
#include "staticCFG.h"


#endif
