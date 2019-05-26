struct A {};

struct X
   {
     struct Y {};
   };

void foobar1()
   {
     int X::Y::* X::Y::* X::Y::* X::Y::* *p2 = 0L;
   }

void foobar2()
   {
     X::Y* X::Y::* X::Y::* X::Y::* X::Y::* *p3 = 0L;
   }
 
