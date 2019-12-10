#ifndef ROSE_Partitioner2_ControlFlowGraph_H
#define ROSE_Partitioner2_ControlFlowGraph_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Utility.h>

#include <Sawyer/BiMap.h>
#include <Sawyer/Graph.h>
#include <Sawyer/GraphIteratorSet.h>
#include <Sawyer/GraphIteratorBiMap.h>
#include <Sawyer/HashMap.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>

#include <boost/serialization/access.hpp>
#include <list>
#include <ostream>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Control flow graph vertex. */
class CfgVertex {
    VertexType type_;                                   // type of vertex, special or not
    rose_addr_t startVa_;                               // address of start of basic block
    BasicBlock::Ptr bblock_;                            // basic block, or null if only a place holder
    FunctionSet owningFunctions_;                       // functions to which vertex belongs

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(type_);
        s & BOOST_SERIALIZATION_NVP(startVa_);
        s & BOOST_SERIALIZATION_NVP(bblock_);
        s & BOOST_SERIALIZATION_NVP(owningFunctions_);
    }
#endif

public:
    // intentionally undocumented. Necessary for serialization of Sawyer::Container::Graph
    CfgVertex()
        : type_(V_USER_DEFINED), startVa_(0) {}

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
     *  Calling this for a vertex that's not a basic block or user defined type results in undefined behavior.
     *
     *  See also, @ref optionalAddress.
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

    /** Safe version of starting address.
     *
     *  This is the same as @ref address but doesn't fail if there is no address. */
    Sawyer::Optional<rose_addr_t> optionalAddress() const;

    /** Address of last item in the vertex.
     *
     *  If the vertex is a basic block with more than one instruction, then return the address of its last instruction.
     *  Otherwise, this is the same as @ref optionalAddress. */
    Sawyer::Optional<rose_addr_t> optionalLastAddress() const;

    /** Compute entire address set.
     *
     *  Constructs and returns an address set containing all addresses represented by this vertex.  For basic block vertices,
     *  this is the union of the addresses occupied by the instructions (not just the starting address of each
     *  instruction).  For user-defined vertices, this method returns a singleton address. All other vertex types return an
     *  empty set since it is generally their predecessors that have addresses (e.g., each undiscovered basic block is a
     *  basic-block CFG vertex whose successor is the special "undiscovered" vertex which has no distinct address. */
    AddressIntervalSet addresses() const;

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

    /** Add a function to the list of functions that own this vertex.
     *
     *  Returns true if the function was added, false if it was already an owner of the vertex. */
    bool insertOwningFunction(const Function::Ptr &function) {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        ASSERT_not_null(function);
        return owningFunctions_.insert(function);
    }

    /** Remove a function from the list of functions that own this vertex.
     *
     *  Causes the specified function to no longer be listed as an owner of this vertex. Does nothing if the function is not an
     *  owner to begin with. */
    void eraseOwningFunction(const Function::Ptr &function) {
        ASSERT_require(V_BASIC_BLOCK==type_ || V_USER_DEFINED==type_);
        if (function != NULL)
            owningFunctions_.erase(function);
    }

    /** Determines if a function owns this vertex.
     *
     *  Returns true if the specified function is listed as an owning function of this vertex, false otherwise. */
    bool isOwningFunction(const Function::Ptr &function) const {
        return owningFunctions_.exists(function);
    }

    /** Number of functions that own this vertex. */
    size_t nOwningFunctions() const {
        return owningFunctions_.size();
    }

    /** Property: Owning functions.
     *
     *  Certain kinds of vertices can be owned by zero or more functions, and this property holds the set of such
     *  functions. See also, @ref insertOwningFunction, @ref eraseOwningFunction, and @ref isOwningFunction. This property is
     *  available for @ref V_BASIC_BLOCK and @ref V_USER_DEFINED vertices.
     *
     *  @{ */
    const FunctionSet& owningFunctions() const {
        return owningFunctions_;
    }
    FunctionSet& owningFunctions() {
        return owningFunctions_;
    }
    /** @} */

    /** Is block a function entry block?
     *
     *  Returns true (a non-null function pointer) if this block serves as the entry block for some function. */
    Function::Ptr isEntryBlock() const;

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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(type_);
        s & BOOST_SERIALIZATION_NVP(confidence_);
    }
