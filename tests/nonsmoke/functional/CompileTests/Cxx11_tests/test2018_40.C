template < typename template_parm1 >
struct template_struct1
   { 
  // template_parm1 array1[2]; 
     template_parm1 array1[1]; 
   };

// template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1, 1} , template_struct1< int > {1, 1}};
// template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1} , template_struct1< int > {1}};
template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1} };
