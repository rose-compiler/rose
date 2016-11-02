#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 4) )
// This test fails for the 4.2.4 version of GNU g++.

void foo()
   {
     int x;
     __decltype(x + 0) y;
   }

#endif
