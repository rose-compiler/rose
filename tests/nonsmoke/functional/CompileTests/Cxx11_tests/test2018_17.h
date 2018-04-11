// #include "algorithm"

// namespace XXX
// {
template<typename _CharT>
struct char_traits
   {
   };

template<>
struct char_traits<unsigned int>
   {
     typedef unsigned int int_type;
   };

template<typename _Tp >
class vector
   {
   };
// }

vector<char_traits<unsigned int>::int_type> local2;

// Put this into a header file if required.
// vector<unsigned> method1();

