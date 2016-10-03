shared int* foobar()
   {
     shared int* foobar_ptr;

     foobar_ptr = 0L;

     return foobar_ptr;
   }

void foo()
   {
     int i;

     shared int* ptr;
     ptr = foobar();

  // Dereference of function return value.
     i = *foobar();

     *foobar() = i;
   }
