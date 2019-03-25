// This test code only fails for ROSE (in the backend 
// compilation) when compiled using GNU 6.1 (not GNU 5.1).

class X { };

void foobar()
   {
     int X::*pointerToMember = 0;
     pointerToMember == (2 * (1 - 1));
     pointerToMember == static_cast<int>(0.0);
   }

