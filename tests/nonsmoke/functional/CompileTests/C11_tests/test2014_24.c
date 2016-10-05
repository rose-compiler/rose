
// GNU 4.8 does not support this header file yet.
// -D__GNUC__=4 -D__GNUC_MINOR__=8 -D__GNUC_PATCHLEVEL__=1
#if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4
  #include<threads.h>
#endif

