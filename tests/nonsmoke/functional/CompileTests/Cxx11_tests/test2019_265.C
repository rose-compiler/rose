class c_
   {
     public:
          int i;
          template <class T> T h(); // { return i; }
   };

class d_ : private c_
   {
     public:
          using c_::i;
          using c_::h;
   };

d_ o_;

void foobar()
   {
     o_.i;
     o_.h<long>();
   }

