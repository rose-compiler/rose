/*
   This demonstrates a error in the unparsing of #include directives relative to the "{" and "}"
   of a block, class definition, enum, namespace, etc.  The level of detail was not previously 
   implemented in ROSE.  It was not always required, but since I was fixing test2005_26.C and
   that test code made the problem more visible, I decided to fix it correctly so that we 
   generate better quality code now and we avoid potential pathological problems related to
   the position of commented relative to the closing brace of a block, enum, namespace or
   class definition. I believe that all possible cases are not handled.
 */

#include "test2005_26.h"

void foo()
   {
     #include "test2005_26.h"
   }

void foobar()
   {
     #include "test2005_26.h"
     if (true)
        {
          #include "test2005_26.h"
        }
       else
        {
          #include "test2005_26.h"
        }

        {
          #include "test2005_26.h"
        }

     enum enumType
        {
           unknownValue = 0
        /* Comment inside of enum! */
        };

     for (int i=0; i < 10; i++)
        {
          #include "test2005_26.h"
        }

     switch (true)
        {
       /* Comment in switch (top) */
          case false:
             {
            /* comment in case */
               #include "test2005_26.h"
             }
       /* comment in between case and default */
          default:
             {
            /* comment in default case */
               #include "test2005_26.h"
             }

       /* Comment in switch (bottom) */
        }
   }

namespace X1
   {
     #include "test2005_26.h"
   }

namespace X2
   {
     #include "test2005_26.h"
     namespace XX
        {
          #include "test2005_26.h"
        }
   }

namespace X3
   {
     #include "test2005_26.h"
     class XX
        {
          #include "test2005_26.h"
        };

  // Example of "}" in a namespace (test that we correctly jump over it when searching for the ending brace associated with the namespace).
     enum numbers { zero,one,two };
     enum more_numbers { three,four,five } more_numbers_variable;
   }

class Y
   {
     #include "test2005_26.h"
   };



// DQ (5/11/2016): GNU 6.1 does not allow a global variable to be named "main".
// int main;
