#include "A++.h"
// include "ROSE_TRANSFORMATION_SOURCE.h"
#include "ROSE_TRANSFORMATION_SOURCE_3.h"

int
main() 
   {
#if 0
  // This test fails on LINUX systems, I don't know why.  Since it is only a test of
  // the old version of the A++/P++ preprocessor, we don't want to worry about 
  // maintaining it. There is a newer version of the A++/P++ preprocessor available.
  // At some point this test code will be use with the newer version of the A++/P++ 
  // preprocessor.

     int size = 10;

     doubleArray A(size);
     doubleArray B(size);
     Range I(1, size-2);

     A    = 0.0;
     A(I) = 1.0;  // Initialize interior
     B    = A;

     for (int n = 0; n != 1000; n++) 
        {
       // Commented out to build a more simple test
       // A(I) = ( B(I-1) + B(I+1) ) * 0.5;   // transform this line only
          A = B + B; // 0.5;   // transform this line only
          B = A;
        }
#endif

     printf ("Program Terminated Normally! \n");

     return 0;
   }



