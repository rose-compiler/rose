namespace X
   {
     namespace N
        {
          void f()
             {
             }
        }
   }


void foobar()
   {
     namespace A = X::N;
     A::f();
   }
