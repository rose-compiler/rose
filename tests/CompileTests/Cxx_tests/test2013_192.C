// #include "string"

template <typename T>
class basic_string
   {
     public:
         basic_string() {}

         basic_string(T a,T b) {}
   };

typedef basic_string <char*> string;

string SimpleItoa(unsigned int i) 
   {
     char* x;
     char buffer[42];

  // This is unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string < char * > (buffer,x);
  // Should be unparsed as: return std::basic_string< char ,class std::char_traits< char  > ,class std::allocator< char  > > ::basic_string(buffer,x);
     return string(buffer,x);
   }
