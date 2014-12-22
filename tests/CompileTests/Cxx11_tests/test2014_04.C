// Example of C11 atomic use (not yet supported in EDG).
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 8 || __GNUC__ > 4

// This is ony required for testing when we short circuit the ROSE builtin header files (compiler defines).
// #define __GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1

#include <atomic>
std::atomic<int> counter;
#endif

