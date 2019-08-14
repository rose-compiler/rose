struct A {};

struct X
   {
     struct Y {};

     int Y::* Y::* Y::* Y::* *p2 = 0L;
   };

void foobar()
   {
     int X::Y::* X::Y::* X::Y::* X::Y::* *p3 = 0L;
   }
 
