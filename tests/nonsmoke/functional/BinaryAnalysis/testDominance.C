// Print the CFG immediate dominators for each vertex of each function's CFG
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <tutorial/binaryDominance.C>

#endif
