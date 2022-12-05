#ifndef ROSE_BinaryAnalysis_Concolic_LinuxTraceConcreteResult_H
#define ROSE_BinaryAnalysis_Concolic_LinuxTraceConcreteResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutorResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Results of the execution. */
class LinuxTraceConcreteResult: public ConcreteExecutorResult {
    using Super = ConcreteExecutorResult;

public:
    /** Shared ownership pointer. */
    using Ptr = LinuxTraceConcreteResultPtr;

private:
    int exitStatus_;                                    // as returned by wait
    AddressSet executedVas_;

protected:
    LinuxTraceConcreteResult();
    LinuxTraceConcreteResult(double rank, const AddressSet &executedVas);
public:
    ~LinuxTraceConcreteResult();

public:
    /** Allocating constructor. */
    static Ptr instance(double rank, const AddressSet &executedVas);

    /** Property: Program exit status.
     *
     *  This is the status obtained from wait(2).
     *
     * @{ */
    int exitStatus() const;
    void exitStatus(int);
    /** @} */

    /** Property: Executed addresses.
     *
     * @{ */
    const AddressSet& executedVas() const;
    AddressSet& executedVas();
    /** @} */

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ConcreteExecutorResult);
        s & BOOST_SERIALIZATION_NVP(exitStatus_);
        s & BOOST_SERIALIZATION_NVP(executedVas_);
    }
};

} // namespace
} // namespace
} // namespace

#endif
#endif
