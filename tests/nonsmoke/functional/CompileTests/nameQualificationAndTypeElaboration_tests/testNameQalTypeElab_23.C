// number #23

namespace X
   {
     class X1{};
   }

void foo()
   {
  // using namespace X;
     using X::X1;
     
     {
        
     class X1 {};

     X1 a1;
     X::X1 a2;
     }
     
   }
