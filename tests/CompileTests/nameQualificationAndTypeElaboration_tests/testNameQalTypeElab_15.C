class X1 {};
namespace X
   {
     class X1 {};
     namespace Y
        {
          class X1 {};
        }
   }

void foo( X::X1 x1, X::Y::X1 x2 )
   {
   }