#endif

public:
    /** Construct a new normal edge. */
    CfgEdge(): type_(E_NORMAL), confidence_(ASSUMED) {}

    /** Construct an edge with a specified type and confidence. */
    /*implicit*/ CfgEdge(EdgeType type, Confidence confidence=ASSUMED): type_(type), confidence_(confidence) {}

    /** Return edge type. */
    EdgeType type() const { return type_; }

    /** Property: Confidence.
     *
     *  The confidence that this edge is correct.
     *
     * @{ */
    Confidence confidence() const { return confidence_; }
    void confidence(Confidence c) { confidence_ = c; }
    /** @} */
};

/** Control flow graph. */
typedef Sawyer::Container::Graph<CfgVertex, CfgEdge> ControlFlowGraph;

/** Sort edges by source vertex address. */
bool sortEdgesBySrc(const ControlFlowGraph::ConstEdgeIterator&, const ControlFlowGraph::ConstEdgeIterator&);

/** Sort edges by target vertex address. */
bool sortEdgesByDst(const ControlFlowGraph::ConstEdgeIterator&, const ControlFlowGraph::ConstEdgeIterator&);

/** Sort edges by edge ID number. */
bool sortEdgesById(const ControlFlowGraph::ConstEdgeIterator&, const ControlFlowGraph::ConstEdgeIterator&);

/** Sort vertices by address. */
bool sortVerticesByAddress(const ControlFlowGraph::ConstVertexIterator&, const ControlFlowGraph::ConstVertexIterator&);

/** Sort vertices by vertex ID number. */
bool sortVerticesById(const ControlFlowGraph::ConstVertexIterator&, const ControlFlowGraph::ConstVertexIterator&);

/** Print control flow graph vertex. */
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::Vertex&);

/** Print control flow graph edge. */
std::ostream& operator<<(std::ostream&, const ControlFlowGraph::Edge&);

/** Mapping from basic block starting address to CFG vertex. */
typedef Sawyer::Container::HashMap<rose_addr_t, ControlFlowGraph::VertexIterator> CfgVertexIndex;

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
typedef Sawyer::Container::GraphIteratorSet<ControlFlowGraph::VertexIterator> CfgVertexSet;
typedef Sawyer::Container::GraphIteratorSet<ControlFlowGraph::ConstVertexIterator> CfgConstVertexSet;
/** @} */

/** Set of CFG edge pointers.
 *
 * @{ */
typedef Sawyer::Container::GraphIteratorSet<ControlFlowGraph::EdgeIterator> CfgEdgeSet;
typedef Sawyer::Container::GraphIteratorSet<ControlFlowGraph::ConstEdgeIterator> CfgConstEdgeSet;
/** @} */

/** Map vertices from one CFG to another CFG and vice versa. */
typedef Sawyer::Container::GraphIteratorBiMap<ControlFlowGraph::ConstVertexIterator,
                                              ControlFlowGraph::ConstVertexIterator> CfgVertexMap;

/** Base class for CFG-adjustment callbacks.
 *
 *  Users may create subclass objects from this class and pass their shared-ownership pointers to the partitioner, in which
 *  case the partitioner will invoke one of the callback's virtual function operators every time the control flow graph
 *  changes (the call occurs after the CFG has been adjusted).  Multiple callbacks are allowed; the list is obtained with
 *  the @ref cfgAdjustmentCallbacks method. */
