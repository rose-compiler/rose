
#include "inputmoveDeclarationToInnermostScope_test2014_19.h"

#define HAVE_VALUE

#ifdef HAVE_VALUE

namespace X {

  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }

}  // closing brace for namespace statement

#endif
