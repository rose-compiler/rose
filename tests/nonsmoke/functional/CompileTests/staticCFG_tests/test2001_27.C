#if 0
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Make boolean syntax work even when boolean is broken. */
#ifdef BOOL_IS_BROKEN
#ifndef BOOL_IS_TYPEDEFED
typedef int bool;
#define BOOL_IS_TYPEDEFED 1
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif
#endif


#include "A++.h"
// #include "ROSE_TRANSFORMATION_SOURCE.h"

int
main() 
   {
     int size = 10;
     doubleArray A(10);

     Range I(1, size-2);
     A(I) = 0;

#if 0
     doubleArray B(size);
     Range I(1, size-2);
#endif

#if 0
     A    = 0.0;
     A(I) = 1.0;  // Initialize interior
     B    = A;

#define COMMENT_OUT_COMMENTS TRUE

     for (int n = 0; n != 1000; n++) 
        {
#if COMMENT_OUT_COMMENTS
       // Commented out to build a more simple test (works with Sage2 but fails with Sage3!)
#endif
       // A(I) = ( B(I-1) + B(I+1) ) * 0.5;   // transform this line only
          A = B + B; // 0.5;   // transform this line only
          B = A;
        }

     printf ("Program Terminated Normally! \n");
#endif

     return 0;
   }



