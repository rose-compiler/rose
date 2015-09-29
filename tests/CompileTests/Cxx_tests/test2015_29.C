
#include "test2015_29.h"

// "inline" keyword will not be output in generated code.
//    This is likely because it was specified to use the GNU attribute "always_inline" in the function prototype.
//    However, the GNU 4.2.4 compiler can required this (e.g for Google protobuffer application), though this example 
//    code does not demostrate the error. The error only appears to happen on the Jenkins test on RHEL5 using the 
//    GNU 4.2.4 compiler.  On my RHEL6 (desktop) system it works fine. Google protobuffer application also compiles 
//    fine on RHEL6 using GNU 4.2.4 compiler.
inline int X::foo1()
   {
     return 0;
   }

#if 0
void X::foobar()
   {
     int a = foo1();
   }
#endif
