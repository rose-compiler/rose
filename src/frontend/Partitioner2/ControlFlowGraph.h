#ifndef ROSE_Partitioner2_ControlFlowGraph_H
#define ROSE_Partitioner2_ControlFlowGraph_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>

#include <Sawyer/BiMap.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Map.h>

#include <list>
#include <ostream>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Control flow graph vertex. */
class CfgVertex {
    VertexType type_;                                   // type of vertex, special or not
    rose_addr_t startVa_;                               // address of start of basic block
    BasicBlock::Ptr bblock_;                            // basic block, or null if only a place holder
    Function::Ptr function_;                            // function to which vertex belongs, if any

public:
    /** Construct a basic block placeholder vertex. */
    explicit CfgVertex(rose_addr_t startVa): type_(V_BASIC_BLOCK), startVa_(startVa) {}

    /** Construct a basic block vertex. */
    explicit CfgVertex(const BasicBlock::Ptr &bb): type_(V_BASIC_BLOCK), bblock_(bb) {
        ASSERT_not_null(bb);
        startVa_ = bb->address();
    }

    /** Construct a special vertex. */
    /*implicit*/ CfgVertex(VertexType type): type_(type), startVa_(0) {
        ASSERT_forbid2(type==V_BASIC_BLOCK, "this constructor does not create basic block or placeholder vertices");
    }

    /** Returns the vertex type. */
    VertexType type() const { return type_; }

    /** Property: starting address.
     *
     *  The virtual address for a placeholder or basic block.  User-defined vertices can use this for whatever they want.
     *
     * @{ */
    rose_addr_t address() const {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        return startVa_;
    }
    void address(rose_addr_t va) {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        startVa_ = va;
    }
    /** @} */

    /** Property: basic block.
     *
     *  Pointer to a basic block.  This property is available for @ref V_BASIC_BLOCK or @ref V_USER_DEFINED vertices. A @ref
     *  V_BASIC_BLOCK vertex with a null @ref bblock property is called a "placeholder".
     *
     * @{ */
    const BasicBlock::Ptr& bblock() const {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        return bblock_;
    }
    void bblock(const BasicBlock::Ptr &bb) {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        bblock_ = bb;
    }
    /** @} */

    /** Property: owning function.
     *
     *  Pointer to a function that owns this vertex.  For instance, a basic block (@ref V_BASIC_BLOCK) can belong to a
     *  function.  This property is available for @ref V_BASIC_BLOCK and @ref V_USER_DEFINED vertices.
     *
     *  @{ */
    const Function::Ptr& function() const {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        return function_;
    }
    void function(const Function::Ptr &f) {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        function_ = f;
    }
    /** @} */

    /** Turns a basic block vertex into a placeholder.
     *
     *  The basic block pointer is reset to null. */
    void nullify() {
        ASSERT_require(V_BASIC_BLOCK==type_);
        bblock_ = BasicBlock::Ptr();
    }
};

/** Control flow graph edge. */
class CfgEdge {
private:
    EdgeType type_;
    Confidence confidence_;
public:
    CfgEdge(): type_(E_NORMAL), confidence_(ASSUMED) {}
    /*implicit*/ CfgEdge(EdgeType type, Confidence confidence=ASSUMED): type_(type), confidence_(confidence) {}
    EdgeType type() const { return type_; }
    Confidence confidence() const { return confidence_; }
    void confidence(Confidence c) { confidence_ = c; }
};

/** Control flow graph. */
typedef Sawyer::Container::Graph<CfgVertex, CfgEdge> ControlFlowGraph;

/** Mapping from basic block starting address to CFG vertex. */
typedef Sawyer::Container::Map<rose_addr_t, ControlFlowGraph::VertexIterator> CfgVertexIndex;

/** List of CFG vertex pointers.
 *
 * @{ */
typedef std::list<ControlFlowGraph::VertexIterator> CfgVertexList;
typedef std::list<ControlFlowGraph::ConstVertexIterator> CfgConstVertexList;
/** @} */

/** List of CFG edge pointers.
 *
 * @{ */
typedef std::list<ControlFlowGraph::EdgeIterator> CfgEdgeList;
typedef std::list<ControlFlowGraph::ConstEdgeIterator> CfgConstEdgeList;
/** @} */

/** Set of CFG vertex pointers.
 *
 * @{ */
typedef std::set<ControlFlowGraph::VertexIterator> CfgVertexSet;
typedef std::set<ControlFlowGraph::ConstVertexIterator> CfgConstVertexSet;
/** @} */

/** Set of CFG edge pointers.
 *
 * @{ */
typedef std::set<ControlFlowGraph::EdgeIterator> CfgEdgeSet;
typedef std::set<ControlFlowGraph::ConstEdgeIterator> CfgConstEdgeSet;
/** @} */

/** Map vertices from one CFG to another CFG and vice versa. */
typedef Sawyer::Container::BiMap<ControlFlowGraph::ConstVertexIterator, ControlFlowGraph::ConstVertexIterator> CfgVertexMap;




/** Base class for CFG-adjustment callbacks.
 *
 *  Users may create subclass objects from this class and pass their shared-ownership pointers to the partitioner, in which
 *  case the partitioner will invoke one of the callback's virtual function operators every time the control flow graph
 *  changes (the call occurs after the CFG has been adjusted).  Multiple callbacks are allowed; the list is obtained with
 *  the @ref cfgAdjustmentCallbacks method. */
