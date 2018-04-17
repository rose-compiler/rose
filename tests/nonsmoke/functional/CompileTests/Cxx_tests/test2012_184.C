// void foo(int x, int y = x);
void foo(int x, int y);

// int x = 0;

// void foo(int x,int y = x)
void foo(int x,int y)
{
}

int x = 0;

#if !defined(__clang__)
#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                     (__GNUC_MINOR__ == 4 && \
                      __GNUC_PATCHLEVEL__ >= 0)))

  // printf("gcc version >=4.4.0\n");
  void foo(int x, int y);
#else
  void foo(int x, int y = x);
#endif
#else
  // printf("clang and gcc version >=4.4.0\n");
  void foo(int x, int y);
#endif
