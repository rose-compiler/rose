// This is a reduced version of test2005_114.C

void foo ()
   {
  // This does not unparse to be ANYTHING, it is ignored (perhaps as it should be)
     union {};
  // union { int x; int y; };
   }

