
// Put a variable into a header file and force 
// it to be included via the GNU -include option.
// #include "testOptions.h"

int
main ()
   {
  // Note that "x" is defined in "testOptions.h" which
  // is preincluded using the GNU "-include" option.
     x = 0;

     return 0;
   }
