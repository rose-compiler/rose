namespace D 
   {
     int f();

     class X
        {
       // Having a non-defining declaration here allows the name qualification to work fine (for the call to "D::f();").
          friend int f();
        };
   }

void foobar()
   {
     D::f();
   }
