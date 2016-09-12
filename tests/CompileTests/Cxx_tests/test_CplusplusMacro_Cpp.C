// The __cplusplus should be defined for both C and C++ 
// modes or ROSE when using filename with C++ extension.
// Where it is defined is should have the value "1" 
// (typically the default value for a defined macro not 
// assigned an explicit value).
#ifdef __cplusplus
  #warning "__cplusplus defined"
  #if (__cplusplus == 0)
    #warning "__cplusplus == 0"
  #else
    #if (__cplusplus == 1)
      #warning "__cplusplus == 1"
    #else
      #warning "__cplusplus not 0 or 1, value unknown"
    #endif
  #endif
#else
  #error "__cplusplus NOT defined"
#endif

// DQ (11/29/2006): Added ROSE_LANGUAGE_MODE macro for use in:
//      rose_edg_required_macros_and_functions.h.
// Note that ROSE_LANGUAGE_MODE should always be defined since we require it to
// communicate to the ROSE spsecific header files if we are in C or C++ mode.
// Note that the macro __cplusplus macro is not enough since it is true when
// compiling in C mode if the source filename uses a C++ file name extension
// (suffix).
#ifdef ROSE_LANGUAGE_MODE
  #if (ROSE_LANGUAGE_MODE == 0)
    #warning "This is the ROSE C mode, independent of the setting of the __cplusplus Macro"
  #else
    #if (ROSE_LANGUAGE_MODE == 1)
      #warning "This is the ROSE C++ mode, independent of the setting of the __cplusplus Macro"
    #else
      #error "This is an unknown ROSE language mode, independent of the setting of the __cplusplus Macro"
    #endif
  #endif
// #else
//  #warning "ROSE_LANGUAGE_MODE is defined for internal ROSE use only, and is not defined for the vendor compiler.
#endif

// DQ (9/10/2016): Checking how _GLIBCXX_CONSTEXPR is being set for C++11 support.
// #include <bits/c++config.h>
#include "rose_c++config.h"

#ifdef _GLIBCXX_CONSTEXPR
   #warning "_GLIBCXX_CONSTEXPR IS defined"
#else
   #warning "_GLIBCXX_CONSTEXPR is NOT defined"
#endif

#if (_GLIBCXX_CONSTEXPR == constexpr)
   #warning "_GLIBCXX_CONSTEXPR IS defined to be constexpr"
#else
   #warning "_GLIBCXX_CONSTEXPR is NOT defined to be constexpr"
#endif

#warning "_GLIBCXX_CONSTEXPR " _GLIBCXX_CONSTEXPR


inline _GLIBCXX_CONSTEXPR double
abs(double __x)
   {
     return __builtin_fabs(__x);
   }
