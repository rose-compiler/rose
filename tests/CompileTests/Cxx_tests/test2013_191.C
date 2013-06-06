
template<typename _CharT > class basic_string;

typedef basic_string<char> string;

template<typename _CharT>
class basic_string
   {
     public:
          template<class _InputIterator> basic_string(_InputIterator __beg, _InputIterator __end);
   };

string SimpleItoa(unsigned int i) 
   {
     char* x;
     char buffer[42];

  // This is unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string < char * > (buffer,x);
  // Should be unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string(buffer,x);
     return string(buffer,x);
   }
