#ifndef ROSE_Partitioner2_DataFlow_H
#define ROSE_Partitioner2_DataFlow_H

#include <BinaryDataFlow.h>
#include <BinaryStackVariable.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Function.h>
#include <Sawyer/Graph.h>

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
    BasicBlock::Ptr bblock_;                            // attached to BBLOCK vertices
    Function::Ptr callee_;                              // function represented by FAKED_CALL
    Function::Ptr parentFunction_;                      // function "owning" this vertex
    size_t inliningId_;                                 // invocation ID for inlining functions during inter-procedural

public:
    /** Construct a basic block vertex.  The basic block pointer should not be a null pointer. */
    explicit DfCfgVertex(const BasicBlock::Ptr &bblock, const Function::Ptr &parentFunction, size_t inliningId)
        : type_(BBLOCK), bblock_(bblock), parentFunction_(parentFunction), inliningId_(inliningId) {
        ASSERT_not_null(bblock);
    }

    /** Construct a faked call vertex. The function may be null if indeterminate. */
    explicit DfCfgVertex(const Function::Ptr &function, const Function::Ptr &parentFunction, size_t inliningId)
        : type_(FAKED_CALL), callee_(function), parentFunction_(parentFunction), inliningId_(inliningId) {}

    /** Construct a vertex of specified type that takes no auxiliary data. */
    explicit DfCfgVertex(Type type, const Function::Ptr &parentFunction, size_t inliningId)
        : type_(type), parentFunction_(parentFunction), inliningId_(inliningId) {
        ASSERT_require2(BBLOCK!=type && FAKED_CALL!=type, "use a different constructor");
    }

    /** Type of the vertex.
     *
     *  Vertex types are immutable, defined when the vertex is created. Every vertex has a type. */
    Type type() const { return type_; }

    /** Basic block.
     *
     *  The basic block for a vertex is immutable, defined when the vertex is created.  Only basic block vertices have a basic
     *  block; other vertex types will return a null pointer. */
    const BasicBlock::Ptr& bblock() const { return bblock_; }

    /** Function represented by faked call. */
    const Function::Ptr& callee() const { return callee_; }

    /** Function owning this vertex.
     *
     *  Pointer to the function to which this vertex belongs. For basic blocks, it's a function to which the basic block
     *  belongs; for faked calls, its the caller function; for function returns, it's the function returning; for the
     *  indeterminate vertex it's a null pointer. */
    Function::Ptr parentFunction() const { return parentFunction_; }

    /** Inlining invocation number.
     *
     *  For a graph constructed for interprocedural data-flow, some function calls are replaced by the called function's
     *  body. Each time this inlining happens, a counter is incremented and all new vertices are created using this
     *  counter. The counter starts at zero. */
    size_t inliningId() const { return inliningId_; }
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
typedef Sawyer::Container::Graph<DfCfgVertex> DfCfg;

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
    bool operator()(const ControlFlowGraph&, const ControlFlowGraph::ConstEdgeIterator&, size_t depth) ROSE_OVERRIDE {
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
DfCfg buildDfCfg(const Partitioner&, const ControlFlowGraph&, const ControlFlowGraph::ConstVertexIterator &startVertex,
                 InterproceduralPredicate &predicate = NOT_INTERPROCEDURAL);

/** Emit a data-flow CFG as a GraphViz file. */
void dumpDfCfg(std::ostream&, const DfCfg&);

/** Choose best function for data-flow summary vertex.
 *
 *  When replacing a function call edge with a function summary, we insert a data-flow vertex that points to a function. During
 *  the data-flow processing, the function's information summarizes the data-flow state changes that are necessary. If multiple
 *  functions own the target block of a function call edge then we need to choose the "best" function to use. */
Function::Ptr bestSummaryFunction(const FunctionSet &functions);

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
    BaseSemantics::DispatcherPtr cpu_;
    BaseSemantics::SValuePtr callRetAdjustment_;
    const RegisterDescriptor STACK_POINTER_REG;
    const RegisterDescriptor INSN_POINTER_REG;
    CallingConvention::DefinitionPtr defaultCallingConvention_;
public:
    /** Construct from a CPU.
     *
     *  Constructs a new transfer function using the specified @p cpu. */
    explicit TransferFunction(const BaseSemantics::DispatcherPtr &cpu)
        : cpu_(cpu), STACK_POINTER_REG(cpu->stackPointerRegister()), INSN_POINTER_REG(cpu->instructionPointerRegister()) {
        size_t adjustment = STACK_POINTER_REG.nBits() / 8; // sizeof return address on top of stack
        callRetAdjustment_ = cpu->number_(STACK_POINTER_REG.nBits(), adjustment);
    }

    /** Construct an initial state. */
    BaseSemantics::StatePtr initialState() const;

    /** Property: Virtual CPU.
     *
     *  This is the same pointer specified in the constructor. */
    BaseSemantics::DispatcherPtr cpu() const { return cpu_; }

    /** Property: Default calling convention.
     *
     *  The default calling convention is used whenever a call is made to a function that has no calling convention
     *  information. It specifies which registers should be clobbered by the call and how the stack and instruction poniter
     *  are adjusted when returning from the call.  The default calling convention may be a null pointer to indicate that
     *  absolutely nothing is known about the convention of non-analyzed functions.
     *
     * @{ */
    CallingConvention::DefinitionPtr defaultCallingConvention() const { return defaultCallingConvention_; }
    void defaultCallingConvention(const CallingConvention::DefinitionPtr &x) { defaultCallingConvention_ = x; }
    /** @} */

    // Required by data-flow engine
    std::string printState(const BaseSemantics::StatePtr &state);

    // Required by data-flow engine: compute new output state given a vertex and input state.
    BaseSemantics::StatePtr operator()(const DfCfg&, size_t vertexId, const BaseSemantics::StatePtr &incomingState) const;
};

