// This test code demonstates the use of forward declarations
// it is designed so that an error in how the source position
// will cause a lagitimate erro in the compilation.

// This will build a SgClassDeclaration and a SgClassDefinition
// where the class definition is marked as compiler generated.
class Y;
#include "test2007_41.h"

// Redundant forward class declaration
class Y;
class Y;

// This will build a new SgClassDeclaration, and reuse the existing
// SgClassDefinition (???).
class Y {};

// Redundant forward class declaration
class Y;
class Y;


