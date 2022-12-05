#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_ConcreteExecutor_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_ConcreteExecutor_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>

#include <boost/filesystem.hpp>
#include <boost/process/child.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** Concrete executor for M68k system emulation. */
class ConcreteExecutor: public Concolic::ConcreteExecutor {
public:
    /** Reference counting pointer. */
    using Ptr = ConcreteExecutorPtr;

protected:
    explicit ConcreteExecutor(const DatabasePtr&);
public:
    ~ConcreteExecutor();

public:
    /** Allocating constructor. */
    static Ptr instance(const DatabasePtr&);

public:
    virtual Concolic::ConcreteExecutorResultPtr execute(const TestCasePtr&) override;

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
