#ifndef ROSE_Partitioner2_ControlFlowGraph_H
#define ROSE_Partitioner2_ControlFlowGraph_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Function.h>

#include <sawyer/Graph.h>
#include <sawyer/Map.h>

#include <list>
#include <ostream>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Control flow graph vertex. */
class CfgVertex {
    friend class Partitioner;
private:
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
    explicit CfgVertex(VertexType type): type_(type), startVa_(0) {
        ASSERT_forbid2(type==V_BASIC_BLOCK, "this constructor does not create basic block or placeholder vertices");
    }

    /** Returns the vertex type. */
    VertexType type() const { return type_; }

    /** Return the starting address of a placeholder or basic block. */
    rose_addr_t address() const {
        ASSERT_require(V_BASIC_BLOCK==type_);
        return startVa_;
    }

    /** Return the basic block pointer.
     *
     *  A null pointer is returned when the vertex is only a basic block placeholder. */
    const BasicBlock::Ptr& bblock() const {
        ASSERT_require(V_BASIC_BLOCK==type_);
        return bblock_;
    }

    /** Return the function pointer.
     *
     *  A basic block may belong to a function, in which case the function pointer is returned. Otherwise the null pointer is
     *  returned. */
    const Function::Ptr& function() const {
        ASSERT_require(V_BASIC_BLOCK==type_);
        return function_;
    }

    /** Turns a basic block vertex into a placeholder.
     *
     *  The basic block pointer is reset to null. */
    void nullify() {
        ASSERT_require(V_BASIC_BLOCK==type_);
        bblock_ = BasicBlock::Ptr();
    }

private:
    // Change the basic block pointer.  Users are not allowed to do this directly; they must go through the Partitioner API.
    void bblock(const BasicBlock::Ptr &bb) {
        bblock_ = bb;
    }

    // Change the function pointer.  Users are not allowed to do this directly; they must go through the Partitioner API.
    void function(const Function::Ptr &f) {
        function_ = f;
    }
};

/** Control flow graph edge. */
class CfgEdge {
private:
    EdgeType type_;
public:
    CfgEdge(): type_(E_NORMAL) {}
    explicit CfgEdge(EdgeType type): type_(type) {}
    EdgeType type() const { return type_; }
};

/** Control flow graph. */
typedef Sawyer::Container::Graph<CfgVertex, CfgEdge> ControlFlowGraph;

/** Mapping from basic block starting address to CFG vertex. */
typedef Sawyer::Container::Map<rose_addr_t, ControlFlowGraph::VertexNodeIterator> VertexIndex;

/** List of CFG vertex pointers.
 *
 * @{ */
typedef std::list<ControlFlowGraph::VertexNodeIterator> VertexList;
typedef std::list<ControlFlowGraph::ConstVertexNodeIterator> ConstVertexList;
/** @} */

/** List of CFG edge pointers.
 *
 * @{ */
typedef std::list<ControlFlowGraph::EdgeNodeIterator> EdgeList;
typedef std::list<ControlFlowGraph::ConstEdgeNodeIterator> ConstEdgeList;
/** @} */

} // namespace
} // namespace
} // namespace

#endif
