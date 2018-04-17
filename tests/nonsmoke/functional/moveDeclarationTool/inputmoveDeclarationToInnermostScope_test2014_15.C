
#include "inputmoveDeclarationToInnermostScope_test2014_15.h"

namespace X {

  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }
}

