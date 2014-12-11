#ifndef ROSE_BinaryAnalysis_DataFlow_H
#define ROSE_BinaryAnalysis_DataFlow_H

#include "DataFlowSemantics2.h"
#include "Diagnostics.h"
#include "SymbolicSemantics2.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <sawyer/GraphTraversal.h>
#include <sawyer/DistinctList.h>
#include <sstream>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {

/** Various tools for data flow analysis.
 *
 *  @section s1 Comparison with source code data flow
 *
 *  Binary data flow analysis differs from source code data flow analysis in that source code has well-known, named variables
 *  and the analysis is often simplified by considering only those variables (e.g., whole arrays, not array element
 *  granularity), and by ignoring aliasing issues.  But because binaries have no named variables but rather one large array of
 *  memory with computed addresses, a binary data flow analysis <em>must</em> be at address granularity and account for
 *  aliasing. In fact, because of the rich set of addressing modes, it is very common that within a single instruction some of
 *  the inputs and/or outputs depend on other inputs.  The binary data flow implemented by this class uses abstract locations
 *  (@ref AbstractLocation) to represent variables. An abstract location is either a register or a memory address.  Registers
 *  are represented by a @ref RegisterDescriptor while addresses are represented by a semantic value in some user-specified
 *  domain (often symbolic, see @ref rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValue).
 *
 *  Another difference is in the number of basic operations that need to be supported. Source-level languages typically have at
 *  most a few dozen basic operations for which data flow must be explicitly encoded in the analysis, while binary analysis
 *  needs basic data flow information about hundreds of CPU instructions across the supported architectures.  In fact, there
 *  are so many unique instructions that the implementation here takes a different approach:  it leverages the fact that ROSE
 *  has instruction semantics and is able to discern data flow within an instruction (or even across an entire basic block) by
 *  executing the instruction (or block) in a special data flow semantic domain.  The domain is an aggregate domain consisting
 *  of a main user-specified domain (often symbolic), a domain to track data flow, and any number of additional user-specified
 *  domains.  This approach works to discover multiple dataflow within the executed item (instruction or basic block); most CPU
 *  instructions have multiple data flows (e.g., "PUSH" has at least three independent data flows: incrementing of the
 *  instruction pointer, moving data between stack and register, incrementing the stack pointer).
 *
 *  Two additional minor things to be aware of in binary data flow are (1) the difference in size between registers and memory
 *  locations, and (2) the use of idioms to perform common tasks.  For instance, a read from memory to a 32-bit register is
 *  actually four separate 8-bit reads at the dataflow level since memory holds 8-bit values.  An example of an idiom is the
 *  "XOR EAX, EAX" instruction to set the EAX register to zero (this is important for taint analysis, since it effectively
 *  removes any taint from EAX).
 *
 *  @section s2 Prerequisites for the core data flow algorithm
 *
 *  Before the core data flow analysis can run (e.g., taint analysis) there needs to be a number of pieces of information
 *  available:
 *
 *  @li A control flow graph over the part of the specimen being analyzed (e.g., control flow over a function).  The vertices
 *      of the CFG can be either instructions or basic blocks (since a basic block is effectively a large instruction).
 *
 *  @li A lattice of values so that a value can be stored at each variable.  The values form a lattice so that the
 *      meet/join/merge function, when combining two values, always results in a value which is a common parent of the input
 *      values.
 *
 *  @li A state type that maps variables to values of the lattice.  The number of variables must be finite for the data flow
 *      analysis to reach a fixed point.
 * 
 *  @li A transfer function that uses the data flow information for the execution unit (instruction or basic block) to create a
 *      new state from an existing state.
 * 
 *  @li A merge function so that an input state can be computed for a CFG vertex when that vertex has multiple incoming control
 *      flow paths.
 * 
 *  @li A starting vertex in the CFG and its initial incoming state.
 *
 *  @section s3 Computing prerequisites
 *
 *  Obviously one needs to parse the ELF or PE container, locate and disassemble instructions, and optionally group those
 *  instructions into units on which the core data flow algorithm will operate (probably basic blocks and functions).  This is
 *  performed with the usual ROSE binary analysis mechanisms.
 *
 *  Once the scope of the analysis is determined (e.g., a particular function), a control flow graph is obtained with @ref
 *  rose::BinaryAnalysis::ControlFlow, which produces a CFG whose vertices are either instructions or basic blocks (user's
 *  choice). Any kind of graph supporting the Boost Graph Library (BGL) API is permissible if it also satisfies the
 *  requirements layed out in the ControlFlow documentation.
 *
 *  Data flow paths per CFG vertex need to be computed and made available to the transfer function.  The transfer function will
 *  use these paths when creating an output state from an input state.  The endpoints of each path are the variables, so this
 *  step also provides a list of all the variables that will appear in the data flow states.  One way to obtain this
 *  information is to use the DataFlowSemantics instruction semantics; another way is to call the @ref buildGraphPerVertex
 *  method.  The former method allows the user to choose the memory address domain; the latter uses the symbolic domain. In any
 *  case, @ref getUniqueVariables will return a list of variables.
 *
 *  @section s4 Core data flow engine
 *
 *  The core data flow engine uses the prerequisites mentioned above and traverses the CFG until the state converges to a fixed
 *  point. */
