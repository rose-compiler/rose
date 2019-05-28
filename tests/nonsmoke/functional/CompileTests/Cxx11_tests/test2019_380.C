#include<typeinfo>

struct A
   {
     typedef int mytype;
   };

struct X
   {
     struct Y
        {
       // int i, j;
        };
   };

void foobar()
   {
  // Unparses as: int    Y::* *p1 = new int    Y::* (&X::Y::j);
  // Should be:   int X::Y::* *p1 = new int X::Y::* (&X::Y::j);
  // int X::Y::* *p1 = new int X::Y::* (&X::Y::j);
  // new int X::Y::* (&X::Y::j);
     typeid(A::mytype X::Y::*);
   }
 
