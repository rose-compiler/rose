

void foobar()
   {
     struct A 
        {
          union 
             {
               char b;
               char c;
             };
        };

     char (A::*d) = &A::b;
     char (A::*e) = &A::c;
   }
 
