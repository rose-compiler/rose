struct a_ 
   {
  // trivial dtor
     int b { 1+2*3 }; // brace-or-equal-init, constant expr
     int c; // non-static data mem and base classes literal types
     explicit constexpr a_(int e) : c(e) { } // constexpr ctor
     constexpr operator int() { return b+c; }
   };

struct c_ 
   {
     a_ d; // non-static data mem literal type
     constexpr c_() : d(4+5*6) { } // ctor call, constant expr
     constexpr operator int() { return d.b+d.c; }
   };

struct f_ 
   {
  // trivial dtor
     int g { 1+2*3 }; // brace-or-equal-init, constant expr
  // non-static data mem and base classes literal types
  // aggregate type 
     constexpr operator int() { return g; }
   };

struct h_ : a_ 
   {  // base class literal type
  // trivial dtor
     int i = 1+2*3; // brace-or-equal-init, constant expr
  // non-static data mem and base classes literal types
     template<class T>
     explicit constexpr h_(T e) : a_((int)e) { }
  // constexpr ctor template
     constexpr int j() { return i+b+c; }
   };

void foobar()
   {
     a_ a (1);
     char aa[ a_(1) ];
     c_ c ;
     char ca[ c ];
     f_ f ;
     char fa[ f ];
     h_ h (1);
     char ha[ h ];
   }
