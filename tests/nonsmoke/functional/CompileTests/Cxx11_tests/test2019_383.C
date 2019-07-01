struct A {};

struct X
   {
     struct Y {};
   };

void foobar()
   {
     int A::* const X::Y::* *p2 = 0L;
   }
 
