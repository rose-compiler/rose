template<class _CharT> 
struct char_traits 
   {
     typedef unsigned long   int_type;
   };

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
