// This is a simpler case of test2004_145.C (which is also failing).
class X
   {
     public:
          static void foo();
       // void foo();
   };

void foobar()
   {
  // X a;
  // a.foo();
     X::foo();
   }


