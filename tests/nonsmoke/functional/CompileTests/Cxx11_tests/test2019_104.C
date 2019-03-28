namespace N
   {
     enum class color : long;
     int foo(color c);
   }

enum class N::color : long { R, O, Y };

void foobar()
   {
     N::foo(N::color::R);
   }


