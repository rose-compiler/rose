// Test use of A++ header files in building a domain-specific grammar

#include "A++.h"

class X
   {
     public:
          class descriptorType
             {
               public:
                    int a;
                    int b;
                    int c;

                    void fooInClass_DEF();
             };

     public:
          int a;
          int b;
          int c;

          void fooInClass_ABC();
   };

void fooInGlobalScope ()
   {
   }

int
main ()
   {
     return 0;
   }

