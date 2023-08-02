template <const double& T>
class A
   {
     public:
       // A(double a);
   };

namespace B
   {
     extern const double VALUE;
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
   }

namespace E
   {
     extern const double VALUE;
  // Original code is: typedef A<VALUE> typeZ;
  // Generated code is: typedef class A< B::VALUE > typeZ;
  // Error: the name should not be qualified, but I think it is incorrect in the AST.
   };

typedef A<B::VALUE> BtypeZ;
typedef A<E::VALUE> EtypeZ;
