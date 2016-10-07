extern int *__errno_location (void) __attribute__ ((__nothrow__)) __attribute__ ((__const__));

int foobar()
   {
     int n;
#if 0
     while (1)
        {
          n = 0;
#endif
          if (n > 0) 
             {
               char *iptr;
               for (;;) 
                  {
                    int newn = 0;
                    if (newn < 0) 
                       {
                         if ((*__errno_location ()) != 4 ) 
                            {}
                         continue;
                       }
                  }
             }
#if 0
        }
#endif

     return 0;
   }

