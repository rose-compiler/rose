
#if 0
int foo1();
int __attribute__ ((pure)) foo1();
int __attribute__ ((always_inline)) foo1();
int __attribute__ ((no_inline)) foo1();
#endif

int __attribute__ ((always_inline)) foo1()
   {
     return 0;
   }

#if 0
int __attribute__ ((noinline)) foo2();
int __attribute__ ((noinline)) foo2()
   {
     return 0;
   }

int __attribute__ ((pure)) foo3();
int __attribute__ ((pure)) foo3()
   {
     return 0;
   }

__attribute__ ((pure)) int foo4();

__attribute__ ((pure)) int foo4()
   {
     return 0;
   }

int foo5() __attribute__ ((pure));
#endif

// Note that information about the location of the attribute within the function declaration is not avaiable in ROSE.
__attribute__ ((always_inline)) int foo6()
   {
     return 0;
   }

int foo7() __attribute__ ((always_inline));

#if 0
// This is not allowed
int foo7() __attribute__ ((always_inline)) 
   {
     return 0;
   }
#endif

#if 0
int main()
   {
     return foo1() + foo2() + foo3();
   }
#endif

