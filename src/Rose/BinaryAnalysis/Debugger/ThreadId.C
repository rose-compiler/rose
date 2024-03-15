#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/ThreadId.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

ThreadId
ThreadId::unspecified() {
    return ThreadId();
}

} // namespace
} // namespace
} // namespace

#endif
