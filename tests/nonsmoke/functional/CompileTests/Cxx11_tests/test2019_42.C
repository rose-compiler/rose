namespace X
   {
     struct A
        {
          int b { 1+2*3 };
          int c;
          explicit constexpr A(int e);
          constexpr operator int();
        };
   }

void foobar()
   {
     X::A a (1);
     char aa[ X::A(1) ];
   }
