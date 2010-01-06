class X1 {};
class X
   {
     public:
     class X1 {};
     class X2
        {
          public:
          class X1 {};
          class X 
             {
               public:
               class X1 {};
             };

       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( X::X1 x )
             {
             }

       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( ::X::X1 x )
             {
             }

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
        };
   };

void foo( X::X1 x1, X::X2::X1 x2 )
   {
   }

