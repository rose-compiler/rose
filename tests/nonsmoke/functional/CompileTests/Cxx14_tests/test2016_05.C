
// C++14
[=] 
   {                                     // ok, deduces "-> some_type"
     while( something() ) 
        {
          if( expr ) 
             {
                return foo() * 42;            // with arbitrary control flow
             }
        }

     return bar.baz(84);                   // & multiple returns
   }                                         //   (types must be the same)
