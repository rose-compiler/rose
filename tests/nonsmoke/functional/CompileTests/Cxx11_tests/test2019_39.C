struct A
   {
     int b { 1+2*3 };
     int c;
     explicit constexpr A(int e) : c(e) { }
     constexpr operator int() { return b+c; }
   };

struct C
   {
     A d;
     constexpr C() : d(4+5*6) { }
     constexpr operator int() { return d.b+d.c; }
   };

struct F 
   {
     int g { 1+2*3 };
     constexpr operator int() { return g; }
   };

struct H : A 
   {
     int i = 1+2*3;
     template<class T>
     explicit constexpr H(T e) : A((int)e) { }
     constexpr int j() { return i+b+c; }
   };

void foobar()
   {
     A a (1);
     char aa[ A(1) ];
     C c ;
     char ca[ c ];
     F f ;
     char fa[ f ];
     H h (1);
     char ha[ h ];
   }
