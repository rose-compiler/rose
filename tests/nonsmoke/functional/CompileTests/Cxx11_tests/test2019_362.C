namespace D 
   {
     int f();

     struct Z
        {
          struct Y
             {
               struct X
                  {
                    friend int f() 
                       {
                         return 2;
                       }

                  };
             };
        };
   }

void foobar()
   {
     D::f();
   }
