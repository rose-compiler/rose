#ifndef	_ROSE_SPECIFIC_COMPLEX_H
#define	_ROSE_SPECIFIC_COMPLEX_H	1

/* Note that /usr/include/complex.h defines _Complex_I as (__extension__ 1.0iF)
   and EDG can not handle the "iF" literal suffix and reports an error.
   This ROSE specific solution allows us to define _Complex_I after it is
   set by /usr/include/complex.h and use a value that is EDG specific.
 */

#include "/usr/include/complex.h"

/* redefine _Complex_I to be what EDG defines as __I__ */
#undef _Complex_I
#define _Complex_I __I__

/* endif for _ROSE_SPECIFIC_SYS_CDEFS_H */
#endif
