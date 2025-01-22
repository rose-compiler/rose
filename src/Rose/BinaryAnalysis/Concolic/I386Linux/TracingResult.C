#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/I386Linux/TracingResult.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/export.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

TracingResult::TracingResult() {}

TracingResult::TracingResult(double rank, const AddressSet &executedVas)
    : Super(rank), exitStatus_(-1), executedVas_(executedVas) {}

TracingResult::~TracingResult() {}

TracingResult::Ptr
TracingResult::instance(double rank, const AddressSet &executedVas) {
    return Ptr(new TracingResult(rank, executedVas));
}

int
TracingResult::exitStatus() const {
    return exitStatus_;
}

void
TracingResult::exitStatus(int status) {
    exitStatus_ = status;
}

const AddressSet&
TracingResult::executedVas() const {
    return executedVas_;
}

AddressSet&
TracingResult::executedVas() {
    return executedVas_;
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::I386Linux::TracingResult);
#endif

#endif
