// Not that specification of noreturn with a function that has a 
// return value causes a segfault when run (and a warning when compiled).

#if 1

int foo1();

int __attribute__ ((noreturn)) foo1();
// int __attribute__ ((noreturn)) foo1();
// int __attribute__ ((noreturn)) foo1();
#endif

int __attribute__ ((noreturn)) foo1()
   {
     return 47;
   }

int main()
   {
     return foo1();
   }
