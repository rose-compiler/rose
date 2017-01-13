
int __attribute__ ((always_inline)) foo1()
   {
     return 0;
   }

int __attribute__ ((noinline)) foo2()
   {
     return 0;
   }

int __attribute__ ((pure)) foo3()
   {
     return 0;
   }

int main()
   {
     return foo1() + foo2() + foo3();
   }

