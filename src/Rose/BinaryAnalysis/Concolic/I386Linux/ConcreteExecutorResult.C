#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ConcreteExecutorResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

static std::string
nameCompletionStatus(int processDisposition) {
    std::string res = "unknown";

    if (WIFEXITED(processDisposition)) {
        res = "exit";
    } else if (WIFSIGNALED(processDisposition)) {
        res = "signal";
    } else if (WIFSTOPPED(processDisposition)) {
        res = "stopped";
    } else if (WIFCONTINUED(processDisposition)) {
        res = "resumed";
    }
    return res;
}

ConcreteExecutorResult::ConcreteExecutorResult()
    : exitStatus_(-1) {}

ConcreteExecutorResult::ConcreteExecutorResult(double rank, int exitStatus)
    : Super(rank), exitStatus_(exitStatus), exitKind_(nameCompletionStatus(exitStatus)) {}

ConcreteExecutorResult::~ConcreteExecutorResult() {}

ConcreteExecutorResult::Ptr
ConcreteExecutorResult::instance(double rank, int exitStatus) {
    return Ptr(new ConcreteExecutorResult(rank, exitStatus));
}

int
ConcreteExecutorResult::exitStatus() const {
    return exitStatus_;
}

void
ConcreteExecutorResult::exitStatus(int status) {
    exitStatus_ = status;
    exitKind_ = nameCompletionStatus(status);
}

std::string
ConcreteExecutorResult::out() const {
    return capturedOut_;
}

void
ConcreteExecutorResult::out(const std::string &output) {
    capturedOut_ = output;
}

std::string
ConcreteExecutorResult::err() const {
    return capturedErr_;
}

void
ConcreteExecutorResult::err(const std::string &output) {
    capturedErr_ = output;
}

std::string
ConcreteExecutorResult::exitKind() const {
    return exitKind_;
}

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::I386Linux::ConcreteExecutorResult);

#endif
