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
typedef uint64_t rose_addr_t; /* address and size (file and memory) */
#define ROSE_DEPRECATED_FUNCTION 
#define __builtin_constant_p (exp) (0)

#include "roseInternal.h"


//#include "rose_attributes_list.h"
#include "rose_config.h"
#include "virtualCFG.h" 
#include "virtualBinCFG.h" 


#endif
