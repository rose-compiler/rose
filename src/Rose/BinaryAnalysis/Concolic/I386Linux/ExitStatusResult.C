#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/I386Linux/ExitStatusResult.h>

#include <boost/serialization/export.hpp>

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

ExitStatusResult::ExitStatusResult()
    : exitStatus_(-1) {}

ExitStatusResult::ExitStatusResult(double rank, int exitStatus)
    : Super(rank), exitStatus_(exitStatus), exitKind_(nameCompletionStatus(exitStatus)) {}

ExitStatusResult::~ExitStatusResult() {}

ExitStatusResult::Ptr
ExitStatusResult::instance(double rank, int exitStatus) {
    return Ptr(new ExitStatusResult(rank, exitStatus));
}

int
ExitStatusResult::exitStatus() const {
    return exitStatus_;
}

void
ExitStatusResult::exitStatus(int status) {
    exitStatus_ = status;
    exitKind_ = nameCompletionStatus(status);
}

std::string
ExitStatusResult::out() const {
    return capturedOut_;
}

void
ExitStatusResult::out(const std::string &output) {
    capturedOut_ = output;
}

std::string
ExitStatusResult::err() const {
    return capturedErr_;
}

void
ExitStatusResult::err(const std::string &output) {
    capturedErr_ = output;
}

std::string
ExitStatusResult::exitKind() const {
    return exitKind_;
}

} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::Concolic::I386Linux::ExitStatusResult);

#endif
