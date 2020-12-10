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
#if 1
          struct N
             {
               static void f();
             };
#endif
          void N::f()
             {
               using namespace ::X::N;
               i = 46;
             }
        }
   }


