#ifndef ROSE_Partitioner2_DataFlow_H
#define ROSE_Partitioner2_DataFlow_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Function.h>
#include <sawyer/Graph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Dataflow utilities. */
namespace DataFlow {

/** CFG vertex for dataflow analysis.
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

public:
    /** Construct a basic block vertex.  The basic block pointer should not be a null pointer. */
    explicit DfCfgVertex(const BasicBlock::Ptr &bblock): type_(BBLOCK), bblock_(bblock) {
        ASSERT_not_null(bblock);
    }

    /** Construct a faked call vertex. The function may be null if indeterminate. */
    explicit DfCfgVertex(const Function::Ptr &function): type_(FAKED_CALL), callee_(function) {}
    
    /** Construct a vertex of specified type that takes no auxiliary data. */
    explicit DfCfgVertex(Type type): type_(type) {
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
};

/** Control flow graph used by dataflow analysis.
 *
 *  The CFG used for dataflow is slightly different than the global CFG maintained by the partitioner. The partitioner's global
 *  CFG is tuned for discovering basic blocks and deciding which basic blocks are owned by which functions, whereas a
 *  dataflow's CFG is tuned for performing data flow analysis.  A dataflow CFG is usually constructed from the partitioner's
 *  global CFG, but differs in the following ways:
 *
 *  @li First, dataflow analysis is usually performed on a subset of the partitioner's global CFG. This function uses the @p
 *      startVertex to select some connected subgraph, such as a subgraph corresponding to a single function definition when
 *      given the entry block.
 *
 *  @li Function return blocks (e.g., RET instructions) are handled differently during dataflow.  In the partitioner's global
 *      CFG each return block is marked as a function return and has single successor--the indeterminate vertex.  In a dataflow
 *      CFG the return blocks are not handled specially, but rather all flow into a single special return vertex that has no
 *      instructions.  This allows data to be merged from all the return points.
 *
 *  @li Function call sites are modified.  In the partitioner global CFG a function call (e.g., CALL instruction) has an edge
 *      (or edges) going to the entry block of the called function(s) and a special call-return edge to the return site if
 *      there is one (usually the fall-through address). A data-flow analysis often needs to perform some special action for
 *      the call-return, thus a call-return edge in the global CFG gets transformed to an edge-vertex-edge sequence in the
 *      dataflow CFG where the middle vertex is a special CALLRET vertex with no instructions. */
typedef Sawyer::Container::Graph<DfCfgVertex> DfCfg;

/** Predicate that decides when to use inter-procedural dataflow.
 *
 *  The predicate is invoked with the global CFG and a function call edge and should return true if the called function should
 *  be included into the dataflow graph.  If it returns false then the graph will have a single FAKED_CALL vertex to represent
 *  the called function. */
class InterproceduralPredicate {
public:
    virtual ~InterproceduralPredicate() {}
    virtual bool operator()(const ControlFlowGraph&, const ControlFlowGraph::ConstEdgeNodeIterator&) = 0;
};

/** build a cfg useful for dataflow analysis.
 *
 *  The returned CFG will be constructed from the global CFG vertices that are reachable from @p startVertex such that the
 *  reached vertex belongs to the same function as @p startVertex.
 *
 *  @sa DfCfg */
DfCfg buildDfCfg(const Partitioner&, const ControlFlowGraph&, const ControlFlowGraph::ConstVertexNodeIterator &startVertex,
                 InterproceduralPredicate&);

/** Emit a dataflow CFG as a GraphViz file. */
void dumpDfCfg(std::ostream&, const DfCfg&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
