// Sequence of difficult test problems for ROSE (tests the unparser)
// All have to do with the declaration of structures (struct and class 
// constructs).

// This needs to be defined in a separate file so that if it is unparsed
// without the 2nd part of the type, "[10]", the error will be caught by
// the backend compiler.
#include "test2003_08.h"
Test_2nd_Part_Of_Type* Test_2nd_Part_Of_Type::PointerArray [10];
