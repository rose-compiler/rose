//
void foo(int x, int y);

int x = 57;

#if !defined(__clang__)
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
#else
 // clang and gcc version >=4.4.0
void foo(int x, int y);
#endif

void foo(int x, int y)
   {
   }
