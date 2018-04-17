
namespace A
   {
     void foo();
   }

namespace B
   {
     void foo();
   }

// use qualified function name to define functions
void A::foo() {}
void B::foo() {}