/** Data-flow merge function.
 *
 *  Computes the meet of two states, merging the source state into the destination state and returning true iff the destination
 *  state changed. */
typedef Rose::BinaryAnalysis::DataFlow::SemanticsMerge MergeFunction;

/** Data-Flow engine. */
typedef Rose::BinaryAnalysis::DataFlow::Engine<DfCfg, BaseSemantics::StatePtr, TransferFunction, MergeFunction> Engine;

/** Returns the list of all known stack variables.
 *
 *  A stack variable is any memory location whose address is a constant offset from an initial stack pointer.  That is, the
 *  address has the form (add SP0 CONSTANT) where SP0 is a variable supplied as an argument to this function.  When CONSTANT is
 *  zero the expression is simplified to SP0, so that also is accepted. Although memory is byte addressable and values are
 *  stored as individual bytes in memory, this function attempts to sew related addresses back together again to produce
 *  variables that are multiple bytes.  There are many ways to do this, all of which are heuristic.
 *
 *  The @p ops provides the operators for comparing stack pointers, and also provides the state which is examined to find the
 *  stack variables.  The underlying memory state should be of type @ref InstructionSemantics2::BaseSemantics::MemoryCellList
 *  "MemoryCellList" or a subclass, or else no stack variables will be found. */
StackVariables findStackVariables(const BaseSemantics::RiscOperatorsPtr &ops,
                                  const BaseSemantics::SValuePtr &initialStackPointer);

/** Returns the list of all known local variables.
 *
 *  A local variable is any stack variable whose starting address is less than the specified stack pointer.  For the definition
 *  of stack variable, see @ref findStackVariables. */
StackVariables findLocalVariables(const BaseSemantics::RiscOperatorsPtr &ops,
                                  const BaseSemantics::SValuePtr &initialStackPointer);

/** Returns the list of all known function arguments.
 *
 *  A function argument is any stack variable whose starting address is greater than or equal to the specified stack pointer.
 *  For the definition of stack variable, see @ref findStackVariables.  On architectures that pass a return address on the top
 *  of the stack, that return address is considered to be the first argument of the function. */
StackVariables findFunctionArguments(const BaseSemantics::RiscOperatorsPtr &ops,
                                     const BaseSemantics::SValuePtr &initialStackPointer);

/** Returns a list of global variables.
 *
 *  The returned abstract locations all point to memory. The @p wordNBytes is the maximum size for any returned variable;
 *  larger units of memory written to by the same instruction will be broken into smaller variables. */
std::vector<AbstractLocation> findGlobalVariables(const BaseSemantics::RiscOperatorsPtr &ops, size_t wordNBytes);



} // namespace
} // namespace
} // namespace
} // namespace

#endif
