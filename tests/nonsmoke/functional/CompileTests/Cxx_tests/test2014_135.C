
template <class T>
struct Base
{
};

namespace XXX 
   {
     class BlockMapping ;
   }

namespace 
   {
     struct ModifyBlockExtents : Base<XXX::BlockMapping> {};
   }

