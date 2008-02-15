// Note that this test is correct, but since it specifies something that is an non-standard extension 
// to C (and C++) and one nott supported by the GNU compilers, the generated code fails to compile.

// This test code demonstates the use of forward declarations of enum (non-standard C and C++)
// it is designed so that an error in how the source position will cause a legitimate error in 
// the final compilation (of the generated code).

// Test source position of forward declaration of enum (not legal C, 
// but accepted by many compilers).  We want it to work.
enum numbers;
#include "test2007_42.h"

enum numbers;

// This is the definition;
enum numbers {};

// It appears that this redundant enum declaration is present in EDG, built as an IR node
// in the translation from EDG, but not output in the unparsed code in ROSE. This is not
// a crisis, since it is redundant and meaningless (as best I can tell).
enum numbers;
