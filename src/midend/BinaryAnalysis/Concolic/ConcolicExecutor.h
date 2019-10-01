#ifndef ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H

#include <BinaryDebugger.h>
#include <DispatcherX86.h>
#include <Sawyer/FileSystem.h>
#include <SymbolicSemantics2.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables that exist in the specimen.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes where a symbolic variable came from. */
struct VariableProvenance {
    /** From whence a variable came. */
    enum Whence {
        INVALID,                                        /**< Provenance record is invalid (default constructed). */
        REGISTER,                                       /**< Variable came from reading a register. */
        MEMORY                                          /**< Variable came from reading a memory location. */
    };
    Whence whence;                                      /**< Where did symbolic variable come from? */
    RegisterDescriptor reg;                             /**< The register where the variable was defined. */
    InstructionSemantics2::BaseSemantics::SValuePtr addr; /**< Memory address where the variable was defined. */

    /** Default constructor. */
    VariableProvenance()
        : whence(INVALID) {}

    /** Construct a register provenance record. */
    explicit VariableProvenance(RegisterDescriptor reg)
        : whence(REGISTER), reg(reg) {}

    /** Construct a memory provenance record. */
    explicit VariableProvenance(const InstructionSemantics2::BaseSemantics::SValuePtr &addr)
        : whence(MEMORY), addr(addr) {}
};

/** Mapping of symbolic variables to their provenance information. */
typedef Sawyer::Container::Map<SymbolicExpr::LeafPtr, VariableProvenance> Variables;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Concolic emulation semantics.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Semantics for concolic execution. */
namespace Emulation {

typedef InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr; /**< Pointer to semantic values. */
typedef InstructionSemantics2::SymbolicSemantics::SValue SValue; /**< Type of semantic values. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterStatePtr RegisterStatePtr; /**< Pointer to semantic registers. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterState RegisterState; /**< Type of semantic register space. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryStatePtr MemoryStatePtr; /**< Pointer to semantic memory. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryState MemoryState; /**< Type of semantic memory space. */
typedef InstructionSemantics2::SymbolicSemantics::StatePtr StatePtr; /**< Pointer to semantic machine state. */
typedef InstructionSemantics2::SymbolicSemantics::State State; /**< Semantic machine state. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr; /**< Pointer to semantic operations. */

/** Semantic operations. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
public:
    /** Base class. */
    typedef InstructionSemantics2::SymbolicSemantics::RiscOperators Super;

    /** Special "path" Boolean register. */
    const RegisterDescriptor REG_PATH;

private:
    const Partitioner2::Partitioner &partitioner_;      // ROSE disassembly info about the specimen
    Debugger::Ptr process_;                             // subordinate process
    Variables variables_;                               // where did symbolic variables come from?

protected:
    /** Allocating constructor. */
    RiscOperators(const Partitioner2::Partitioner &partitioner, const Debugger::Ptr &process,
                  const InstructionSemantics2::BaseSemantics::StatePtr &state, const SmtSolverPtr &solver)
        : Super(state, solver), REG_PATH(*state->registerState()->get_register_dictionary()->lookup("path")),
          partitioner_(partitioner), process_(process) {
        name("Concolic-symbolic");
        (void) SValue::promote(state->protoval());
    }

public:
    /** Allocating constructor. */
    static RiscOperatorsPtr instance(const Partitioner2::Partitioner&, const DebuggerPtr&process,
                                     const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                                     const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic pointer downcast. */
    static RiscOperatorsPtr promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    /** Symbolic variables. */
    const Variables& variables() const {
        return variables_;
    }

    // Overrides documented in base class
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
    }
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr &state,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
    }

public:
    /** Property: Partitioner. */
    const Partitioner2::Partitioner& partitioner() const {
        return partitioner_;
    }

    /** Property: Subordinate process. */
    Debugger::Ptr process() const {
        return process_;
    }
    
    /** Number of bits in a word.
     *
     *  The definition of "word" is the natural width of the instruction pointer, stack pointer, most general-purpose
     *  registers, etc. */
    size_t wordSizeBits() const;

    /** Register definitions. */
    const RegisterDictionary* registerDictionary() const;

