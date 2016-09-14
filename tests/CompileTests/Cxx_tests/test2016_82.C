// DQ (9/10/2016): Checking how _GLIBCXX_CONSTEXPR is being set for C++11 support.
#include <bits/c++config.h>
// #include "rose_c++config.h"

#ifdef _GLIBCXX_CONSTEXPR
   #warning "_GLIBCXX_CONSTEXPR IS defined"
#else
   #warning "_GLIBCXX_CONSTEXPR is NOT defined"
#endif

#if 0
#if (_GLIBCXX_CONSTEXPR == constexpr)
   #warning "_GLIBCXX_CONSTEXPR IS defined to be constexpr"
#else
   #warning "_GLIBCXX_CONSTEXPR is NOT defined to be constexpr"
#endif
#endif

#warning "_GLIBCXX_CONSTEXPR " _GLIBCXX_CONSTEXPR

#if 0
// DQ (9/13/2016): This does not work for older compilers.
inline _GLIBCXX_CONSTEXPR double
abs(double __x)
   {
     return __builtin_fabs(__x);
   }
#endif
