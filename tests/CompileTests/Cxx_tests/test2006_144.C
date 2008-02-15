
class X
   {
     public:
          int x2;
          int x3;
   };

void foo( int x );
X* bfb();

int SideEffect = 0;

void foobar( int x )
   {
     foo(x++);
     X* Xvar;
  // foo( bfb()->x3 += (SideEffect++, bfb())->x2 );
  // bfb()->x3 += (SideEffect, bfb())->x2;
  // bfb()->x3 += bfb()->x2;
  // (Xvar   ,   Xvar)->x2 = 0;
     (SideEffect   ,   Xvar)->x2 = 0;
   }


