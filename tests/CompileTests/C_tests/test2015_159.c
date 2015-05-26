// This is an example of a recursive macro, it is detect when it is in the input source file.
// to demonstrate the bug it has to be in a header file.
// #define always_inline __attribute__ (( always_inline )) __inline__
// #define inline always_inline

#include "test2015_159.h"

static void foobar();

static inline void foobar()
   {
   }

void foo()
   {
     foobar();
   }

