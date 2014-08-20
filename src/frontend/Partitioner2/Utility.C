#include "sage3basic.h"
#include <Partitioner2/Utility.h>

#include "Diagnostics.h"
#include <Partitioner2/Partitioner.h>


namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Sawyer::Message::Facility mlog("rose::BinaryAnalysis::Partitioner2");

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog.initStreams(Diagnostics::destination);
        Diagnostics::mfacilities.insert(mlog);
    }
}

bool
sortBasicBlocksByAddress(const BasicBlock::Ptr &a, const BasicBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

bool
sortDataBlocks(const DataBlock::Ptr &a, const DataBlock::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a!=b) {
        if (a->address() != b->address())
            return a->address() < b->address();
        if (a->size() < b->size())
            return true;
    }
    return false;
}

bool
sortFunctionsByAddress(const Function::Ptr &a, const Function::Ptr &b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return a->address() < b->address();
}

// Sort by the successor expressions. If both expressions are concrete then disregard their widths and treat them as unsigned
// when comparing.
bool
sortByExpression(const BasicBlock::Successor &a, const BasicBlock::Successor &b) {
    if (a.expr()->is_number() && b.expr()->is_number())
        return a.expr()->get_number() < b.expr()->get_number();
    if (a.expr()->is_number() || b.expr()->is_number())
        return a.expr()->is_number();                   // concrete values are less than abstract expressions
    return a.expr()->get_expression()->structural_compare(b.expr()->get_expression()) < 0;
}

bool
sortVerticesByAddress(const ControlFlowGraph::ConstVertexNodeIterator &a,
                      const ControlFlowGraph::ConstVertexNodeIterator &b) {
    const CfgVertex &av = a->value();
    const CfgVertex &bv = b->value();
    if (av.type() != bv.type() || av.type() != V_BASIC_BLOCK)
        return av.type() < bv.type();
    return av.address() < bv.address();
}

bool
sortEdgesBySrc(const ControlFlowGraph::ConstEdgeNodeIterator &a,
               const ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->source(), b->source());
}

bool
sortEdgesByDst(const ControlFlowGraph::ConstEdgeNodeIterator &a,
               const ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->target(), b->target());
}

bool
sortBlocksForAst(SgAsmBlock *a, SgAsmBlock *b) {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    if (a->get_address() != b->get_address())
        return a->get_address() < b->get_address();
    if (a->get_statementList().size()>0 && b->get_statementList().size()>0) {
        // Sort so basic blocks come before data blocks when they start at the same address, regardless of size
        bool a_isBasicBlock = NULL!=isSgAsmInstruction(a->get_statementList()[0]);
        bool b_isBasicBlock = NULL!=isSgAsmInstruction(b->get_statementList()[0]);
        if (a_isBasicBlock != b_isBasicBlock)
            return a_isBasicBlock;
    }
    return false;
}

std::ostream&
operator<<(std::ostream &out, const ControlFlowGraph::VertexNode &x) {
    out <<Partitioner::vertexName(x);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const ControlFlowGraph::EdgeNode &x) {
    out <<Partitioner::edgeName(x);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUser &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsers &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressUsageMap &x) {
    x.print(out);
    return out;
}


} // namespace
} // namespace
} // namespace
