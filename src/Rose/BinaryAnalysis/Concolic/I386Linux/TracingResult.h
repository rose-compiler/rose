#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_TracingResult_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_TracingResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/AddressSet.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

/** Results of the execution. */
class TracingResult: public Concolic::ConcreteResult {
    using Super = Concolic::ConcreteResult;

public:
    /** Shared ownership pointer. */
    using Ptr = TracingResultPtr;

private:
    int exitStatus_;                                    // as returned by wait
    AddressSet executedVas_;

protected:
    TracingResult();
    TracingResult(double rank, const AddressSet &executedVas);
public:
    ~TracingResult();

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
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(exitStatus_);
        s & BOOST_SERIALIZATION_NVP(executedVas_);
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
