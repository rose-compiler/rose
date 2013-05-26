
// template<typename _CharT > class basic_string;
// typedef basic_string<char> string;

template<typename _CharT>
class basic_string
   {
     public:
#if 1
       // template<class _InputIterator> basic_string(_InputIterator __beg, _InputIterator __end = 0L);
          template<class _InputIterator> basic_string(_InputIterator __beg, _InputIterator __end );
#else
          template<class _InputIterator> basic_string(_InputIterator __beg);
#endif
   };

typedef basic_string<char> string;

void SimpleItoa() 
   {
     char* x;

  // This is unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string < char * > (buffer,x);
  // Should be unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string(buffer,x);
  // string a = string(x,x);
     string a = string(x,x);
   }
