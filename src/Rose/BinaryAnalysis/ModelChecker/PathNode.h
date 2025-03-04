#ifndef ROSE_BinaryAnalysis_ModelChecker_PathNode_H
#define ROSE_BinaryAnalysis_ModelChecker_PathNode_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER

#include <Rose/BinaryAnalysis/ModelChecker/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/SmtSolver.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

/** One node of an execution path.
 *
 *  An execution path node points to its predecessor, but the predecessor does not point back. Execution paths must always be
 *  referenced according to their final node, and all references are counted.  This method of organizing the nodes has a number of
 *  advantages over bi-directional pointing.
 *
 *  The execution nodes form a tree data structure. A worklist points to the leaves of the tree and worker threads try to advance
 *  the frontier.  The tree is dynamically created by the workers, and parts of it are deleted when no longer required. */
class PathNode: public std::enable_shared_from_this<PathNode> {
public:
    /** Shared-ownership pointer. */
    using Ptr = PathNodePtr;

private:
    const Ptr parent_;                                             // Previous node in execution path. Null for root node.
    const ExecutionUnitPtr executionUnit_;                         // The thing to execute, non-null

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;                    // protects all following data members

    // Path states are used as follows:
    //
    //    When a path is executed, its incomingState_ is copied, modified by the thread's semantics operators, and the
    //    resulting state is stored as outgoingState_. The incomingState_ (which is shared among paths, see next bullet) is
    //    then set to null.
    //
    //    When a path is extended by creating new paths each with an additional node, the new nodes' incomingState_ points to
    //    the original path's outgoingState_, then the original path's outgoingState_ is set to null. Thus the new paths all
    //    point to a shared incoming state.
    InstructionSemantics::BaseSemantics::StatePtr incomingState_;  // shared state before execution, then null afterward
    InstructionSemantics::BaseSemantics::StatePtr outgoingState_;  // state after execution, or null if error during execution

    bool executionFailed_ = false;                                 // true iff execution failed (and outgoingState_ therefore null)
    std::vector<SymbolicExpression::Ptr> assertions_;              // assertions for the model checker for this node of the path
    SmtSolver::Evidence evidence_;                                 // SMT evidence that path to and including this node is feasible
    std::vector<TagPtr> tags_;                                     // tags associated with this node of the path
    uint32_t id_ = 0;                                              // a random path ID number
    double processingTime_ = NAN;                                  // time required to execute and extend this node
    double sortKey_ = NAN;                                         // field for user-defined sorting

protected:
    PathNode() = delete;

    /** Allocating constructor.
     *
     *  Create an initial path node. This node has no parent. See @ref instance. */
    PathNode(const ExecutionUnitPtr&);

    /** Allocating constructor.
     *
     *  Create a non-initial path node. This node is a continuation of the path ending at parent. See @ref instance. */
    PathNode(const Ptr &parent, const ExecutionUnitPtr&, const SymbolicExpression::Ptr &assertion,
             const SmtSolver::Evidence&, const InstructionSemantics::BaseSemantics::StatePtr &parentOutgoingState);

public:
    ~PathNode();

public:
    /** Construct a root node.
     *
     *  A root node is the starting point for all paths within that virtual execution tree.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(const ExecutionUnitPtr&);

    /** Construct a child node.
     *
     *  A child node has the specified parent path as a prefix. The child node's incoming state will be the (eventually
     *  computed) outgoing state of the parent node, and a new outgoing state will eventually be computed for this node by
     *  "executing" this node in a worker thread. The @p assertion must be true but is not checked here.
     *
     *  Thread safety: This constructor is thread safe. */
    static Ptr instance(const Ptr &parent, const ExecutionUnitPtr&, const SymbolicExpression::Ptr &assertion,
                        const SmtSolver::Evidence&, const InstructionSemantics::BaseSemantics::StatePtr &parentOutgoingState);

    /** Property: Identifier.
     *
     *  Every node has a randomly generated identifier.
     *
     *  Thread safety: This property accessor is thread safe. */
    uint32_t id() const;

    /** Property: Key for user-defined sorting.
     *
     *  This property holds a user-defined value that can be used by the @ref PathPrioritizer functions. The model checker
     *  framework itself does not use this property for anything and all new nodes initialize this property to NaN. If this
     *  is used for sorting, it generally shouldn't be changed once the node has been sorted into position. E.g., a priority
     *  queue will not automatically resort its members if any of their keys change.
     *
     *  Thread safety: This property accessor is thread safe.
     *
     * @{ */
    double sortKey() const;
    void sortKey(double);
    /** @} */

    /** Property: Parent node.
     *
     *  The parent node is the execution path predecessor, the node that was executed prior to this current node. All nodes
     *  have a non-null predecessor except for the first node of an execution path, whose parent is null.
     *
     *  This property is read-only. The preceding node was specified when this object was created.
     *
     *  Thread safety: This method is thread safe. */
    Ptr parent() const;

    /** Property: Unit of execution.
     *
     *  The unit of execution is the thing that's executed at this node of the path. It can be a single instruction, a basic
     *  block, a function summary, or anything else defined by the user by subclassing the @ref ExecutionUnit type.
     *
     *  This property is read-only. The execution unit was specified when this object was created.
     *
     *  Thread safety: This method is thread safe. */
    ExecutionUnitPtr executionUnit() const;

    /** Length of node in steps.
     *
     *  Returns the length of this node. The length is measured in terms of number of steps, which in turn is computed from the
     *  associated @ref ExecutionUnit::nSteps properties.
     *
     *  Thread safety: This method is thread safe. */
    size_t nSteps() const;

    /** Insert an assertion for the SMT solver.
     *
     *  An assertion is a Boolean expression that can be true according to the model checker. The most common use for this during
     *  model checking is to indicate how the conditional branches in the path depend on other variables.
     *
     *  Thread safety: This method is thread safe. */
    void assertion(const SymbolicExpression::Ptr&);

    /** Returns all the assersions for this node.
     *
     *  Thread safety: This method is thread safe. */
    std::vector<SymbolicExpression::Ptr> assertions() const;

    /** Returns the SMT solver evidence for path feasibility.
     *
     *  Returns that evidence that shows that the path ending at this node is feasible.
     *
     *  Thread safety: This method is thread safe. */
    SmtSolver::Evidence evidence() const;

    /** Execute this node.
     *
     *  If necessary, executes all predecessors recursively and then executes this node. Afterward, this node and all
     *  predecessors will store their outgoing states. Only nodes that have not been previously executed are executed.
     *
     *  Although the RiscOperators may point to a solver already, the solver supplied as an argument is the one that's being
     *  used by the model checker itself.
     *
     *  Thread safety: This method is thread safe, but the RISC operators must be thread local. */
    void execute(const SettingsPtr&, const SemanticCallbacksPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                 const SmtSolver::Ptr&);

    /** Don't execute this node.
     *
     *  This is similar to calling @ref execute in a way that fails. The path is marked so that @ref executionFailed returns
     *  true.
     *
     *  Thread safety: This method is thread safe. */
    void doNotExecute();

    /** Property: Whether execution has failed.
     *
     *  This property is true if there was a previous attempt to execute this node and that attempt failed.
     *
     *  Thread safety: This method is thread safe. */
    bool executionFailed() const;

    /** Returns a copy of the outgoing state.
     *
     * If this node has been successfully executed, then a copy of its outgoing state is returned, otherwise null is
     * returned.
     *
     * See also, @ref execute, which creates the outgoing state if necessary.
     *
     * Thread safety: This method is thread safe. The state is owned by only this node. */
    InstructionSemantics::BaseSemantics::StatePtr copyOutgoingState() const;

