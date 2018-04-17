#if 1
int foo1();
int __attribute__ ((pure)) foo1();
int __attribute__ ((always_inline)) foo1();
int __attribute__ ((no_inline)) foo1();
#endif

int __attribute__ ((always_inline)) foo1()
   {
     return 42;
   }

int main()
   {
     return foo1();
   }
