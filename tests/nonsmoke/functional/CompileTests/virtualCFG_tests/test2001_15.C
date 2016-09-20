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

#if 0
extern "C" int a;
int b;

extern "C"   
   {
#include "test2001_15.h"

// This is legal code to place an extern "C" (with braces) inside of another extern "C" with braces.
extern "C" { /* test parser */ int c1; }

#define TEST_ONE_LINE_EXTERN_C_WITH_BRACES
extern "C" { int c2; }

#define TEST_MULTI_LINE_EXTERN_C_WITH_BRACES
extern "C" {  
     int d; 
   }
extern "C" int e;
int f;
   }

#define TEST_ONE_LINE_EXTERN_C_WITH_BRACES
extern "C" { int z; }


#undef USING_Cxx_COMPILER
#ifndef USING_Cxx_COMPILER
extern "C"
   {
#endif

   int g;

#ifndef USING_Cxx_COMPILER
   }
#endif

#endif

// This should unparse correctly (many other case will not)
extern "C"   
   {
#include "test2001_15.h"
   }

int
main ()
   {
     return 0;
   }

















