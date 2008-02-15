// Markus Kowarschik, 10/2002
// There's a problem with #include directives at the end
// of a scope, see also mkTest_01.C

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
#include "mkTest_03.h"
}
