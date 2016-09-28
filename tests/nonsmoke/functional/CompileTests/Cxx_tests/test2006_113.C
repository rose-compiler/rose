
int x;
int y;

// This is a error in C, but works fine in C++
// Note the operator> returs a "bool" type in C++, but an "int" type in C.
// int z = x > y;

void foo()
   {
     int z = x > y;
   }
