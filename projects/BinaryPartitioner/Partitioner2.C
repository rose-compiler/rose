#include "Partitioner2.h"
#include "SymbolicSemantics2.h"
#include "DispatcherM68k.h"
#include "Diagnostics.h"

#if 1 // DEBUGGING [Robb P. Matzke 2014-08-02]
#include "AsmUnparser_compat.h"
#endif

#include <boost/foreach.hpp>

using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Sawyer::Message::Facility Partitioner::mlog("Partitioner2");

// class method
void Partitioner::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog.initStreams(Diagnostics::destination);
        Diagnostics::mfacilities.insert(mlog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::BasicBlock::init(const Partitioner *partitioner) {
    initDiagnostics();
    if (dispatcher_ = partitioner->newDispatcher()) {
        finalState_ = dispatcher_->get_operators()->get_state(); // points into the dispatcher, so always up to date
        initialState_ = finalState_->clone();                    // make a copy so it doesn't ever change
    }
}

void
Partitioner::BasicBlock::clearCache() {
    uncacheSuccessors();
    uncacheIsFunctionCall();
}

SgAsmInstruction*
Partitioner::BasicBlock::instructionExists(rose_addr_t startVa) const {
    BOOST_FOREACH (SgAsmInstruction *insn, insns_) {
        if (insn->get_address() == startVa)
            return insn;
    }
    return NULL;
}

Sawyer::Optional<size_t>
Partitioner::BasicBlock::instructionExists(SgAsmInstruction *toFind) const {
    for (size_t i=0; i<insns_.size(); ++i) {
        if (insns_[i]==toFind)
            return i;
    }
    return Sawyer::Nothing();
}

void
Partitioner::BasicBlock::append(SgAsmInstruction *insn) {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to append instruction");
    ASSERT_not_null(insn);
    ASSERT_require2(!insns_.empty() || insn->get_address()==startVa_,
                    "address of first instruction (" + StringUtility::addrToString(insn->get_address()) + ") "
                    "must match block address (" + StringUtility::addrToString(startVa_) + ")");
    ASSERT_require2(std::find(insns_.begin(), insns_.end(), insn) == insns_.end(),
                    "instruction can only occur once in a basic block");

    // Process the instruction to create a new state
    clearCache();
    insns_.push_back(insn);
    if (finalState_) {
        try {
            dispatcher_->processInstruction(insn);
        } catch (...) {
            finalState_ = BaseSemantics::StatePtr();    // turns off semantics for the remainder of this block
        }
    }
}

rose_addr_t
Partitioner::BasicBlock::fallthroughVa() const {
    ASSERT_require(!insns_.empty());
    return insns_.back()->get_address() + insns_.back()->get_size();
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
            ++current;
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
    undiscoveredVertex_ = cfg_.insertVertex(CfgVertex(V_UNDISCOVERED));
    indeterminateVertex_ = cfg_.insertVertex(CfgVertex(V_INDETERMINATE));
    nonexistingVertex_ = cfg_.insertVertex(CfgVertex(V_NONEXISTING));
}

BaseSemantics::DispatcherPtr
Partitioner::newDispatcher() const {
    if (instructionProvider_.dispatcher() == NULL)
        return BaseSemantics::DispatcherPtr();          // instruction semantics are not implemented for this architecture
    const RegisterDictionary *registers = instructionProvider_.registerDictionary();
    BaseSemantics::RiscOperatorsPtr ops = Semantics::RiscOperators::instance(registers, solver_);
    return instructionProvider_.dispatcher()->create(ops);
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

Partitioner::ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustNonexistingEdges(const ControlFlowGraph::VertexNodeIterator &vertex) {
    ASSERT_require(vertex!=cfg_.vertices().end());
    ASSERT_not_null2(vertex->value().bblock(), "vertex must have been discovered");
    ASSERT_require2(vertex->value().bblock()->isEmpty(), "vertex must be non-existing");
    cfg_.clearOutEdges(vertex);
    return cfg_.insertEdge(vertex, nonexistingVertex_);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    ASSERT_require2(placeholder != cfg_.vertices().end(), "invalid basic block placeholder");
    BasicBlock::Ptr bb = placeholder->value().bblock();
    return bb!=NULL ? bb : discoverBasicBlockInternal(placeholder->value().address());
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlock(rose_addr_t startVa) {
    ControlFlowGraph::VertexNodeIterator placeholder = placeholderExists(startVa);
    return placeholder==cfg_.vertices().end() ? discoverBasicBlockInternal(startVa) : discoverBasicBlock(placeholder);
}

Partitioner::BasicBlock::Ptr
Partitioner::discoverBasicBlockInternal(rose_addr_t startVa) {
    // If the first instruction of this basic block already exists (in the middle of) some other basic block then the other
    // basic block is called a "conflicting block".  This only applies for the first instruction of this block, but is used in
    // the termination conditions below.
    InsnBlockPair conflict;
    if (instructionExists(startVa).assignTo(conflict))
        ASSERT_forbid(conflict.insn()->get_address() == conflict.bblock()->address());// handled in discoverBasicBlock

    // Keep adding instructions until we reach a termination condition.  The termination conditions are enumerated in detail in
    // the doxygen documentation for this function. READ IT AND KEEP IT UP TO DATE!!!
    BasicBlock::Ptr retval = BasicBlock::instance(startVa, this);
    rose_addr_t va = startVa;
    while (1) {
        SgAsmInstruction *insn = discoverInstruction(va);
        if (insn==NULL)                                                 // case: no instruction available
            break;
        retval->append(insn);
        if (insn->is_unknown())                                         // case: "unknown" instruction
            break;
        if (bblockIsFunctionCall(retval))                               // case: bb looks like a function call
            break;
        BasicBlock::Successors successors = bblockSuccessors(retval);
        if (successors.size()!=1)                                       // case: not exactly one successor
            break;
        SValuePtr successorExpr = successors.front().expr();
        if (!successorExpr->is_number())                                // case: successor is indeterminate
            break;
        rose_addr_t successorVa = successorExpr->get_number();
        if (successorVa == startVa)                                     // case: successor is our own basic block
            break;
        if (retval->instructionExists(successorVa))                     // case: successor is inside some other block
            break;
        if (placeholderExists(successorVa)!=cfg_.vertices().end())      // case: successor is an existing block
            break;
        InsnBlockPair ibpair;
        if (instructionExists(successorVa).assignTo(ibpair)) {          // case: successor is inside an existing block
            if (ibpair.bblock() != conflict.bblock())
                break;
        }
        va = successorVa;
    }

    retval->freeze();
    return retval;
}

Partitioner::ControlFlowGraph::VertexNodeIterator
Partitioner::truncateBasicBlock(const ControlFlowGraph::VertexNodeIterator &basicBlock, SgAsmInstruction *insn) {
    ASSERT_require(basicBlock != cfg_.vertices().end());
    ASSERT_not_null(insn);
    BasicBlock::Ptr bb = basicBlock->value().bblock();
    ASSERT_not_null(bb);
    ASSERT_require(bb->instructionExists(insn));
    ASSERT_require2(bb->instructions().front() != insn, "instruction must not be the initial instruction");

    // For now we do a niave approach; this could be faster [Robb P. Matzke 2014-08-02]
    nullifyBasicBlock(basicBlock);                      // throw away the original block
    ControlFlowGraph::VertexNodeIterator newPlaceholder = insertPlaceholder(insn->get_address());
    BasicBlock::Ptr newBlock = discoverBasicBlock(basicBlock); // rediscover original block, but terminate at newPlaceholder
    insertBasicBlock(basicBlock, newBlock);             // insert new block at original placeholder and insert successor edge
    return newPlaceholder;
}

Partitioner::ControlFlowGraph::VertexNodeIterator
Partitioner::insertPlaceholder(rose_addr_t startVa) {
    ControlFlowGraph::VertexNodeIterator placeholder = placeholderExists(startVa);
    if (placeholder == cfg_.vertices().end()) {
        InsnBlockPair ibpair;
        if (instructionExists(startVa).assignTo(ibpair)) {
            ControlFlowGraph::VertexNodeIterator conflictBlock = placeholderExists(ibpair.bblock()->address());
            placeholder = truncateBasicBlock(conflictBlock, ibpair.insn());
            ASSERT_require(placeholder->value().address() == startVa);
        } else {
            placeholder = cfg_.insertVertex(CfgVertex(startVa));
            vertexIndex_.insert(startVa, placeholder);
            adjustPlaceholderEdges(placeholder);
        }
    }
    return placeholder;
}

void
Partitioner::insertBasicBlock(const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    ControlFlowGraph::VertexNodeIterator placeholder = insertPlaceholder(bb->address()); // insert or find existing
    insertBasicBlock(placeholder, bb);
}

void
Partitioner::insertBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, const BasicBlock::Ptr &bb) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_not_null(bb);
    ASSERT_require2(placeholder->value().address() == bb->address(), "wrong placeholder for basic block");
    if (placeholder->value().bblock() == bb)
        return;                                         // nothing to do since basic block is already in the CFG
    nullifyBasicBlock(placeholder);                     // remove the old block if necessary to make room for the new
    bool isFunctionCall = bblockIsFunctionCall(bb);

    // Make sure placeholders exist for the concrete successors
    bool hadIndeterminate = false;
    typedef std::pair<ControlFlowGraph::VertexNodeIterator, CfgEdge> VertexEdgePair;
    std::vector<VertexEdgePair> successors;
    BOOST_FOREACH (const BasicBlock::Successor &successor, bblockSuccessors(bb)) {
        CfgEdge edge(isFunctionCall ? E_FCALL : E_NORMAL);
        if (successor.expr()->is_number()) {
            successors.push_back(VertexEdgePair(insertPlaceholder(successor.expr()->get_number()), edge));
        } else if (!hadIndeterminate) {
            successors.push_back(VertexEdgePair(indeterminateVertex_, edge));
            hadIndeterminate = true;
        }
    }

    // Function calls get an additional return edge because we assume they return to the fall-through address
    if (isFunctionCall)
        successors.push_back(VertexEdgePair(insertPlaceholder(bb->fallthroughVa()), CfgEdge(E_FRET)));

    // Make CFG edges
    cfg_.clearOutEdges(placeholder);
    BOOST_FOREACH (const VertexEdgePair &pair, successors)
        cfg_.insertEdge(placeholder, pair.first, pair.second);

    // Insert the basicblock
    placeholder->value().bblock(bb);
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
        addrUsageMap_.insert(InsnBlockPair(insn, bb));
    if (bb->isEmpty())
        adjustNonexistingEdges(placeholder);

    bblockInserted(placeholder);
}

Partitioner::BasicBlock::Successors
Partitioner::bblockSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    BasicBlock::Successors successors;

    if (bb->isEmpty() || bb->cachedSuccessors().assignTo(successors))
        return successors;

    SgAsmInstruction *firstInsn = bb->instructions().front();
    ASSERT_not_null(bb->dispatcher());
    BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
    RegisterDescriptor REG_IP = instructionProvider_.instructionPointerRegister();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        std::vector<Semantics::SValuePtr> worklist(1, Semantics::SValue::promote(ops->readRegister(REG_IP)));
        while (!worklist.empty()) {
            Semantics::SValuePtr pc = worklist.back();
            worklist.pop_back();

            if (pc->is_number()) {
                successors.push_back(BasicBlock::Successor(pc));
                continue;
            }

            if (InsnSemanticsExpr::InternalNodePtr ifNode = pc->get_expression()->isInternalNode()) {
                if (ifNode->get_operator()==InsnSemanticsExpr::OP_ITE) {
                    Semantics::SValuePtr expr = Semantics::SValue::promote(ops->undefined_(ifNode->get_nbits()));
                    expr->set_expression(ifNode->child(1));
                    worklist.push_back(expr);
                    expr = Semantics::SValue::promote(ops->undefined_(ifNode->get_nbits()));
                    expr->set_expression(ifNode->child(2));
                    worklist.push_back(expr);
                    continue;
                }
            }

            successors.push_back(BasicBlock::Successor(pc));
        }
        return bb->cacheSuccessors(successors);
    }

    // We don't have semantics, so delegate to the SgAsmInstruction subclass (which might try some other semantics).
    bool complete = true;
    Disassembler::AddressSet successorVas = firstInsn->get_successors(bb->instructions(), &complete, &memoryMap_);
    BOOST_FOREACH (rose_addr_t va, successorVas)
        successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->number_(REG_IP.get_nbits(), va))));
    if (!complete)
        successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->undefined_(REG_IP.get_nbits()))));
    return bb->cacheSuccessors(successors);
}

