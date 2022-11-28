#ifndef ROSE_BinaryAnalysis_Concolic_M68kSystem_Architecture_H
#define ROSE_BinaryAnalysis_Concolic_M68kSystem_Architecture_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Debugger/BasicTypes.h>

#include <boost/process/child.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bare Motorola 68000 system using QEMU
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Architecture: public Concolic::Architecture {
    using Super = Concolic::Architecture;

public:
    /** Reference counting pointer. */
    using Ptr = ArchitecturePtr;

private:
    boost::process::child qemu_;
    Debugger::GdbPtr debugger_;

protected:
    Architecture(const DatabasePtr&, TestCaseId, const Partitioner2::Partitioner&);
public:
    ~Architecture();

public:
    /** Allocating constructor for test case.
     *
     *  Constructs a new executor for the specified test case.
     *
     * @{ */
    static Ptr instance(const DatabasePtr&, TestCaseId, const Partitioner2::Partitioner&);
    static Ptr instance(const DatabasePtr&, const TestCasePtr&, const Partitioner2::Partitioner&);
    /** @} */

public:
    // These are documented in the base class.
    virtual void configureSystemCalls() override;
    virtual void configureSharedMemory() override;
    virtual void load(const boost::filesystem::path&) override;
    virtual bool isTerminated() override;
    virtual ByteOrder::Endianness memoryByteOrder() override;
    virtual std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED) override;
    virtual rose_addr_t ip() override;
    virtual void ip(rose_addr_t) override;
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryAdjustEvents(const MemoryMap::Ptr&, rose_addr_t insnVa) override;
    virtual std::vector<ExecutionEventPtr> createRegisterRestoreEvents() override;
    virtual bool playEvent(const ExecutionEventPtr&) override;
    virtual void mapMemory(const AddressInterval&, unsigned permissions) override;
    virtual void unmapMemory(const AddressInterval&) override;
    virtual size_t writeMemory(rose_addr_t, const std::vector<uint8_t>&) override;
    virtual std::vector<uint8_t> readMemory(rose_addr_t, size_t) override;
    virtual void writeRegister(RegisterDescriptor, uint64_t) override;
    virtual void writeRegister(RegisterDescriptor, const Sawyer::Container::BitVector&) override;
    virtual Sawyer::Container::BitVector readRegister(RegisterDescriptor) override;
    virtual void executeInstruction(const Partitioner2::Partitioner&) override;
    virtual void executeInstruction(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, SgAsmInstruction*) override;
    virtual void createInputVariables(const Partitioner2::Partitioner&, const Emulation::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr&) override;
    virtual void systemCall(const Partitioner2::Partitioner&,
                            const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
