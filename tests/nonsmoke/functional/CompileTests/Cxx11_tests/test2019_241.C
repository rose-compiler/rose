namespace N
   {
     static union 
        {
          int a;
        };
   }

void foobar()
   {
     &N::a;
   }

