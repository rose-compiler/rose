#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingExecutor_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_TracingExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>

#include <Sawyer/Trace.h>

#include <boost/filesystem.hpp>
#if BOOST_VERSION < 108600
    #include <boost/process/child.hpp>
#else
    #include <boost/process/v1/child.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** Concrete executor for M68k system emulation. */
class TracingExecutor: public Concolic::ConcreteExecutor {
    using Super = Concolic::ConcreteExecutor;

public:
    /** Reference counting pointer. */
    using Ptr = TracingExecutorPtr;

protected:
    explicit TracingExecutor(const std::string&);       // for factories
    explicit TracingExecutor(const DatabasePtr&);
public:
    ~TracingExecutor();

public:
    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

    /** Factory constructor. */
    static Ptr factory();

public:
    virtual bool matchFactory(const std::string&) const override;
    virtual Concolic::ConcreteExecutorPtr instanceFromFactory(const DatabasePtr&) override;
    virtual Concolic::ConcreteResultPtr execute(const TestCasePtr&) override;

private:
    // Start running QEMU asynchronously
    boost::process::child startQemu(const boost::filesystem::path &firmwareName);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
