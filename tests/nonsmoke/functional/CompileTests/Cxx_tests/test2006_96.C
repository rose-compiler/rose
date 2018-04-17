// Test of use of C++ typeid function
/*
Lvalue-to-rvalue, array-to-pointer, and function-to-pointer conversions will not be applied 
to expr. For example, the output of the following example will be int [10], not int *:


Possible error, the generated code is:
  int myArray[10];
  ( *((&std::cout))<<(typeid(int [10])).name())<<std::endl;
*/

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>
#include <typeinfo>
using namespace std;

int main() {
  int myArray[10];
  cout << typeid(myArray).name() << endl;
}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

