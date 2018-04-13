

template<typename _CharT>
struct char_traits
   {
     typedef unsigned int int_type;
   };

template<typename _Tp >
class vector
   {
   };

vector<unsigned> local1;

// This type will be output as: vector<unsigned> since it is an equivalent type.
vector<char_traits<unsigned int>::int_type> local2;


