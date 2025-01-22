#include <featureTests.h>
#include <iostream>

static void
binaryAnalysisFeatures() {
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    std::cout <<"Boost serialization (ROSE_ENABLE_BOOST_SERIALIZATION)                  enabled\n";
#else
    std::cout <<"Boost serialization (ROSE_ENABLE_BOOST_SERIALIZATION)                  disabled\n";
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    std::cout <<"Binary analysis (ROSE_ENABLE_BINARY_ANALYSIS)                          enabled\n";
#else
    std::cout <<"Binary analysis (ROSE_ENABLE_BINARY_ANALYSIS)                          disabled\n";
#endif

#ifdef ROSE_ENABLE_ASM_AARCH32
    std::cout <<"  ARM AArch32 decoding (ROSE_ENABLE_ASM_AARCH32)                       enabled\n";
#else
    std::cout <<"  ARM AArch32 decoding (ROSE_ENABLE_ASM_AARCH32)                       disabled\n";
#endif

#ifdef ROSE_ENABLE_ASM_AARCH64
    std::cout <<"  ARM AArch64 decoding (ROSE_ENABLE_ASM_AARCH64)                       enabled\n";
#else
    std::cout <<"  ARM AArch64 decoding (ROSE_ENABLE_ASM_AARCH64)                       disabled\n";
#endif

#ifdef ROSE_ENABLE_DEBUGGER_LINUX
    std::cout <<"  Linux PTRACE debugging (ROSE_ENABLE_DEBUGGER_LINUX)                  enabled\n";
#else
    std::cout <<"  Linux PTRACE debugging (ROSE_ENABLE_DEBUGGER_LINUX)                  disabled\n";
#endif

#ifdef ROSE_ENABLE_DEBUGGER_GDB
    std::cout <<"  GDB debugging interface (ROSE_ENABLE_DEBUGGER_GDB)                   enabled\n";
#else
    std::cout <<"  GDB debugging interface (ROSE_ENABLE_DEBUGGER_GDB)                   disabled\n";
#endif

#ifdef ROSE_ENABLE_CONCOLIC_TESTING
    std::cout <<"  Binary concolic testing (ROSE_ENABLE_CONCOLIC_TESTING)               enabled\n";
#else
    std::cout <<"  Binary concolic testing (ROSE_ENABLE_CONCOLIC_TESTING)               enabled\n";
#endif

#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION
    std::cout <<"  Fast library identification (ROSE_ENABLE_LIBRARY_IDENTIFICATION)     enabled\n";
#else
    std::cout <<"  Fast library identification (ROSE_ENABLE_LIBRARY_IDENTIFICATION)     disabled\n";
#endif

#ifdef ROSE_ENABLE_MODEL_CHECKER
    std::cout <<"  Binary model checking (ROSE_ENABLE_MODEL_CHECKER                     enabled\n";
#else
    std::cout <<"  Binary model checking (ROSE_ENABLE_MODEL_CHECKER                     disabled\n";
#endif
}

int main() {
    binaryAnalysisFeatures();
}
