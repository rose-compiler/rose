// Markus Kowarschik, 10/2002
// There's a problem with #include directives at the end
// of a scope: in this case the #include directive gets
// attached to the return statement and thus unparsed
// in a different scope

// DQ (9/10/2005): This now works, so we can test it!

int
main ()
{

  {
    int x;
#include "test2005_157.h"
  }

  return 0;
}
