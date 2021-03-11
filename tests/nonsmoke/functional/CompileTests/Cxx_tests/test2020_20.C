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
       // This is required for this bug.
          struct N
             {
            // static void f();
             };
#endif
          void X::f()
             {
               using namespace X;
               i = 45;
             }
        }
   }

