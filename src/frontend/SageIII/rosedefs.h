#ifndef __rosedefs
#define __rosedefs


// DQ (3/12/2006): We want to remove config.h from being placed in every source file
#include <rose_config.h>

// DQ (4/21/2009): Error checking to avoid difficult to debug ODR violations on 32-bit systems.
#if defined(_SYS_STAT_H)
#warning "sys/stat.h should not have been included before the _FILE_OFFSET_BITS macro is set! (use rose.h first...)"
#endif

// DQ (4/21/2009): This must be included before rose_paths.h since that 
// header includes the STL string header which will include sys/stat.h first.
// Force 64-bit file offsets in struct stat
#define _FILE_OFFSET_BITS 64

// DQ (4/21/2009): This must be set before sys/stat.h is included by any other header file.
// Use of _FILE_OFFSET_BITS macro is required on 32-bit systems to controling size of "struct stat"
#if !(defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#error "The _FILE_OFFSET_BITS macro should be set before any sys/stat.h is included by any other header file!"
#endif



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
#ifdef _MSC_VER
#include "stdint.h"
//typedef __int64 uint64_t;
#endif
typedef uint64_t rose_addr_t; /* address and size (file and memory) */
#define ROSE_DEPRECATED_FUNCTION 


#include "roseInternal.h"


//#include "rose_attributes_list.h"
//#include "rose_config.h"
#include "virtualCFG.h" 
#include "virtualBinCFG.h" 


#endif
