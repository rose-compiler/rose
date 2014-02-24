// Not that in old-style C, the arguments will be unparsed in a 
// different order, and this appears to be related to why "count"
// is swapped with "tm2" in the generated code.

#define DEMO_BUG 1

void
foobar(x, y)
#if DEMO_BUG
    int y;
    double x;
#else
    double x;
    int y;
#endif
   {
  // Original code is: x = y; 
     x = y; 
   }