class CfgAdjustmentCallback: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<CfgAdjustmentCallback> Ptr;

    /** Arguments for attaching a basic block.
     *
     *  After a basic block is attached to the CFG/AUM, or after a placeholder is inserted into the CFG, these arguments
     *  are passed to the callback.  If a basic block was attached then the @ref bblock member will be non-null, otherwise
     *  the arguments indicate that a placeholder was inserted. This callback is invoked after the changes have been made
     *  to the CFG/AUM. The partitioner is passed as a const pointer because the callback should not modify the CFG/AUM;
     *  this callback may represent only one step in a larger sequence, and modifying the CFG/AUM could confuse things. */
    struct AttachedBasicBlock {
        const Partitioner *partitioner;             /**< Partitioner in which change occurred. */
        rose_addr_t startVa;                        /**< Starting address for basic block or placeholder. */
        BasicBlock::Ptr bblock;                     /**< Optional basic block; otherwise a placeholder operation. */
        AttachedBasicBlock(Partitioner *partitioner, rose_addr_t startVa, const BasicBlock::Ptr &bblock)
            : partitioner(partitioner), startVa(startVa), bblock(bblock) {}
    };

    /** Arguments for detaching a basic block.
     *
     *  After a basic block is detached from the CFG/AUM, or after a placeholder is removed from the CFG, these arguments
     *  are passed to the callback.  If a basic block was detached then the @ref bblock member will be non-null, otherwise
     *  the arguments indicate that a placeholder was removed from the CFG.  This callback is invoked after the changes
     *  have been made to the CFG/AUM. The partitioner is passed as a const pointer because the callback should not modify
     *  the CFG/AUM; this callback may represent only one step in a larger sequence, and modifying the CFG/AUM could
     *  confuse things. */
    struct DetachedBasicBlock {
        const Partitioner *partitioner;             /**< Partitioner in which change occurred. */
        rose_addr_t startVa;                        /**< Starting address for basic block or placeholder. */
        BasicBlock::Ptr bblock;                     /**< Optional basic block; otherwise a placeholder operation. */
        DetachedBasicBlock(Partitioner *partitioner, rose_addr_t startVa, const BasicBlock::Ptr &bblock)
            : partitioner(partitioner), startVa(startVa), bblock(bblock) {}
    };

    /** Called when basic block is attached or placeholder inserted. */
    virtual bool operator()(bool chain, const AttachedBasicBlock&) = 0;

    /** Called when basic block is detached or placeholder erased. */
    virtual bool operator()(bool chain, const DetachedBasicBlock&) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Control flow graph utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Insert one control flow graph into another.
 *
 *  The @p vmap is updated with the mapping of vertices from source to destination. Upon return, <code>vmap[srcVertex]</code>
 *  will point to the corresponding vertex in the destination graph. */
void insertCfg(ControlFlowGraph &dst, const ControlFlowGraph &src, CfgVertexMap &vmap /*out*/);

/** Find back edges.
 *
 *  Performs a depth-first forward traversal of the @p cfg beginning at the specified vertex. Any edge encountered which points
 *  back to some vertex in the current traversal path is added to the returned edge set. */
CfgConstEdgeSet findBackEdges(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &begin);

/** Find function call edges.
 *
 *  Returns the list of function call edges for the specified vertex. */
CfgConstEdgeSet findCallEdges(const ControlFlowGraph::ConstVertexIterator &callSite);

/** Find called functions.
 *
 *  Given some vertex in a CFG, return the vertices representing the functions that are called. */
CfgConstVertexSet findCalledFunctions(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &callSite);

/** Return outgoing call-return edges.
 *
 *  A call-return edge represents a short-circuit control flow path across a function call, from the call site to the return
 *  target. */
CfgConstEdgeSet findCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &callSite);

/** Find function return vertices.
 *
 *  Returns the list of vertices with outgoing E_FUNCTION_RETURN edges. */
CfgConstVertexSet findFunctionReturns(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &beginVertex);

/** Erase multiple edges.
 *
 *  Erases each edge in the @p toErase set. */
void eraseEdges(ControlFlowGraph&, const CfgConstEdgeSet &toErase);

/** Vertices that are incident to specified edges. */
CfgConstVertexSet findIncidentVertices(const CfgConstEdgeSet&);

/** Find vertices that have zero degree.
 *
 *  Returns a set of vertices that have no incoming or outgoing edges. If @p vertices are specified, then limit the return
 *  value to the specified vertices; this mode of operation can be significantly faster than scanning the entire graph.
 *
 * @{ */
CfgConstVertexSet findDetachedVertices(const ControlFlowGraph&);
CfgConstVertexSet findDetachedVertices(const CfgConstVertexSet &vertices);
/** @} */

/** Return corresponding iterators.
 *
 *  Given a set of iterators and a vertex map, return the corresponding iterators by following the forward mapping. Any vertex
 *  in the argument that is not present in the mapping is silently ignored. */
CfgConstVertexSet forwardMapped(const CfgConstVertexSet&, const CfgVertexMap&);

/** Return corresponding iterators.
 *
 *  Given a set of iterators and a vertex map, return the corresponding iterators by following the reverse mapping. Any vertex
 *  in the argument that is not present in the mapping is silently ignored. */
CfgConstVertexSet reverseMapped(const CfgConstVertexSet&, const CfgVertexMap&);

} // namespace
} // namespace
} // namespace

#endif
