struct A {};

struct X
   {
     struct Y {};

     int Y::* *p2 = 0L;
   };

#if 1
void foobar()
   {
     int X::Y::* *p3 = 0L;
   }
#endif
 
