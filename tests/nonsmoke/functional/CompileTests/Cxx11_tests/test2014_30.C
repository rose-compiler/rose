// This is a test code from Cxx_tests that is an issue for Reed's deleteAST
// implementation.  He has implemented a work around to not search for a symbol
// if it does not exist.  The point in this code is there should likely 
// be a symbol (but it is not found thorugh the 2nd declaration, because it
// is not being properly marked with a reference to the first non-defining 
// declaration.
namespace s = std;

// This second declaration is allowed, but a symbol will not be built 
// for it and the declaration does not point to the previous declaration 
// as being the first non-defining declaration.
namespace s = std;

