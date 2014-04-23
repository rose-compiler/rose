#include <math.h>

#include "/usr/include/math.h"

// #include <stdio.h>

#ifdef __USE_BSD
  #warning "__USE_BSD IS defined!"
#else
  #warning "__USE_BSD is NOT defined!"
#endif

#ifdef __USE_XOPEN
  #warning "__USE_XOPEN IS defined!"
#else
  #warning "__USE_XOPEN is NOT defined!"
#endif

#if defined(__PURE_SYS_C99_HEADERS__)
  #warning "__PURE_SYS_C99_HEADERS__ IS defined!"
#else
  #warning "__PURE_SYS_C99_HEADERS__ is NOT defined!"
  #if !defined(__PURE_INTEL_C99_HEADERS__)
    #warning "__PURE_INTEL_C99_HEADERS__ is NOT defined!"
  #endif
#endif

// #include <my_math.h>

int main(int argc, char *argv[])
{
// printf("%f\n", M_PI*exp(3.0)) ;
// printf("%f\n", exp(3.0)) ;

   float x = M_PI*exp(3.0);
   return 0 ;
}

