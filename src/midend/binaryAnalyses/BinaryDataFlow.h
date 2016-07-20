#ifndef ROSE_BinaryAnalysis_DataFlow_H
#define ROSE_BinaryAnalysis_DataFlow_H

#include "DataFlowSemantics2.h"
#include "Diagnostics.h"
#include "SymbolicSemantics2.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/DistinctList.h>
#include <sstream>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {

/** Various tools for data-flow analysis.
 *
 *  @section binary_dataflow_s1 Comparison with source code data-flow
 *
 *  Binary data-flow analysis differs from source code data-flow analysis in that source code has well-known, named variables
 *  and the analysis is often simplified by considering only those variables (e.g., whole arrays, not array element
 *  granularity), and by ignoring aliasing issues.  But because binaries have no named variables but rather one large array of
 *  memory with computed addresses, a binary data-flow analysis <em>must</em> be at address granularity and account for
 *  aliasing. In fact, because of the rich set of addressing modes, it is very common that within a single instruction some of
 *  the inputs and/or outputs depend on other inputs.
 *
 *  Another difference is in the number of basic operations that need to be supported. Source-level languages typically have at
 *  most a few dozen basic operations for which data-flow must be explicitly encoded in the analysis, while binary analysis
 *  needs basic data-flow information about hundreds of CPU instructions across the supported architectures.  In fact, there
 *  are so many unique instructions that the implementation here takes a different approach:  it leverages the fact that ROSE
 *  has instruction semantics and is able to discern data-flow within an instruction (or even across an entire basic block) by
 *  executing the instruction (or block) in a special data-flow semantic domain.  The domain is an aggregate domain consisting
 *  of a main user-specified domain (often symbolic), a domain to track data-flow, and any number of additional user-specified
 *  domains.  This approach works to discover multiple data-flow within the executed item (instruction or basic block); most CPU
 *  instructions have multiple data flows (e.g., "PUSH" has at least three independent data flows: incrementing of the
 *  instruction pointer, moving data between stack and register, incrementing the stack pointer).
 *
 *  Two additional minor things to be aware of in binary data-flow are (1) the difference in size between registers and memory
 *  locations, and (2) the use of idioms to perform common tasks.  For instance, a read from memory to a 32-bit register is
 *  actually four separate 8-bit reads at the data-flow level since memory holds 8-bit values.  An example of an idiom is the
 *  "XOR EAX, EAX" instruction to set the EAX register to zero (this is important for taint analysis, since it effectively
 *  removes any taint from EAX).
 *
 *  @section binary_dataflow_s2 The major components
 *
 *   The core component of the binary data-flow is an @ref Engine class template. Its purpose is to collect all the types
 *   and objects necessary for a data-flow analysis and to iteratively solve a data-flow equation.
 *
 *   Additional components provide functionality that's useful during the core solving process, but not necessary. For
 *   instance, one can use abstract locations (@ref AbstractLocation) to represent variables. An abstract location is either a
 *   register or a memory address.  Registers are represented by a @ref RegisterDescriptor while addresses are represented by a
 *   semantic value in some user-specified domain (often symbolic, see @ref
 *   rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValue).
 *
 *  @section s2 Prerequisites for the core data flow-engine
 *
 *   See @ref Engine for how data-flow is solved. In short, data-flow consists of values (taken from a lattice) that are stored
 *   in variables (abstract locations, memory, registers).  A set of variables and their values (a.k.a., state) is stored at
 *   each vertex in a control flow graph. The solver iterates over the control flow graph. It invokes a user-defined transfer
 *   function at each vertex. The transfer function takes an incoming state and produces an outgoing state. The incoming state
 *   for a vertex is calculated by merging the outgoing states of its predecessor vertices. When the iteration reaches a fixed
 *   point the analysis is complete. */
