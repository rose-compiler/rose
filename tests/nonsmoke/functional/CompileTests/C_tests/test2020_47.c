void foobar()
   {
     int x;

     x = ({ union { int i; } u; u.i = 42; });
   }
