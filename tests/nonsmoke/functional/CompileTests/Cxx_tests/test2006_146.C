// Tensilica's Xtensa compiler has a confliction function prototype declaration
#ifdef USING_XTENSA_BACKEND_COMPILER

#include <stdio.h>

// This will cause C compilation to fail, but I think it should fail.
// extern int dprintf (int __fd, __const char *__restrict __fmt, ...) __attribute__ ((__format__ (__printf__, 2, 3)));

// DQ (3/21/2009): Cygwin defines this function differently (fails in virtualCFG tests).
#if !defined(__CYGWIN__)

#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                     (__GNUC_MINOR__ == 4 && \
                      __GNUC_PATCHLEVEL__ >= 1)))

// Liao 5/4/2010
// Ubuntu 9.1 GCC 4.4.1 has a conflicting function prototype declaration
// 
int dprintf(int, const char *restrict, ...);

#else
// DQ (3/29/2010):There is a bug report that this function does not exist on: Ubuntu 9.1 gcc version 4.4.1 (Ubuntu 4.4.1-4ubuntu9)
void dprintf(int line, long level, char *prefix, ...);
#endif
#endif

#endif
