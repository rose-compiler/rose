#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#ifdef ROSE_IMPL
#include <Rose/Sarif/Location.h>
#endif

namespace Rose {
namespace Sarif {

/** A location visited by an analysis tool.
 *
 *  This object represents a location visited by an analysis tool in the course of simulating or monitoring the execution of a
 *  program.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_flow */
[[Rosebud::no_constructors]]
class ThreadFlowLocation: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Message.
     *
     *  The message is optional, but a non-empty message must be present if the location is null when this object is emitted. */
    [[Rosebud::property]]
    std::string message;

public:
    /** Optional location.
     *
     *  Some analyses are not able to provide a location for every step in the code flow. If there is no location, then a @ref
     *  message should be present. */
    [[Rosebud::property]]
    Edge<Location> location;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor.
     *
     *  Although the constructor accepts an empty message and/or null location, a thread flow location should have a non-empty
     *  message, a non-null location, or both at the time it is emitted.
     *
     * @{ */
    static Ptr instance(const std::string &message);
    static Ptr instance(const LocationPtr&);
    static Ptr instance(const std::string &message, const LocationPtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
