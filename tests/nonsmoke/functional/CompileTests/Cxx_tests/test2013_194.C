
template<typename _CharT>
class basic_string
   {
     public:
          template<class _InputIterator> basic_string(_InputIterator __beg, _InputIterator __end );
   };

typedef basic_string<char> string;

void SimpleItoa() 
   {
     char* x;

  // Since there is not function or member function (or constructor) called "string", we have to use the name of the member function.
  // This is currently unparsed as: string a = basic_string< char > ::basic_string < char * > (x,x);
  // But should be unparsed as: string a = basic_string< char > ::basic_string (x,x);
     string a = string(x,x);

  // This is an error in EDG as well.
  // string b = basic_string< char > ::basic_string<char*> (x,x);
  // string b = basic_string<char*>(x,x);
  // string b = basic_string(x,x);

  // These work in EDG:
  // string b = basic_string<char>(x,x);
  // string c = basic_string< char > ::basic_string<char> (x,x);
   }
