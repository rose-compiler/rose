// Example from Peter of how function parameters are used to resolve functions
// even when they are not explicitly qualified.  In the code below,
// "foo(c)" is resolved to "X::foo(c)".  The reason why explicit
// name qualification is not required is because overloaded
// operators should not require explicit namespace qualification
// (and no using directive should be required to use overloaded
// operators).

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>

struct S {};

void foo(S& s) { std::cout << "foo" << std::endl; }

namespace X
   {
     struct C : S {};

     void foo(C& c) { std::cout << "X::foo" << std::endl; }
   }

int main()
   {
     X::C c;

     X::foo(c);
     ::foo(c);
     foo(c);
     (foo)(c);
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

