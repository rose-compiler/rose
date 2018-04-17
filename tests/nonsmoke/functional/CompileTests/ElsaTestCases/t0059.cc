// cc.in59
// ambiguous expression from mozilla

//template <class CharT>
struct nsReadableFragment
  {
    const void*   mFragmentIdentifier;

    unsigned long
    GetIDAsInt() const
      {
        typedef char* char_ptr;
        typedef unsigned long ulong;
        
        // this line caused a problem because it is two ambiguities
        // nested inside another ambiguity, and while checking the
        // first toplevel ambiguitiy I set the 'type' field of one
        // of the lower ones (to ST_ERROR), and then the next time
        // I checked it I didn't generate an error message because
        // 'type' was already set
        return ulong(char_ptr(mFragmentIdentifier)-char_ptr(0));
      }
  };
