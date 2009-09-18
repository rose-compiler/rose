void assert(int a);

int test(int x)
   {
     assert(x < x*2);
     return 0;
   }
