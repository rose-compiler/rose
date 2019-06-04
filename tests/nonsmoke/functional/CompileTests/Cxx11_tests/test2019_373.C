
void foobar()
   {
     struct A 
        {
          union 
             {
               char b;
             };
        };

  // char (A::*d) = &A::b;
     &A::b;
   }
 