bool
Partitioner::bblockIsFunctionCall(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->cachedIsFunctionCall().assignTo(retval))
        return retval;

    SgAsmInstruction *lastInsn = bb->instructions().back();
    ASSERT_not_null(bb->dispatcher());
    BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Is the block fall-through address equal to the value on the top of the stack?
        RegisterDescriptor REG_IP = instructionProvider_.instructionPointerRegister();
        RegisterDescriptor REG_SP = instructionProvider_.stackPointerRegister();
        RegisterDescriptor REG_SS = instructionProvider_.stackSegmentRegister();
        rose_addr_t returnVa = bb->fallthroughVa();
        BaseSemantics::SValuePtr returnExpr = ops->number_(REG_IP.get_nbits(), returnVa);
        BaseSemantics::SValuePtr sp = ops->readRegister(REG_SP);
        BaseSemantics::SValuePtr topOfStack = ops->undefined_(REG_IP.get_nbits());
        topOfStack = ops->readMemory(REG_SS, sp, topOfStack, ops->boolean_(true));
        BaseSemantics::SValuePtr z = ops->equalToZero(ops->add(returnExpr, ops->negate(topOfStack)));
        bool isRetAddrOnTopOfStack = z->is_number() ? (z->get_number()!=0) : false;
        if (!isRetAddrOnTopOfStack)
            return bb->cacheIsFunctionCall(false);
        
        // If the only successor is also the fall-through address then this isn't a function call.  This case handles code that
        // obtains the code address in position independent code. For example, x86 "A: CALL B; B: POP EAX" where A and B are
        // consecutive instruction addresses.
        BasicBlock::Successors successors = bblockSuccessors(bb);
        if (1==successors.size() && successors[0].expr()->is_number() && successors[0].expr()->get_number()==returnVa)
            return bb->cacheIsFunctionCall(false);

        // This appears to be a function call
        return bb->cacheIsFunctionCall(true);
    }

    // We don't have semantics, so delegate to the SgAsmInstruction subclass (which might try some other semantics).
    return bb->cacheIsFunctionCall(lastInsn->is_function_call(bb->instructions(), NULL, NULL));
}

