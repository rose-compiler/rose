// This test code demonstates the use of forward declarations
// it is designed so that an error in how the source position
// will cause a lagitimate erro in the compilation.

#if 0
// This will build a SgClassDeclaration and a SgClassDefinition
// where the class definition is marked as compiler generated.
template <typename T> class Y;
#include "test2007_47.h"

// This will build a new SgClassDeclaration, and reuse the existing
template <typename T> class Y {};

#endif


// before first declaration
class Y;
// after first declaration

// before defining declaration
class Y {};
// after defining declaration
