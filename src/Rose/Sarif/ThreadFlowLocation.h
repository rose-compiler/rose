#ifndef ROSE_Sarif_ThreadFlowLocation_H
#define ROSE_Sarif_ThreadFlowLocation_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

namespace Rose {
namespace Sarif {

/** A location visited by an analysis tool.
 *
 *  This object represents a location visited by an analysis tool in the course of simulating or monitoring the execution of a
 *  program. */
class ThreadFlowLocation: public Node {
public:
    /** Shared-ownership pointer to a @ref ThreadFlowLocation.
     *
     * @{ */
    using Ptr = ThreadFlowLocationPtr;
    using ConstPtr = ThreadFlowLocationConstPtr;
    /** @} */

private:
    std::string message_;                               // optional

public:
    /** Optional location.
     *
     *  Some analyses are not able to provide a location for every step in the code flow. If there is no location, then a @ref
     *  message should be present. */
    Edge<Location> location;                            // optional

public:
    ~ThreadFlowLocation();
protected:
    ThreadFlowLocation(const std::string&, const LocationPtr&);
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

    /** Property: Message.
     *
     *  The message is optional, but a non-empty message must be present if the location is null when this object is emitted.
     *
     * @{ */
    const std::string& message() const;
    void message(const std::string&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    bool emitMessage(std::ostream&, const std::string &prefix);
    void checkLocationChange(const LocationPtr&, const LocationPtr&);
    void handleLocationChange(const LocationPtr&, const LocationPtr&);
};

} // namespace
} // namespace

#endif
#endif
