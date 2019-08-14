long double operator "" _w(long double);

void foobar()
   {
     1.2_w;    // calls operator "" _w(1.2L)
   }