class DataFlow {
public:
    /** Data-flow graph.
     *
     *  Vertices are abstract locations (register descriptors and/or memory addresses) and edges indicate the flow of data from
     *  abstract location to another.  The edge IDs are in the order they were added; each edge also has a sequence number
     *  that is initially equal to the edge ID, although edge IDs can change if edges are erased from the graph). */
    typedef InstructionSemantics2::DataFlowSemantics::DataFlowGraph Graph;

    /** Variable participating in data flow.
     *
     *  A variable in binary data-flow analysis is an abstract location referencing either a register or memory cell.  The
     *  address for memory locations is an arbitrary semantic expression (@ref
     *  rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValue). */
    typedef AbstractLocation Variable;

    /** List of variables. */
    typedef std::list<Variable> VariableList;

    /** Map from CFG vertex to data-flow graph.
     *
     *  This data structure holds a data-flow graph for each control flow graph vertex.  The data-flow graph represents
     *  data-flow from one abstract location to another.  The union of all abstract locations across all control flow vertices
     *  is the set of variables present in the specimen being analyzed.  The map is keyed by CFG vertex IDs. */
    typedef Sawyer::Container::Map<size_t, Graph> VertexFlowGraphs;

public:
    /** Data-flow exception base class. */
    class Exception: public std::runtime_error {
    public:
        explicit Exception(const std::string &s): std::runtime_error(s) {}
    };

    /** Exceptions when a fixed point is not reached. */
    class NotConverging: public Exception {
    public:
        explicit NotConverging(const std::string &s): Exception(s) {}
    };

private:
    InstructionSemantics2::BaseSemantics::RiscOperatorsPtr userOps_; // operators (and state) provided by the user
    InstructionSemantics2::DataFlowSemantics::RiscOperatorsPtr dfOps_; // data-flow operators (which point to user ops)
    InstructionSemantics2::BaseSemantics::DispatcherPtr dispatcher_; // copy of user's dispatcher but with DataFlowSemantics
    static Sawyer::Message::Facility mlog;              // diagnostics for data-flow
    void init(const InstructionSemantics2::BaseSemantics::DispatcherPtr&);

public:
    /** Constructor.
     *
     *  Constructs a new data-flow analysis framework using the specified user-level instruction semantics.  The semantics
     *  should consist of an instruction dispatcher appropriate for the architecture being analyzed, and an associated semantic
     *  domain that will be used to calculate memory addresses. The SymbolicSemantics domain is typically used. */
    DataFlow(const InstructionSemantics2::BaseSemantics::DispatcherPtr &userDispatcher) {
        init(userDispatcher);
    }

    /** Initialize diagnostics.
     *
     *  This is called by rose::Diagnostics::initialize. */
    static void initDiagnostics();

public:
    /** Compute data-flow.
     *
     *  Computes and returns a graph describing how data-flow occurs for the specified instructions (which must have linear
     *  control flow, e.g., from a single basic block). The vertices of the returned graph contain the abstract locations that
     *  are referenced (read or written) and the edges indicate the data-flow.  The edge values are integers imparting an
     *  ordering to the data-flow.
     *
     *  @{ */
    Graph buildGraph(const std::vector<SgAsmInstruction*>&);
    /** @} */

