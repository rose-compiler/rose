template<typename T>
class A {};

namespace B
   {
     class A {};
   }

namespace C
   {
     class A {};
   };

typedef A<B::A> typeB;
typedef A<C::A> typeC;

typedef A<const B::A> ctypeB;
typedef A<const C::A> ctypeC;

typedef A<B::A *> ptypeB;
typedef A<C::A *> ptypeC;

typedef A<B::A &> rtypeB;
typedef A<C::A &> rtypeC;

typedef A<B::A const &> crtypeB;
typedef A<const C::A &> crtypeC;

