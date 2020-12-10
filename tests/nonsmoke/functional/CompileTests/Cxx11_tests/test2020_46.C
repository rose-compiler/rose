struct A;
int foo(int t, A a);

struct A 
   {
     friend int foo(int, A);
   };
