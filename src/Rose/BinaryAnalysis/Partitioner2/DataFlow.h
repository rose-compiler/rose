#ifndef ROSE_BinaryAnalysis_Partitioner2_DataFlow_H
#define ROSE_BinaryAnalysis_Partitioner2_DataFlow_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

#include <Rose/BinaryAnalysis/DataFlow.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Sawyer/Graph.h>

#include <ostream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Data-flow utilities. */
namespace DataFlow {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Control Flow Graph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** CFG vertex for data-flow analysis.
 *
 *  @sa DfCfg */
class DfCfgVertex {
public:
    /** Vertex type. */
    enum Type {
        BBLOCK,                                         /**< Vertex represents a basic block. */
        FAKED_CALL,                                     /**< Represents a whole called function. */
        FUNCRET,                                        /**< Vertex represents returning to the caller. */
        INDET,                                          /**< Indeterminate basic block where no information is available. */
    };

private:
    Type type_;
    BasicBlockPtr bblock_;                              // attached to BBLOCK vertices
    FunctionPtr callee_;                                // function represented by FAKED_CALL
    FunctionPtr parentFunction_;                        // function "owning" this vertex
    size_t inliningId_;                                 // invocation ID for inlining functions during inter-procedural

public:
    ~DfCfgVertex();

    /** Construct a basic block vertex.  The basic block pointer should not be a null pointer. */
    DfCfgVertex(const BasicBlockPtr&, const FunctionPtr &parentFunction, size_t inliningId);

    /** Construct a faked call vertex. The function may be null if indeterminate. */
    DfCfgVertex(const FunctionPtr &function, const FunctionPtr &parentFunction, size_t inliningId);

    /** Construct a vertex of specified type that takes no auxiliary data. */
    DfCfgVertex(Type type, const FunctionPtr &parentFunction, size_t inliningId);

    /** Type of the vertex.
     *
     *  Vertex types are immutable, defined when the vertex is created. Every vertex has a type. */
    Type type() const;

    /** Basic block.
     *
     *  The basic block for a vertex is immutable, defined when the vertex is created.  Only basic block vertices have a basic
     *  block; other vertex types will return a null pointer. */
    const BasicBlockPtr& bblock() const;

    /** Function represented by faked call. */
    const FunctionPtr& callee() const;

    /** Function owning this vertex.
     *
     *  Pointer to the function to which this vertex belongs. For basic blocks, it's a function to which the basic block
     *  belongs; for faked calls, its the caller function; for function returns, it's the function returning; for the
     *  indeterminate vertex it's a null pointer. */
    FunctionPtr parentFunction() const;

    /** Inlining invocation number.
     *
     *  For a graph constructed for interprocedural data-flow, some function calls are replaced by the called function's
     *  body. Each time this inlining happens, a counter is incremented and all new vertices are created using this
     *  counter. The counter starts at zero. */
    size_t inliningId() const;

    /** Virtual address of vertex.
     *
     *  Returns the virtual address if known, otherwise nothing. */
    Sawyer::Optional<Address> address() const;

    /** Print a short description of this vertex.
     *
     *  The description is a single-line string with no line termination. */
    void print(std::ostream&) const;

