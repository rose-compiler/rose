struct A {};

struct X
   {
     struct Y {};
   };

void foobar()
   {
  // Original code: int A::* const X::Y::* *p2;
  // int A::* const X::Y::* *p2;
  // int X::Y::*p2;
  // A* X::Y::* *p2;
  // X::Y* X::Y::* *p2;
     int A::* const X::Y::* *p2;
   }

