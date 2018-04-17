template<typename _CharT>
class istreambuf_iterator;

// template<typename _CharT>
class basic_streambuf 
   {
     public:
          friend class istreambuf_iterator<char>;
       // friend class istreambuf_iterator;
   };
