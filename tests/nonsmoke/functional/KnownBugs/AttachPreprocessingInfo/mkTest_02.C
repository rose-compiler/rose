// Markus Kowarschik, 10/2002
// Problem: The #include directive at the end of this file
// gets unparsed after the last AST node to be visited in
// the course of the tree traversal, see also
// ROSE/TESTS/CompileTests/A++Code/lazy_task.C

int foo1(void);
int foo2(void);

int
main()
{
  typedef int boole;
  int i= foo1();
  return i;
}

int
foo1(void)
{
  int r= 0;
  for(int j= 0; j<10; j++)
    r +=j;
  return r;
}

#include "mkTest_02.h"

// EOF
