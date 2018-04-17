// Markus Kowarschik, 10/2002
// There's a problem with #include directives at the end
// of a scope: in this case the #include directive gets
// attached to the return statement and thus unparsed
// in a different scope

int
main ()
{

  {
    int x;
#include "mkTest_01.h"
  }

  return 0;
}
