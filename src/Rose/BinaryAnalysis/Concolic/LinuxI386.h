#ifndef ROSE_BinaryAnalysis_Concolic_LinuxI386_H
#define ROSE_BinaryAnalysis_Concolic_LinuxI386_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Debugger.h>

#include <boost/filesystem.hpp>
#include <Sawyer/Callbacks.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Features specific to native Linux ELF i386 specimens. */
class LinuxI386: public Architecture, public Sawyer::SharedFromThis<LinuxI386> {
    using Super = Architecture;

public:
    /** Reference counting pointer. */
    using Ptr = LinuxI386Ptr;

    /** Context for system call callbacks. */
    class SyscallContext: public Concolic::SyscallContext {
    public:
        const Partitioner2::Partitioner &partitioner;   /**< Information about the specimen. */
        DebuggerPtr debugger;                           /**< Subordinate Linux process for concrete semantics. */

        /** Constructor. */
        SyscallContext(const LinuxI386::Ptr &architecture, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                       const Partitioner2::Partitioner&, const DebuggerPtr&);

        ~SyscallContext();
    };

private:
    DebuggerPtr debugger_;
    rose_addr_t scratchVa_ = 0;                         // subordinate address for scratch page
    bool markingArgvAsInput_ = true;
    bool markingEnvpAsInput_ = false;

    // In order to give system calls unique names, we number them sequentially per function.
    std::map<uint64_t, size_t> syscallSequenceNumbers_;

    // Function and location of last system call execution event when playing events back. The location is the
    // ExecutionLocation primary key, and the execution events that follow with the same primary key are all side effects of
    // the system call.
    std::pair<uint64_t /*functionNumber*/, uint64_t /*primaryLocation*/> playingSyscall_ = {0, UINT64_MAX};

protected:
    LinuxI386(const DatabasePtr&, TestCaseId);
public:
    ~LinuxI386();

public:
    /** Allocating constructor for test case.
     *
     *  Constructs a new executor for the specified test case.
     *
     * @{ */
    static Ptr instance(const DatabasePtr&, TestCaseId);
    static Ptr instance(const DatabasePtr&, const TestCasePtr&);
    /** @} */

public:
    /** Create event for system call return.
     *
     *  Apply a concrete system call return value to the symbolic state and create the corresponding execution event to be
     *  replayed later in a child test case. If you modify the event further, be sure to save it in the database. */
    ExecutionEventPtr applySystemCallReturn(const Partitioner2::Partitioner&,
                                            const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                            const std::string &syscallName, rose_addr_t syscallVa);

    /** The register where system call return values are stored. */
    RegisterDescriptor systemCallReturnRegister();

public:
    // These are documented in the base class.
    virtual void configureSystemCalls() override;
    virtual void load(const boost::filesystem::path&) override;
    virtual bool isTerminated() override;
    virtual ByteOrder::Endianness memoryByteOrder() override;
    virtual std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED) override;
    virtual rose_addr_t ip() override;
    virtual void ip(rose_addr_t) override;
    virtual std::vector<ExecutionEventPtr> createMemoryRestoreEvents() override;
    virtual std::vector<ExecutionEventPtr> createMemoryHashEvents() override;
    virtual std::vector<ExecutionEventPtr> createRegisterRestoreEvents() override;
    virtual bool playEvent(const ExecutionEventPtr&) override;
    virtual void mapMemory(const AddressInterval&, unsigned permissions) override;
    virtual void unmapMemory(const AddressInterval&) override;
    virtual size_t writeMemory(rose_addr_t, const std::vector<uint8_t>&) override;
    virtual std::vector<uint8_t> readMemory(rose_addr_t, size_t) override;
    virtual void writeRegister(RegisterDescriptor, uint64_t) override;
    virtual void writeRegister(RegisterDescriptor, const Sawyer::Container::BitVector&) override;
    virtual Sawyer::Container::BitVector readRegister(RegisterDescriptor) override;
    virtual void executeInstruction() override;
    virtual void executeInstruction(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, SgAsmInstruction*) override;
    virtual void createInputVariables(InputVariables&, const Partitioner2::Partitioner&,
                                      const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                      const SmtSolver::Ptr &solver) override;
    virtual void systemCall(const Partitioner2::Partitioner&,
                            const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

private:
    // Maps a scratch page for internal use and updates scratchVa_ with the address of the page.
    void mapScratchPage();

    // List of process memory segments that are not special.
    std::vector<MemoryMap::ProcessMapRecord> disposableMemory();

    // Unmap nearly all memory. The scratch page is not unmapped, nor is the VDSO or VVAR.
    void unmapAllMemory();

    // When stopped at the beginning of a system call, return the system call function number. See <sys/unistd_32.h> for the
    // mapping function system call number to the Linux kernel function that handles it.
    uint64_t systemCallFunctionNumber(const Partitioner2::Partitioner&,
                                      const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    // Returns the system call argument.
    InstructionSemantics2::BaseSemantics::SValuePtr
    systemCallArgument(const Partitioner2::Partitioner&,
                       const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, size_t argNumber);

    // Returns the system call return value.
    InstructionSemantics2::BaseSemantics::SValuePtr
    systemCallReturnValue(const Partitioner2::Partitioner&,
                          const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    // Modify the symbolic system call return value.
    InstructionSemantics2::BaseSemantics::SValuePtr
    systemCallReturnValue(const Partitioner2::Partitioner&,
                          const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                          const InstructionSemantics2::BaseSemantics::SValuePtr&);

};

} // namespace
} // namespace
} // namespace

#endif
#endif
