#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <string>

#ifdef ROSE_IMPL
#include <Rose/Sarif/ThreadFlow.h>
#endif

namespace Rose {
namespace Sarif {

/** Progress of one or more programs through one or more thread flows.
 *
 *  A CodeFlow object describes the progress of one or more programs through one or more thread flows, which together lead to the
 *  detection of a problem in the system being analyzed. A thread flow is a temporally ordered sequence of code locations occurring
 *  within a single thread of execution, typically an operating system thread or a fiber. The thread flows in a code flow may lie
 *  within a single process, within multiple processes on the same machine, or within multiple processes on multiple machines.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_flow */
class CodeFlow: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Message string.
     *
     *  Optional message associated a code flow. */
    [[Rosebud::property]]
    std::string message;

    /** Code flows per thread. */
    [[Rosebud::not_null]]
    EdgeVector<ThreadFlow> threadFlows;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor. */
    static Ptr instance(const std::string &message);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
