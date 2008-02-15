// Markus Kowarschik, 10/2002
// There's a problem with #include directives at the end
// of a scope, see also mkTest_01.C

// DQ (9/10/2005): This now works, so we can test it!

int foo(void);

int
main ()
{
  int i= foo();
  return i;
}

int
foo(void)
{
  int x= 2;
#include "test2005_159.h"
}
