// #include <stdint.h>
typedef unsigned long int uint64_t;

void f() 
   {
     uint64_t  a;

  // What is the return result of sizeof() operator (should be size_t, but Philippe reports it return "int").
     uint64_t  n = sizeof(a) / 2;
   }
