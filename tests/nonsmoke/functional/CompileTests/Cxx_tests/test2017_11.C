// This is similar to test2017_10.C except with a class name that does not match the template argument name.
typedef long unsigned int size_t;

class StringXXX
   {
     public:
          bool append(const char *s, size_t arg_length);
   };

template<class String> inline
void append_string(String& dst, const String& src,
                   const typename String::size_type beg,
                   const typename String::size_type end) 
   {
     dst.append(src.begin()+beg, src.begin()+end);
   }

