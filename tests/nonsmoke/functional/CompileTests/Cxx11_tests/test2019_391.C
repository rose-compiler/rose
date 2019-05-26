struct A {};

struct X
   {
     struct Y {};

     int Y::* *p2;
   };

#if 1
void foobar()
   {
     int X::Y::* *p3;
   }
#endif
 
