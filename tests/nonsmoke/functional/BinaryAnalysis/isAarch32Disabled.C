// Used to conditionally disable ARM AArch32 testing.
#include <featureTests.h>
#include <iostream>

int main() {
#ifndef ROSE_ENABLE_ASM_AARCH32
    std::cout <<"no ARM AArch32 support\n";
#endif
}
