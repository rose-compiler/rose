class c_
   {
     public:
          int i;
          inline int f() { ++i; return i; }
          inline int g() { ++i; return i; };  // semicolon allowed here
          template <class T> T h() { return i; } // member template
          c_() : i(2) { }
   };

class d_ : private c_
   {
     public:
          using c_::i;
          using c_::f;
          using c_::g;
          using c_::h;
          d_() : c_() { }
   };

d_ o_;

void foobar()
   {
     o_.i;
     int j = o_.f() + o_.g();
     // ieq(j, 7);
     // ieq(o_.i, 4);
     o_.h<long>();
     o_.f();
     o_.g();
   }


