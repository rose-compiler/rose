
namespace M
   {
     int i = 73;
     namespace N
        {
          int i = 22;
          struct M
             {
               static void f();
             };
          struct N
             {
               static void f();
             };
          void M::f()
             {
               namespace A = M;
            // Original code: A::i;
            // Unparses to M::i;
               A::i;
            // We might want to unparse to "::M::i" instead of "A::i".
               ::M::i;
             }
          void N::f()
             {
               namespace A = ::M::N;
               A::i;
             }
          void f()
             {
               namespace A = M;
            // Original code: A::i;
            // Unparses to M::i;
               A::i;
             }
        }
   }

void foobar()
   {
     M::N::M::f();
     M::N::N::f();
     M::N::f();
   }

