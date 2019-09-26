// Lambda Capture of *this by Value as [=,*this]

#include<assert.h>

struct S 
   {
     int x ;
     void f()
        {
       // The following lambda captures are currently identical
          auto a = [&]() { x = 42 ; }; // OK: transformed to (*this).x
          auto b = [=]() { x = 43 ; }; // OK: transformed to (*this).x
          a();
          assert( x == 42 );
          b();
          assert( x == 43 );
        }
   };

