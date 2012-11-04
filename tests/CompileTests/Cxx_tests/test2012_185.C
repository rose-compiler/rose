void foo(int x, int y);

int x = 9;

void foo(int x, int y);

#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                       (__GNUC_MINOR__ == 4 && \
                                             __GNUC_PATCHLEVEL__ >= 0)))
   
 //gcc version >=4.4.0
void foo(int x, int y)
   {
   }

#else
 //gcc version <4.4.0
void foo(int x, int y = x)
   {
   }
#endif


