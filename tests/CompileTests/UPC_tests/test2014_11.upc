shared int* foobar()
   {
     shared int* foobar_ptr;

     foobar_ptr = 0L;

     return foobar_ptr;
   }

void foo()
   {
     shared int* ptr;
     ptr = foobar();
   }
