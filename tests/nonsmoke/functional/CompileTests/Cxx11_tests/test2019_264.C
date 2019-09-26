class c_
   {
     public:
          int i;
          template <class T> T h() { return i; } // member template
       // c_() : i(2) { }
   };

class d_ : private c_
   {
     public:
          using c_::i;
          using c_::h;
          d_() : c_() { }
   };

d_ o_;

void foobar()
   {
     o_.i;
     o_.h<long>();
   }