class CfgAdjustmentCallback: public Sawyer::SharedObject {
public:
    /** Shared ownership pointer to @ref CfgAdjustmentCallback. See @ref heap_object_shared_ownership. */
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
 *  target.
 *
 *  If a partitioner and control flow graph are specified, then this returns all edges of type @ref E_CALL_RETURN. If a vertex
 *  is specified, then it returns only those call-return edges that emanate from said vertex.
 *
 * @{ */
CfgConstEdgeSet findCallReturnEdges(const Partitioner&, const ControlFlowGraph&);
CfgConstEdgeSet findCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &callSite);
/** @} */

/** Find all function return vertices.
 *
 *  Returns the list of vertices with outgoing E_FUNCTION_RETURN edges. */
CfgConstVertexSet findFunctionReturns(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &beginVertex);

/** Find function return edges organized by function.
 *
 *  Finds all control flow graph edges that are function return edges and organizes them according to the function from
 *  which they emanate.  Note that since a basic block can be shared among several functions (usually just one though), an
 *  edge may appear multiple times in the returned map.
 *
 *  If a control flow graph is supplied, it must be compatible with the specified partitioner. If no control flow graph
 *  is specified then the partitioner's own CFG is used.
 *
 * @{ */
Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet> findFunctionReturnEdges(const Partitioner&);
Sawyer::Container::Map<Function::Ptr, CfgConstEdgeSet> findFunctionReturnEdges(const Partitioner&, const ControlFlowGraph&);
/** @} */

/** Erase multiple edges.
 *
 *  Erases each edge in the @p toErase set. Upon return, the toErase set's values will all be invalid and should not be dereferenced. */
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

/** Rewrite function return edges.
 *
 *  Given a graph that has function return edges (@ref E_FUNCTION_RETURN) that point to the indeterminate vertex, replace them
 *  with function return edges that point to the return sites. The return sites are the vertices pointed to by the call-return
 *  (@ref E_CALL_RETURN) edges emanating from the call sites for said function.  The graph is modified in place. The resulting
 *  graph will usually have more edges than the original graph. */
void expandFunctionReturnEdges(const Partitioner&, ControlFlowGraph &cfg/*in,out*/);

/** Generate a function control flow graph.
 *
 *  A function control flow graph is created by erasing all parts of the global control flow graph (@p gcfg) that aren't
 *  owned by the specified function.  The resulting graph will contain an indeterminate vertex if the global CFG contains
 *  an indeterminate vertex and the function has any edges to the indeterminate vertex that are not E_FUNCTION_RETURN
 *  edges. In other words, the indeterminate vertex is excluded unless there are things like branches or calls whose target
 *  address is a register.
 *
 *  Upon return, the @p entry iterator points to the vertex of the return value that serves as the function's entry point.
 *
 *  Note that this method of creating a function control flow graph can be slow since it starts with a global control flow
 *  graph. It has one benefit over @ref functionCfgByReachability though: it will find all vertices that belong to the
 *  function even if they're not reachable from the function's entry point, or even if they're only reachable by traversing
 *  through a non-owned vertex. */
ControlFlowGraph functionCfgByErasure(const ControlFlowGraph &gcfg, const Function::Ptr &function,
                                      ControlFlowGraph::VertexIterator &entry/*out*/);

/** Generate a function control flow graph.
 *
 *  A function control flow graph is created by traversing the specified global control flow grap (@p gcfg) starting at
 *  the specified vertex (@p gcfgEntry). The traversal follows only vertices that are owned by the specified function, and
 *  the indeterminate vertex. The indeterminate vertex is only reachable through edges of types other than @ref
 *  E_FUNCTION_RETURN.
 *
 *  The function control flow graph entry point corresponding to @p gcfgEntry is always vertex number zero in the return
 *  value.  However, if @p gcfgEntry points to a vertex not owned by @p function, or @p gcfg is empty then the returned
 *  graph is also empty.
 *
 *  Note that this method of creating a function control graph can be much faster than @ref functionCfgByErasure since
 *  it only traverses part of the global CFG, but the drawback is that the return value won't include vertices that are
 *  not reachable from the return value's entry vertex. */
ControlFlowGraph functionCfgByReachability(const ControlFlowGraph &gcfg, const Function::Ptr &function,
                                           const ControlFlowGraph::ConstVertexIterator &gcfgEntry);

} // namespace
} // namespace
} // namespace

#endif
