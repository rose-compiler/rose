#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Settings.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ConcolicExecutorSettings::~ConcolicExecutorSettings() {}

ConcolicExecutorSettings::ConcolicExecutorSettings()
    : traceSemantics(false) {}

} // namespace
} // namespace
} // namespace

#endif
