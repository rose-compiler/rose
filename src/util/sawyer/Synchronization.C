#include <sawyer/Synchronization.h>

namespace Sawyer {

// In order to get thread safety, you probably need to add "-pthread" to all GCC compile and link lines.  It is not sufficient
// to only add "-lpthreads" to the link lines.
#if SAWYER_MULTI_THREADED == 0
# ifdef _MSC_VER
#  pragma message("Sawyer multi-threading is disabled; even documented thread-safe functions are unsafe!")
# else
#  warning "Sawyer multi-threading is disabled; even documented thread-safe functions are unsafe!"
# endif
#endif

static SAWYER_THREAD_TRAITS::RecursiveMutex bigMutex_;

// thread-safe
SAWYER_EXPORT SAWYER_THREAD_TRAITS::RecursiveMutex&
bigMutex() {
    return bigMutex_;
}

} // namespace
