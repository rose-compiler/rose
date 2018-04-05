#ifdef _GLIBCXX_USE_C99_STDINT_TR1
# if defined (__UINT_LEAST16_TYPE__) && defined(__UINT_LEAST32_TYPE__)
namespace std
{
  typedef __UINT_LEAST16_TYPE__ uint_least16_t;
  typedef __UINT_LEAST32_TYPE__ uint_least32_t;
}
# else
#  include <cstdint>
# endif
#endif

template<class _CharT> 
struct char_traits 
   {
     typedef unsigned long   int_type;
   };

#if 0
// Specialization allows GNU to to compile test code, but fails for EDG.
template<> struct char_traits <unsigned int>
   {
     typedef unsigned int int_type;
   };
#endif

#ifdef _GLIBCXX_USE_C99_STDINT_TR1
// To GNU g++ "char32_t" is not the same as "unsigned int"
template<>
struct char_traits<char32_t>
   {
     typedef uint_least32_t int_type;
   };
#endif

// Build a simple vector class template
template<typename _Tp > class my_vector { };

my_vector<char_traits< unsigned int > ::int_type > local_A;

// Error: no suitable user-defined conversion from "my_vector<unsigned long>" to "my_vector<unsigned int>" exists
my_vector<unsigned> local_B = local_A;

