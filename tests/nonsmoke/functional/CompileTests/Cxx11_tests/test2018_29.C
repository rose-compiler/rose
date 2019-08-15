template < typename template_parm1, int template_parm2 >
struct template_struct1
   { 
     template_parm1 array1[template_parm2]; 
   };

template_struct1< template_struct1< int , 2 > , 2 > local2 = {template_struct1< int , 2 > {1, 1} , template_struct1< int , 2 > {1, 1}};
