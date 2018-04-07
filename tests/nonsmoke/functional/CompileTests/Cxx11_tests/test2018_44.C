template < typename template_parm1 >
struct template_struct1
   { 
     template_parm1 array1[1]; 
   };

// Should be unparsed as:
// template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1} };
   template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1} };

// Note: When the array is size 2, then we need to unparse as:
// template_struct1< template_struct1< int > > local2 = {template_struct1< int > {1, 1} , template_struct1< int > {1, 1}};
