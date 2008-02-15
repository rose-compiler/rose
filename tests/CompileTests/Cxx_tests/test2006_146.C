#include <stdio.h>

// This will cause C compilation to fail, but I think it should fail.
// extern int dprintf (int __fd, __const char *__restrict __fmt, ...) __attribute__ ((__format__ (__printf__, 2, 3)));

void dprintf(int line, long level, char *prefix, ...);
