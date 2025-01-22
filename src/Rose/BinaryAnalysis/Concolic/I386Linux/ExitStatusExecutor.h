#ifndef ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusExecutor_H
#define ROSE_BinaryAnalysis_Concolic_I386Linux_ExitStatusExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#endif

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
    using Persona = Sawyer::Optional<unsigned long>;

protected:
    bool useAddressRandomization_ = false;              // enable/disable address space randomization in the OS

protected:
    explicit ExitStatusExecutor(const std::string &name); // for creating a factory
    explicit ExitStatusExecutor(const DatabasePtr&);

public:
    ~ExitStatusExecutor();

    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Allocate a factory. */
    static Ptr factory();

    /** Property: Address space randomization.
     *
     *  This property controls whether the specimen is executed with or without OS address space randomization. It's usually
     *  better to turn off randomization for repeatable results, and it is therefore off by default.
     *
     * @{ */
    bool useAddressRandomization() const { return useAddressRandomization_; }
    void useAddressRandomization(bool b) { useAddressRandomization_ = b; }
    /** @} */

public:
    virtual bool matchFactory(const std::string&) const override;
    virtual Concolic::ConcreteExecutorPtr instanceFromFactory(const DatabasePtr&) override;
    virtual Concolic::ConcreteResultPtr execute(const TestCasePtr&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
