namespace Y
   {
     namespace Z { void foo(); }
     void foobar();
   }

void foobarA()
   {
     using namespace Y::Z;

     foo();
   }



