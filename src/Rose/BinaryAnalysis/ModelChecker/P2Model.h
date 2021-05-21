#ifndef ROSE_BinaryAnalysis_ModelChecker_P2Model_H
#define ROSE_BinaryAnalysis_ModelChecker_P2Model_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Sawyer/CommandLine.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** Model checking model using Partitioner.
 *
 *  This model checker uses @ref Rose::BinaryAnalysis::Partitioner2 data structures and is based on the
 *  model defined at one time by @ref Rose::BinaryAnalysis::FeasiblePath, which this model is meant to replace.
 *
 *  This model uses the symbolic semantics domain. Registers are stored in a generic register state that adapts to the
 *  registers that are in use. Memory can be list- or map-based and could possibly support purely symbolic memory as well. The
 *  list- and map-based memory states can also use concrete state from a @ref MemoryMap. */
namespace P2Model {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings for this model checker
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Settings for this model. */
struct Settings {
    /** Type of memory state to use. */
    enum class MemoryType {
        LIST,                                           /**< Reverse chronological list. This is more accurate. */
        MAP                                             /**< Map indexed by symbolic address hash. This is faster. */
    };

    TestMode nullRead = TestMode::MUST;                 /**< How to test for reads from the null page. */
    TestMode nullWrite = TestMode::MUST;                /**< How to test for writes from the null page. */
    rose_addr_t maxNullAddress = 4095;                  /**< Maximum address of the null page. */
    bool debugNull = false;                             /**< When debugging is enabled, show null pointer checking? */
    Sawyer::Optional<rose_addr_t> initialStackVa;       /**< Address for initial stack pointer. */
    MemoryType memoryType = MemoryType::MAP;            /**< Type of memory state. */
    bool solverMemoization = true;                      /**< Whether the SMT solver should use memoization. */
};

class SemanticCallbacks;

/** Command-line switches for settings.
 *
 *  Returns a description of command-line switches that can be used to initialize the specified settings object. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics domain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** RISC operators for model checking. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
    using Super = InstructionSemantics2::SymbolicSemantics::RiscOperators;

public:
    using Ptr = RiscOperatorsPtr;

private:
    const Settings settings_;
    const Partitioner2::Partitioner &partitioner_;
    SmtSolver::Ptr modelCheckerSolver_;
    size_t nInstructions_ = 0;
    SemanticCallbacks *semantics_ = nullptr;

protected:
    RiscOperators(const Settings&, const Partitioner2::Partitioner&, SemanticCallbacks*,
                  const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

public: // Standard public construction-like functions
    ~RiscOperators();

    static Ptr instance(const Settings&, const Partitioner2::Partitioner&, SemanticCallbacks*,
                        const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr&, const SmtSolverPtr&) const override;

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr&, const SmtSolverPtr&) const override;

    static Ptr promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x);

public: // Supporting functions
    /** Property: Model checker SMT solver.
     *
     *  This property holds the solver used for model checking, which can be different or the same as the solver used generally
     *  by this object's RISC operators.
     *
     * @{ */
    SmtSolver::Ptr modelCheckerSolver() const;
    void modelCheckerSolver(const SmtSolver::Ptr&);
    /** @} */


    /** Test whether the specified address is considered to be null.
     *
     *  An address need not be zero in order to be null. For instance, on Linux the entire first page of memory is generally
     *  unmapped in order to increase the chance that a null pointer to a struct/class/array still causes a segmentation fault
     *  even when accessing a member other than the first member. */
    bool isNull(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, TestMode, IoMode);

    /** Property: Number of instructions executed.
     *
     *  This property contains teh number of instructions executed. It is incremented automatically at the end of each
     *  instruction, but the user can reset it to any other value.
     *
     * @{ */
    size_t nInstructions() const;
    void nInstructions(size_t);
    /** @} */

public: // Override RISC operations
    virtual void finishInstruction(SgAsmInstruction*) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics2::BaseSemantics::SValuePtr &cond) override;

    void
    writeMemory(RegisterDescriptor segreg, const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics2::BaseSemantics::SValuePtr &value,
                const InstructionSemantics2::BaseSemantics::SValuePtr &cond) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High-level semantic operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Virtual definition of semantic operations for model checking. */
