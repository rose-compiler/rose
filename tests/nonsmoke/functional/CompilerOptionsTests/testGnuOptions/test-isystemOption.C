
// Put a variable into a header file and force 
// it to be included via the GNU -isystem option.
#include "testOptions.h"

int
main ()
   {
  // Note that "x" is defined in "testOptions.h" which
  // is explicitly included above but for which the include 
  // path is specified using the GNU "-isystem" option.
     x = 0;

     return 0;
   }
