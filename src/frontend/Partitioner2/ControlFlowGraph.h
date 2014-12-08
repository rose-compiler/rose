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
    Confidence confidence_;
public:
    CfgEdge(): type_(E_NORMAL), confidence_(ASSUMED) {}
    explicit CfgEdge(EdgeType type, Confidence confidence=ASSUMED): type_(type), confidence_(confidence) {}
    EdgeType type() const { return type_; }
    Confidence confidence() const { return confidence_; }
    void confidence(Confidence c) { confidence_ = c; }
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

} // namespace
} // namespace
} // namespace

#endif
