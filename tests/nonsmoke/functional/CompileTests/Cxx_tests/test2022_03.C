template <const double& T>
class A
   {
     public:
        A(double a);
   };

namespace B
   {
     extern const double VALUE;
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
  // The scope should be namespace B, so how do we know when a template instantiation is in the wrong scope?
  // I think that this maybe something that is a special rule just for template instantiations.
     typedef A<VALUE> typeZ;
   }

namespace E
   {
     extern const double VALUE;
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
     typedef A<VALUE> typeZ;
   };
