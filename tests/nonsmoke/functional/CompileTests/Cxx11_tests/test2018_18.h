#include "string"

namespace std
   {
#if 0
template<typename _CharT>
struct char_traits
   {
   };

template<>
struct char_traits<unsigned int>
   {
     typedef unsigned int int_type;
   };
#endif

template<typename _Tp >
class X_vector
   {
   };
   }

// std::vector<std::char_traits<unsigned int>::int_type> local2;
std::X_vector<std::char_traits<unsigned int>::int_type> local2;

// Put this into a header file if required.
std::X_vector<unsigned> method1();

