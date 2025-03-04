#ifndef ROSE_BinaryAnalysis_Concolic_Emulation_H
#define ROSE_BinaryAnalysis_Concolic_Emulation_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Sawyer/FileSystem.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Semantics for concolic execution. */
namespace Emulation {

typedef InstructionSemantics::SymbolicSemantics::Formatter SValueFormatter; /**< How to format expressions when printing. */
typedef InstructionSemantics::SymbolicSemantics::SValuePtr SValuePtr; /**< Pointer to semantic values. */
typedef InstructionSemantics::SymbolicSemantics::SValue SValue; /**< Type of semantic values. */
typedef InstructionSemantics::SymbolicSemantics::RegisterStatePtr RegisterStatePtr; /**< Pointer to semantic registers. */
typedef InstructionSemantics::SymbolicSemantics::RegisterState RegisterState; /**< Type of semantic register space. */
typedef InstructionSemantics::SymbolicSemantics::MemoryStatePtr MemoryStatePtr; /**< Pointer to semantic memory. */
typedef InstructionSemantics::SymbolicSemantics::MemoryState MemoryState; /**< Type of semantic memory space. */
typedef InstructionSemantics::SymbolicSemantics::StatePtr StatePtr; /**< Pointer to semantic machine state. */
typedef InstructionSemantics::SymbolicSemantics::State State; /**< Semantic machine state. */

/** Values thrown when subordinate exits. */
class Exit: public Exception {
    uint64_t status_;

public:
    explicit Exit(uint64_t status)
        : Exception("subordinate exit"), status_(status) {}

    ~Exit() throw () {}

    /** Symbolic exit status. */
    uint64_t status() const {
        return status_;
    }
};

/** Semantic operations. */
class RiscOperators: public InstructionSemantics::SymbolicSemantics::RiscOperators {
public:
    /** Shared ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    /** Base class. */
    typedef InstructionSemantics::SymbolicSemantics::RiscOperators Super;

    /** Special "path" Boolean register. */
    const RegisterDescriptor REG_PATH;

    /** Settings for the emulation. */
    struct Settings {
    };

private:
    Settings settings_;                                 // emulation settings
    DatabasePtr db_;                                    // concolic database connection
    TestCasePtr testCase_;                              // test case whose instructions are being processed
    Partitioner2::PartitionerConstPtr partitioner_;     // ROSE disassembly info about the specimen
    ArchitecturePtr process_;                           // subordinate process, concrete state
    bool hadSystemCall_ = false;                        // true if we need to call process_->systemCall, cleared each insn
    ExecutionEventPtr hadSharedMemoryAccess_;           // set when shared memory is read, cleared each instruction
    bool isRecursive_ = false;                          // if set, avoid calling user-defined functions

protected:
    /** Allocating constructor. */
    RiscOperators(const Settings&, const DatabasePtr&, const TestCasePtr&, const Partitioner2::PartitionerConstPtr&,
                  const ArchitecturePtr&, const InstructionSemantics::BaseSemantics::StatePtr&, const SmtSolverPtr&);
public:
    ~RiscOperators();

public:
    /** Allocating constructor. */
    static RiscOperatorsPtr instance(const Settings &settings, const DatabasePtr&, const TestCasePtr&,
                                     const Partitioner2::PartitionerConstPtr&, const ArchitecturePtr &process,
                                     const InstructionSemantics::BaseSemantics::SValuePtr &protoval,
                                     const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic pointer downcast. */
    static RiscOperatorsPtr promote(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Overrides documented in base class
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::SValuePtr &/*protoval*/,
           const SmtSolverPtr& = SmtSolverPtr()) const override;
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::StatePtr &/*state*/,
           const SmtSolverPtr& = SmtSolverPtr()) const override;

public:
    /** Property: Settings.
     *
     *  The settings are read-only, set when this object was created. */
    const Settings& settings() const;

    /** Property: Partitioner. */
    Partitioner2::PartitionerConstPtr partitioner() const;

    /** Property: Test case. */
    TestCasePtr testCase() const;

    /** Property: Database. */
    DatabasePtr database() const;

    /** Property: Concrete half of the concolic executor semantics. */
    ArchitecturePtr process() const;

    /** Property: Input variables.
     *
     *  This is a mapping from symbolic variables to execution events.
     *
     * @{ */
    InputVariablesPtr inputVariables() const;
    /** @} */

