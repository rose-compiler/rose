// This test demonstrates a bug in quoting of strings within macros passed as -D options.
// This code must be compiled using -DHEADER_FILE=\""stdio.h\"" to demonstrat the bug.

#ifdef HEADER_FILE
// #error "HEADER FILE = HEADER_FILE"
#include HEADER_FILE
#endif
