template<int v>
class A {};

namespace B
   {
     constexpr int val = 8;
   }

namespace E
   {
     constexpr int val = 10;
   }

typedef A<B::val> typeB;
typedef A<E::val> typeE;
typedef A<B::val + E::val> typeD;

