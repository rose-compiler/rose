class A {};

A x;

void foo()
   {
     float x;

  // Global qualification for "x" is required here.
     A y = ::x;
   }
