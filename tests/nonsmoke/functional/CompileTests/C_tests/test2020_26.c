// RC-68: (need to be split between header and source files.
// Note that RC-70 is another (single file) example of this.


// Since the macro is defined in the header file, we can't detect it unless
// we collected all of the #defines from all possible header files, which is 
// prohibatively expensive.  An alternative would be to support an option to
// generate a *.i file (or a *.ii file, for C++) which would put all of the
// include files into the generated file (unparsing the full translation unit),
// and as a result supress the expansion of all macros.


// #include "rc-68-1.h"
#include "test2020_26.h"

X * foo (void)
   {
     X x;

  // "bar" appears to be a recursive macro (might have to implement option to 
  // generate *.i and *.ii intermediate files to avoid re-expansion of macros.
     return bar(&x);
   }

