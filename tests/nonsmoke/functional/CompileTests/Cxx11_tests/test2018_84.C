
template <typename t_Type1>
class t_Class1 {};

namespace namespace1 
   {
     struct struct1 {};
   }

template < typename t_Type5>
using alias_Class2 = t_Class1< t_Type5 >; 

alias_Class2<namespace1::struct1> local1;

