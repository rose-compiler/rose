/* Prints information about the division of a binary executable into functions.
 *
 * Usage: binaryFunctionBoundary a.out
 *
 * Note: most of the code that was here has been migrated into src/frontend/Disassemblers/detectFunctionStarts.C on 2009-01-22. */
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <AsmFunctionIndex.h>

using namespace rose::BinaryAnalysis;

int
main(int argc, char *argv[]) {
    SgProject *project = frontend(argc, argv);
    std::cout <<"Functions detected from binary executable:\n"
              <<AsmFunctionIndex(project).sort_by_entry_addr();
}

#endif
