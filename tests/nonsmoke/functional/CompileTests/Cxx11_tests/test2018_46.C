namespace xxx
   {
     template < typename template_parm1 >
     struct template_struct1
        { 
          template_parm1 array1[1]; 
        };
   }

// NOTE: The initializer's class name requires name qualification.
xxx::template_struct1< xxx::template_struct1< int > > local2 = { xxx::template_struct1< int > {1} };
