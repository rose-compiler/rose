// 1st foo function
void foo(float x);

// 2nd foo function
// void foo(double x);

#include "foo.h"

void foobar()
   {
     float y = 5;
     foo(y);

  // 2nd foo function called.
     foo(7.0);
   }
