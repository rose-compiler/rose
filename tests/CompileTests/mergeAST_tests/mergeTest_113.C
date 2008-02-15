namespace std
   {
     template<class _CharT> struct char_traits
  // : public __gnu_cxx::char_traits<_CharT>
    { };

     template<> struct char_traits<char>
        {
          typedef char              char_type;
          typedef int               int_type;
        };
   }

namespace std
   {
     template<typename _Alloc> class allocator;
     template<class _CharT> struct char_traits;
   }

std::char_traits<char> x;

