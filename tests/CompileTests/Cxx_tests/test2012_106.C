void foo (int x)
   {
     class A0 {public: int foo (int x) {return x;}} x2;

#if 1
     for (class A0 {public: int foo (int x) {return x;}} x2; x2.foo (0); x)
#else
     for (class A0 x2; x2.foo(0); x)
#endif
        {
        }

   }
