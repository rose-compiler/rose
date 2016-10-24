
#if 0
class A
   {
     public:
         mutable int xyz;

         A () {};
         inline explicit A (int x) {};

         int foo1() { return 0; };
         int foo2() const { return 0; };
         int foo3() volatile { return 0; };
         int* foo4() __restrict__ { return 0; };
         static int foo5() { return 0; };
//       int foo6() mutable { return 0; };
   };
#endif

void foo ( int );
void foo ( register int i )
   {

   }



int main()
   {
#if 0
     A a;
     a.foo2();
     a.foo3();

     int* __restrict__ x;
#endif


     return 0;
   }

