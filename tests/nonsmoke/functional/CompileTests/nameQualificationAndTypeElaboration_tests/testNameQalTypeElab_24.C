// number #24

namespace X
   {
     class X1{};
   }
namespace Y
   {
     using namespace X;
     class X1{};
   }

void foo()
   {
  // using namespace X;
  // using X::X1;
     using namespace X;
     
     {
        
//     class X1 {};

     Y::X1 a1;
     X::X1 a2;
     }
     
   }