class DataFlow {
public:
    /** Data flow graph.
     *
     *  Vertices are abstract locations (register descriptors and/or memory addresses) and edges indicate the flow of data from
     *  abstract location to another.  The edge IDs are in the order they were added; each edge also has a sequence number
     *  that is initially equal to the edge ID, although edge IDs can change if edges are erased from the graph). */
    typedef InstructionSemantics2::DataFlowSemantics::DataFlowGraph Graph;

    /** Variable participating in data flow.
     *
     *  A variable in binary data flow analysis is an abstract location referencing either a register or memory cell.  The
     *  address for memory locations is an arbitrary semantic expression (@ref
     *  rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::SValue). */
    typedef AbstractLocation Variable;

    /** List of variables. */
    typedef std::list<Variable> VariableList;

    /** Map from CFG vertex to data flow graph.
     *
     *  This data structure holds a data flow graph for each control flow graph vertex.  The data flow graph represents data
     *  flow from one abstract location to another.  The union of all abstract locations across all control flow vertices is
     *  the set of variables present in the specimen being analyzed.  The map is keyed by CFG vertex IDs. */
    typedef Sawyer::Container::Map<size_t, Graph> VertexFlowGraphs;

private:
    InstructionSemantics2::BaseSemantics::RiscOperatorsPtr userOps_; // operators (and state) provided by the user
    InstructionSemantics2::DataFlowSemantics::RiscOperatorsPtr dfOps_; // data flow operators (which point to user ops)
    InstructionSemantics2::BaseSemantics::DispatcherPtr dispatcher_; // copy of user's dispatcher but with DataFlowSemantics
    static Sawyer::Message::Facility mlog;              // diagnostics for data flow
    void init(const InstructionSemantics2::BaseSemantics::DispatcherPtr&);

public:
    /** Constructor.
     *
     *  Constructs a new data flow analysis framework using the specified user-level instruction semantics.  The semantics
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
    /** Compute data flow.
     *
     *  Computes and returns a graph describing how data flow occurs for the specified instructions (which must have linear
     *  control flow, e.g., from a single basic block). The vertices of the returned graph contain the abstract locations that
     *  are referenced (read or written) and the edges indicate the data flow.  The edge values are integers imparting an
     *  ordering to the data flow.
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
    /** Compute data flow per CFG vertex.
     *
     *  This method traverses the control flow graph to discover the abstract locations that are referenced by each vertex and
     *  the flow edges between those abstract locations.
     *
     *  The algorithm currently implemented here is to use an aggregate semantic domain consisting of a user-supplied semantic
     *  domain in conjunction with a data flow discovery domain, and to process each CFG vertex one time in some arbitrary
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
        postState[startVertex] = userOps_->get_state();

        typedef Sawyer::Container::Algorithm::DepthFirstForwardEdgeTraversal<const CFG> Traversal;
        for (Traversal t(cfg, cfg.findVertex(startVertex)); t; ++t) {
            typename CFG::ConstVertexNodeIterator source = t->source();
            typename CFG::ConstVertexNodeIterator target = t->target();
            InstructionSemantics2::BaseSemantics::StatePtr state = postState[target->id()];
            if (state==NULL) {
                ASSERT_not_null(postState[source->id()]);
                state = postState[target->id()] = postState[source->id()]->clone();
                userOps_->set_state(state);
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

    /** Data flow engine.
     *
     *  The data flow engine traverses the supplied control flow graph, runs the transfer function at each vertex, and merges
     *  data state objects as necessary.
     *
     *  The template arguments are:
     *
     *  @li @p CFG is the type for the control flow graph.  It must implement the Sawyer::Container::Graph API and have
     *      vertices that point to instructions (SgAsmInstruction) or basic blocks (SgAsmBlock).
     *
     *  @li @p StatePtr is a pointer to a data state that maps variables to values.  Variables are abstract locations and
     *      values are members of a lattice as described in the documentation for the DataFlow class.  Since the engine doesn't
     *      implement any particular ownership paradigm, the state pointer type is usually some kind of smart pointer.  The
     *      engine requires that the pointer have a copy constructor and assignment operator.
     *
     *  @li @p TransferFunction is a functor that is invoked at each CFG vertex to create a new data state from a previous
     *      data state.  The functor is called with three arguments: a const reference to the control flow graph, the CFG
     *      vertex ID for the vertex being processed, and the incoming state for that vertex.  The call should return a pointer
     *      to a new state.
     *
     *  The control flow graph and transfer function are specified in the engine's constructor.  The starting CFG vertex and
     *  its initial state are supplied when the engine starts to run. */
    template<class CFG, class StatePtr, class TransferFunction>
    class Engine {
        const CFG &cfg_;
        TransferFunction &xfer_;
        typedef std::vector<StatePtr> VertexStates;
        VertexStates incomingState_;                    // incoming data flow state per CFG vertex ID
        VertexStates outgoingState_;                    // outgoing data flow state per CFG vertex ID
        typedef Sawyer::Container::DistinctList<size_t> WorkList;
        WorkList workList_;                             // CFG vertex IDs to be visited, last in first out w/out duplicates