    /** Single-line description of this vertex.
     *
     *  The description is a single-line string with no line termination. */
    std::string toString() const;
};

/** Control flow graph used by data-flow analysis.
 *
 *  The CFG used for data-flow is slightly different than the global CFG maintained by the partitioner. The partitioner's global
 *  CFG is tuned for discovering basic blocks and deciding which basic blocks are owned by which functions, whereas a
 *  data-flow's CFG is tuned for performing data flow analysis.  A data-flow CFG is usually constructed from the partitioner's
 *  global CFG, but differs in the following ways:
 *
 *  @li First, data-flow analysis is usually performed on a subset of the partitioner's global CFG. This function uses the @p
 *      startVertex to select some connected subgraph, such as a subgraph corresponding to a single function definition when
 *      given the entry block.
 *
 *  @li Function return blocks (e.g., RET instructions) are handled differently during data-flow.  In the partitioner's global
 *      CFG each return block is marked as a function return and has single successor--the indeterminate vertex.  In a data-flow
 *      CFG the return blocks are not handled specially, but rather all flow into a single special return vertex that has no
 *      instructions.  This allows data to be merged from all the return points.
 *
 *  @li Function call sites are modified.  In the partitioner global CFG a function call (e.g., CALL instruction) has an edge
 *      (or edges) going to the entry block of the called function(s) and a special call-return edge to the return site if
 *      there is one (usually the fall-through address). A data-flow analysis often needs to perform some special action for
 *      the call-return, thus a call-return edge in the global CFG gets transformed to an edge-vertex-edge sequence in the
 *      data-flow CFG where the middle vertex is a special CALLRET vertex with no instructions. */
using DfCfg = Sawyer::Container::Graph<DfCfgVertex>;

/** Predicate that decides when to use inter-procedural data-flow.
 *
 *  The predicate is invoked with the global CFG and a function call edge and should return true if the called function should
 *  be included into the data-flow graph.  If it returns false then the graph will have a single FAKED_CALL vertex to represent
 *  the called function. */
class InterproceduralPredicate {
public:
    virtual ~InterproceduralPredicate() {}
    virtual bool operator()(const ControlFlowGraph&, const ControlFlowGraph::ConstEdgeIterator&, size_t depth) = 0;
};

/** Predicate that always returns false, preventing interprocedural analysis. */
class NotInterprocedural: public InterproceduralPredicate {
public:
    bool operator()(const ControlFlowGraph&, const ControlFlowGraph::ConstEdgeIterator&, size_t /*depth*/) override {
        return false;
    }
};
extern NotInterprocedural NOT_INTERPROCEDURAL;

/** Unpacks a vertex into a list of instructions. */
std::vector<SgAsmInstruction*> vertexUnpacker(const DfCfgVertex&);

/** build a cfg useful for data-flow analysis.
 *
 *  The returned CFG will be constructed from the global CFG vertices that are reachable from @p startVertex such that the
 *  reached vertex belongs to the same function as @p startVertex.
 *
 *  @sa DfCfg */
DfCfg buildDfCfg(const PartitionerConstPtr&, const ControlFlowGraph&, const ControlFlowGraph::ConstVertexIterator &startVertex,
                 InterproceduralPredicate &predicate = NOT_INTERPROCEDURAL);

/** Emit a data-flow CFG as a GraphViz file. */
void dumpDfCfg(std::ostream&, const DfCfg&);

/** Choose best function for data-flow summary vertex.
 *
 *  When replacing a function call edge with a function summary, we insert a data-flow vertex that points to a function. During
 *  the data-flow processing, the function's information summarizes the data-flow state changes that are necessary. If multiple
 *  functions own the target block of a function call edge then we need to choose the "best" function to use. */
FunctionPtr bestSummaryFunction(const FunctionSet &functions);

/** Find the return vertex.
 *
 *  Finds the FUNCRET vertex. Function returns all flow into this special vertex, but if there are no function return blocks
 *  then this vertex also doesn't exist (in which case the end iterator is returned).  Do not call this if there's a chance
 *  that the data-flow CFG has more than one FUNCRET vertex. */
template<class DfCfg>
typename Sawyer::Container::GraphTraits<DfCfg>::VertexIterator
findReturnVertex(DfCfg &dfCfg) {
    using namespace Sawyer::Container;
    typename GraphTraits<DfCfg>::VertexIterator retval = dfCfg.vertices().end();
    for (typename GraphTraits<DfCfg>::VertexIterator vi = dfCfg.vertices().begin(); vi != dfCfg.vertices().end(); ++vi) {
        if (vi->value().type() == DfCfgVertex::FUNCRET) {
            ASSERT_require(retval == dfCfg.vertices().end());
            retval = vi;
        }
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Transfer function
//
// The transfer function is reponsible for taking a CFG vertex and an initial state and producing the next state, the final
// state for that vertex.  Users can use whatever transfer function they want; this one is based on the DfCfg and an
// instruction semantics state.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Data-Flow transfer functor. */
class TransferFunction {
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu_;
    InstructionSemantics::BaseSemantics::SValuePtr callRetAdjustment_;
    const RegisterDescriptor STACK_POINTER_REG;
    const RegisterDescriptor INSN_POINTER_REG;
    CallingConvention::DefinitionPtr defaultCallingConvention_;
    bool ignoringSemanticFailures_;

public:
    ~TransferFunction();

    /** Construct from a CPU.
     *
     *  Constructs a new transfer function using the specified @p cpu. */
    explicit TransferFunction(const InstructionSemantics::BaseSemantics::DispatcherPtr&);

    /** Construct an initial state. */
    InstructionSemantics::BaseSemantics::StatePtr initialState() const;

    /** Property: Virtual CPU.
     *
     *  This is the same pointer specified in the constructor. */
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu() const;

    /** Property: Default calling convention.
     *
     *  The default calling convention is used whenever a call is made to a function that has no calling convention
     *  information. It specifies which registers should be clobbered by the call and how the stack and instruction poniter
     *  are adjusted when returning from the call.  The default calling convention may be a null pointer to indicate that
     *  absolutely nothing is known about the convention of non-analyzed functions.
     *
     * @{ */
    CallingConvention::DefinitionPtr defaultCallingConvention() const;
    void defaultCallingConvention(const CallingConvention::DefinitionPtr&);
    /** @} */

    /** Property: Whether to ignore instructions with unknown semantics.
     *
     *  If true, then an instruction with unknown semantics will cause the state to be updated in some way that is not
     *  well defined, depending on exactly when the semantic error occurs during the semantic execution of the instruction. If false,
     *  then an exception is thrown.
     *
     * @{ */
    bool ignoringSemanticFailures() const;
    void ignoringSemanticFailures(bool);
    /** @} */

    // Required by data-flow engine
    std::string toString(const InstructionSemantics::BaseSemantics::StatePtr &state);

    // Required by data-flow engine: compute new output state given a vertex and input state.
    InstructionSemantics::BaseSemantics::StatePtr
    operator()(const DfCfg&, size_t vertexId, const InstructionSemantics::BaseSemantics::StatePtr &incomingState) const;
};

/** Data-flow merge function.
 *
 *  Computes the meet of two states, merging the source state into the destination state and returning true iff the destination
 *  state changed. */
typedef Rose::BinaryAnalysis::DataFlow::SemanticsMerge MergeFunction;

/** Data-Flow engine. */
using Engine = Rose::BinaryAnalysis::DataFlow::Engine<DfCfg,
                                                      InstructionSemantics::BaseSemantics::StatePtr,
                                                      TransferFunction,
                                                      MergeFunction>;

/** Returns the list of all known stack variables.
 *
 *  A stack variable is any memory location whose address is a constant offset from an initial stack pointer.  That is, the
 *  address has the form (add SP0 CONSTANT) where SP0 is a variable supplied as an argument to this function.  When CONSTANT is
 *  zero the expression is simplified to SP0, so that also is accepted. Although memory is byte addressable and values are
 *  stored as individual bytes in memory, this function attempts to sew related addresses back together again to produce
 *  variables that are multiple bytes.  There are many ways to do this, all of which are heuristic.
 *
 *  The @p ops provides the operators for comparing stack pointers, and also provides the state which is examined to find the
 *  stack variables.  The underlying memory state should be of type @ref InstructionSemantics::BaseSemantics::MemoryCellList
 *  "MemoryCellList" or a subclass, or else no stack variables will be found. */
Variables::StackVariables findStackVariables(const FunctionPtr &function,
                                             const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops,
                                             const InstructionSemantics::BaseSemantics::SValuePtr &initialStackPointer);

/** Returns the list of all known local variables.
 *
 *  A local variable is any stack variable whose starting address is less than the specified stack pointer.  For the definition
 *  of stack variable, see @ref findStackVariables. */
Variables::StackVariables findLocalVariables(const FunctionPtr &function,
                                             const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops,
                                             const InstructionSemantics::BaseSemantics::SValuePtr &initialStackPointer);

/** Returns the list of all known function arguments.
 *
 *  A function argument is any stack variable whose starting address is greater than or equal to the specified stack pointer.
 *  For the definition of stack variable, see @ref findStackVariables.  On architectures that pass a return address on the top
 *  of the stack, that return address is considered to be the first argument of the function. */
Variables::StackVariables findFunctionArguments(const FunctionPtr &function,
                                                const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops,
                                                const InstructionSemantics::BaseSemantics::SValuePtr &initialStackPointer);

/** Returns a list of global variables.
 *
 *  The returned abstract locations all point to memory. The @p wordNBytes is the maximum size for any returned variable;
 *  larger units of memory written to by the same instruction will be broken into smaller variables. */
std::vector<AbstractLocation> findGlobalVariables(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops,
                                                  size_t wordNBytes);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
