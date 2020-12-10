
// associated issue is ROSE-1620.

// I can't demonstrate that the source position is incorrect for any of this, except 
// that the associated SgAssignInitializer has an incorrect ending position (however, 
// the starting position is correct).  The SgAssignInitializer ending position should 
// be fixed, but it is still correctly marked as compiler generated (so it might be
// a lower priority detail).  Need to discuss this further.

struct A 
   {
     static const int v = 0;
   };

const int A::v;

