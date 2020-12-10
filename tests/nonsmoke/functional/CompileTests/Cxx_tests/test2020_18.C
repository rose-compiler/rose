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
               i = 42;
             }
          void N::f()
             {
               using namespace ::X::N;
               i = 42;
             }
          void f()
             {
               using namespace X;
               i = 42;
             }
        }
   }


void foobar()
   {
     namespace A = X::N;
     A::X::f();
     A::N::f();
     A::f();
   }