public:
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor reg, const InstructionSemantics2::BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

private:
    // Handles a Linux system call of the INT 0x80 variety.
    void systemCall();

    // Mark locations of specimen command-line arguments.
    void markProgramArguments();
};

/**< Pointer to virtual CPU. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

/** CPU for concolic emulation. */
class Dispatcher: public InstructionSemantics2::DispatcherX86 {
    typedef InstructionSemantics2::DispatcherX86 Super;
protected:
    /** Constructor. */
    explicit Dispatcher(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops)
        : Super(ops, RiscOperators::promote(ops)->wordSizeBits(), RiscOperators::promote(ops)->registerDictionary()) {}

public:
    /** Allocating constructor. */
    static DispatcherPtr instance(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops) {
        return DispatcherPtr(new Dispatcher(ops));
    }

public:
    /** Concrete instruction pointer. */
    rose_addr_t concreteInstructionPointer() const;

    /** True if subordinate process has terminated.
     *
     *  Once the subordinate process terminates no more instructions can be processed and no state information
     *  is available. */
    bool isTerminated() const;

public:
    // overrides
    virtual void processInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Concolic executor.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Concolic executor.
 *
 *  Executes a test case both concretely and symbolically and generates new test cases. */
class ConcolicExecutor: public Sawyer::SharedObject {
public:
    /** Reference counting pointer to @ref ConcolicExecutor. */
    typedef Sawyer::SharedPointer<ConcolicExecutor> Ptr;

    /** Settings to control various aspects of an executor. */
    struct Settings {
        Partitioner2::EngineSettings partitionerEngine;
        Partitioner2::LoaderSettings loader;
        Partitioner2::DisassemblerSettings disassembler;
        Partitioner2::PartitionerSettings partitioner;
    };

private:
    Settings settings_;

protected:
    ConcolicExecutor() {}

public:
    /** Allcoating constructor. */
    static Ptr instance();

    /** Property: Configuration settings.
     *
     *  These settings control the finer aspects of this @ref ConcolicExecutor. They should generally be set immediately
     *  after construction this executor and before any operations are invoked that might use the settings.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */

    /** Describe command-line switches for settings.
     *
     *  Returns a list of command-line switches, organized into groups of related switches, that can be inserted into
     *  a command-line parser in order to parse all the concolic executor settings and generate documentation for them.
     *
     *  The supplied @ref settings reference provides the defaults for the documentation, and is also captured and used later
     *  as the destination for command-line switch arguments when the command-line is parsed and applied. */
    static std::vector<Sawyer::CommandLine::SwitchGroup> commandLineSwitches(Settings &settings /*in,out*/);

    /** Execute the test case.
     *
     *  Executes the test case to produce new test cases. */
    std::vector<TestCase::Ptr> execute(const DatabasePtr&, const TestCase::Ptr&);

    /** Print information about symbolic variables.
     *
     *  This is mostly for debugging. It prints each symbolic variable and some information about where it came from in
     *  the specimen. */
    void printVariables(std::ostream&, const Emulation::StatePtr&) const;

private:
    // Disassemble the specimen and cache the result in the database. If the specimen has previously been disassembled
    // then reconstitute the analysis results from the database.
    Partitioner2::Partitioner partition(const DatabasePtr&, const Specimen::Ptr&);

    // Create the process for the concrete execution.
    Debugger::Ptr makeProcess(const DatabasePtr&, const TestCase::Ptr&, Sawyer::FileSystem::TemporaryDirectory&);

    // Run the execution
    void run(const Emulation::DispatcherPtr&);

    // TODO: Lots of properties to control the finer aspects of executing a test case!
};

} // namespace
} // namespace
} // namespace

#endif
