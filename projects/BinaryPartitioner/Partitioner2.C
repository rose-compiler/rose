#include "Partitioner2.h"
#include "SymbolicSemantics2.h"
#include <boost/foreach.hpp>

using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::BasicBlock::append(SgAsmInstruction *insn) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to append instruction");
    ASSERT_not_null(insn);
    ASSERT_require2(insns_.empty() || insn->get_address()==startVa_,
                    "address of first instruction (" + StringUtility::addrToString(insn->get_address()) + ") "
                    "must match block address (" + StringUtility::addrToString(startVa_) + ")");
    ASSERT_require2(std::find(insns_.begin(), insns_.end(), insn) == insns_.end(),
                    "instruction can only occur once in a basic block");
    insns_.push_back(insn);
}

SgAsmInstruction*
Partitioner::BasicBlock::instructionExists(rose_addr_t startVa) const {
    BOOST_FOREACH (SgAsmInstruction *insn, insns_) {
        if (insn->get_address() == startVa)
            return insn;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      InsnBlockPairs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner::BasicBlock::Ptr
Partitioner::InsnBlockPairs::instructionExists(SgAsmInstruction *insn) const {
    if (!insn)
        return BasicBlock::Ptr();
    InsnBlockPair needle(insn, BasicBlock::Ptr());      // basic block is not used for binary search
    ASSERT_require(isConsistent());
    std::vector<InsnBlockPair>::const_iterator lb = std::lower_bound(pairs_.begin(), pairs_.end(), needle);
    if (lb==pairs_.end() || lb->insn()!=insn)
        return BasicBlock::Ptr();
    ASSERT_not_null(lb->bblock());
    return lb->bblock();
}

Sawyer::Optional<Partitioner::InsnBlockPair>
Partitioner::InsnBlockPairs::instructionExists(rose_addr_t startVa) const {
    // This could be a binary search, but since instructions seldom overlap much, linear is almost certainly ok.
    BOOST_FOREACH (const InsnBlockPair &pair, pairs_) {
        if (pair.insn()->get_address() == startVa)
            return pair;
    }
    return Sawyer::Nothing();
}

Partitioner::InsnBlockPairs&
Partitioner::InsnBlockPairs::insert(const InsnBlockPair &pair) {
    ASSERT_not_null(pair.insn());
    ASSERT_not_null(pair.bblock());
    ASSERT_forbid(instructionExists(pair.insn()));
    ASSERT_require(isConsistent());
    std::vector<InsnBlockPair>::iterator lb = std::lower_bound(pairs_.begin(), pairs_.end(), pair);
    ASSERT_require2(lb==pairs_.end() || lb->insn()!=pair.insn(), "instruction/block pair already exists in the list");
    pairs_.insert(lb, pair);
    ASSERT_require(isConsistent());
    return *this;
}

Partitioner::InsnBlockPairs&
Partitioner::InsnBlockPairs::erase(SgAsmInstruction *insn) {
    if (insn!=NULL) {
        ASSERT_require(isConsistent());
        InsnBlockPair needle(insn, BasicBlock::Ptr());
        std::vector<InsnBlockPair>::iterator lb = std::lower_bound(pairs_.begin(), pairs_.end(), needle);
        if (lb!=pairs_.end() && lb->insn()==insn)
            pairs_.erase(lb);
    }
    return *this;
}

Partitioner::InsnBlockPairs
Partitioner::InsnBlockPairs::intersection(const InsnBlockPairs &other) const {
    InsnBlockPairs retval;
    size_t i=0, j=0;
    while (i<size() && j<other.size()) {
        while (i<size() && pairs_[i] < other.pairs_[j])
            ++i;
        while (j<other.size() && other.pairs_[j] < pairs_[i])
            ++j;
        if (i<size() && j<other.size() && pairs_[i]==other.pairs_[j]) {
            retval.pairs_.push_back(pairs_[i]);
            ++i;
            ++j;
        }
    }
    ASSERT_require(retval.isConsistent());
    return retval;
}

Partitioner::InsnBlockPairs
Partitioner::InsnBlockPairs::union_(const InsnBlockPairs &other) const {
    InsnBlockPairs retval;
    size_t i=0, j=0;
    while (i<size() && j<other.size()) {
        if (pairs_[i] < other.pairs_[j]) {
            retval.pairs_.push_back(pairs_[i++]);
        } else if (pairs_[i] == other.pairs_[j]) {
            retval.pairs_.push_back(pairs_[i++]);
            ++j;
        } else {
            retval.pairs_.push_back(other.pairs_[j++]);
        }
    }
    ASSERT_require(i>=size() || j>=other.size());
    while (i<size())
        retval.pairs_.push_back(pairs_[i++]);
    while (j<other.size())
        retval.pairs_.push_back(other.pairs_[j++]);
    ASSERT_require(retval.isConsistent());
    return retval;
}

bool
Partitioner::InsnBlockPairs::isConsistent() const {
    if (!pairs_.empty()) {
        std::vector<InsnBlockPair>::const_iterator current = pairs_.begin();
        std::vector<InsnBlockPair>::const_iterator next = current;
        while (current != pairs_.end()) {
            if (NULL==current->insn()) {
                ASSERT_not_null(current->insn());
                return false;
            }
            if (NULL==current->bblock()) {
                ASSERT_not_null(current->bblock());
                return false;
            }
            if (++next != pairs_.end()) {
                if (!(*current < *next)) {
                    ASSERT_forbid2(*next < *current, "list is not sorted");
                    ASSERT_require2(*current < *next, "list contains a duplicate");
                    return false;
                }
            }
        }
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      AddressUsageMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner::BasicBlock::Ptr
Partitioner::AddressUsageMap::instructionExists(SgAsmInstruction *insn) const {
    const InsnBlockPairs noPairs;
    return insn ? map_.getOptional(insn->get_address()).orElse(noPairs).instructionExists(insn) : BasicBlock::Ptr();
}

Sawyer::Optional<Partitioner::InsnBlockPair>
Partitioner::AddressUsageMap::instructionExists(rose_addr_t startVa) const {
    const InsnBlockPairs noPairs;
    if (Sawyer::Optional<InsnBlockPair> found = map_.getOptional(startVa).orElse(noPairs).instructionExists(startVa)) {
        if (found->insn()->get_address() == startVa)
            return found;
    }
    return Sawyer::Nothing();
}

Partitioner::BasicBlock::Ptr
Partitioner::AddressUsageMap::bblockExists(rose_addr_t startVa) const {
    if (Sawyer::Optional<InsnBlockPair> found = instructionExists(startVa)) {
        if (found->bblock()->address() == startVa)
            return found->bblock();
    }
    return BasicBlock::Ptr();
}

void
Partitioner::AddressUsageMap::insert(const InsnBlockPair &pair) {
    ASSERT_not_null(pair.insn());
    ASSERT_not_null(pair.bblock());
    ASSERT_forbid(instructionExists(pair.insn()));
    AddressInterval insnInterval = AddressInterval::baseSize(pair.insn()->get_address(), pair.insn()->get_size());
    Map adjustment;
    adjustment.insert(insnInterval, InsnBlockPairs(pair));
    BOOST_FOREACH (const Map::Node &node, map_.findAll(insnInterval)) {
        InsnBlockPairs newPairs = node.value();
        newPairs.insert(pair);
        adjustment.insert(insnInterval.intersection(node.key()), newPairs);
    }
    map_.insertMultiple(adjustment);
}

void
Partitioner::AddressUsageMap::erase(SgAsmInstruction *insn) {
    if (insn) {
        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
        Map adjustment;
        BOOST_FOREACH (const Map::Node &node, map_.findAll(insnInterval)) {
            InsnBlockPairs newPairs = node.value();
            newPairs.erase(insn);
            if (!newPairs.isEmpty())
                adjustment.insert(insnInterval.intersection(node.key()), newPairs);
        }
        map_.erase(insnInterval);
        map_.insertMultiple(adjustment);
    }
}

Partitioner::InsnBlockPairs
Partitioner::AddressUsageMap::spanning(const AddressInterval &interval) const {
    InsnBlockPairs retval;
    size_t nIters = 0;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval)) {
        retval = 0==nIters++ ? node.value() : retval.intersection(node.value());
        if (retval.isEmpty())
            break;
    }
    return retval;
}

Partitioner::InsnBlockPairs
Partitioner::AddressUsageMap::overlapping(const AddressInterval &interval) const {
    InsnBlockPairs retval;
    BOOST_FOREACH (const Map::Node &node, map_.findAll(interval))
        retval = retval.union_(node.value());
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Partitioner
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::init() {
    undiscoveredVertex_ = cfg_.insertVertex(CFGVertex(CFGVertex::VERTEX_UNDISCOVERED));
}

void
Partitioner::nullifyBasicBlock(const ControlFlowGraph::VertexNodeIterator &vertex) {
    ASSERT_require2(vertex != cfg_.vertices().end(), "basic block vertex must exist");
    if (BasicBlock::Ptr bb = vertex->value().bblock()) {
        vertex->value().nullify();
        adjustPlaceholderEdges(vertex);
        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
            addrUsageMap_.erase(insn);
        bblockErased(bb);
    }
}

void
Partitioner::eraseBasicBlock(const ControlFlowGraph::VertexNodeIterator &vertex) {
    ASSERT_require2(vertex != cfg_.vertices().end(), "basic block vertex must exist");
    BasicBlock::Ptr bb = vertex->value().bblock();
    if (bb)
        nullifyBasicBlock(vertex);                      // removes self edges, notifies subclasses of CFG changes
    ASSERT_require2(vertex->nInEdges()==0, "vertex to be erased must have no incoming edges other than self edges");
    vertexIndex_.erase(vertex->value().address());
    cfg_.eraseVertex(vertex);
    if (bb)
        bblockErased(bb);
}

Partitioner::ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    ASSERT_require(placeholder!=cfg_.vertices().end());
    ASSERT_require2(NULL==placeholder->value().bblock(), "vertex must be strictly a placeholder");
    cfg_.clearOutEdges(placeholder);
    return cfg_.insertEdge(placeholder, undiscoveredVertex_);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    ASSERT_require2(placeholder != cfg_.vertices().end(), "invalid basic block placeholder");
    BasicBlock::Ptr bb = placeholder->value().bblock();
    if (bb)
        return bb;
    bb = discoverBasicBlockInternal(placeholder->value().address());
    placeholder->value().bblock(bb);
    return bb;
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(rose_addr_t startVa) {
    ControlFlowGraph::VertexNodeIterator placeholder = placeholderExists(startVa);
    return placeholder==cfg_.vertices().end() ? discoverBasicBlockInternal(startVa) : discoverBasicBlock(placeholder);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlockInternal(rose_addr_t startVa) {
    // If the first instruction of this basic block already exists (in the middle of) some other basic block then this basic
    // block is said to be "start-conflicting" with another block and we behave a little differently than normal (see rule 8
    // below).
    InsnBlockPair conflict;
    if (instructionExists(startVa).assignTo(conflict))
        ASSERT_forbid(conflict.insn()->get_address() == conflict.bblock()->address());// handled in discoverBasicBlock

    // Add instructions to the basic block until one of the following conditions:
    //   (1) We could not obtain an instructon.
    //          The instruction provider should only return null if the address is not mapped or is not mapped with execute
    //          permission.  The basic block's final instruction is the previous instruction, if any.  If the block is empty
    //          then it is said to be non-existing and its successors are a special case when it's eventually added to the CFG.
    //   (2) The instruction is an "unknown" instruction.
    //          The instruction provider returns an unknown instruction if it isn't able to disassemble something but the
    //          disassembly address is mapped with execute permission.  The partitioner treats this "unknown" instruction as a
    //          valid instruction with indeterminate successors.
    //   (3) The instruction doesn't have exactly one successor
    //          Basic blocks cannot have a non-final instruction that branches, so we're done.
    //   (4) The instruction successor is not a constant
    //          If the successor is not a constant then we don't know what's next.  This can happen for things like
    //          unconditional branches indirect through memory (e.g., return-from-function, jumps through a table, etc).
    //   (5) The successor address is the starting address for the block on which we're working.
    //          A basic block's instructions are unique by definition, so we're done.
    //   (6) The successor address is an address of a (non-initial) instruction in this block.
    //          Basic blocks cannot have a non-initial instruction with more than one incoming edge, therefore we've
    //          added too many instructions to this block.  We could proceed two ways:
    //             (A) We could toss the instruction with the back-edge and make the block terminate at the previous
    //                 instruction. This causes the basic block to be as big as possible for as long as possible, which is a
    //                 good thing if we determine later that the instruction with the back-edge is not reachable anyway.
    //             (B) We could truncate the basic block at the back-edge target (i.e., the instruction at that address is not
    //                 part of the basic block). This is good because it converges to a steady state faster.
    //   (7) The successor address is the starting address of basic block already in the CFG
    //          This is a common case and probably means that what we discovered earlier is correct.
    //   (8) The successor address is an instruction already in the CFG other than in the conflict block.
    //          If the first instruction of the block on which we're working is an instruction in the middle of some other
    //          basic block in the CFG, then we allow this block to use some of the same instructions as in the conflicting
    //          block and we do not terminate construction of this block at this time. Usually what happens is this new block
    //          uses all the final instructions from the conflicting block; an exception is when an opaque predicate in the
    //          conflicting block is no longer opaque in the new block.  Eventually when the new block is added to the CFG the
    //          conflicting block will be truncated.  On the other hand, if this isn't the first instruction of this block,
    //          then we're done -- we've reached a point where we would introduce an edge in the CFG from the end of this block
    //          into the middle of some other block (the other block will need to be truncated, split into two parts).
    BasicBlock::Ptr retval = BasicBlock::instance(startVa);
    rose_addr_t va = startVa;
    while (1) {
        SgAsmInstruction *insn = (*instructionProvider_)[startVa];
        if (insn==NULL)                                                 // case 1
            break;
        retval->append(insn);
        if (insn->is_unknown())                                         // case 2
            break;
        std::vector<SValuePtr> successors = retval->successors();
        if (successors.size()!=1)                                       // case 3
            break;
        SValuePtr successorExpr = successors.front();
        if (!successorExpr->is_number())                                // case 4
            break;
        rose_addr_t successorVa = successorExpr->get_number();
        if (successorVa == startVa)                                     // case 5
            break;
        if (retval->instructionExists(successorVa))                     // case 6
            break;                                                      // option A
        if (placeholderExists(successorVa)!=cfg_.vertices().end())      // case 7
            break;
        InsnBlockPair ibpair;
        if (instructionExists(successorVa).assignTo(ibpair)) {          // case 8
            if (ibpair.bblock() != conflict.bblock())
                break;
        }
        va = successorVa;
    }

    return retval;
}

} // namespace
} // namespace
} // namespace
