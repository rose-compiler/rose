#include <cmath>

// DQ (3/31/2020): Adding support for Clang on Mac.
#ifndef __APPLE__
inline float abs (float r)
   {
     return 0.0;
   }
#endif
