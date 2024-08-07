#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <string>

#ifdef ROSE_IMPL
#include <Rose/Sarif/ThreadFlowLocation.h>
#endif


namespace Rose {
namespace Sarif {

/** Sequence of code locations that specify a single-threaded path.
 *
 *  A thread flow is a sequence of code locations that specify a possible path through a single thread of execution such as an
 *  operating system thread or a fiber.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_flow */
class ThreadFlow: public Node {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Message string.
     *
     *  Optional message string. */
    [[Rosebud::property]]
    std::string message;

public:
    /** Locations visited by an analysis tool. */
    [[Rosebud::not_null]]
    EdgeVector<ThreadFlowLocation> locations;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor.
     *
     *  The message may be an empty string. */
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
