#include <type_traits>

void foobar()
   {
     struct A { };
     struct B { char b; };
     struct C : A { };
     struct D : A, C { char d; };
     struct F { B ba, bb; };
     struct G { D da, db; };
     union E 
        {
          F f;
          G g;
        } eo;

     bool e = std::is_standard_layout<E>::value;
   }
