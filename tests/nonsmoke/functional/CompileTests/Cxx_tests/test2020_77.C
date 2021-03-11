
class B {};

class A
   {
     public:
          A();
          void foo(B);
   };

// #include "test_104.h"
template < class T >
void bafoobar (void (T::*method)(B), T* instance, int val = 42)
   {
   }

A::A()
   {
     bafoobar(&A::foo,this);
   }