    /** Functor to return instructions for a cfg vertex.
     *
     *  This is the default unpacker that understands how to unpack SgAsmInstruction and SgAsmBlock. If a CFG has other types
     *  of vertices then the user will need to pass a different unpacker.  The unpacker function operates on one CFG vertex
     *  value which it takes as an argument. It should return a vector of instructions in the order they would be executed. */
    class DefaultVertexUnpacker {
    public:
        typedef std::vector<SgAsmInstruction*> Instructions;
        Instructions operator()(SgAsmInstruction *insn) { return Instructions(1, insn); }
        Instructions operator()(SgAsmBlock *blk);
    };

public:
    /** Compute data-flow per CFG vertex.
     *
     *  This method traverses the control flow graph to discover the abstract locations that are referenced by each vertex and
     *  the flow edges between those abstract locations.
     *
     *  The algorithm currently implemented here is to use an aggregate semantic domain consisting of a user-supplied semantic
     *  domain in conjunction with a data-flow discovery domain, and to process each CFG vertex one time in some arbitrary
     *  depth-first order.  This isn't a rigorous analysis, but it is usually able to accurately identify local and global
     *  variables, although typically not through pointers or array indexing.
     *
     * @{ */
    template<class CFG, class VertexUnpacker>
    VertexFlowGraphs buildGraphPerVertex(const CFG &cfg, size_t startVertex, VertexUnpacker vertexUnpacker) {
        using namespace Diagnostics;
        ASSERT_this();
        ASSERT_require(startVertex < cfg.nVertices());
        Stream mesg(mlog[WHERE] <<"buildGraphPerVertex startVertex=" <<startVertex);

        VertexFlowGraphs result;
        result.insert(startVertex, buildGraph(vertexUnpacker(cfg.findVertex(startVertex)->value())));
        std::vector<InstructionSemantics2::BaseSemantics::StatePtr> postState(cfg.nVertices()); // user-defined states
        postState[startVertex] = userOps_->currentState();

        typedef Sawyer::Container::Algorithm::DepthFirstForwardEdgeTraversal<const CFG> Traversal;
        for (Traversal t(cfg, cfg.findVertex(startVertex)); t; ++t) {
            typename CFG::ConstVertexIterator source = t->source();
            typename CFG::ConstVertexIterator target = t->target();
            InstructionSemantics2::BaseSemantics::StatePtr state = postState[target->id()];
            if (state==NULL) {
                ASSERT_not_null(postState[source->id()]);
                state = postState[target->id()] = postState[source->id()]->clone();
                userOps_->currentState(state);
                std::vector<SgAsmInstruction*> insns = vertexUnpacker(target->value());
                result.insert(target->id(), buildGraph(insns));
            }
        }
        
        mesg <<"; processed " <<StringUtility::plural(result.size(), "vertices", "vertex") <<"\n";
        return result;
    }

    template<class CFG>
    VertexFlowGraphs buildGraphPerVertex(const CFG &cfg, size_t startVertex) {
        return buildGraphPerVertex(cfg, startVertex, DefaultVertexUnpacker());
    }
    /** @} */

    /** Get list of unique variables.
     *
     *  This returns a list of unique variables by computing the union of all variables across the index.  Uniqueness is
     *  determined by calling AbstractLocation::mustAlias. The variables are returned in no particular order. */
    VariableList getUniqueVariables(const VertexFlowGraphs&);

    /** Basic merge operation.
     *
     *  This merge operator invokes the merge operation in the state. It is here mostly for backward compatibility. */
    template<class StatePtr>
    class BasicMerge {
    public:
        bool operator()(const StatePtr &dst, const StatePtr &src) const {
            return dst->merge(src);
        }
    };

    /** Basic merge operation for instruction semantics.
     *
     *  This merge operator is for data-flow that uses an instruction semantics state. */
    class SemanticsMerge {
        InstructionSemantics2::BaseSemantics::RiscOperatorsPtr ops_;
    public:
        explicit SemanticsMerge(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops): ops_(ops) {}
        explicit SemanticsMerge(const InstructionSemantics2::BaseSemantics::DispatcherPtr &cpu): ops_(cpu->get_operators()) {}

        bool operator()(const InstructionSemantics2::BaseSemantics::StatePtr &dst,
                        const InstructionSemantics2::BaseSemantics::StatePtr &src) const {
            struct T {
                InstructionSemantics2::BaseSemantics::RiscOperatorsPtr ops;
                InstructionSemantics2::BaseSemantics::StatePtr state;
                T(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &ops)
                    : ops(ops), state(ops->currentState()) {}
                ~T() { ops->currentState(state); }
            } t(ops_);
            ops_->currentState(src);
            return dst->merge(src, ops_.get());
        }
    };
    
