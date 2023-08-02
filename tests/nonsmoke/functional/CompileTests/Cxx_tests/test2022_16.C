template<typename T, T * ptr>
class A {};

namespace B
   {
     class A {};
     A a;
   }

namespace C
   {
     B::A a;
   };

typedef A<B::A, &B::a> typeB;
typedef A<B::A, &C::a> typeC;

