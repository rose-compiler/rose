void foobar()
   {
  // if ( ({ union { int i; } u; u.i = 42; }) == 0);
     if ( ({ struct { int iii; } u; u.iii = 42; }) == 0);
   }
