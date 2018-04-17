
namespace X
   {
     int a;
   }

using namespace X;

// This is allowed, and any use of "a" must be qualified (at least partially qualified).
int a;

namespace X
   {
     int b;
   }

// Note that "::a" or "X::a" is required but that ROSE does not 
// compute that the qualified name is required.
int global_1 = ::a;

// This demonstrates that when b was built, it was added 
// to the global scope using the previous using declaration.
int global_2 = b;


