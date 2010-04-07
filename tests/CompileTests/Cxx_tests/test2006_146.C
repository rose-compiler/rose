#include <stdio.h>

// This will cause C compilation to fail, but I think it should fail.
// extern int dprintf (int __fd, __const char *__restrict __fmt, ...) __attribute__ ((__format__ (__printf__, 2, 3)));

// DQ (3/21/2009): Cygwin defines this function differently (fails in virtualCFG tests).
#if !defined(__CYGWIN__)

// DQ (3/29/2010):There is a bug report that this function does not exist on: Ubuntu 9.1 gcc version 4.4.1 (Ubuntu 4.4.1-4ubuntu9)
void dprintf(int line, long level, char *prefix, ...);

#endif
