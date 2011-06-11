#ifndef	_ROSE_SPECIFIC_SYS_CDEFS_H
#define	_ROSE_SPECIFIC_SYS_CDEFS_H	1

/* This file is required to avoid compiling VLA
   specific code in Red Hat Linus 7.3
   Avoids problem error:
   "/usr/include/_G_config.h", line 50: error: type containing an unknown-size array is not allowed
      struct __gconv_info __cd; 
                          ^

   Change this path if your C compiler include files are not in the normal place.
   This should be the standard location for this file, but it will have to be 
   modified if it is in a different place.
 */

#ifdef __attribute_malloc__
   #undef __attribute_malloc__
#endif

#include "/usr/include/sys/cdefs.h"

/* Define __flexarr to not require use of Variable Length Array (VLA) feature. */
#undef __flexarr
#define __flexarr [1]

/* endif for _ROSE_SPECIFIC_SYS_CDEFS_H */
#endif