    class BorrowedOutgoingState {
        friend class PathNode;
    private:
        PathNode* const node;
    public:
        const InstructionSemantics::BaseSemantics::StatePtr state;
    private:
        BorrowedOutgoingState() = delete;
        BorrowedOutgoingState& operator=(const BorrowedOutgoingState&) = delete;
        BorrowedOutgoingState(PathNode*, const InstructionSemantics::BaseSemantics::StatePtr&);
    public:
        ~BorrowedOutgoingState();
    };

    /** Borrow the state from a node.
     *
     *  The semantic states generally don't have thread-safe APIs because they're designed to be transformed by calling a
     *  sequence of state-modifying functions.  The model checker logic should prevent two threads from ever
     *  accessing a node's state concurrently, but we want to be able to check it, which is what this function does. It operates
     *  by having the node relinguish its state, and then returning the state at the end of the scope (either normal scope exit
     *  or exception).  The borrowing is not foolproof--nothing prevents the borrower from keeping a reference to the state and
     *  continuing to use it after the borrow is returned.
     *
     * @code
     *  {
     *    PathNode::Ptr node = ...; // Some node not accessible by other threads, but we're not absolutely sure
     *    auto borrowed = node->borrowOutgoingState(); // I now have the state, and the node doesn't have it
     *    doSomethingWith(borrowed.state); // I can access and even modify the state with no other threads interfering
     *    throw x; // I can even throw an exception
     *  } // the state is now returned to the node, with abort if some other thread managed to give it a new state
     * @endcode
     *
     *  Thread safety: This method is thread safe. */
    BorrowedOutgoingState borrowOutgoingState();

    /** Release the outgoing state.
     *
     *  The outgoing state for this node is set to null. This should only be done after this path has been extended in
     *  all the ways it will ever be extended and its outgoing state pointer has been copied into the incoming state of each
     *  path extension.
     *
     *  Thread safety: This method is thread safe. */
    void releaseOutgoingState();

    /** Append tags.
     *
     *  The specified tags are added to the end of the tag list for this node.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    void appendTag(const TagPtr&);
    void appendTags(const std::vector<TagPtr>&);
    /** @} */

    /** Number of tags attached to this node.
     *
     *  Thread safety: This method is thread safe. */
    size_t nTags() const;

    /** All tags for this node.
     *
     *  Thread safety: This method is thread safe. */
    std::vector<TagPtr> tags() const;

    /** Time spent processing this node.
     *
     *  Returns the elapsed time to process this node, measured in seconds.
     *
     *  Thread safety: This method is thread safe. */
    double processingTime() const;

    /** Add additional time to the processing time.
     *
     *  Updates this node's @ref processingTime by added the specified time in seconds.
     *
     *  Thread safety: This method is thread safe. */
    void incrementProcessingTime(double seconds);

    /** Address of execution unit.
     *
     *  Thread safety: This method is thread safe. */
    Sawyer::Optional<Address> address() const;

    /** Printable name for this node.
     *
     *  The returned string is suitable for terminal output, used mostly for debugging.
     *
     *  Thread safety: This method is thread safe. */
    std::string printableName() const;

    /** Output path node header as YAML.
     *
     *  The output does not include the individual steps for the node. */
    void toYamlHeader(const SettingsPtr&, std::ostream&, const std::string &prefix) const;

    /** Output the steps as YAML. */
    void toYamlSteps(const SettingsPtr&, std::ostream&, const std::string &prefix,
                     size_t stepOrigin, size_t maxSteps) const;

    /** Return steps as SARIF locations. */
    std::vector<Sarif::LocationPtr> toSarif(size_t maxSteps) const;

private:
    void restoreOutgoingState(const InstructionSemantics::BaseSemantics::StatePtr&);

    std::pair<InstructionSemantics::BaseSemantics::StatePtr, bool>
    incomingState_NS(const SettingsPtr&, const SemanticCallbacksPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                     const SmtSolver::Ptr&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
