
#include "inputmoveDeclarationToInnermostScope_test2014_23.h"

namespace std {
  template<typename _CharT>
    class numpunct
    {
    public:
      string
      grouping() const
       { return ""; }
    };
}

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
