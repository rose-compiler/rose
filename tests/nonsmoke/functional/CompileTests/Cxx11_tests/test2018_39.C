// template < typename template_parm1, int template_parm2 >
// template < int template_parm2 >
template < typename template_parm1 >
struct template_struct1
   { 
  // template_parm1 array1[template_parm2]; 
  // int array1[template_parm2]; 
     template_parm1 array1[2]; 
   };

// template_struct1< template_struct1< int , 2 > , 2 > local2 = {template_struct1< int , 2 > {1, 1} , template_struct1< int , 2 > {1, 1}};
// template_struct1< 2 > local2 = { template_struct1< 2 > {1, 1}};
template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1, 1} , template_struct1< int > {1, 1}};