class SemanticCallbacks: public Rose::BinaryAnalysis::ModelChecker::SemanticCallbacks {
public:
    using Ptr = SemanticCallbacksPtr;
    using UnitCounts = Sawyer::Container::Map<rose_addr_t, size_t>;

private:
    Settings settings_;                                 // settings are set by the constructor and not modified thereafter
    const Partitioner2::Partitioner &partitioner_;      // generally shouldn't be changed once model checking starts

    mutable SAWYER_THREAD_TRAITS::Mutex unitsMutex_;    // protects only the units_ data member
    Sawyer::Container::Map<rose_addr_t, ExecutionUnitPtr> units_; // cached execution units

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::set<uint64_t> seenStates_;                     // states we've seen, by hash
    size_t nDuplicateStates_ = 0;                       // number of times a previous state is seen again
    size_t nSolverFailures_ = 0;                        // number of times the SMT solver failed
    UnitCounts unitsReached_;                           // number of times basic blocks were reached

protected:
    SemanticCallbacks(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::Partitioner&);
public:
    ~SemanticCallbacks();

public:
    static Ptr instance(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::Partitioner&);

public:
    /** Property: Partitioner being used. */
    const Partitioner2::Partitioner& partitioner() const;

public:
    /** Property: Number of duplicate states.
     *
     *  This read-only property returns the number of times that a semantic state was encountered that had been encountered
     *  previously. Whenever a state is re-encountered, the corresponding path is not extended since extending it would also
     *  result in duplicate states (even though the paths to those states are different).
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nDuplicateStates() const;

    /** Property: Number of times the SMT solver failed.
     *
     *  This read-only property contains the number of times that an attempt was made to extend a path and the SMT solver
     *  returned an "unknown" answer (i.e., neither "satisfied" nor "unsatisfied"). An "unknown" result is usually caused
     *  only when a per-call solver timeout is enabled and the solver was unable to finish in the allotted time.
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nSolverFailures() const;

    /** Property: Number of execution units reached.
     *
     *  This read-only property returns the number of unique execution unit addresses that were reached (i.e., semantically
     *  executed) during exploration. An execution unit is normally either a basic block or an externally defined function.
     *  However, if we attempt to execute an instruction that wasn't known to the partitioner, then each such instruction
     *  is counted separately without being part of a basic block.
     *
     *  Thread safety: This property accessor is thread safe. */
    size_t nUnitsReached() const;

    /** Property: Execution units reached.
     *
     *  Returns a mapping from execution unit address to the number of times that the address was semantically executed. The
     *  counts are returned by value rather than reference in order to achieve thread safety.
     *
     *  Thread safety: This property accessor is thread safe. */
    UnitCounts unitsReached() const;


    /** Filter null dereferences.
     *
     *  Returns true to accept the nullptr dereference, or false to say that it's not really a nullptr dereference.
     *
     *  The default implementation always returns true.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual bool filterNullDeref(const InstructionSemantics2::BaseSemantics::SValuePtr &addr, TestMode testMode, IoMode ioMode);

public:
    virtual void reset() override;

    virtual InstructionSemantics2::BaseSemantics::RegisterStatePtr createInitialRegisters() override;

    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr createInitialMemory() override;

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr createRiscOperators() override;

    virtual void
    initializeState(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics2::BaseSemantics::DispatcherPtr
    createDispatcher(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual SmtSolver::Ptr createSolver() override;

    virtual void attachModelCheckerSolver(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&,
                                          const SmtSolver::Ptr&) override;

    virtual std::vector<TagPtr>
    preExecute(const ExecutionUnitPtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    instructionPointer(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<NextUnit>
    nextUnits(const PathPtr&, const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&) override;

private:
    // Records the state hash and returns true if we've seen it before.
    bool seenState(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&);

    // Find and cache the execution unit at the specified address.
    ExecutionUnitPtr findUnit(rose_addr_t va);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