    /** Data-flow engine.
     *
     *  The data-flow engine traverses the supplied control flow graph, runs the transfer function at each vertex, and merges
     *  data state objects as necessary.
     *
     *  The template arguments are:
     *
     *  @li @p CFG is the type for the control flow graph.  It must implement the Sawyer::Container::Graph API and have
     *      vertex ID numbers of type @c size_t.  The data-flow will follow the edges of this graph, invoking a transfer
     *      function (see below) at each vertex.  The vertices are usually basic blocks or instructions, although they can be
     *      anything that the transfer function can understand.  For instance, its possible for a called function to be
     *      represented by a special vertex that encapsulates the entire effect of the function, either encoded in the vertex
     *      itself or as a special case in the transfer function.  Although a normal CFG can be used, one often passes either a
     *      subgraph of the CFG or an entirely different kind of control flow graph. Subgraphs are useful for solving data-flow
     *      for a single function, and different types are useful for passing additional/different information to the transfer
     *      function.
     *
     *  @li @p StatePtr is a pointer to a data structure that stores an analysis state. A state object is attached to each
     *      vertex of the CFG to represent the data-flow state at that vertex.  The state is logically a map of abstract
     *      locations (e.g., variables) and their current values.  For example, an instruction semantics @c State object can
     *      serve as data-flow states.  Since the engine doesn't implement any particular ownership paradigm, the state pointer
     *      type is usually some kind of shared-ownership pointer.  The engine requires that the pointer have a copy
     *      constructor and assignment operator and that it can be compared to a null pointer.  See the description of the
     *      MergeFunction below for more info about the values stored in each abstract location.
     *
     *  @li @p TransferFunction is a functor that is invoked at each CFG vertex to create a new data state from a previous
     *      data state.  The functor is called with three arguments: a const reference to the control flow graph, the CFG
     *      vertex ID for the vertex being processed, and the incoming state for that vertex.  The call should return a pointer
     *      to a new state.  If the transfer function is called with only one argument, the state, then it should just create a
     *      new state which is a copy of the argument.  For instance, if the CFG vertices contain SgAsmInstruction nodes and
     *      the @p StatePtr is an instruction semantics state, then the transfer function would most likely perform these
     *      operations: create a new state by cloning the incoming state, attach the new state to an instruction semantics
     *      dispatcher (virtual CPU), call the dispatcher's @c processInstruction, return the new state.
     *
     *  @li @p MergeFunction is a functor that takes two state pointers and merges the second state into the first state. It
     *      returns true if the first state changed, false if there was no change. In order for a data-flow to reach a fixed
     *      point the values must form a lattice and a merge operation should return a value which is the greatest lower
     *      bound. This implies that the lattice has a bottom element that is a descendent of all other vertices.  However, the
     *      data-flow engine is designed to also operate in cases where a fixed point cannot be reached.
     *
     *  A common configuration for an engine is to use a control-flow graph whose vertices are basic blocks, whose @p StatePtr
     *  is an @ref InstructionSemantics2::BaseSemantics::State "instruction semantics state", whose @p TransferFunction calls
     *  @ref InstructionSemantics2::BaseSemantics::Dispatcher::processInstruction "Dispatcher::processInstruction", and whose
     *  @p MergeFunction calls the state's @ref InstructionSemantics2::BaseSemantics::State::merge "merge" method.
     *
     *  The control flow graph and transfer function are specified in the engine's constructor.  The starting CFG vertex and
     *  its initial state are supplied when the engine starts to run. */
    template<class CFG, class StatePtr, class TransferFunction, class MergeFunction = BasicMerge<StatePtr> >
    class Engine {
    public:
        typedef std::vector<StatePtr> VertexStates;     /**< Data-flow states indexed by vertex ID. */

