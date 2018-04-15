
template <typename t_Type1, typename t_Type2>
class t_Class1;

struct struct1 {};

template <typename t_Type3>
class t_Class1 <struct1, t_Type3> {}; 

namespace namespace1 
   {
     struct struct2 {};
   }

template < typename t_Type4 >
struct t_struct3 
   {
     typedef struct1 typedef1;
   };

template < >
struct t_struct3< namespace1::struct2 > 
   {
     typedef struct1 typedef1;
   };

template < typename t_Type5, typename t_Type6>
using t_Class2 = t_Class1< typename t_struct3< t_Type5 >::typedef1, t_Type6 >; 

t_Class2<namespace1::struct2, int> local1;

 
 
#if 0
t_Class2<namespace1::struct2, int> local1;
gets this error on Ares in ROSE 0.9.9.246:
rose_ROSE-25.cc(30): error: identifier "struct2" is undefined
  t_Class2 < struct2  , int  >  local1;
             ^
#endif

 
