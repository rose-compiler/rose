// This fails because a a loop detected in the parents:
// Error: node is already in set and defines a cycle: node = 0x7fe5a918f010 = SgFunctionParameterList 
//  --- seen element: element = 0x7fe5a79a1078 = SgAssignInitializer 
//  --- seen element: element = 0x7fe5a79d4160 = SgFunctionCallExp 
//  --- seen element: element = 0x7fe5a7a0b148 = SgExprListExp 
//  --- seen element: element = 0x7fe5a7a3e0f0 = SgLambdaExp 
//  --- seen element: element = 0x7fe5a904ca18 = SgInitializedName 
//  --- seen element: element = 0x7fe5a918f010 = SgFunctionParameterList 
// Exiting after error! 

// To deomonstrate the error: Need to alternatively use Robb's (better) version
// of the template implementation for getEnclosingNode() in sageInterface.h.
// The reason why Robb's version is better is because it saves the parents 
// visited to a list which defines a better cycle detection.
// However, it detects errors in files that have been passing for a long
// time, so we need to retrofit this test into place as part of a 
// seperate issue to be debugged.

namespace B 
   {
  // The g++ backend compiler requires the function definition instead of the function declaration.
  // template <class T> int c(T const& t);
     template <class T> int c(T const& t) { return 0; }

     template <class T> int e(int = c( []{} ));
   }

void foo() 
   { 
     B::e<long>();
   }
