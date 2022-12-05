#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_ConcreteExecutorResult_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_ConcreteExecutorResult_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutorResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** M68k concrete execution results. */
class ConcreteExecutorResult: public Concolic::ConcreteExecutorResult {
    using Super = Concolic::ConcreteExecutorResult;

public:
    /** Reference counting pointer. */
    using Ptr = ConcreteExecutorResultPtr;

protected:
    ConcreteExecutorResult();                           // for boost::serialization
    explicit ConcreteExecutorResult(double rank);
public:
    ~ConcreteExecutorResult();

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
