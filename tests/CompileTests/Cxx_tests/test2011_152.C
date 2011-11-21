// Sequence of difficult test problems for ROSE (tests the unparser)
// All have to do with the declaration of structures (struct and class 
// constructs).

// This needs to be defined in a separate file so that if it is unparsed
// without the 2nd part of the type, "[10]", the error will be caught by
// the backend compiler.
// #include "test2003_08.h"

class X
   {
  // static X* PointerArray [10];
  // static int PointerArray [10];
     static int value;
   };

// X* X::PointerArray [10];
// int X::PointerArray [10];
int X::value;
