
// #include <string>
#include "inputmoveDeclarationToInnermostScope_test2014_23.h"

namespace std {

  template<typename _CharT>
    class numpunct 
    {
    public:
#if 1
      string
      grouping() const
#if 0
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
