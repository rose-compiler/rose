#ifndef ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H
#define ROSE_BinaryAnalysis_Concolic_ConcolicExecutor_H

#include <BaseSemantics2.h>
#include <ConcreteSemantics2.h>
#include <SymbolicSemantics2.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** Semantics for concolic execution. */
namespace Emulation {

typedef InstructionSemantics2::BaseSemantics::SValuePtr BaseValuePtr; /**< Pointer to semantic base values. */
typedef InstructionSemantics2::BaseSemantics::RegisterStatePtr BaseRegistersPtr;/**< Pointer to semantic base registers. */
typedef InstructionSemantics2::BaseSemantics::MemoryStatePtr BaseMemoryPtr; /**< Pointer to semantic base memory. */
typedef InstructionSemantics2::BaseSemantics::StatePtr BaseStatePtr; /**< Pointer to semantic base machine state. */
typedef InstructionSemantics2::BaseSemantics::RiscOperatorsPtr BaseOperatorsPtr; /**< Pointer to semantic base operators. */
typedef InstructionSemantics2::BaseSemantics::DispatcherPtr BaseCpuPtr;/**< Pointer to semantic base CPU. */

typedef InstructionSemantics2::ConcreteSemantics::SValuePtr ConcreteValuePtr; /**< Pointer to concrete values. */
typedef InstructionSemantics2::ConcreteSemantics::SValue ConcreteValue; /**< Type of concrete values. */
typedef InstructionSemantics2::ConcreteSemantics::RegisterStatePtr ConcreteRegistersPtr; /**< Pointer to concrete registers. */
typedef InstructionSemantics2::ConcreteSemantics::RegisterState ConcreteRegisters; /**< Type of concrete register space. */
typedef InstructionSemantics2::ConcreteSemantics::MemoryStatePtr ConcreteMemoryPtr; /**< Pointer to concrete memory. */
typedef InstructionSemantics2::ConcreteSemantics::MemoryState ConcreteMemory; /**< Type of concrete memory space. */
typedef InstructionSemantics2::ConcreteSemantics::StatePtr ConcreteStatePtr; /**< Pointer to concrete machine state. */
typedef InstructionSemantics2::ConcreteSemantics::State ConcreteState; /**< Concrete machine state. */
typedef boost::shared_ptr<class ConcreteOperators> ConcreteOperatorsPtr; /**< Pointer to concrete semantics operators. */

/** Concrete semantic operators. */
class ConcreteOperators: public InstructionSemantics2::ConcreteSemantics::RiscOperators {
public:
    /** Base class. */
    typedef InstructionSemantics2::ConcreteSemantics::RiscOperators Super;

protected:
    /** Constructor for subclasses. Most users will use @ref instance. */
    ConcreteOperators(const BaseValuePtr &protoval, const SmtSolverPtr &solver)
        : Super(protoval, solver) {
        name("Concolic-concrete");
        (void) ConcreteValue::promote(protoval);
    }

