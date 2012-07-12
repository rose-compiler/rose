namespace X
   {
     class A {};
   }

X::A x;

void foo()
   {
     float x;

  // Global qualification for "x" is required here.
     X::A y = ::x;
   }
