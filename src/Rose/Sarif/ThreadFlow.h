#ifndef ROSE_Sarif_ThreadFlow_H
#define ROSE_Sarif_ThreadFlow_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <string>

namespace Rose {
namespace Sarif {

/** Sequence of code locations that specify a single-threaded path.
 *
 *  A thread flow is a sequence of code locations that specify a possible path through a single thread of execution such as an
 *  operating system thread or a fiber. */
class ThreadFlow: public Node {
public:
    /** Shared ownership pointer to a @ref ThreadFlow object.
     *
     *  @{ */
    using Ptr = ThreadFlowPtr;
    using ConstPtr = ThreadFlowConstPtr;
    /** @} */

private:
    std::string message_;                               // optional

public:
    /** Locations visited by an analysis tool. */
    EdgeVector<ThreadFlowLocation> locations;           // one or more

public:
    ~ThreadFlow();
protected:
    explicit ThreadFlow(const std::string &message);

public:
    /** Allocating constructor.
     *
     *  The message may be an empty string. */
    static Ptr instance(const std::string &message = std::string());

    /** Property: Message string.
     *
     *  Optional message string.
     *
     * @{ */
    const std::string& message() const;
    void message(const std::string&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    void checkLocationsResize(int delta, const ThreadFlowLocationPtr&);
    void handleLocationsResize(int delta, const ThreadFlowLocationPtr&);
    bool emitMessage(std::ostream&, const std::string &prefix);
};

} // namespace
} // namespace

#endif
#endif
