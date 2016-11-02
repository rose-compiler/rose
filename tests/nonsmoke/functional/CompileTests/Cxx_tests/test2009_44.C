/*
The code below works with g++, but not with the Identity translator:

-Jeff
*/

void foo (int a)
   {
   }

class A
   {
     public:
          A() {}
         ~A() {}
          void foo(int a, int b) ;
   };

void A::foo(int a, int b)
   {
     ::foo(b) ;  /* The :: is lost during ROSE translation */
   }

