// This demonstrates template argument value expressions
// not being shared.  This is now fixed.
template<bool B> struct ABC;

template<bool B> class DEF
   {
     public:
          typedef ABC<B> abc_type;
   };

DEF<true> X;

