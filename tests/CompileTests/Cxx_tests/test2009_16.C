// THIS TEST SHOULD FAIL

// Test compilation of a file with a header file that does not exist.
// We want to test how it fails, and have it fail in ROSE rather than 
// in EDG, so that we can use the exception handling to catch it.
#include <fail.h>
