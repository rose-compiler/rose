// Markus Kowarschik, 10/2002
// Error: If a header file contains a class declaration with a statically declared
// member variable and the source file (which includes this header file) declares
// this variable, the resulting AST contains two SgMemberVariableDeclaration
// objects both of which have the same file information. (This is probably just
// one example where the file information is not correct.)
// Other example where I recognized this problem:
// TESTS/CompileTests/A++Code/data_hash.h,
// TESTS/CompileTests/A++Code/data_hash.C

#include "mkTest_04.h"

int A::staticVar= 0;

int
main(void)
{
  return 0;
}
