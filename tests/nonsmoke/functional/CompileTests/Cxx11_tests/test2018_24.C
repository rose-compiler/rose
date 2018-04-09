
template<typename _CharT>
struct _Char_types
   {
     typedef unsigned long   int_type;
   };

template<typename _CharT>
struct my_char_traits
   {
     typedef typename _Char_types<_CharT>::int_type    int_type;
   };

template<class _CharT> struct char_traits : public my_char_traits<_CharT> { };

#if 1
// Specialization allows GNU to to compile test code, but failes for EDG.
template<> struct char_traits <unsigned int>
   {
     typedef unsigned int int_type;
   };
#endif

#include <cstdint>
#include <stdint.h>
#include <wchar.h>

// typedef unsigned char32_t;
// typedef unsigned uint_least32_t;

#if 1
// #ifdef _GLIBCXX_USE_WCHAR_T
  /// 21.1.3.2  char_traits specializations
template<>
struct char_traits<wchar_t>
   {
  // typedef wint_t       int_type;
     typedef unsigned int int_type;
   };
// #endif
#endif

#if 1
template<>
struct char_traits<char32_t>
   {
   // typedef uint_least32_t int_type;
     typedef unsigned int int_type;
   };
#endif

template<typename _Tp > class my_vector { };

my_vector<char_traits< unsigned int > ::int_type > local_A;
// my_vector<char_traits< int > ::int_type > local_A;

// Error: no suitable user-defined conversion from "my_vector<unsigned long>" to "my_vector<unsigned int>" exists
my_vector<unsigned> local_B = local_A;

