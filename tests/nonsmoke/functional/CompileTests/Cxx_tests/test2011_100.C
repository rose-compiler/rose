struct B
   {
     void f(char);
   };

struct D : B 
   {
     using B::f;
   };

