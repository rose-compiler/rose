template < typename template_parm1 >
struct template_struct1
   { 
     template_parm1 array1[1]; 
     template_parm1 array2[1]; 
  // int i;
   };

// template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1}, 42 };
template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1}, template_struct1< int > {42} };
