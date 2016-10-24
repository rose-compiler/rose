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

namespace X
   {
     #include "test2005_26.h"
   }

class Y
   {
     #include "test2005_26.h"
   };




int main;
