#include <string>

namespace XXX 
   {
     class BlockMapping ;
   }

namespace 
   {
     struct ModifyBlockExtents : std::unary_function<XXX::BlockMapping, void> {};
   }

