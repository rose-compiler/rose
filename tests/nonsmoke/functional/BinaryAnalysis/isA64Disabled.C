// Used to conditionally disable ARM testing.
#include <featureTests.h>
#include <iostream>

int main() {
#ifndef ROSE_ENABLE_ASM_A64
    std::cout <<"no ARM AArch64 A64 support\n";
#endif
}
