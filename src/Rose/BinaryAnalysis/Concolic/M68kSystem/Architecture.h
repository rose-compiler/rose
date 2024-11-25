#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_Architecture_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/Yaml.h>

#if BOOST_VERSION < 108600
    #include <boost/process/child.hpp>
#else
    #include <boost/process/v1/child.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

/** Bare Motorola 68000 system using QEMU. */
class Architecture: public Concolic::Architecture {
    using Super = Concolic::Architecture;

public:
    /** Reference counting pointer. */
    using Ptr = ArchitecturePtr;

private:
    boost::process::child qemu_;
    Yaml::Node config_;                                 // configuration info from the `instance` call

protected:
    explicit Architecture(const std::string&);          // for factories
    Architecture(const DatabasePtr&, TestCaseId);
public:
    ~Architecture();

public:
    /** Allocating constructor for test case.
     *
     *  Constructs a new executor for the specified test case.
     *
     * @{ */
    static Ptr instance(const DatabasePtr&, TestCaseId, const Yaml::Node &config);
    static Ptr instance(const DatabasePtr&, const TestCasePtr&, const Yaml::Node &config);
    /** @} */

    /** Allocating constructor to create a factory. */
    static Ptr factory();

    // Supporting functions
private:
    // Return the address of the first instruction to be executed, if known.
    Sawyer::Optional<rose_addr_t> entryAddress();

    // These are documented in the base class.
public:
    virtual bool matchFactory(const Yaml::Node&) const override;
    virtual Super::Ptr instanceFromFactory(const DatabasePtr&, TestCaseId, const Yaml::Node &config) const override;
    virtual Partitioner2::PartitionerPtr partition(const Partitioner2::EnginePtr&, const std::string &specimen) override;
    virtual void configureSystemCalls() override;
    virtual void load(const boost::filesystem::path&) override;
    virtual ByteOrder::Endianness memoryByteOrder() override;
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryAdjustEvents(const MemoryMapPtr&, rose_addr_t insnVa) override;
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() override;
    virtual void mapMemory(const AddressInterval&, unsigned permissions) override;
    virtual void unmapMemory(const AddressInterval&) override;
    virtual void createInputVariables(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr&) override;
    virtual void systemCall(const Partitioner2::PartitionerConstPtr&,
                            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
        makeDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
