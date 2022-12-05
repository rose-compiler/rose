#ifndef ROSE_BinaryAnalysis_Concolic_LinuxTraceConcrete_H
#define ROSE_BinaryAnalysis_Concolic_LinuxTraceConcrete_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Concrete executor to trace a native ELF executable.
 *
 *  Ranks executables by the size of the set of addresses that were executed. */
class LinuxTraceConcrete: public ConcreteExecutor {
public:
    /** Reference counting pointer to a @ref LinuxTraceConcrete. */
    using Ptr = Sawyer::SharedPointer<LinuxTraceConcrete>;

protected:
    LinuxTraceConcrete();

    explicit LinuxTraceConcrete(const DatabasePtr&);

public:
    ~LinuxTraceConcrete();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Specimen exit status, as returned by wait. */
    static int exitStatus(const ConcreteExecutorResultPtr&);

    /** Executed virtual addresses. */
    const AddressSet& executedVas(const ConcreteExecutorResultPtr&);

    ConcreteExecutorResultPtr execute(const TestCasePtr&) override;

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
