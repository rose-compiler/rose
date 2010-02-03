#ifndef __fileoffsetbits
#define __fileoffsetbits
// DQ (3/12/2006): This is included here as specified in the Autoconf manual (using <> instead of "")
// We have also abandoned the ifdef HAVE_CONFIG_H cpp conditional use of rose_config.h as well.
// This is placed here in sage3.h instead of in rose.h because it needs to always be seen even 
// by internal ROSE files that only include sage3.h.

#include <rose_config.h>
// DQ (4/21/2009): Error checking to avoid difficult to debug ODR violations on 32-bit systems.
#if defined(_SYS_STAT_H)
#error "sys/stat.h should not have been included before the _FILE_OFFSET_BITS macro is set! (use rose.h first...)"
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

#endif
