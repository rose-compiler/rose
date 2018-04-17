#include "A++.h"
// include "ROSE_TRANSFORMATION_SOURCE.h"
// include "ROSE_TRANSFORMATION_SOURCE_3.h"
#include "ROSE_TRANSFORMATION_SOURCE_3.h"

int
main() 
   {
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

     printf ("Program Terminated Normally! \n");

     return 0;
   }



