#if 0
#include <string>
#else
// int x;
namespace std 
   {
     template<typename _Alloc> class allocator;

     template<class _CharT> struct char_traits;

  // template<typename _CharT, typename _Traits = char_traits<_CharT>,typename _Alloc = allocator<_CharT> > class basic_string;
  // template<typename _CharT> class basic_string;
      template<typename _CharT> class basic_string {};

     template<> struct char_traits<char>;

     typedef basic_string<char> string;
   }
#endif

namespace std {

  template<typename _CharT>
    class numpunct 
    {
    public:
#if 1
      string
      grouping() const
#if 1
       { string s; return s; }
#else
       { return ""; }
#endif
#else
      string
      grouping() const;
#endif
    };

}

  void foo()
     {
       int x;
       if (1)
          {
            x = 4;
          }
     }
