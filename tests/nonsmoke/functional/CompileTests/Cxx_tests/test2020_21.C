
#include<stdio.h>

namespace X
   {
     int i = 43;
     namespace N
        {
       // Note: this is the variable set in "i = 45" below.
          int i = 44;
          struct X
             {
               static void f();
             };

       // This is required for this bug.
          struct N
             {
            // static void f();
             };

          void X::f()
             {
               using namespace X;
               printf ("i = %d \n",i);

            // Original code: "i = 45"
            // Unparsed as: N::i = 45;
               i = 45;
             }
        }
   }

int main(int arg, char** argc)
   {
     X::N::X::f();

     return 0;
   }
