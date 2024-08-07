#ifndef ROSE_BinaryAnalysis_Debugger_ThreadId_H
#define ROSE_BinaryAnalysis_Debugger_ThreadId_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Thread ID.
 *
 *  Identifies a particular thread to be debugged. A thread ID is needed when querying or modifying information that's specific
 *  to a particular thread, such as register values. */
class ThreadId {
protected:
    int id_ = -1;
public:
    /** Return the ID representing an unspecified thread.
     *
     *  This is used by debugger implementations that don't operate on threads. */
    static ThreadId unspecified();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
