// DQ (4/12/2005):
// Note that in the compilation of the generated code by the g++ 3.4.3 and 3.3.2 
// compiler it complains that we should not use the ".h" header file extension.

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include<iostream.h>

int main()
   {
  // Test output operator
     cout << "Output tests string" << endl;

#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     cout << "Output tests string" << endl;
#endif
#endif

  // std::endl(cout);

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

