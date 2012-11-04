// GCC 4.4 and 4.5 cannot compile the code properly
// Liao, 11/2/2012
 int x;

#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                       (__GNUC_MINOR__ == 4 && \
                                             __GNUC_PATCHLEVEL__ >= 0)))

 //gcc version >=4.4.0
void foo(int x, int y);
#else
 //gcc version <4.4.0
void foo(int x, int y = x);

#endif

void foo(int x, int y)
   {
   }

