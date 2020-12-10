void foobar()
   {
     if ( ({ union { int i; } u; u.i = 42; }) == 0);

  // if ( ({ struct { int i; } u; u.i = 42; }) == 0);
   }
