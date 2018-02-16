#ifndef ROSE_isnan_H
#define ROSE_isnan_H

#if __GNUC__ < 6
    #include <cmath>

    // MS 2016: isnan is available in C++11 std namespace, but not in C++98
    // std namespace. On *some* systems C++11 cmath puts it inside *and* outside the std
    // namespace.
    #if __cplusplus >= 201103L
        #if __GNUC__ == 4
            // The qualified "std::isnan" will not parse correctly with GCC-4.8.5 C++11 because isnan expands to
            // "__builtin_isnan" which is not a member of "std" namespace.
            //#define rose_isnan(x) std::isnan(x)
            #define rose_isnan(x) isnan(x)
        #else
            #define rose_isnan(x) std::isnan(x)
        #endif
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

    // DQ (11/16/2017): This is not defined properly for the Intel compiler when qualified
    // because it will be expanded to an expression:
    // std::(( sizeof( x ) > sizeof( double )) ? __isnanl( (long double)(x) ) : (( sizeof( x ) == sizeof( float )) ?
    // __isnanf( (float)(x) ) : __isnan( (double)(x) ) ) ).
    // and qualification of such an expression is not legal syntax.
    // #define rose_isnan(x) std::isnan(x)
    #ifdef __INTEL_COMPILER
        #define rose_isnan(x) isnan(x)
    #else
        #define rose_isnan(x) std::isnan(x)
    #endif

#endif 

#endif
