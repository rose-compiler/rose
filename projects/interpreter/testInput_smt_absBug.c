#include <limits.h>

void assert(int);

int abs(int x)
   {
     int rv;
     if (x > 0)
          rv = x;
     else
          rv = -x;
     return rv;
   }

int test(int x)
   {
     assert(abs(x) >= 0);
     return 0;
   }
