void foobar()
   {
  // null pointer-to-member different from pointer to any member
     struct B { int bi; };
     int B::*bpm1 = 0;
     int B::*bpm2 = &B::bi;
   }
