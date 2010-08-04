// Example program showing the use of extern "C" and testing
// the unparsing of this of this language construct.

/* There are several cases to worry about.
1. use of extern "C" to surround the inclusion of a header file
     extern "C"
        {
     #include "test2001_15.h"
        }

2. use of extern "C" to declare a variable
     extern "C" int x;

3. combination of the two


Solutions:
   1. We can't just modify each header file since we would have 
      to rewrite ALL header files (system header files included 
      and this seems to be a bad idea.
   2. It is hard to unparse case 1 (above) since the use of 
      extern "C" is not seen in the main source file.

 */

// This should unparse correctly (many other cases will not)
extern "C"   
   {
#include "test2001_18A.h"
   }

// This should unparse correctly (many other cases will not)
extern "C" {
#include "test2001_18B.h"
   }

int
main ()
   {
     return 0;
   }


