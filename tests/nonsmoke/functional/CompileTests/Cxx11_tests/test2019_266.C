class c_
   {
     public:
          int i;
          template <class T> T h();
   };

class d_ : private c_
   {
     public:
       // using c_::i;
          using c_::h;
   };

