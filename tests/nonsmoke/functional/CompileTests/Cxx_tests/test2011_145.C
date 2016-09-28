
// Example of processing the implicit variable: __PRETTY_FUNCTION__: 
//    could not trace back to SgFunctionDefinition node from SgBasicBlock

#include<assert.h>

struct Y
   {
     void foo1()
        {
          assert (!"Inside of struct Y::foo1 (using assert)");
        }
   };