SgAsmInstruction *
Partitioner::discoverInstruction(rose_addr_t startVa) {
    return instructionProvider_[startVa];
}

void
Partitioner::bblockInserted(const ControlFlowGraph::VertexNodeIterator &newVertex) {
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
    if (debug) {
        rose_addr_t va = newVertex->value().address();
        BasicBlock::Ptr bb = newVertex->value().bblock();
        debug <<"inserted basic-block" <<(bb?"":" placeholder") <<" at " <<addrToString(va) <<"\n";
        if (bb) {
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                debug <<"  + " <<unparseInstructionWithAddress(insn) <<"\n";
        }
    }

    checkConsistency();
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::InsertionArgs(this, newVertex));
    checkConsistency();
}

void
Partitioner::bblockErased(const BasicBlock::Ptr &removedBlock) {
    using namespace StringUtility;
    ASSERT_not_null(removedBlock);
    Stream debug(mlog[DEBUG]);
    if (debug) {
        debug <<"removed basic-block at " <<addrToString(removedBlock->address()) <<"\n";
        BOOST_FOREACH (SgAsmInstruction *insn, removedBlock->instructions())
            debug <<"  - " <<unparseInstructionWithAddress(insn) <<"\n";
    }

    checkConsistency();
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::ErasureArgs(this, removedBlock));
    checkConsistency();
}

void
Partitioner::checkConsistency() const {
    static const bool extraDebuggingOutput = false;
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
#ifndef NDEBUG
    if (extraDebuggingOutput)
        debug <<"checking partitioner consistency...\n";
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASICBLOCK) {
            if (extraDebuggingOutput && debug) {
                debug <<"  basic block" <<(vertex.value().bblock()?"":" placeholder")
                      <<" " <<addrToString(vertex.value().address()) <<"\n";
                debug <<"    outgoing edges:";
                BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.outEdges()) {
                    const ControlFlowGraph::VertexNode &target = *edge.target();
                    switch (target.value().type()) {
                        case V_BASICBLOCK:
                            debug <<" " <<addrToString(target.value().address());
                            break;
                        case V_INDETERMINATE:
                            debug <<" indeterminate";
                            break;
                        case V_NONEXISTING:
                            debug <<" nonexisting";
                            break;
                        case V_UNDISCOVERED:
                            debug <<" undiscovered";
                            break;
                    }
                }
                debug <<"\n";
            }

            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                ASSERT_require(bb->address() == vertex.value().address());
                if (bb->isEmpty()) {
                    // Non-existing basic block
                    ASSERT_require2(vertex.nOutEdges()==1,
                                    "nonexisting block " + addrToString(bb->address()) + " must have one outgoing edge");
                    ControlFlowGraph::ConstEdgeNodeIterator edge = vertex.outEdges().begin();
                    ASSERT_require2(edge->target() == nonexistingVertex_,
                                    "nonexisting block " + addrToString(bb->address()) + " edges must go to a special vertex");
                } else {
                    // Existing basic block
                    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                        BasicBlock::Ptr bb2 = addrUsageMap_.instructionExists(insn);
                        ASSERT_not_null2(bb2,
                                         "instruction " + addrToString(insn->get_address()) + " in block " +
                                         addrToString(bb->address()) + " must be present in the AUM");
                        ASSERT_require2(bb2 == bb,
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must belong to correct basic block");
                        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
                        InsnBlockPairs ibpairs = addrUsageMap_.spanning(insnInterval);
                        ASSERT_require2(ibpairs.instructionExists(insn),
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must span its own address interval in the AUM");
                    }
                }
            } else {
                // Basic block placeholder
                ASSERT_require2(vertex.nOutEdges() == 1,
                                "placeholder " + addrToString(vertex.value().address()) + " must have exactly one outgoing edge");
                ControlFlowGraph::ConstEdgeNodeIterator edge = vertex.outEdges().begin();
                ASSERT_require2(edge->target() == undiscoveredVertex_,
                                "placeholder " + addrToString(vertex.value().address()) + " edge must go to a special vertex");
            }
        } else {
            // Special vertices
            ASSERT_require2(vertex.nOutEdges()==0,
                            "special vertices must have no outgoing edges");
        }
    }
