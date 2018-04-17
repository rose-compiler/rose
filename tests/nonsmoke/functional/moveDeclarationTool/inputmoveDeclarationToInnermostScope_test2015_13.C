#include "inputmoveDeclarationToInnermostScope_test2015_13.h"

namespace std {

  template<typename _CharT>
    class numpunct 
    {
    public:
#if 1
      string
      grouping() const
#if 1
       { string s; return s; }
#else
       { return ""; }
#endif
#else
      string
      grouping() const;
#endif
    };

}

  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }
