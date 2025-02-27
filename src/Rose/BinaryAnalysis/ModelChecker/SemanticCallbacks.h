#ifndef ROSE_BinaryAnalysis_ModelChecker_SemanticCallbacks_H
#define ROSE_BinaryAnalysis_ModelChecker_SemanticCallbacks_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/Yaml.h>

#ifdef ROSE_HAVE_YAMLCPP
#include <yaml-cpp/yaml.h>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** User-defined functions for model checking semantics.
 *
 *  This class contains functions that are called by the model checker engine and which are meant to be overridden by
 *  users. This base class provides some reasonable default implementations for some of the functions, but the others are pure
 *  virtual. */
class SemanticCallbacks {
public:
    /** Shared-ownership pointer. */
    using Ptr = SemanticCallbacksPtr;

private:
    const SettingsPtr mcSettings_;                      // model checker settings

protected:
    SemanticCallbacks() = delete;
    explicit SemanticCallbacks(const SettingsPtr&);
public:
    virtual ~SemanticCallbacks();

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions for creating the semantics framework
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Reset internal data.
     *
     *  This is called when the model checker framework is reset. It should reset most internal data associated with the model
     *  checking process itself, but should not reset settings that control how the model checker works.
     *
     *  Thread safety: This method is not thread safe. */
    virtual void reset() {}

    /** Property: Model checker settings.
     *
     *  Returns a pointer to the model checker settings.
     *
     *  Thread safety: This method is thread safe. */
    SettingsPtr mcSettings() const;

