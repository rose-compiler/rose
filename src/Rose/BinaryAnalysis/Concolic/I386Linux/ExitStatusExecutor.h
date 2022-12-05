#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusExecutor_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Sawyer/Optional.h>
#include <Sawyer/SharedPointer.h>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace I386Linux {

/** Concrete executor for Linux i386 ELF executables. */
class ExitStatusExecutor: public Concolic::ConcreteExecutor {
    using Super = Concolic::ConcreteExecutor;

public:
    /** Reference counting pointer to a @ref ConcreteExecutor. */
    using Ptr = ExitStatusExecutorPtr;

    /** Holds an optional personality-value.
     *
     *  This indicates whether address randomization should be turned on or off. */
    typedef Sawyer::Optional<unsigned long> Persona;

protected:
    bool useAddressRandomization_;                      // enable/disable address space randomization in the OS

protected:
    explicit ExitStatusExecutor(const DatabasePtr&);

public:
    ~ExitStatusExecutor();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Property: Address space randomization.
     *
     *  This property controls whether the specimen is executed with or without OS address space randomization. It's usually
     *  better to turn off randomization for repeatable results, and it is therefore off by default.
     *
     * @{ */
    bool useAddressRandomization() const { return useAddressRandomization_; }
    void useAddressRandomization(bool b) { useAddressRandomization_ = b; }
    /** @} */

    virtual Concolic::ConcreteResultPtr execute(const TestCasePtr&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
