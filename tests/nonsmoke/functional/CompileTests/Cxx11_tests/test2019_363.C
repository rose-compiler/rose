namespace D 
   {
     int f();

     struct X
        {
          friend int f() 
             {
               return 2;
             }
        };
   }

void foobar()
   {
     D::f();
   }
