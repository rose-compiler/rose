template <const double& T>
class A
   {
     public:
        A(double a);
   };

namespace B
   {
     extern const double VALUE;
  // This is only an issue for testTemplates, and not for testTralslator.
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

#if 0
#include "test2022_01.h"
#else
// Header file (test2022_01.h) includes code:
namespace E
   {
     extern const double VALUE;
     typedef A<VALUE> typeZ;
   };
#endif

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

// class J : public H {};

// class K : public J
class K : public H
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
  // The following works:
  // (this) -> y .  setY (A< E::VALUE> (x .  getY ()));
  // So the name qualification is incorrectly using "B::" and should be "E::"
  // y.setY(E::typeZ(x.getY()));
     y.setY(E::typeZ(42.0));
   }

