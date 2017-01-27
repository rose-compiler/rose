// Used to conditionally disable binary tests.
#include "conditionalDisable.h"
#include <iostream>

int main() {
#ifdef ROSE_BINARY_TEST_DISABLED
    std::cout <<ROSE_BINARY_TEST_DISABLED <<"\n";
#endif
}
