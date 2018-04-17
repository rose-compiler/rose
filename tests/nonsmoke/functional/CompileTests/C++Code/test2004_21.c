/* The following test code is all ANSI C (showing old-style and new-style function 
definitions (compiles using gcc -std=c99, for example, but does not compile with g++).
The old-style C represented in the function definitions finally 
works in ROSE. Note that the filename suffix must be ".c" and not ".C"
to be compiled with gcc using old-style syntax (not the case for EDG).

   These tests were prepared as part of the new old-style function declaration 
handling in ROSE (required to compile older laboratory application written in C).
*/

/* Change 0 to 1 to demonstrate errors in compiling using GNU gcc and/or EDG (in C mode) */
#if 0
#define DEMO_ERROR_CASE
#endif

/* Note that "float" in place of "double", in foo7, is permitted by GNU (compiles) but is 
   ignored (as I understand it) since all floats are pushed onto the stack as a double 
   under GNU (or is it C (old-style only)?). foo7 case is an error with EDG (but 
   works with GNU gcc).  It seems impossible to pass a float using EDG as a C compiler
   foo6 is an error with GNU gcc, and also with EDG.
 */
#ifdef DEMO_ERROR_CASE
/* Example compiles fine with GNU gcc:
   Error in EDG (ROSE): declaration is incompatible with "int foo7(int, float, int *)"
   This is only an error for the old-style function declarations (passing a float
   with the new-style compiles fine (though it might still be ignored internally)).
*/
int foo7 (int x, float y, int* z);
#else
int foo7 (int x, double y, int* z);
#endif

#if 1
int foo7 (x,y,z)
   int x;
   float y;
   int* z;
   {
     x = 42 + y + *z;
     return x;
   }
#endif

#ifdef DEMO_ERROR_CASE
/* Error in GNU gcc: argument `y' doesn't match prototype */
int foo6 (int x, float y, int* z);
#else
int foo6 (int x, double y, int* z);
#endif

#if 1
int foo6(x,y,z)
   int x;
   double y;
   int* z;
   {
     x = 42 + y + *z;
     return x;
   }
#endif

/* Demonstrate prototype with consistant but different 
   declaration than associated function definition 
 */
int foo1 (int x, double y, int* z);
int foo1(x,y,z)
   int x;
   float y;
   int* z;
   {
     x = 42 + y + *z;
     return x;
   }

/* New style ANSI function prototype and new style ANSI function definition */
int foo2 (int x);
int foo2(int x)
   {
     x = 42;
     return x;
   }

/* Old style ANSI function prototype and new style ANSI function definition */
int foo3();
int foo3(int x)
   {
     x = 42;
     return x;
   }

/* Old style ANSI function prototype and old style ANSI function definition */
int foo4();
int foo4(x)
   int x;
   {
     x = 42;
     return x;
   }

/* Use of double in prototype. */
int foo5 (int x, double y, int* z);
int foo5(x,y,z)
   int x;
   double y;
   int* z;
   {
      x = 42 + y + *z; /* Make sure that everything is used! */
     return x;
   }

/* New style ANSI function prototype and new style ANSI function definition 
   Note that passing a float is just fine in this case using the new-style.
*/
int foo8 (float x);
int foo8(float x)
   {
     x = 42;
     return x;
   }


