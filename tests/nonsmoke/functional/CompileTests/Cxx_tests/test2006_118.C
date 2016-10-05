
// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include<complex>

::std::complex<double> foo;

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

