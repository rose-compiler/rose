// Used to conditionally disable ARM AArch64 testing.
#include <featureTests.h>
#include <iostream>

int main() {
#ifndef ROSE_ENABLE_ASM_AARCH64
    std::cout <<"no ARM AArch64 support\n";
#endif
}
