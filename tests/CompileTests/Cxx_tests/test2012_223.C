// This is a simpler version of test2012_106.C

void foo (int x)
   {
     class A0 {};

  // Note that the scope of the class declaration apepars to be the SgBasicBlock (this will have to be fixed).
  // for (class A0 {public: int foo (int x) {return x;}} x2; x2.foo (0); x)
     for (class A0 {} x2; true; x)
        {
        }
     

   }
