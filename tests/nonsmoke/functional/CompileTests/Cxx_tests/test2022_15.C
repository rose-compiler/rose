template<typename T, T * ptr>
class A {};

namespace B
   {
     class A {};
     A a;
   }

namespace C
   {
     class A {};
     A a;
   };

typedef A<B::A, &B::a> typeB;
typedef A<C::A, &C::a> typeC;

