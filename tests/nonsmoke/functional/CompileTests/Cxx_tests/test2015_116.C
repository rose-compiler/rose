
// This fails when processed using the move-tool
// (see tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_141.C)
// Note that this is a modification of test2015_115.C.

#include <list>
#include <string>
#include "boost/shared_ptr.hpp"

namespace XXX {
  namespace YYY {
    class ZZZ
       {
         public: 
              bool foo(int var) const;
       };
  }
}

namespace Ares {

void foobar()
   {
     const boost::shared_ptr<XXX::YYY::ZZZ > var_0;

     std::list<std::string> var_1;

     std::list<std::string> var_2;

  // bool var_3;

     for (std::list<std::string>::iterator i(var_2.begin()); i != var_2.end(); i++)
        {
          for (std::list<std::string>::iterator j(var_1.begin()); j != var_1.end(); j++) 
             {
               if (true)
                  {
                    int var_4 = 42;
                    if (true) 
                       {
                         bool var_3 = var_0->foo(var_4);
                       } 
                  }
             }
        }
   }

}

