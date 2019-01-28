struct a_ 
   {
  // trivial dtor
     int b { 1+2*3 }; // brace-or-equal-init, constant expr
     int c; // non-static data mem and base classes literal types
     explicit constexpr a_(int e) : c(e) { } // constexpr ctor
     constexpr operator int() { return b+c; }
   };


void foobar()
   {
     a_ a (1);
     char aa[ a_(1) ];
   }