    public:
        /** Constructor.
         *
         *  Constructs a new data flow engine that will operate over the specified control flow graph using the specified
         *  transfer function.  The control flow graph is incorporated into the engine by reference; the transfer functor is
         *  copied. */
        Engine(const CFG &cfg, TransferFunction &xfer)
            : cfg_(cfg), xfer_(xfer) {}

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
        }
        
        /** Runs one iteration.
         *
         *  Runs one iteration of data flow analysis by consuming the first item on the work list.  Returns false if the
         *  work list is empty (before of after the iteration). */
        bool runOneIteration() {
            using namespace Diagnostics;
            if (!workList_.isEmpty()) {
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
                typename CFG::ConstVertexNodeIterator vertex = cfg_.findVertex(cfgVertexId);
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
                BOOST_FOREACH (const typename CFG::EdgeNode &edge, vertex->outEdges()) {
                    size_t nextVertexId = edge.target()->id();
                    StatePtr targetState = incomingState_[nextVertexId];
                    if (targetState==NULL) {
                        SAWYER_MESG(mlog[DEBUG]) <<"    forwarded to vertex #" <<nextVertexId <<"\n";
                        incomingState_[nextVertexId] = state;
                        workList_.pushBack(nextVertexId);
                    } else if (targetState->merge(state)) {
                        SAWYER_MESG(mlog[DEBUG]) <<"    merged with vertex #" <<nextVertexId <<" (which changed as a result)\n";
                        workList_.pushBack(nextVertexId);
                    } else {
                        SAWYER_MESG(mlog[DEBUG]) <<"     merged with vertex #" <<nextVertexId <<" (no change)\n";
                    }
                }
            }
            return !workList_.isEmpty();
        }
        
        /** Run data flow until it reaches a fixed point.
         *
         *  Run data flow starting at the specified control flow vertex with the specified initial state until the state
         *  converges to a fixed point no matter how long that takes. */
        void runToFixedPoint(size_t startVertexId, const StatePtr &initialState) {
            reset(startVertexId, initialState);
            while (runOneIteration()) /*void*/;
        }

        /** Return the initial state for the specified CFG vertex. */
        StatePtr getInitialState(size_t cfgVertexId) const {
            return incomingState_[cfgVertexId];
        }

        /** Return the final state for the specified CFG vertex.  Users call this to get the results. */
        StatePtr getFinalState(size_t cfgVertexId) const {
            return outgoingState_[cfgVertexId];
        }

        const VertexStates& getFinalStates() const {
            return outgoingState_;
        }
    };
};

} // namespace
} // namespace

#endif
