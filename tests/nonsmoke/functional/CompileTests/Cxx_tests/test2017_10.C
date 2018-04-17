
typedef long unsigned int size_t;

class String
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

