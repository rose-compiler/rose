template <const double& T>
class A
   {
     public:
        A(double a);
   };

namespace B
   {
     extern const double VALUE;
  // Generated code includes (error): 
  // template<> class A< B::VALUE > ;
     typedef A<VALUE> typeZ;
   }

class C
   {
     protected:
          B::typeZ zz;
   };

class D
   {
     public:
          double getY() const;
   };

#include "test2022_01.h"
// Header file (test2022_01.h) includes code:
// namespace E
//    {
//      extern const double VALUE;
//      typedef A<VALUE> typeZ;
//    };

class F
   {
     public:
          void setY(E::typeZ zz);
   };

class H
   {
     protected:
          F y;
   };

class J : public H {};

class K : public J
// class K : public H
   {
     public:
          virtual void setW();
          virtual const D& getX();
   };

void K::setW()
   {
     const D & x = getX();
  // Original code:  y.setY(E::typeZ(x.getY()));
  // Generated code: (this) -> y .  setY (A< B::VALUE> (x .  getY ()));
     y.setY(E::typeZ(x.getY()));
   }

