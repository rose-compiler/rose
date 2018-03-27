

template<typename _CharT>
struct char_traits
   {
     typedef unsigned int int_type;
   };

template<typename _Tp >
class vector
   {
   };

vector<char_traits<unsigned int>::int_type> local2;

// This type will be output as: vector<char_traits<unsigned int>::int_type> since it is an equivalent type.
vector<unsigned> local1;


