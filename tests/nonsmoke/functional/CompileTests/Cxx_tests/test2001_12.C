// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include "A++.h"

int
main ()
   {
  // Test 1
     doubleArray* arrayPtr1 = new doubleArray(42);

  // Test 2
     doubleArray* arrayPtr2 = new doubleArray();

  // Test 3
     char* copyString = new char[1];

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

