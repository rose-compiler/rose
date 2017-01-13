// number #16

class X1 {};
namespace X
   {
     class X1 {};
     namespace X
        {
          class X1 {};

       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( X::X1 x1, X1 x2 )
             {
             }

       // EDG thinks this is different from void foo( X::X1 x1, X1 x2 ), 
       // but g++ correctly identifies that it is a redefinition.
       // void foo( ::X::X1 x1, ::X::X::X1 x2 )
          void foo( ::X::X1 x1, ::X::X::X1 x2, int x3 )
             {
             }
        }
   }

void foo( X::X1 x1, X::X::X1 x2 )
   {
   }


