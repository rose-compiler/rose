
void foo()
   {
     sizeof(int);
     sizeof(long);
     sizeof("string");
     sizeof(1+2);

  // Example of expression in sizeof operator
#if defined(__clang__)
  // I think that Clang is correct to require this, since it is a single character hex literal.
     int x = sizeof('\xff');
#else
     int x = sizeof('\x1234567890123456789012345678901234567890');
#endif

  // DQ (6/12/2011): Added additional test...
     char name[100] = "very long name...";
     sizeof(name);
   }
