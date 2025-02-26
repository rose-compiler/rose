#ifndef ROSE_BinaryAnalysis_ModelChecker_PartitionerModel_H
#define ROSE_BinaryAnalysis_ModelChecker_PartitionerModel_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/ModelChecker/Variables.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/Stack.h>

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
namespace PartitionerModel {

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
    TestMode oobRead = TestMode::OFF;                   /**< How to test for out-of-bounds reads. */
    TestMode oobWrite = TestMode::OFF;                  /**< How to test for out-of-bounds writes. */
    TestMode uninitVar = TestMode::OFF;                 /**< How to test for uninitialized variable reads. */
    rose_addr_t maxNullAddress = 4095;                  /**< Maximum address of the null page. */
    bool debugNull = false;                             /**< When debugging is enabled, show null pointer checking? */
    Sawyer::Optional<rose_addr_t> initialStackVa;       /**< Address for initial stack pointer. */
    MemoryType memoryType = MemoryType::MAP;            /**< Type of memory state. */
    bool solverMemoization = false;                     /**< Whether the SMT solver should use memoization. */
    bool traceSemantics = false;                        /**< Whether to trace all RISC operators. */
};

class SemanticCallbacks;

/** Command-line switches for debug settings.
 *
 *  Inserts some switches into the specified switch group. */
void commandLineDebugSwitches(Sawyer::CommandLine::SwitchGroup&, Settings&);

/** Command-line switches for model settings.
 *
 *  Returns a description of command-line switches that can be used to initialize the specified settings object. Switches
 *  related to debugging are not included in the return value (see also, @ref commandLineDebugSwitches). */
Sawyer::CommandLine::SwitchGroup commandLineModelSwitches(Settings&);

/** Command-line switches for settings.
 *
 *  Returns a description of command-line switches that can be used to initialize the specified settings object. Debug switches
 *  are included in the return value. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function call stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Describes one funtion call in the call stack. */
class FunctionCall {
    Partitioner2::FunctionPtr function_;
    rose_addr_t initialStackPointer_ = 0;
    rose_addr_t framePointerDelta_ = (rose_addr_t)(-4); // Normal frame pointer w.r.t. initial stack pointer
    Sawyer::Optional<rose_addr_t> returnAddress_;
    Variables::StackVariables stackVariables_;

public:
    ~FunctionCall();
    FunctionCall(const Partitioner2::FunctionPtr&, rose_addr_t initialSp, Sawyer::Optional<rose_addr_t> returnAddress,
                 const Variables::StackVariables&);

    /** Property: The function called. */
    Partitioner2::FunctionPtr function() const;

    /** Property: Initial stack pointer.
     *
     *  The concrete value of the stack pointer at the start of the function. */
    rose_addr_t initialStackPointer() const;

    /** Property: Stack variables. */
    const Variables::StackVariables& stackVariables() const;

    /** Property: Frame pointer delta.
     *
     *  Amount to add to the initial stack pointer to get the usual frame pointer.
     *
     * @{ */
    rose_addr_t framePointerDelta() const;
    void framePointerDelta(rose_addr_t);
    /** @} */

    /** Computed frame pointer.
     *
     *  This is the frame pointer computed from the initial stack pointer and the frame pointer delta. */
    rose_addr_t framePointer(size_t nBits) const;

    /** Address to which function returns
     *
     * @{ */
    Sawyer::Optional<rose_addr_t> returnAddress() const;
    void returnAddress(Sawyer::Optional<rose_addr_t>);
    /** @} */

    /** Some basic info about the function call. */
    std::string printableName(size_t nBits) const;
};

/** Function call stack.
 *
 *  A stack of functions that are in progress, with the most recent function call at the top of the stack. */
using FunctionCallStack = Sawyer::Container::Stack<FunctionCall>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics value
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Symbolic values with memory regions.
 *
 *  Values are symbolic, and some values also have memory region information. */
class SValue: public InstructionSemantics::SymbolicSemantics::SValue {
public:
    /** Base class. */
    using Super = InstructionSemantics::SymbolicSemantics::SValue;

