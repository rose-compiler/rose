void assert(int);

int test(int x)
   {
     assert(x >= 0 || x < 0);
     return 0;
   }
