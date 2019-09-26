struct A
   {
     int b { 1+2*3 };
     int c;
     explicit constexpr A(int e);
     constexpr operator int();
   };


void foobar()
   {
     A a (1);
     char aa[ A(1) ];
   }
