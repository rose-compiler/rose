
namespace X
   {
     int a;
   }

using namespace X;

namespace X
   {
     int b;
   }

int global_1 = a;

// This demonstrates that when b was built, it was added 
// to the global scope using the previous using declaration.
int global_2 = b;


