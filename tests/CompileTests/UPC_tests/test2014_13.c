shared int* foobar()
   {
#if 0
     shared int* foobar_ptr;

     foobar_ptr = 0L;

     return foobar_ptr;
#else
     return 0L;
#endif
   }

void foo()
   {
#if 0
     shared int* ptr;
     ptr = foobar();
#endif
   }
