
template <class T> class X {};

void foobar() 
   {
     typedef struct {} B;

  // BUG: Unparsed as:  X< {} > x;
  // FIX: Since there is a typedef, we need to select one of the typedefs that is not private, and is named.
     X<B> x;
   }

