struct A 
   {
     A(int i);
   };

struct B : A 
   {
     using A::A;
   };

