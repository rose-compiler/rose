int foo(int n, int array[sizeof(n)]);

void foobar()
   {
     static int array[2] = {5, 6};

  /* foo(1, a); */
     foo(2, a);
   }
