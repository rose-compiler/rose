/* The following code is all ANSI C 
(compiles using gcc -std=c99, for example, but does not compile with g++).
The old-style C represented in the function definitions does not currently 
work in ROSE.

   NOTE: In order to work with the gcc C compiler using old-style function definitions
we need to use a file with *.c (and not *.C).
*/

#if 1
/* int foo1 (int x); */
/* int foo1 (int x, float y, int z); */
int foo1 (int x);
#endif

int foo1(x)
   int x;
   {
     x = 42;
     return x;
   }

#if 0
int foo2 (int x);

int foo2(int x)
   {
     x = 42;
     return x;
   }

int foo3();

int foo3(int x)
   {
     x = 42;
     return x;
   }

int foo4();

int foo4(x)
   int x;
   {
     x = 42;
     return x;
   }
#endif