    /** Shared-ownership pointer. */
    using Ptr = SValuePtr;

private:
    AddressInterval region_;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(region_);
    }
#endif

public:
    ~SValue();

protected:
    SValue();

    SValue(size_t nBits, uint64_t number);

    explicit SValue(const SymbolicExpression::Ptr&);

public:
    /** Instantiate a new prototypical value.
     *
     *  Prototypical values are only used for their virtual constructors. */
    static Ptr instance();

    /** Instantiate a new data-flow bottom value. */
    static Ptr instanceBottom(size_t nBits);

    /** Instantiate a new undefined value. */
    static Ptr instanceUndefined(size_t nBits);

    /** Instantiate a new unspecified value. */
    static Ptr instanceUnspecified(size_t nBits);

    /** Instantiate a new concrete value. */
    static Ptr instanceInteger(size_t nBits, uint64_t value);

    /** Instantiate a new symbolic value. */
    static Ptr instanceSymbolic(const SymbolicExpression::Ptr &value);

public:
    virtual InstructionSemantics::BaseSemantics::SValuePtr bottom_(size_t nBits) const override;
    virtual InstructionSemantics::BaseSemantics::SValuePtr undefined_(size_t nBits) const override;
    virtual InstructionSemantics::BaseSemantics::SValuePtr unspecified_(size_t nBits) const override;
    virtual InstructionSemantics::BaseSemantics::SValuePtr number_(size_t nBits, uint64_t value) const override;
    virtual InstructionSemantics::BaseSemantics::SValuePtr boolean_(bool value) const override;
    virtual InstructionSemantics::BaseSemantics::SValuePtr copy(size_t newNBits = 0) const override;

    virtual Sawyer::Optional<InstructionSemantics::BaseSemantics::SValuePtr>
    createOptionalMerge(const InstructionSemantics::BaseSemantics::SValuePtr &other,
                        const InstructionSemantics::BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const override;

public:
    /** Promote a base value to a MemoryRegionSemantics value.
     *
     *  The value @p v must have a MemoryRegionSemantics::SValue dynamic type. */
    static Ptr promote(const InstructionSemantics::BaseSemantics::SValuePtr&);

public:
    /** Property: Optional memory region.
     *
     *  This property describes a contiguous region of memory into which this value points if this value were to be
     *  treated as a pointer.  The region usually corresponds to a variable in the source language.
     *
     *  @{ */
    AddressInterval region() const;
    void region(const AddressInterval&);
    /** @} */

public:
    virtual void print(std::ostream&, InstructionSemantics::BaseSemantics::Formatter&) const override;
    virtual void hash(Combinatorics::Hasher&) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer for semantic state. */
using StatePtr = boost::shared_ptr<class State>;

/** Semantic state. */
class State: public InstructionSemantics::SymbolicSemantics::State {
public:
    /** Base type. */
    using Super = InstructionSemantics::SymbolicSemantics::State;

    /** Shared-ownership pointer. */
    using Ptr = StatePtr;

private:
    FunctionCallStack callStack_;

protected:
    State();
    State(const InstructionSemantics::BaseSemantics::RegisterStatePtr&,
          const InstructionSemantics::BaseSemantics::MemoryStatePtr&);

    // Deep copy
    State(const State&);

public:
    /** Allocating constructor. */
    static Ptr instance(const InstructionSemantics::BaseSemantics::RegisterStatePtr&,
                        const InstructionSemantics::BaseSemantics::MemoryStatePtr&);

    /** Deep-copy allocating constructor. */
    static Ptr instance(const StatePtr&);

    /** Virtual constructor. */
    virtual InstructionSemantics::BaseSemantics::StatePtr
    create(const InstructionSemantics::BaseSemantics::RegisterStatePtr&,
           const InstructionSemantics::BaseSemantics::MemoryStatePtr&) const override;

    /** Virtual copy constructor. */
    virtual InstructionSemantics::BaseSemantics::StatePtr clone() const override;

    /** Checked dynamic cast. */
    static Ptr promote(const InstructionSemantics::BaseSemantics::StatePtr&);

public:
    /** Function call stack.
     *
     *  The stack of functions whose calls are in progress. The top of the stack is the most recent function call.
     *
     * @{ */
    const FunctionCallStack& callStack() const;
    FunctionCallStack& callStack();
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics domain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** RISC operators for model checking. */
class RiscOperators: public InstructionSemantics::SymbolicSemantics::RiscOperators {
public:
    using Super = InstructionSemantics::SymbolicSemantics::RiscOperators;

    using Ptr = RiscOperatorsPtr;

private:
    const Settings settings_;
    Partitioner2::PartitionerConstPtr partitioner_;     // not null
    SmtSolver::Ptr modelCheckerSolver_;                 // solver used for model checking, different than RISC operators
    size_t nInstructions_ = 0;
    SemanticCallbacks *semantics_ = nullptr;
    AddressInterval stackLimits_;                       // where the stack can exist in memory
    bool computeMemoryRegions_ = false;                 // compute memory regions. This is needed for OOB analysis.
    const Variables::GlobalVariables &gvars_;

    mutable SAWYER_THREAD_TRAITS::Mutex variableFinderMutex_; // protects the following data members
    Variables::VariableFinderPtr variableFinder_unsync;       // shared by all RiscOperator objects

protected:
    RiscOperators(const Settings&, const Partitioner2::PartitionerConstPtr&, ModelChecker::SemanticCallbacks*,
                  const InstructionSemantics::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&,
                  const Variables::VariableFinderPtr&, const Variables::GlobalVariables&);

public: // Standard public construction-like functions
    ~RiscOperators();

    static Ptr instance(const Settings&, const Partitioner2::PartitionerConstPtr&, ModelChecker::SemanticCallbacks*,
                        const InstructionSemantics::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&,
                        const Variables::VariableFinderPtr&, const Variables::GlobalVariables&);

    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::SValuePtr&, const SmtSolverPtr&) const override;

    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::StatePtr&, const SmtSolverPtr&) const override;

    static Ptr promote(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &x);

public: // Supporting functions
    /** Property: Partitioner.
     *
     *  The partitioner specified when this object was constructed. */
    Partitioner2::PartitionerConstPtr partitioner() const;

    /** Property: Model checker SMT solver.
     *
     *  This property holds the solver used for model checking, which can be different or the same as the solver used generally
     *  by this object's RISC operators.
     *
     * @{ */
    SmtSolver::Ptr modelCheckerSolver() const;
    void modelCheckerSolver(const SmtSolver::Ptr&);
    /** @} */

    /** Property: Semantic callbacks.
     *
     *  These are the non-null semantic callbacks that were provided to the constructor. */
    SemanticCallbacks* semanticCallbacks() const;

    /** Property: Compute memory regions for variables.
     *
     *  If true, then memory regions are computed and used. Otherwise the SValue memory regions are always empty.
     *
     * @{ */
    bool computeMemoryRegions() const;
    void computeMemoryRegions(bool);
    /** @} */

    /** Test whether the specified address is considered to be null.
     *
     *  An address need not be zero in order to be null. For instance, on Linux the entire first page of memory is generally
     *  unmapped in order to increase the chance that a null pointer to a struct/class/array still causes a segmentation fault
     *  even when accessing a member other than the first member.
     *
     *  If a null dereference is detected, then a NullDereferenceTag is thrown. */
    void checkNullAccess(const InstructionSemantics::BaseSemantics::SValuePtr &addr, TestMode, IoMode);

    /** Test whether the specified address is out of bounds for variables.
     *
     *  If an OOB access is detected, then an OutOfBoundsTag is thrown. */
    void checkOobAccess(const InstructionSemantics::BaseSemantics::SValuePtr &addr, TestMode, IoMode, size_t nBytes);

    /** Test whether the specified address accesses an uninitialized variable.
     *
     *  If an uninitialized access is detected, then an UninitReadTag is thrown. */
    void checkUninitVar(const InstructionSemantics::BaseSemantics::SValuePtr &addr, TestMode, size_t nBytes);

    /** Property: Number of instructions executed.
     *
     *  This property contains teh number of instructions executed. It is incremented automatically at the end of each
     *  instruction, but the user can reset it to any other value.
     *
     * @{ */
    size_t nInstructions() const;
    void nInstructions(size_t);
    /** @} */

    /** Ensure call stack has a root function. */
    void maybeInitCallStack(rose_addr_t insnVa);

    /** Push a function onto the call stack. */
    void pushCallStack(const Partitioner2::FunctionPtr &callee, rose_addr_t initialSp, Sawyer::Optional<rose_addr_t> returnVa);

    /** Pop a function from the call stack.
     *
     *  This pops the function from the top of the call stack and optionally discards memory that is beyond the top of the
     *  stack. */
    void popCallStack();

    /** Remove old call stack entries.
     *
     *  Look at the current stack pointer and remove those function call entries that are beyond. In effect, this cleans up
     *  those functions that have returned.
     *
     *  Returns the number of items poppped from the call stack. */
    size_t pruneCallStack();

    /** Print information about the function call stack. */
    void printCallStack(std::ostream&, const std::string &prefix);

    /** Assign a region to an expression.
     *
     *  The region is assigned to the first argument, which is also returned.
     *
     *  If the first argument already has a region, then nothing is done.
     *
     *  Otherwise, if additional arguments are specified and exactly one has a region, then that region is assigned to the
     *  result.
     *
     *  Otherwise, we scan the function call stack (stored in the current semantic state) and look for a variable whose
     *  addresses include the result value. If such a variable is found, then its memory region is assigned to the result
     *  expression.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics::BaseSemantics::SValuePtr &result);

    InstructionSemantics::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics::BaseSemantics::SValuePtr &result,
                 const InstructionSemantics::BaseSemantics::SValuePtr &a);

    InstructionSemantics::BaseSemantics::SValuePtr
    assignRegion(const InstructionSemantics::BaseSemantics::SValuePtr &result,
                 const InstructionSemantics::BaseSemantics::SValuePtr &a,
                 const InstructionSemantics::BaseSemantics::SValuePtr &b);
    /** @} */

public: // Override RISC operations
    virtual void finishInstruction(SgAsmInstruction*) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    number_(size_t nBits, uint64_t value) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    extract(const InstructionSemantics::BaseSemantics::SValuePtr&, size_t begin, size_t end) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    concat(const InstructionSemantics::BaseSemantics::SValuePtr &lowBits,
           const InstructionSemantics::BaseSemantics::SValuePtr &highBits) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    shiftLeft(const InstructionSemantics::BaseSemantics::SValuePtr &a,
              const InstructionSemantics::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    shiftRight(const InstructionSemantics::BaseSemantics::SValuePtr &a,
               const InstructionSemantics::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    shiftRightArithmetic(const InstructionSemantics::BaseSemantics::SValuePtr &a,
                         const InstructionSemantics::BaseSemantics::SValuePtr &nBits) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    unsignedExtend(const InstructionSemantics::BaseSemantics::SValue::Ptr &a, size_t newWidth) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    signExtend(const InstructionSemantics::BaseSemantics::SValue::Ptr &a, size_t newWidth) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    add(const InstructionSemantics::BaseSemantics::SValuePtr &a,
        const InstructionSemantics::BaseSemantics::SValuePtr &b) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    addCarry(const InstructionSemantics::BaseSemantics::SValuePtr &a,
             const InstructionSemantics::BaseSemantics::SValuePtr &b,
             InstructionSemantics::BaseSemantics::SValuePtr &carryOut /*out*/,
             InstructionSemantics::BaseSemantics::SValuePtr &overflowed /*out*/) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    subtract(const InstructionSemantics::BaseSemantics::SValuePtr &a,
             const InstructionSemantics::BaseSemantics::SValuePtr &b) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    subtractCarry(const InstructionSemantics::BaseSemantics::SValuePtr &a,
                  const InstructionSemantics::BaseSemantics::SValuePtr &b,
                  InstructionSemantics::BaseSemantics::SValuePtr &carryOut /*out*/,
                  InstructionSemantics::BaseSemantics::SValuePtr &overflowed /*out*/) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    addWithCarries(const InstructionSemantics::BaseSemantics::SValuePtr &a,
                   const InstructionSemantics::BaseSemantics::SValuePtr &b,
                   const InstructionSemantics::BaseSemantics::SValuePtr &c,
                   InstructionSemantics::BaseSemantics::SValuePtr &carryOut /*out*/) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readRegister(RegisterDescriptor, const InstructionSemantics::BaseSemantics::SValuePtr&) override;

    virtual void
    writeRegister(RegisterDescriptor, const InstructionSemantics::BaseSemantics::SValuePtr&) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
               const InstructionSemantics::BaseSemantics::SValuePtr &cond) override;

    virtual void
    writeMemory(RegisterDescriptor segreg, const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics::BaseSemantics::SValuePtr &value,
                const InstructionSemantics::BaseSemantics::SValuePtr &cond) override;
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
    Partitioner2::PartitionerConstPtr partitioner_;     // generally shouldn't be changed once model checking starts, non-null
    SmtSolver::Memoizer::Ptr smtMemoizer_;              // memoizer shared among all solvers
    AddressInterval stackRegion_;                       // addresses considered to be in the stack's area of memory

    mutable SAWYER_THREAD_TRAITS::Mutex unitsMutex_;    // protects only the units_ data member
    Sawyer::Container::Map<rose_addr_t, ExecutionUnitPtr> units_; // cached execution units

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    std::set<uint64_t> seenStates_;                     // states we've seen, by hash
    size_t nDuplicateStates_ = 0;                       // number of times a previous state is seen again
    size_t nSolverFailures_ = 0;                        // number of times the SMT solver failed
    UnitCounts unitsReached_;                           // number of times basic blocks were reached
    Variables::VariableFinderPtr variableFinder_;       // also shared by all RiscOperator objects
    Variables::GlobalVariables gvars_;                  // all global variables identified by variableFinder_

    // This model is able to follow a single path specified by the user. In order to do that, the user should call
    // followOnePath to provide the ordered list of execution units. The nextCodeAddresses function is overridden to
    // return only the address of the next execution unit in this list, or none when the list is empty. The findUnit
    // function is enhanced to return the next execution unit and remove it from the list.
    //
    // These are also protected by mutex_
    bool followingOnePath_ = false;
    std::list<ExecutionUnitPtr> onePath_;               // execution units yet to be consumed by findUnit

protected:
    SemanticCallbacks(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::PartitionerConstPtr&);
public:
    ~SemanticCallbacks();

public:
    static Ptr instance(const ModelChecker::SettingsPtr&, const Settings&, const Partitioner2::PartitionerConstPtr&);

public:
    /** Property: Partitioner being used. */
    Partitioner2::PartitionerConstPtr partitioner() const;

public:
    /** Return the settings for this model. */
    const Settings& p2Settings() const;

    /** Cause this model to follow only one path through the specimen.
     *
     *  The path is provided as an ordered list of execution units. The initial execution unit will not appear in this list
     *  since it should have been added to the model checker work list already, and it must be the only work on that list. In
     *  this mode of operation, the model checker is single threaded and will follow only this specified path until it reaches
     *  the end or some other condition causes the model checker to abandon the path earlier.
     *
     *  When the model checker runs, the @ref ModelChecker::SemanticCallbacks::nextCodeAddresses method is overridden to return
     *  only the address of the next execution unit on this list, and the internal @c findUnit function returns the next unit,
     *  removing it from the list. When the list becomes empty, then no more code addresses or units are returned.
     *
     *  If this function is called when the model checker is already in @ref followingOnePath mode, the behavior is as if
     *  @ref followingOnePath was cleared first.
     *
     *  Thread safety: This function is thread safe. */
    void followOnePath(const std::list<ExecutionUnitPtr>&);

    /** Property: Whether we are in follow-one-path mode.
     *
     *  This property can be queried or modified at any time. The @ref followOnePath function sets the property, which must
     *  then be cleared manually if you want to continue to use the model checker in its normal mode. Clearing this property
     *  will also discard any execution units that remain for the one path mode. Setting the property manually has no effect
     *  if the property was already set, but if it was clear then the model checker behaves as if it reached the end of the
     *  one path: it will stop exploring.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     * @{ */
    bool followingOnePath() const;
    void followingOnePath(bool);
    /** @} */

    /** Property: SMT solver memoizer.
     *
     *  This is the memoizer used each time a new SMT solver is created. An initial memoizer is created by the @c
     *  SemanticCallbacks constructor if the @p solverMemoization field of the @ref Settings is set. If memoization is
     *  not enabled in the settings, then a memoizer is not used even if one is set for this property.
     *
     * @{ */
    SmtSolver::Memoizer::Ptr smtMemoizer() const;
    void smtMemoizer(const SmtSolver::Memoizer::Ptr&);
    /** @} */

    /** Property: Stack region.
     *
     *  Part of the address space reserved for the stack. */
    AddressInterval stackRegion() const;

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
    virtual bool filterNullDeref(const InstructionSemantics::BaseSemantics::SValuePtr &addr, SgAsmInstruction*,
                                 TestMode testMode, IoMode ioMode);

    /** Filter out of bounds access.
     *
     *  Returns true to accept an out of bounds access, or false to say that it's a false positive.
     *
     *  The arguments are the symbolic virtual starting address for the accessed memory (@p addr), the memory region that's
     *  associated with the symbolic address (@p referencedRegion), the memory region that the I/O operation is actually
     *  accessing (@p accessedRegion), the instruction that's accessing that memory region (@p insn), whether the accessed
     *  memory "must" or "may" be outside the referenced region (@p testMode), and whether the instruction is reading or
     *  writing to the memory (@p ioMode). The @p intendedVariable and @p intendedVariableLocation is information about the
     *  stack variable that was intended to be accessed and its location in memory. The @p accessedVariable and @p
     *  accessedVariableLocation describe a variable (perhaps one of many) that was actually accessed, if any.
     *
     *  The default implementation always returns true.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual bool filterOobAccess(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                                 const AddressInterval &referencedRegion, const AddressInterval &accessedRegion,
                                 SgAsmInstruction *insn, TestMode testMode, IoMode ioMode,
                                 const FoundVariable &intendedVariable, const FoundVariable &accessedVariable);

    virtual bool filterUninitVar(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                                 const AddressInterval &referencedREgion, const AddressInterval &accessedRegion,
                                 SgAsmInstruction *insn, TestMode testMode, const FoundVariable &accessedVariable);

public:
    virtual void reset() override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr protoval() override;

    virtual InstructionSemantics::BaseSemantics::RegisterStatePtr createInitialRegisters() override;

    virtual InstructionSemantics::BaseSemantics::MemoryStatePtr createInitialMemory() override;

    virtual InstructionSemantics::BaseSemantics::StatePtr createInitialState() override;

    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr createRiscOperators() override;

    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    configureRiscOperators(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual void
    initializeState(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    createDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual SmtSolver::Ptr createSolver() override;

    virtual void attachModelCheckerSolver(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                                          const SmtSolver::Ptr&) override;

    virtual CodeAddresses
    nextCodeAddresses(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<TagPtr>
    preExecute(const ExecutionUnitPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<TagPtr>
    postExecute(const ExecutionUnitPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    instructionPointer(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) override;

    virtual std::vector<NextUnit>
    nextUnits(const PathPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&) override;

#ifdef ROSE_HAVE_YAMLCPP
    virtual std::list<ExecutionUnitPtr>
    parsePath(const YAML::Node&, const std::string &sourceName) override;
#endif

    virtual std::list<ExecutionUnitPtr>
    parsePath(const Yaml::Node&, const std::string &sourceName) override;

protected:
    Variables::VariableFinderPtr variableFinder() const;
    Variables::GlobalVariables& globalVariables();

private:
    // Records the state hash and returns true if we've seen it before.
    bool seenState(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    // Find and cache the execution unit at the specified address.
    ExecutionUnitPtr findUnit(rose_addr_t va, const Progress::Ptr&);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
