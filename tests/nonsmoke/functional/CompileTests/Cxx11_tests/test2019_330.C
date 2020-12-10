// I think this test code demonstrates that the SgPointerMemberType should 
// store a reference to the SgClassDeclaration, instead of the SgClassDefinition.

namespace N 
   {
  // This is an internal error in ROSE (without the definition).
     struct DEF; // {};
   }

void foobar( int (N::DEF::*pointer_to_object) )
   {
   }
