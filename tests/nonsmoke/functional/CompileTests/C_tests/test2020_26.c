// RC-68: (need to be split between header and source files.
// Note that RC-70 is another (single file) example of this.

// #include "rc-68-1.h"
#include "test2020_26.h"

X * foo (void)
   {
     X x;

  // "bar" appears to be a recursive macro (might have to implement option to 
  // generate *.i and *.ii intermediate files to avoid re-expansion of macros.
     return bar(&x);
   }

