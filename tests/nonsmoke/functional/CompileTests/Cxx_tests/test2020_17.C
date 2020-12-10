
namespace X
   {
     int xxxxxx = 73;
     namespace Y
        {
       // This declaration is need to force the name qualification
          int xxxxxx = 22;
          struct X
             {
               static void f();
             };

          void X::f()
             {
               namespace A = X;
            // Original code: A::i
            // Bug: unparsed as X::i
               A::xxxxxx;
             }
        }
   }

