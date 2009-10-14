void f(int foo[4])
   {
     foo[0] = foo[1];
   }

void f2(int foo[])
   {
     foo[1] = foo[2];
   }

struct A { int foo[4]; };

int test(int)
   {
     int foo[4]; // = {0, 1, 2, 3};
     foo[0] = 0;
     foo[1] = 1;
     foo[2] = 2;
     foo[3] = 3;
     f(foo);
     f2(foo);
     A afoo, abar;
     void *p = abar.foo;
     abar.foo[0] = 4;
     abar.foo[1] = 5;
     abar.foo[2] = 6;
     abar.foo[3] = 7;
     afoo = abar;
     int *pfoo = foo;
     pfoo[0] = 8;
     return foo[0]+foo[1]+foo[2]+foo[3]+afoo.foo[0]+afoo.foo[1]+afoo.foo[2]+afoo.foo[3]+abar.foo[0]+abar.foo[1]+abar.foo[2]+abar.foo[3];
   }
