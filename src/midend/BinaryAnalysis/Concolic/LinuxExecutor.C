#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    // FIXME[Robb Matzke 2019-04-15]: probably want a better ranking that 0.0, such as a ranking that depends on the exit status.
}

boost::movelib::unique_ptr<ConcreteExecutor::Result>
LinuxExecutor::execute(const TestCase::Ptr&) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

} // namespace
} // namespace
} // namespace
