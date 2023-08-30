
template<typename T>
class A
   {
   };

// This might only appear to be correctly named because it is unparsed using the original template string.
template <typename _Tp>
class A<_Tp*>
   {
   };

namespace B
   {
     class C 
        {
          public:
          typedef C* ptr;
          typedef A<ptr> typeZ;
        };
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
  // The scope should be namespace B, so how do we know when a template instantiation is in the wrong scope?
  // I think that this maybe something that is a special rule just for template instantiations.
     typedef A<C> typeZ;

     template<typename T>
     class BB
        {
        };

   }

namespace E
   {
  // class D {};
     class C {};
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
     typedef A<C> typeZ;

     template<typename T>
     class EE
        {
        };

   };

typedef A<B::typeZ> typeB;
typedef A<E::typeZ> typeE;
typedef A<B::C::ptr> typeF;


typedef A<B::C> typeG;
typedef A<E::C> typeH;

typedef A<B::BB<B::typeZ>> typeI;
