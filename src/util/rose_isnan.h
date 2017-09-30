#ifndef ROSE_isnan_H
#define ROSE_isnan_H


#if (__GNUC__ < 6)
#include <cmath>

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
#include <math.h>
  #define rose_isnan(x) std::isnan(x)

#endif 

#endif
