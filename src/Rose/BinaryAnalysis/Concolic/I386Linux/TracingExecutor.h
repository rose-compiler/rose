#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_TracingExecutor_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_TracingExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/AddressSet.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>

#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

/** Concrete executor to trace a native ELF executable.
 *
 *  Ranks executables by the size of the set of addresses that were executed. */
class TracingExecutor: public Concolic::ConcreteExecutor {
    using Super = Concolic::ConcreteExecutor;

public:
    /** Reference counting pointer to a @ref TracingExecutor. */
    using Ptr = TracingExecutorPtr;


private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {}

protected:
    TracingExecutor();                                  // for serialization
    explicit TracingExecutor(const std::string &name);  // for factories
    explicit TracingExecutor(const DatabasePtr&);

public:
    ~TracingExecutor();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Factory constructor. */
    static Ptr factory();

    /** Specimen exit status, as returned by wait. */
    static int exitStatus(const ConcreteResultPtr&);

    /** Executed virtual addresses. */
    const AddressSet& executedVas(const ConcreteResultPtr&);

    ConcreteResultPtr execute(const TestCasePtr&) override;

    // Documented in super class
    virtual bool matchFactory(const std::string&) const override;
    virtual Concolic::ConcreteExecutorPtr instanceFromFactory(const DatabasePtr&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
