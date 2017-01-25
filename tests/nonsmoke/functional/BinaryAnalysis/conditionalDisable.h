// Used to conditionally disable binary tests.
#ifndef ROSE_BinaryAnalysis_ConditionalDisable_H
#define ROSE_BinaryAnalysis_ConditionalDisable_H

// GCC 4.8.3 c++11 has problems compiling some header that's used by binary analysis. The compiler reports internal errors.
#if defined(__GNUC__) && defined(__cplusplus) && \
    __GNUC__ == 4 && __GNUC_MINOR__ == 8 && __GNUC_PATCHLEVEL__==3 && __cplusplus == 201103ul
    #define ROSE_BINARY_TEST_DISABLED "GCC-4.8.3 c++11 internal errors"

#elif 0
    // used to test that disablement is actually working as expected
    #define ROSE_BINARY_TEST_DISABLED "testing disablement"

#endif

#endif
