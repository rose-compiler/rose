#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingResult_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** M68k concrete execution results. */
class TracingResult: public Concolic::ConcreteResult {
    using Super = Concolic::ConcreteResult;

public:
    /** Reference counting pointer. */
    using Ptr = TracingResultPtr;

protected:
    TracingResult();                                    // for boost::serialization
    explicit TracingResult(double rank);
public:
    ~TracingResult();

public:
    Ptr instance(double rank);

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {}
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