    /** Constructor for subclasses. Most users will use @ref instance. */
    ConcreteOperators(const BaseStatePtr &state, const SmtSolverPtr &solver)
        : Super(state, solver) {
        name("Concolic-concrete");
        (void) ConcreteValue::promote(state->protoval());
    }

public:
    /** Create a new instance of concrete semantic operators. */
    static ConcreteOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Create a new instance of concrete semantic operators. */
    static ConcreteOperatorsPtr instance(const BaseValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Create a new instance of concrete semantic operators. */
    static ConcreteOperatorsPtr instance(const BaseStatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic cast. */
    static ConcreteOperatorsPtr promote(const BaseOperatorsPtr &x);

    // Overrides documented in base class
    virtual BaseOperatorsPtr create(const BaseValuePtr &protoval,
                                    const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual BaseOperatorsPtr create(const BaseStatePtr &state,
                                    const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE;

private:
    // Handles a Linux system call of the INT 0x80 variety.
    void systemCall();
};

typedef InstructionSemantics2::SymbolicSemantics::SValuePtr SymbolicValuePtr; /**< Pointer to symbolic values. */
typedef InstructionSemantics2::SymbolicSemantics::SValue SymbolicValue; /**< Type of symbolic values. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterStatePtr SymbolicRegistersPtr; /**< Pointer to symbolic registers. */
typedef InstructionSemantics2::SymbolicSemantics::RegisterState SymbolicRegisters; /**< Type of symbolic register space. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryStatePtr SymbolicMemoryPtr; /**< Pointer to symbolic memory. */
typedef InstructionSemantics2::SymbolicSemantics::MemoryState SymbolicMemory; /**< Type of symbolic memory space. */
typedef InstructionSemantics2::SymbolicSemantics::StatePtr SymbolicStatePtr; /**< Pointer to symbolic machine state. */
typedef InstructionSemantics2::SymbolicSemantics::State SymbolicState; /**< Symbolic machine state. */
typedef boost::shared_ptr<class SymbolicOperators> SymbolicOperatorsPtr; /**< Pointer to symbolic semantic operations. */

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
    BaseValuePtr addr;                                  /**< Memory address where the variable was defined. */

    /** Default constructor. */
    VariableProvenance()
        : whence(INVALID) {}

    /** Construct a register provenance record. */
    explicit VariableProvenance(RegisterDescriptor reg)
        : whence(REGISTER), reg(reg) {}

    /** Construct a memory provenance record. */
    explicit VariableProvenance(const BaseValuePtr &addr)
        : whence(MEMORY), addr(addr) {}
};

/** Mapping of symbolic variables to their provenance information. */
typedef Sawyer::Container::Map<SymbolicExpr::LeafPtr, VariableProvenance> Variables;

/** Symbolic semantic operations. */
class SymbolicOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
public:
    /** Base class. */
    typedef InstructionSemantics2::SymbolicSemantics::RiscOperators Super;

private:
    Variables variables_;                               // where did symbolic variables come from?

protected: // Real constructors
    /** Constructor for subclasses. Most uses will use @ref instance. */
    SymbolicOperators(const BaseValuePtr &protoval, const SmtSolverPtr &solver)
        : Super(protoval, solver) {
        name("Concolic-symbolic");
        (void) SymbolicValue::promote(protoval);
    }

    /** Constructor for subclasses. Most uses will use @ref instance. */
    SymbolicOperators(const BaseStatePtr &state, const SmtSolverPtr &solver)
        : Super(state, solver) {
        name("Concolic-symbolic");
        (void) SymbolicValue::promote(state->protoval());
    }

public:
    /** Create a new instance of symbolic semantic operations. */
    static SymbolicOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Create a new instance of symbolic semantic operations. */
    static SymbolicOperatorsPtr instance(const BaseValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Create a new instance of symbolic semantic operations. */
    static SymbolicOperatorsPtr instance(const BaseStatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr());

    /** Dynamic pointer cast. */
    static SymbolicOperatorsPtr promote(const BaseOperatorsPtr &x);

    /** Symbolic variables. */
    const Variables& variables() const {
        return variables_;
    }

    // Overrides documented in base class
    virtual BaseOperatorsPtr create(const BaseValuePtr &protoval,
                                    const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual BaseOperatorsPtr create(const BaseStatePtr &state,
                                    const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE;
    virtual BaseValuePtr readRegister(RegisterDescriptor reg, const BaseValuePtr &dflt) ROSE_OVERRIDE;
    virtual BaseValuePtr readMemory(RegisterDescriptor segreg, const BaseValuePtr &addr, const BaseValuePtr &dflt,
                                    const BaseValuePtr &cond) ROSE_OVERRIDE;
};

/** Main semantics class for concolic execution. */
class Semantics {
private:
    static const rose_addr_t initialStackPointer_ = 0xc0000000;
    const Partitioner2::Partitioner &partitioner_;
    const RegisterDictionary *regdict_;
    BaseCpuPtr symbolicCpu_;
    BaseCpuPtr concreteCpu_;
    RegisterDescriptor REG_PATH;

public:
    /** Constructor. */
    Semantics(const Partitioner2::Partitioner &partitioner)
        : partitioner_(partitioner) {
        init();
    }

    /** Partitioner used for disassembling. */
    const Partitioner2::Partitioner& partitioner() const { return partitioner_; }

    /** Symbolic semantic operators. */
    SymbolicOperatorsPtr symbolicOperators() const;

    /** Concrete semantic operators. */
    ConcreteOperatorsPtr concreteOperators() const;

    /** Symbolic registers. */
    SymbolicRegistersPtr symbolicRegisters() const;

    /** Concrete registers. */
    ConcreteRegistersPtr concreteRegisters() const;

    /** Symbolic memory. */
    SymbolicMemoryPtr symbolicMemory() const;

    /** Concrete memory. */
    ConcreteMemoryPtr concreteMemory() const;

    /** Emulate one instruction.
     *
     *  Process the specified instruction and transition concrete and symbolic states based on the instruction's semantics. */
    void processInstruction(SgAsmInstruction *insn);

    /** Concrete value of current instruction pointer. */
    rose_addr_t concreteInstructionPointer() const;

    /** Information about symbolic variables.
     *
     * For each symbolic variable in the symbolic machine state, this map contains information about where the variable came
     * from in the binary specimen. */
    const Variables& symbolicVariables() const;

    /** Print the concrete and symbolic states for debugging. */
    void print(std::ostream &out) const;

private:
    // Post-construction initialization.
    void init();
};

} // namespace
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
    void printVariables(std::ostream&, const Emulation::Semantics&) const;

#if 0 // FIXME[Robb Matzke 2019-06-06]: public for testing, but will eventually be private
private:
#endif
    // Disassemble the specimen and cache the result in the database. If the specimen has previously been disassembled
    // then reconstitute the analysis results from the database.
    Partitioner2::Partitioner partition(const DatabasePtr&, const Specimen::Ptr&);

    // Run the execution
    void run(const Partitioner2::Partitioner&);
    void run(const Partitioner2::Partitioner&, rose_addr_t startVa);

    // TODO: Lots of properties to control the finer aspects of executing a test case!
};

} // namespace
} // namespace
} // namespace

std::ostream&
operator<<(std::ostream &out, const Rose::BinaryAnalysis::Concolic::Emulation::Semantics&);

#endif
