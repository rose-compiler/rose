typedef int HTConverter();

// Original code: HTConverter HTThroughLine;
HTConverter HTThroughLine;

// Original code: HTConverter HTBlackHoleConverter;
HTConverter HTBlackHoleConverter;


#include <assert.h>
void foo()
   {
     int x = 4;

  // This assuingment in an assertion will not generate a warning in GNU.
     assert(x=42);

     if (x==1)
        {
        }
   }
