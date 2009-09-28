#include <limits.h>

void assert(int);

int abs(int x)
   {
     int rv;
     if (x > 0)
          rv = x;
     else if (x == INT_MIN)
          rv = INT_MAX;
     else
          rv = -x;
     return rv;
   }

int limit(int x)
   {
     int rv;
     if (x > 0)
          rv = x;
     else
          rv = 0;
     return rv;
   }

int test(int x)
   {
     assert(limit(x) >= 0);
     assert(abs(x) >= 0);
     assert(abs(x) % 6 >= 0);
     return 0;
   }
