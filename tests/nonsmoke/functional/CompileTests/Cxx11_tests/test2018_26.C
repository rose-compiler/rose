// #include <stdint.h>

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

#if 1
// Specialization allows GNU to to compile test code, but fails for EDG.
template<> struct char_traits <unsigned int>
   {
     typedef unsigned int int_type;
   };
#endif

#if 1
// To GNU g++ "char32_t" is not the same as "unsigned int"
template<>
struct char_traits<char32_t>
   {
  // typedef uint_least32_t int_type;
     typedef unsigned int int_type;
   };
#endif

#if 0
// Build a simple vector class template
template<typename _Tp > class my_vector { };

// my_vector<char_traits< unsigned int > ::int_type > local_A;
my_vector<char_traits< char32_t > ::int_type > local_A;

// Error: no suitable user-defined conversion from "my_vector<unsigned long>" to "my_vector<unsigned int>" exists
my_vector<unsigned> local_B = local_A;
#endif