    /** Property: Had system call.
     *
     *  Cleared at the beginning of each instruction, and set when processing an instruction symbolically that is a system
     *  call. This allows the system call handling to be delayed until after the concrete half of the execution is performed,
     *  namely stepping into the system call but not yet performing it.
     *
     * @{ */
    bool hadSystemCall() const;
    void hadSystemCall(bool);
    /** @} */

    /** Property: Had a shared memory access.
     *
     *  Cleared at the beginning of each instruction, and set when processing an instruction symbolically that accesses known
     *  shared memory.
     *
     * @{ */
    ExecutionEventPtr hadSharedMemoryAccess() const;
    void hadSharedMemoryAccess(const ExecutionEventPtr&);
    /** @} */

    /** Property: Recursive calls through user code.
     *
     *  This is generally tested and set before invoking user-defined code such as callbacks for shared memory operations, and cleared
     *  when the user code returns. If the test is true, then the user code is skipped.
     *
     * @{ */
    bool isRecursive() const;
    void isRecursive(bool);
    /** @} */

    /** Number of bits in a word.
     *
     *  The definition of "word" is the natural width of the instruction pointer, stack pointer, most general-purpose
     *  registers, etc. */
    size_t wordSizeBits() const;

    /** Register definitions. */
    RegisterDictionaryPtr registerDictionary() const;

    /** Create and save info about initial program inputs.
     *
     *  This function creates the execution events and symbolic variables for program arguments (argc, argv, envp) and creates
     *  a mapping from the symbolic variable to its corresponding concrete execution event, and saves the information in
     *  the database. */
    void createInputVariables(const SmtSolver::Ptr&);

    /** Restore saved input variables.
     *
     *  Reads the database to reconstruct information about program inputs. */
    void restoreInputVariables(const SmtSolver::Ptr&);

    /** Print input variables.
     *
     *  Shows the mapping from input variables to their symbolic values. */
    void printInputVariables(std::ostream&) const;

    /** Print SMT solver assertions. */
    void printAssertions(std::ostream&) const;

public:
    // Base class overrides -- the acutal RISC operations

    virtual void startInstruction(SgAsmInstruction*) override;

    virtual void interrupt(int majr, int minr) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    peekRegister(RegisterDescriptor reg, const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    virtual void
    writeRegister(RegisterDescriptor, const InstructionSemantics::BaseSemantics::SValuePtr&) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics::BaseSemantics::SValuePtr &cond) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    peekMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt) override;

    virtual void
    writeMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics::BaseSemantics::SValuePtr &value,
                const InstructionSemantics::BaseSemantics::SValuePtr &cond) override;

    // Call this when the concrete simulation exits.
    void doExit(uint64_t);
};

/**< Pointer to virtual CPU. */
using DispatcherPtr = Sawyer::SharedPointer<class Dispatcher>;

class Dispatcher: public Sawyer::SharedObject {
public:
    using Ptr = DispatcherPtr;

private:
    InstructionSemantics::BaseSemantics::DispatcherPtr inner_;

protected:
    explicit Dispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const ArchitecturePtr&);
public:
    ~Dispatcher();

public:
    static Ptr instance(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const ArchitecturePtr&);

public:
    // These functions are similar to InstructionSemantics::BaseSemantics::Dispatcher
    InstructionSemantics::BaseSemantics::RiscOperators::Ptr operators() const;

public:
    /** Concrete instruction pointer. */
    Address concreteInstructionPointer() const;

    /** True if subordinate process has terminated.
     *
     *  Once the subordinate process terminates no more instructions can be processed and no state information
     *  is available. */
    bool isTerminated() const;

    /** Return the emulation RISC operators.
     *
     *  Returns the @ref Emulation::RiscOperators object even if this dispatcher's immediate RISC operators object is a
     *  TraceSemantics::RiscOperators being used for debugging purposes. */
    RiscOperatorsPtr emulationOperators() const;

    /** Unrwap the RISC operators if tracing is enabled. */
    static RiscOperatorsPtr unwrapEmulationOperators(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Process one instruction concretely and symbolically. */
    virtual void processInstruction(SgAsmInstruction*);

    /** The concrete half of processInstruction. */
    void processConcreteInstruction(SgAsmInstruction*);

    /** Register dictionary from the inner dispatcher. */
    RegisterDictionaryPtr registerDictionary() const;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
