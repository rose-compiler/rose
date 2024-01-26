#ifndef ROSE_Sarif_CodeFlow_H
#define ROSE_Sarif_CodeFlow_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <string>

namespace Rose {
namespace Sarif {

/** Progress of one or more programs through one or more thread flows.
 *
 *  A CodeFlow object describes the progress of one or more programs through one or more thread flows, which together lead to the
 *  detection of a problem in the system being analyzed. A thread flow is a temporally ordered sequence of code locations occurring
 *  within a single thread of execution, typically an operating system thread or a fiber. The thread flows in a code flow may lie
 *  within a single process, within multiple processes on the same machine, or within multiple processes on multiple machines. */
class CodeFlow: public Node {
public:
    /** Shared ownership pointer to a @ref CodeFlow object.
     *
     *  @{ */
    using Ptr = CodeFlowPtr;
    using ConstPtr = CodeFlowConstPtr;
    /** @} */

private:
    std::string message_;                               // optional

public:
    /** Code flows per thread. */
    EdgeVector<ThreadFlow> threadFlows;                 // one or more

public:
    ~CodeFlow();
protected:
    explicit CodeFlow(const std::string &message);

public:
    /** Allocating constructor. */
    static Ptr instance(const std::string &message = std::string());

    /** Property: Message string.
     *
     *  Optional message associated a code flow.
     *
     * @{ */
    const std::string& message() const;
    void message(const std::string&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string &prefix) override;
    std::string emissionPrefix() override;

private:
    void checkThreadFlowsResize(int delta, const ThreadFlowPtr&);
    void handleThreadFlowsResize(int delta, const ThreadFlowPtr&);
    bool emitMessage(std::ostream&, const std::string &prefix);
};

} // namespace
} // namespace

#endif
#endif
