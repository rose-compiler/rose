// Illustration of compiler <complex> class.

// Notice that we must write #include <complex> with angle brackets and not 
// with quotes, as we did when we wanted our own version. In this illustration 
// we have defined our own shorthand dcmplx for a double complex number, 
// working with the template. However, we could have just as well used the 
// double_complex type that is already typedef'ed to complex<double> in the 
// complex header file.

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
#include <complex>
using namespace std;

typedef complex<double> dcmplx;

int main(){

  dcmplx a(5.,6.),b;

  cout << "Enter b: ";
  cin >> b;

  cout << "a = " << a << "\n";
  cout << "b = " << b << "\n";

  cout << "a + b = " << a + b << "\n";
  cout << "a * b = " << a * b << "\n";
  cout << "a / b = " << a / b << "\n";
  cout << "|a| = "   << abs(a) << "\n";
  cout << "complex conjugate of a = " << conj(a) << "\n";
  cout << "norm of a = " << norm(a) << "\n";
  cout << "abs of a = " << abs(a) << "\n";
  cout << "exp(a) = " << exp(a) << "\n";
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

