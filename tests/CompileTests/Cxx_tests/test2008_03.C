void foobar()
   {
     int a,b,c;

  // In C this means
     a ? b : (c = 3);

  // In C++ this means
     (a ? b : c) = 3;

  // What does this mean in ROSE (for C and C++)?
     a ? b : c = 3;
   }

