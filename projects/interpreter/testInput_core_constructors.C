struct W
   {
     int w;

     W(int w) : w(w) {}
   };

struct Z
   {
     int z;

     Z(int z) : z(z) {}
   };

struct X : W, Z
   {
     int a;
     Z foo;

     X(int a) : W(a+12), Z(a-1), a(a), foo(a+1) {}
   };

int test(int p)
   {
     X x(2);
     return x.a+x.w+x.z+x.foo.z;
   }