#endif
}

std::string
Partitioner::vertexName(const ControlFlowGraph::VertexNode &vertex) const {
    switch (vertex.value().type()) {
        case V_BASICBLOCK: {
            std::string retval = StringUtility::addrToString(vertex.value().address());
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (bb->isEmpty())
                    retval += "[X]";                    // non-existing
            } else {
                retval += "[P]";                        // placeholder
            }
            return retval;
        }
        case V_INDETERMINATE:
            return "indeterminate";
        case V_NONEXISTING:
            return "non-existing";
        case V_UNDISCOVERED:
            return "undiscovered";
    }
}

std::string
Partitioner::edgeNameDst(const ControlFlowGraph::EdgeNode &edge) const {
    std::string retval;
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FCALL:
            retval += "[fcall]";
            break;
        case E_FRET:
            retval += "[fret]";
            break;
    }
    return retval + vertexName(*edge.target());
}

std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::EdgeNode &edge) const {
    const ControlFlowGraph::VertexNode &source = *edge.source();
    ASSERT_require(source.value().type()==V_BASICBLOCK);
    ASSERT_not_null(source.value().bblock());
    ASSERT_forbid(source.value().bblock()->isEmpty());
    std::string retval = StringUtility::addrToString(source.value().bblock()->instructions().back()->get_address());
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FCALL:
            retval += "[fcall]";
            break;
        case E_FRET:
            retval += "[fret]";
            break;
    }
    return retval;
}

static bool
sortVerticesByAddress(const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &a,
                    const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &b) {
    return a->value().address() < b->value().address();
}

void
Partitioner::dumpCFG(std::ostream &out, const std::string &prefix) const {
    // Sort the vertices according to basic block starting address.
    std::vector<ControlFlowGraph::ConstVertexNodeIterator> sortedVertices;
    for (ControlFlowGraph::ConstVertexNodeIterator vi=cfg_.vertices().begin(); vi!=cfg_.vertices().end(); ++vi) {
        if (vi->value().type() == V_BASICBLOCK)
            sortedVertices.push_back(vi);
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), sortVerticesByAddress);
    
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexNodeIterator &vertex, sortedVertices) {
        out <<prefix <<"basic block " <<vertexName(*vertex) <<"\n";
        out <<prefix <<"  predecessors:";
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex->inEdges())
            out <<" " <<edgeNameSrc(edge);
        out <<"\n";
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                out <<prefix <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
        }
        out <<prefix <<"  successors:";
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex->outEdges()) {
            out <<" " <<edgeNameDst(edge);
        }
        out <<"\n";
    }
}

SgAsmBlock*
Partitioner::toAst() {
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASICBLOCK) {
            std::cerr <<StringUtility::addrToString(vertex.value().address()) <<": basic block:\n";
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    std::cerr <<"  " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        }
    }
    return NULL;                                        // FIXME[Robb P. Matzke 2014-08-02]
}

} // namespace
} // namespace
} // namespace
