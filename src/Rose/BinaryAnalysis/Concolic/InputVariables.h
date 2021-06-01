#ifndef ROSE_BinaryAnalysis_Concolic_InputVariables_H
#define ROSE_BinaryAnalysis_Concolic_InputVariables_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/SymbolicExpr.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Describes where a symbolic variable came from.
 *
 *  Every variable corresponds to an ExecutionEvent so that if a variable's value is changed then the corresponding execution
 *  event (when copied to the newly generated test case) is also updated. For instance, if main's argc changes from 2 to 1 then
 *  the corresponding memory-write event for the new test case will be set to 2 instead of 1 when it's copied from the parent
 *  test case. Thus, when the new test case runs and the event is reached, the event's action will cause the concrete
 *  execution's argc to be overwritten with 2. */
class InputVariables {
private:
    typedef Sawyer::Container::Map<uint64_t, ExecutionEventPtr> Variables; // map symbolic variable ID to program input
    Variables variables_;

public:
    /** Insert a record describing the number of program arguments. */
    void insertProgramArgumentCount(const ExecutionEventPtr&, const SymbolicExpr::Ptr&);

    /** Insert a record for a program argument.
     *
     *  The @p i and @p j are the indexes for the <code>char *argv[]</code> argument of a C or C++ program's "main" function. */
    void insertProgramArgument(const ExecutionEventPtr&, size_t i, size_t j, const SymbolicExpr::Ptr&);

    /** Insert a record for an environment variable.
     *
     *  The @p i and @p j are the indexes for the <code>char *envp[]</code> argument of a C or C++ program's "main" function. */
    void insertEnvironmentVariable(const ExecutionEventPtr&, size_t i, size_t j, const SymbolicExpr::Ptr&);

    /** Insert a record for a system call return. */
    void insertSystemCallReturn(const ExecutionEventPtr&, const SymbolicExpr::Ptr&);

    /** Re-establish a connection between a variable and an event. */
    void insertEvent(const ExecutionEventPtr&);

    /** Find an input event when given a symbolic variable name. */
    ExecutionEventPtr get(const std::string &symbolicVarName) const;

    /** Print all defined variables. */
    void print(std::ostream&, const std::string &prefix = "") const;

    friend std::ostream& operator<<(std::ostream &out, const InputVariables &x) {
        x.print(out);
        return out;
    }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
