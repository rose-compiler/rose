class A {};

struct B 
   {
     A a;
   };

void foo() 
   {
     A B::*v = &B::a;
   }
