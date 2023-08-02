template <int T>
class A
   {
   };

namespace B
   {
     enum Color { RED, BLUE, GREEN };
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
  // The scope should be namespace B, so how do we know when a template instantiation is in the wrong scope?
  // I think that this maybe something that is a special rule just for template instantiations.
     typedef A<RED> typeZ;
   }

namespace E
   {
     enum Color { WHITE, RED, YELLOW };
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
     typedef A<RED> typeZ;
   };

typedef A<B::RED> typeB;
typedef A<E::RED> typeE;
