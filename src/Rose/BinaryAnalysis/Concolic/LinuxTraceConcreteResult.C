#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/LinuxTraceConcreteResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxTraceConcreteResult::LinuxTraceConcreteResult() {}

LinuxTraceConcreteResult::LinuxTraceConcreteResult(double rank, const AddressSet &executedVas)
    : Super(rank), exitStatus_(-1), executedVas_(executedVas) {}

LinuxTraceConcreteResult::~LinuxTraceConcreteResult() {}

LinuxTraceConcreteResult::Ptr
LinuxTraceConcreteResult::instance(double rank, const AddressSet &executedVas) {
    return Ptr(new LinuxTraceConcreteResult(rank, executedVas));
}

int
LinuxTraceConcreteResult::exitStatus() const {
    return exitStatus_;
}

void
LinuxTraceConcreteResult::exitStatus(int status) {
    exitStatus_ = status;
}

const AddressSet&
LinuxTraceConcreteResult::executedVas() const {
    return executedVas_;
}

AddressSet&
LinuxTraceConcreteResult::executedVas() {
    return executedVas_;
}

} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::LinuxTraceConcreteResult);

#endif
