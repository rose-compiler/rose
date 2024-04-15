#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

TracingResult::TracingResult() {}

TracingResult::TracingResult(double rank)
    : Super(rank) {}

TracingResult::~TracingResult() {}

TracingResult::Ptr
TracingResult::instance(double rank) {
    return Ptr(new TracingResult(rank));
}

const TracingResult::Trace&
TracingResult::trace() const {
    return trace_;
}

TracingResult::Trace&
TracingResult::trace() {
    return trace_;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
