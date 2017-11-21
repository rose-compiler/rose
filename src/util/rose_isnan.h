#ifndef ROSE_isnan_H
#define ROSE_isnan_H

#if 0
  #warning "__GNUC__ = "__GNUC__
  #ifdef __GNUC__
    #warning "__GNUC__ is defined = "__GNUC__
  #else
    #warning "__GNUC__ is not defined"
  #endif
#endif

#if (__GNUC__ < 6)
#include <cmath>

// #warning "__GNUC__ < 6"
// #warning "__cplusplus = "__cplusplus

// MS 2016: isnan is available in C++11 std namespace, but not in C++98
// std namespace. On *some* systems C++11 cmath puts it inside *and* outside the std
// namespace.
#if __cplusplus >= 201103L
  #define rose_isnan(x) std::isnan(x)
#else
  #ifdef _MSC_VER
  #include <float.h> // for _isnan
    #define rose_isnan(x) _isnan(x)
  #else
    #define rose_isnan(x) isnan(x)
  #endif
#endif

#else

// #warning "__GNUC__ >= 6 (or not defined)"

#include <math.h>

// DQ (11/16/2017): This is not defined properly for the Intel compiler when qualified
// because it will be expanded to an expression:
// std::(( sizeof( x ) > sizeof( double )) ? __isnanl( (long double)(x) ) : (( sizeof( x ) == sizeof( float )) ? __isnanf( (float)(x) ) : __isnan( (double)(x) ) ) ).
// and qualification of such an expression is not legal syntax.
// #define rose_isnan(x) std::isnan(x)
#ifdef __INTEL_COMPILER
  #define rose_isnan(x) isnan(x)
#else
  #define rose_isnan(x) std::isnan(x)
#endif

#endif 

#endif
