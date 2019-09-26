void foobar()
   {
     while (0)
          if ( ({ union { int i; } u; u.i = 42; }) == 0)
               break;
   }
