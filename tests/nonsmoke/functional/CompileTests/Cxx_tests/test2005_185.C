// This test code tests the use of the C++ complex class.
// It was a special problem for ROSe since the g++ implementation
// of the complex<float>, complex<double>, and complex<long double>
// use the GNU speicific "__complex__", "__real__", and "__imag__" 
// type modifiers extensions; which are not supported by EDG.
// The fix it to #define each of the type modifiers to "" and
// then edit the complex header file to modify one of the 
// private constructors which used the _ComplexT type (which is 
// a typedef of a __complex__ float, double, long double type).

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include<complex>

std::complex<double> x;

// _Complex double x;

float a;
double b;
long double c;

// I had to comment out a private constructor in g++ complex header file
// so that this cwould compile.  Using the "explicit" keyword with the
// private constructor allowed the other examples to compile.  The 
// private constructor is a non-standard part fo the interface 
// (which is not hidden well enough just be cause it is static).
// The complex header file is modified as part of the ROSE configuration
// where the backend header files are taken and modified for use by EDG.
// This processing is specific to g++ header files, where:
//      "complex(_ComplexT __z) : _M_value(__z) { }"
// is replaces with:
//      "// ***commented out for ROSE*** complex(_ComplexT __z) : _M_value(__z) { }"
std::complex<double> x1(1.0);

std::complex<double> x2 = 1.0;

std::complex<double> x3(1.0,0.0);
std::complex<double> x4 = std::complex<double>(1.0,0.0);

void foo( std::complex<double> y = 1.0 )
   {
     y *= y;
     y /= y;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

