namespace std
   {
     template<typename _CharT>
     class basic_ostream
        {
          public:
               basic_ostream<_CharT> & operator<<(basic_ostream<_CharT> & (*__pf)( basic_ostream<_CharT> &));
        };

     typedef basic_ostream<char> ostream;

     extern ostream cout;

     template <typename _CharT> basic_ostream<_CharT> & endl ( basic_ostream<_CharT> & __os);
   }

void foo()
   {
     std::cout << std::endl;
   }
