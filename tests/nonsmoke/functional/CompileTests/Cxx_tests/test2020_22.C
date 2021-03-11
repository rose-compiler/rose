namespace X
   {
     int i = 43;
     namespace N
        {
          int i = 44;
          struct X
             {
               static void f();
             };
          struct N
             {
               static void f();
             };
          void X::f()
             {
               using namespace X;
               i = 45;
             }
#if 1
          void N::f()
             {
               using namespace ::X::N;
               i = 46;
             }
#endif
#if 1
          void f()
             {
               using namespace X;
               i = 47;
             }
#endif
        }
   }


void foobar()
   {
#if 0
     namespace A = X::N;
     A::X::f();
     A::N::f();
#endif
#if 0
     A::f();
#endif
   }
