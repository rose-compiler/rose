void foobar()
   {
     int x;

     for(;;)
        {
          x = ({ union { int i; } u; u.i = 42; });
          break;         
        }
   }
