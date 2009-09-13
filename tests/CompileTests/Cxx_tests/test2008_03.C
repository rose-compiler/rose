void foobar()
   {
     int a,b,c;

  // In C this means
     a ? b : (c = 3);

  // In C++ this means
     (a ? b : c) = 3;

  // What does this mean in ROSE? For C and C++, do we get it right?)
     a ? b : c = 3;
   }

