
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

template<typename _Tp > class my_vector { };

my_vector<char_traits< unsigned int > ::int_type > local_A;

// Error: no suitable user-defined conversion from "my_vector<unsigned long>" to "my_vector<unsigned int>" exists
my_vector<unsigned> local_B = local_A;

