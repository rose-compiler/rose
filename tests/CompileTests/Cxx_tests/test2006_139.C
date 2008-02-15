
void foo()
   {
     sizeof(int);
     sizeof(long);
     sizeof("string");
     sizeof(1+2);

  // Example of expression in sizeof operator
     int x = sizeof('\x1234567890123456789012345678901234567890');
   }
