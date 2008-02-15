// This test code demonstrates two bugs in current version of ROSE
// Both have to do with the newly fixed defining vs. nondefining declaration handling.
// the array problem is like because the fix to handle the declaration output
// was overly simpleified.

// Class to be used in examples below
class A {};

// Test use of array
A x[10];

// This is an error in the unparser (unparses to "typedef class A *[10]A_Type;")
typedef A* A_Type[10];

// This also causes an error in the unparser: "Error: declStmt->isForward() == false (should be true)"
A_Type X;

// mixing unnamed types in typedefs with arrays (put this into test2004_105.C)
// typedef struct { int x; } ArrayType [100];
