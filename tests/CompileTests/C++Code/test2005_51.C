// Test use of assert macro (debugging use of new header files)

#define TRUE true
#define FALSE false

// #define assert(b)

// assert.h will undefine any previous "assert" macro
#include<assert.h>

int main()
   {
     bool x = FALSE;
     assert(x == TRUE);

     return 0;
   }
