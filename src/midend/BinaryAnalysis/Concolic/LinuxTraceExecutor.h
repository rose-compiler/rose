#ifndef ROSE_BinaryAnalysis_Concolic_LinuxTraceExecutor_H
#define ROSE_BinaryAnalysis_Concolic_LinuxTraceExecutor_H
#include <Concolic/BasicTypes.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Concolic/ConcreteExecutor.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Concrete executor to trace a native ELF executable.
 *
 *  Ranks executables by the size of the set of addresses that were executed. */
class LinuxTraceExecutor: public ConcreteExecutor {
public:
    /** Reference counting pointer to a @ref LinuxTraceExecutor. */
    using Ptr = Sawyer::SharedPointer<LinuxTraceExecutor>;

    /** Results of the execution. */
    class Result: public ConcreteExecutorResult {
    public:
        int exitStatus;                                 // as returned by wait
        AddressSet executedVas;

    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ConcreteExecutorResult);
            s & BOOST_SERIALIZATION_NVP(executedVas);
        }
    };

protected:
    explicit LinuxTraceExecutor(const DatabasePtr&);

public:
    ~LinuxTraceExecutor();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Specimen exit status, as returned by wait. */
    static int exitStatus(const ConcreteExecutorResult*);

    /** Executed virtual addresses. */
    const AddressSet& executedVas(const ConcreteExecutorResult*);

    ConcreteExecutorResult* execute(const TestCasePtr&) override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
