#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

LinuxExecutor::Result::Result(int exitStatus)
    : ConcreteExecutor::Result(0.0), exitStatus_(exitStatus) {
    // FIXME[Robb Matzke 2019-04-15]: probably want a better ranking that 0.0, such as a ranking that depends on the exit status.
}

#if __cplusplus >= 201103L // needs to be fixed; commented out for Jenkins testing
std::unique_ptr<ConcreteExecutor::Result>
LinuxExecutor::execute(const TestCase::Ptr&) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}
#endif

} // namespace
} // namespace
} // namespace