    private:
        const CFG &cfg_;
        TransferFunction &xfer_;
        MergeFunction merge_;
        VertexStates incomingState_;                    // incoming data-flow state per CFG vertex ID
        VertexStates outgoingState_;                    // outgoing data-flow state per CFG vertex ID
        typedef Sawyer::Container::DistinctList<size_t> WorkList;
        WorkList workList_;                             // CFG vertex IDs to be visited, last in first out w/out duplicates
        size_t maxIterations_;                          // max number of iterations to allow
        size_t nIterations_;                            // number of iterations since last reset

    public:
        /** Constructor.
         *
         *  Constructs a new data-flow engine that will operate over the specified control flow graph using the specified
         *  transfer function.  The control flow graph is incorporated into the engine by reference; the transfer functor is
         *  copied. */
        Engine(const CFG &cfg, TransferFunction &xfer, MergeFunction merge = MergeFunction())
            : cfg_(cfg), xfer_(xfer), merge_(merge), maxIterations_(-1), nIterations_(0) {}

        /** Data-flow control flow graph.
         *
         *  Returns a reference to the control flow graph that's being used for the data-flow analysis. The return value is the
         *  same control flow graph as which was supplied to the constructor. */
        const CFG &cfg() const {
            return cfg_;
        }
        
        /** Reset engine to initial state.
         *
         *  This happens automatically by methods such as @ref runToFixedPoint. */
        void reset(size_t startVertexId, const StatePtr &initialState) {
            ASSERT_this();
            ASSERT_require(startVertexId < cfg_.nVertices());
            ASSERT_not_null(initialState);
            incomingState_.clear();
            incomingState_.resize(cfg_.nVertices());
            incomingState_[startVertexId] = initialState;
            outgoingState_.clear();
            outgoingState_.resize(cfg_.nVertices());
            workList_.clear();
            workList_.pushBack(startVertexId);
            nIterations_ = 0;
        }

        /** Max number of iterations to allow.
         *
         *  Allow N number of calls to runOneIteration.  When the limit is exceeded a @ref NotConverging exception is
         *  thrown.
         *
         * @{ */
        size_t maxIterations() const { return maxIterations_; }
        void maxIterations(size_t n) { maxIterations_ = n; }
        /** @} */

        /** Number of iterations run.
         *
         *  The number of times runOneIteration was called since the last reset. */
        size_t nIterations() const { return nIterations_; }
        
