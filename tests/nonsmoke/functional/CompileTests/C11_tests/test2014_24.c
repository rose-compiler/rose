
// GNU 4.8 does not support this header file yet.
// -D__GNUC__=4 -D__GNUC_MINOR__=8 -D__GNUC_PATCHLEVEL__=1
// #if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4

// DQ (1/18/2017): The threads.h header file is optional in 
// C11 and not implemented in any C compiler that I know of.
// #ifndef __STDC_NO_THREADS__
#if 0
  #include<threads.h>
#endif

