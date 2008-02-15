/* The following code is all ANSI C 
(compiles using gcc -std=c99, for example, but does not compile with g++).
The old-style C represented in the function definitions does not currently 
work in ROSE.
*/
#if 1
int foo1(int x);
#endif

int foo1(int x)
{
  x = 42;
  return x;
}

#if 0
#endif