    /** Create a prototypical semantic value.
     *
     *  The prototypical value is used to instantiate other parts of the semantic framework and is what defines the semantic
     *  domain in conjunction with the @ref Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RiscOperators "RISC
     *  operators". The default implementation returns a prototypical value of the @ref
     *  Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics "symbolic domain".
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::SValuePtr protoval();

    /** Create an initial register state.
     *
     *  Creates the initial register state for paths. The registers can be initialize here or in @ref initializeState.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::RegisterStatePtr createInitialRegisters() = 0;

    /** Create an initial memory state.
     *
     *  Creates the initial memory state for paths. The memory can be initialized here or in @ref initializeState.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::MemoryStatePtr createInitialMemory() = 0;

    /** Create an initial state.
     *
     *  Creates the initial state for paths beginning at the specified execution unit. The default implementation
     *  uses the @ref Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::State "base semantics state".
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::StatePtr createInitialState();

    /** Create RISC operators.
     *
     *  Creates a new RISC operators object which, together with the @ref protoval, defines the semantic domain.
     *
     *  The returned operators must have a null initial state and null current state. This is to prevent implementations of
     *  this function from accidentally thinking they can initialize the path's state this way -- they can't. Instead, states
     *  are initialized either when they're created (@ref createInitialRegisters, @ref createInitialMemory, @ref
     *  createInitialState) or when they're first associated with a path (@ref initializeState). The latter is the easiest
     *  since it allows one to initialize the state through the RISC operators.  The model checker framework will swap states
     *  into and out of the RISC operators object as necessary.
     *
     *  The RISC operators may refer to an SMT solver if desired. This solver will remain attached to the operators for the
     *  duration of its existence and will be used for operations but not for the model checking. The SMT solver for the model
     *  checker is created by the @ref createSolver method.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr createRiscOperators() = 0;

    /** Configure RISC operators.
     *
     *  This is normally called by createRiscOperators in order to finish configuring the operators. It returns either the
     *  configured object (the argument) or a new object. For instance, if RISC operators tracing is enabled, then it may
     *  return a new tracing operators object that wraps the argument.
     *
     *  The default implementation simply returns the argument.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe API. */
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    configureRiscOperators(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Create model checker solver.
     *
     *  This solver will be used for model checker operations, but not for RISC operations (see @ref createRiscOperators). The
     *  The model checker will insert various path assertions when necessary. Whenever the model checker calls user code that
     *  might modify the solver, it will protect the call with a solver transaction. If user code needs to permanently add
     *  assertions, there are other mechanisms to do so. Namely, one needs to add the assertion through a model checker API so
     *  that the model checker knows about it.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns will be used only by the calling
     *  thread. */
    virtual SmtSolverPtr createSolver() = 0;

    /** Associate solver with semantics.
     *
     *  Given the semantics RISC operators for a thread, and the SMT solver used by the model checking for that thread,
     *  associate the model checker solver with the operators.  This only needs to do anything if the semantics need access
     *  to the solver. For instance, a null address checker would evaluate the address during memoryRead and memoryWrite
     *  RISC operations and would therefore benefit from being able to use the model checker's current solver state.
     *
     *  The model checker solver can be completely independent of the SMT solver that's sometimes associated with symbolic RISC
     *  operators, or they can be the same solver.
     *
     *  Thread safety: The implementation must be thread safe, but the provided RISC operators and solver will be thread
     *  local. */
    virtual void attachModelCheckerSolver(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                                          const SmtSolverPtr&) {}

    /** Initialize the initial state.
     *
     *  This is useful if initializations were not done in @ref createInitialRegisters, @ref createInitialMemory, or @ref
     *  createInitialState.  The benefit of doing it here is that the same code can be used for different semantic domains,
     *  leaving the other functions to choose the domains.  Doing the initialization here also allows you to use the RISC
     *  operators to initialize the states.
     *
     *  Thread safety: The implementation must be thread safe. However, the RISC operators object is thread-local. */
    virtual void initializeState(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Create an instruction dispatcher.
     *
     *  The instruction dispatcher is used to transition from one state to another when "executing" an instruction.
     *
     *  Thread safety: The implementation must be thread safe, but the object it returns does not need to have a thread safe
     *  API. */
    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    createDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // State query functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Address of the next instruction.
     *
     *  Given a state, return the address of the instruction that would be executed next. It should not modify the current
     *  state associated with the supplied RISC operators.
     *
     *  Thread safety: The implementation must be thread safe, but the provided state will be thread local. */
    virtual InstructionSemantics::BaseSemantics::SValuePtr
    instructionPointer(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) = 0;

    /** Addresses and completeness.
     *
     *  This is the return value for @ref nextCodeAddresses. */
    struct CodeAddresses {
        InstructionSemantics::BaseSemantics::SValuePtr ip; /**< Instruction pointer value. */
        std::set<rose_addr_t> addresses;                    /**< The concrete addresses. */
        bool isComplete = true;                             /**< Whether additional non-concrete addresses were found. */
    };

    /** Possible next code addresses.
     *
     *  This method looks at the instruction pointer register (see @ref instructionPointer) and returns whatever concrete
     *  addresses it can find.  It should not modify the current state associated with the RISC operators.
     *
     *  Thread safety: The implementation must be thread safe, but the provided arguments are all thread local. */
    virtual CodeAddresses nextCodeAddresses(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Execution tree functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Decides whether a path should be executed.
     *
     *  This method is called on the path before the @ref preExecute function is called on the execution unit. If this predicate
     *  returns true then execution is skipped. If the path is to be skipped, then this function must also return a non-null,
     *  static, C-style NUL-terminated string explaining why the path should be skipped (if the path is not skipped then the string
     *  is ignored and may be a null pointer).
     *
     *  This function may have side effects such as adding tags to the end of the path. Skipping a path's execution doesn't skip
     *  whether the path is tested for being interesting and possibly added to the interresting queue.
     *
     *  RiscOperators (in particular the associated state) is not available to this callback because it might not have been computed
     *  yet, and computing it might be expensive. One of the points of this callback is to avoid that computation. If you need to
     *  cause a path to not be executed, you can also use the @ref preExecute callback and have it clear the RiscOperators state to
     *  signal an error.
     *
     *  The default implementation always returns false without any side effects.
     *
     *  Thread safety: The implementation must be thread safe. */
    virtual std::pair<bool, const char*> skipExecution(const PathPtr&);

    /** Called before execution starts.
     *
     *  This method is called before the execution unit at the end of the path is executed. It is allowed to change the state prior
     *  to the execution.  Setting the state to a null pointer informs the caller that execution failed. This function is also a
     *  convenient place at which to create tags to be later attached to the node.
     *
     *  The default implementation does nothing.
     *
     *  Thread safety: The implementation must be thread safe, but the provided RISC operators will be thread local. The
     *  current state to which the RISC operators points will have been copied by this thread. */
    virtual std::vector<TagPtr> preExecute(const PathNodePtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Called after execution ends.
     *
     *  This method is called after the execution unit at the end of the path is executed. It is allowed to change the execution's
     *  resulting state.  Setting the state to a null ointer informs the caller that execution failed. This function is also a
     *  convenient place at which to create tags to be later attached to the node.
     *
     *  The default implementation does nothing.
     *
     *  Thread safety: The implementation must be thread safe, but the provided RISC operators will be thread local. The
     *  current state to which the RISC operators points will have been copied by this thread. */
    virtual std::vector<TagPtr> postExecute(const PathNodePtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    /** Return value for @ref nextUnits. */
    struct NextUnit {
        ExecutionUnitPtr unit;                          /**< Unit to be executed. */
        SymbolicExpression::Ptr assertion;              /**< Path assertion for this unit. */
        SmtSolver::Evidence evidence;                   /**< SMT solver evidence that this is a feasible path. */
    };

    /** Discover next execution units.
     *
     *  Given a semantic state of an execution path, determine how the path should be extended by returning the list of next
     *  execution units.  For instance, a conditional branch instruction would likely result in two new execution units: the
     *  target basic block for when the branch is taken, and the fall-through basic block for when the branch is not taken.
     *
     *  All returned execution units should be feasible. That is, this function should check that the address of a candidate
     *  execution unit satisfies the path constraints that are already represented in the supplied solver.
     *
     *  The order of the returned execution units is irrelevant because they will be sorted within the work list according to
     *  the work list priority function. However, since the user defines both the priority and the execution unit types, this
     *  method can communicate to the priority function via data it stores in the execution unit.
     *
     *  Thread safety: The implementation must be thread safe, but the provided RISC operators will be thread local. The
     *  current state to which the RISC operators points will have been copied by this thread. */
    virtual std::vector<NextUnit>
    nextUnits(const PathPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&, const SmtSolverPtr&) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Path creation functions.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Construct a path from a YAML document.
     *
     *  The specified (subtree) root is a YAML sequence whose elements represent the nodes of the path. Each node has a type
     *  and most nodes also have an address or other identifying information. The path must be valid for the configured
     *  model checker -- it must make sense for the current specimen.
     *
     *  Parse errors are reported by throwing a @ref ParseError exception. The @p sourceName is typically the name of the file
     *  from which the YAML was originally parsed, or an empty string to signify that the name of the source is unknown.
     *
     *  @{ */
#ifdef ROSE_HAVE_YAMLCPP
    virtual std::list<ExecutionUnitPtr>
    parsePath(const YAML::Node&, const std::string &sourceName) = 0;
#endif
    virtual std::list<ExecutionUnitPtr>
    parsePath(const Yaml::Node&, const std::string &sourceName) = 0;
    /** @} */
};

} // namespace
} // namespace
} // namespace

#endif
#endif