        /** Runs one iteration.
         *
         *  Runs one iteration of data-flow analysis by consuming the first item on the work list.  Returns false if the
         *  work list is empty (before of after the iteration). */
        bool runOneIteration() {
            using namespace Diagnostics;
            if (!workList_.isEmpty()) {
                if (++nIterations_ > maxIterations_) {
                    throw NotConverging("data-flow max iterations reached"
                                        " (max=" + StringUtility::numberToString(maxIterations_) + ")");
                }
                size_t cfgVertexId = workList_.popFront();
                if (mlog[DEBUG]) {
                    mlog[DEBUG] <<"runOneIteration: vertex #" <<cfgVertexId <<"\n";
                    mlog[DEBUG] <<"  remaining worklist is {";
                    BOOST_FOREACH (size_t id, workList_.items())
                        mlog[DEBUG] <<" " <<id;
                    mlog[DEBUG] <<" }\n";
                }
                
                ASSERT_require2(cfgVertexId < cfg_.nVertices(),
                                "vertex " + boost::lexical_cast<std::string>(cfgVertexId) + " must be valid within CFG");
                typename CFG::ConstVertexIterator vertex = cfg_.findVertex(cfgVertexId);
                StatePtr state = incomingState_[cfgVertexId];
                ASSERT_not_null2(state,
                                 "initial state must exist for CFG vertex " + boost::lexical_cast<std::string>(cfgVertexId));
                if (mlog[DEBUG]) {
                    std::ostringstream ss;
                    ss <<*state;
                    mlog[DEBUG] <<"  incoming state for vertex #" <<cfgVertexId <<"\n";
                    mlog[DEBUG] <<StringUtility::prefixLines(ss.str(), "    ");
                }

                state = outgoingState_[cfgVertexId] = xfer_(cfg_, cfgVertexId, state);
                ASSERT_not_null2(state, "outgoing state not created for vertex "+boost::lexical_cast<std::string>(cfgVertexId));
                if (mlog[DEBUG]) {
                    std::ostringstream ss;
                    ss <<*state;
                    mlog[DEBUG] <<"  outgoing state for vertex #" <<cfgVertexId <<"\n";
                    mlog[DEBUG] <<StringUtility::prefixLines(ss.str(), "    ");
                }
                
                // Outgoing state must be merged into the incoming states for the CFG successors.  Any such incoming state that
                // is modified as a result will have its CFG vertex added to the work list.
                SAWYER_MESG(mlog[DEBUG]) <<"  forwarding vertex #" <<cfgVertexId <<" output state to "
                                         <<StringUtility::plural(vertex->nOutEdges(), "vertices", "vertex") <<"\n";
                BOOST_FOREACH (const typename CFG::Edge &edge, vertex->outEdges()) {
                    size_t nextVertexId = edge.target()->id();
                    StatePtr targetState = incomingState_[nextVertexId];
                    if (targetState==NULL) {
                        SAWYER_MESG(mlog[DEBUG]) <<"    forwarded to vertex #" <<nextVertexId <<"\n";
                        incomingState_[nextVertexId] = xfer_(state); // copy the state
                        workList_.pushBack(nextVertexId);
                    } else if (merge_(targetState, state)) { // merge state into targetState, return true if changed
                        SAWYER_MESG(mlog[DEBUG]) <<"    merged with vertex #" <<nextVertexId <<" (which changed as a result)\n";
                        workList_.pushBack(nextVertexId);
                    } else {
                        SAWYER_MESG(mlog[DEBUG]) <<"     merged with vertex #" <<nextVertexId <<" (no change)\n";
                    }
                }
            }
            return !workList_.isEmpty();
        }
        
        /** Run data-flow until it reaches a fixed point.
         *
         *  Run data-flow starting at the specified control flow vertex with the specified initial state until the state
         *  converges to a fixed point or the maximum number of iterations is reached (in which case a @ref NotConverging
         *  exception is thrown). */
        void runToFixedPoint(size_t startVertexId, const StatePtr &initialState) {
            reset(startVertexId, initialState);
            while (runOneIteration()) /*void*/;
        }

        /** Return the incoming state for the specified CFG vertex.
         *
         *  This is a pointer to the incoming state for the vertex as of the latest data-flow iteration.  If the data-flow has
         *  not reached this vertex then it is likely to be a null pointer. */
        StatePtr getInitialState(size_t cfgVertexId) const {
            return incomingState_[cfgVertexId];
        }

        /** Return the outgoing state for the specified CFG vertex.
         *
         *  This is a pointer to the outgoing state for the vertex as of the latest data-flow iteration. If the data-flow has
         *  not processed this vertex then it is likely to be a null pointer. */
        StatePtr getFinalState(size_t cfgVertexId) const {
            return outgoingState_[cfgVertexId];
        }

        /** All incoming states.
         *
         *  Returns a vector indexed by vertex ID for the incoming state of each vertex as of the latest data-flow
         *  iteration. States for vertices that have not yet been reached are null pointers. */
        const VertexStates& getInitialStates() const {
            return incomingState_;
        }

        /** All outgoing states.
         *
         *  Returns a vector indexed by vertex ID for the outgoing state of each vertex as of the latest data-flow
         *  iteration. States for vertices that have not yet been processed are null pointers. */
        const VertexStates& getFinalStates() const {
            return outgoingState_;
        }
    };
};

} // namespace
} // namespace

#endif
