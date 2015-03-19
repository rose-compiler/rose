namespace std 
   {
     template<class _CharT> struct char_traits;

     template<typename _CharT> class basic_string {};

     template<> struct char_traits<char>;

//   typedef basic_string<char> string;

  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }
   }
