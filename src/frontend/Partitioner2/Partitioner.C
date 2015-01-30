#include "sage3basic.h"
#include <Partitioner2/Partitioner.h>

#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/Exception.h>
#include <Partitioner2/Utility.h>

#include "AsmUnparser_compat.h"
#include "SymbolicSemantics2.h"
#include "DispatcherM68k.h"
#include "Diagnostics.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stack.h>

using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

// class method
void Partitioner::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog.initStreams(Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
Partitioner::init(Disassembler *disassembler, const MemoryMap &map) {
    instructionProvider_ = InstructionProvider::instance(disassembler, map);
    undiscoveredVertex_ = cfg_.insertVertex(CfgVertex(V_UNDISCOVERED));
    indeterminateVertex_ = cfg_.insertVertex(CfgVertex(V_INDETERMINATE));
    nonexistingVertex_ = cfg_.insertVertex(CfgVertex(V_NONEXISTING));
}

void
Partitioner::init(const Partitioner &other) {
    // All graph vertices need to be converted from other.cfg_ to this->cfg_
    BOOST_FOREACH (const VertexIndex::Node &node, other.vertexIndex_.nodes())
        vertexIndex_.insert(node.key(), convertFrom(other, node.value()));
    undiscoveredVertex_ = convertFrom(other, other.undiscoveredVertex_);
    indeterminateVertex_ = convertFrom(other, other.indeterminateVertex_);
    nonexistingVertex_ = convertFrom(other, other.nonexistingVertex_);
}

void
Partitioner::clear() {
    cfg_.clear();
    vertexIndex_.clear();
    aum_.clear();
    functions_.clear();
}

ControlFlowGraph::VertexNodeIterator
Partitioner::convertFrom(const Partitioner &other, ControlFlowGraph::ConstVertexNodeIterator otherIter) {
    if (otherIter==other.cfg_.vertices().end())
        return cfg_.vertices().end();
    ControlFlowGraph::VertexNodeIterator thisIter = cfg_.findVertex(otherIter->id());
    ASSERT_forbid(thisIter == cfg_.vertices().end());
    return thisIter;
}

// Label the progress report and also show some other statistics.  It is okay for this to be slightly expensive since its only
// called when a progress report is actually emitted.
class ProgressBarSuffix {
    const Partitioner *partitioner_;
public:
    ProgressBarSuffix(): partitioner_(NULL) {}
    explicit ProgressBarSuffix(const Partitioner *p): partitioner_(p) {}
    void print(std::ostream &out) const {
        ASSERT_not_null(partitioner_);
        out <<" bytes"                                  // units for progress report
            <<" " <<StringUtility::plural(partitioner_->nPlaceholders(), "blks")
            <<" " <<StringUtility::plural(partitioner_->nFunctions(), "funcs");
    }
};

static std::ostream&
operator<<(std::ostream &out, const ProgressBarSuffix &x) {
    x.print(out);
    return out;
};

void
Partitioner::reportProgress() const {
    // All partitioners share a single progress bar.
    static Sawyer::ProgressBar<size_t, ProgressBarSuffix> *bar = NULL;

    if (0==progressTotal_) {
        BOOST_FOREACH (const MemoryMap::Node &node, memoryMap_.nodes()) {
            if (0 != (node.value().accessibility() & MemoryMap::EXECUTABLE))
                progressTotal_ += node.key().size();
        }
    }
    
    if (!bar)
        bar = new Sawyer::ProgressBar<size_t, ProgressBarSuffix>(progressTotal_, mlog[MARCH], "cfg");

    if (progressTotal_) {
        // If multiple partitioners are sharing the progress bar then also make sure that the lower and upper limits are
        // appropriate for THIS partitioner.  However, changing the limits is a configuration change, which also immediately
        // updates the progress bar (we don't want that, so update only if necessary).
        bar->suffix(ProgressBarSuffix(this));
        if (bar->domain().first!=0 || bar->domain().second!=progressTotal_) {
            bar->value(0, nBytes(), progressTotal_);
        } else {
            bar->value(nBytes());
        }
    }
}

size_t
Partitioner::nInstructions() const {
    size_t nInsns = 0;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock())
                nInsns += bb->nInstructions();
        }
    }
    return nInsns;
}

size_t
Partitioner::nPlaceholders() const {
    ASSERT_require(cfg_.nVertices() >= nSpecialVertices);
    return cfg_.nVertices() - nSpecialVertices;
}

bool
Partitioner::placeholderExists(rose_addr_t startVa) const {
    return vertexIndex_.exists(startVa);
}

BasicBlock::Ptr
Partitioner::erasePlaceholder(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    BasicBlock::Ptr bblock;
    if (placeholder!=cfg_.vertices().end() && placeholder->value().type()==V_BASIC_BLOCK) {
        rose_addr_t startVa = placeholder->value().address();
        if (bblock = placeholder->value().bblock())
            detachBasicBlock(placeholder);              // removes self edges, notifies subclasses of CFG changes
        if (placeholder->nInEdges()!=0) {
            throw PlaceholderError(startVa, "cannot erase placeholder " + StringUtility::addrToString(startVa) +
                                   " that has " + StringUtility::plural(placeholder->nInEdges(), "incoming edges"));
        }
        vertexIndex_.erase(placeholder->value().address());
        cfg_.eraseVertex(placeholder);
        bblockDetached(startVa, BasicBlock::Ptr());     // null bblock indicates placeholder erasure
    }
    return bblock;
}

size_t
Partitioner::nBasicBlocks() const {
    size_t nBasicBlocks = 0;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK && vertex.bblock())
            ++nBasicBlocks;
    }
    return nBasicBlocks;
}

BasicBlock::Ptr
Partitioner::basicBlockExists(rose_addr_t startVa) const {
    ControlFlowGraph::ConstVertexNodeIterator vertex = findPlaceholder(startVa);
    if (vertex!=cfg_.vertices().end())
        return vertex->value().bblock();
    return BasicBlock::Ptr();
}

BasicBlock::Ptr
Partitioner::basicBlockExists(const BasicBlock::Ptr &bblock) const {
    return bblock==NULL ? BasicBlock::Ptr() : basicBlockExists(bblock->address());
}

BaseSemantics::RiscOperatorsPtr
Partitioner::newOperators() const {
    Semantics::RiscOperatorsPtr ops = Semantics::RiscOperators::instance(instructionProvider_->registerDictionary(), solver_);
    Semantics::MemoryState::promote(ops->get_state()->get_memory_state())->memoryMap(&memoryMap_);
    return ops;
}

BaseSemantics::DispatcherPtr
Partitioner::newDispatcher(const BaseSemantics::RiscOperatorsPtr &ops) const {
    ASSERT_not_null(ops);
    if (instructionProvider_->dispatcher() == NULL)
        return BaseSemantics::DispatcherPtr();          // instruction semantics are not implemented for this architecture
    return instructionProvider_->dispatcher()->create(ops);
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    BasicBlock::Ptr bblock;
    if (placeholder != cfg_.vertices().end() && placeholder->value().type()==V_BASIC_BLOCK) {
        bblock = placeholder->value().bblock();
        placeholder->value().nullify();
        adjustPlaceholderEdges(placeholder);
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
            aum_.eraseInstruction(insn);
        BOOST_FOREACH (const DataBlock::Ptr &dblock, bblock->dataBlocks()) {
            if (0==dblock->decrementOwnerCount())
                detachDataBlock(dblock);
        }
        bblock->thaw();
        bblockDetached(bblock->address(), bblock);
    }
    return bblock;
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(const BasicBlock::Ptr &bblock) {
    if (bblock!=NULL) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(bblock->address());
        if (placeholder!=cfg_.vertices().end() && placeholder->value().bblock()==bblock)
            return detachBasicBlock(placeholder);
    }
    return BasicBlock::Ptr();
}

BasicBlock::Ptr
Partitioner::detachBasicBlock(rose_addr_t startVa) {
    return detachBasicBlock(findPlaceholder(startVa));
}

ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder) {
    ASSERT_require(placeholder!=cfg_.vertices().end());
    ASSERT_require2(NULL==placeholder->value().bblock(), "vertex must be strictly a placeholder");
    cfg_.clearOutEdges(placeholder);
    return cfg_.insertEdge(placeholder, undiscoveredVertex_);
}

ControlFlowGraph::EdgeNodeIterator
Partitioner::adjustNonexistingEdges(const ControlFlowGraph::VertexNodeIterator &vertex) {
    ASSERT_require(vertex!=cfg_.vertices().end());
    ASSERT_not_null2(vertex->value().bblock(), "vertex must have been discovered");
    ASSERT_require2(vertex->value().bblock()->isEmpty(), "vertex must be non-existing");
    cfg_.clearOutEdges(vertex);
    return cfg_.insertEdge(vertex, nonexistingVertex_);
}

BasicBlock::Ptr
Partitioner::discoverBasicBlock(const ControlFlowGraph::ConstVertexNodeIterator &placeholder) const {
    ASSERT_require2(placeholder != cfg_.vertices().end(), "invalid basic block placeholder");
    BasicBlock::Ptr bb = placeholder->value().bblock();
    return bb!=NULL ? bb : discoverBasicBlockInternal(placeholder->value().address());
}

BasicBlock::Ptr
Partitioner::discoverBasicBlock(rose_addr_t startVa) const {
    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(startVa);
    return placeholder==cfg_.vertices().end() ? discoverBasicBlockInternal(startVa) : discoverBasicBlock(placeholder);
}

BasicBlock::Ptr
Partitioner::discoverBasicBlockInternal(rose_addr_t startVa) const {
    // If the first instruction of this basic block already exists (in the middle of) some other basic block then the other
    // basic block is called a "conflicting block".  This only applies for the first instruction of this block, but is used in
    // the termination conditions below.
    AddressUser conflict;
    if (instructionExists(startVa).assignTo(conflict))
        ASSERT_forbid(conflict.insn()->get_address() == conflict.basicBlock()->address());// handled in discoverBasicBlock

    // Keep adding instructions until we reach a termination condition.  The termination conditions are enumerated in detail in
    // the doxygen documentation for this function. READ IT AND KEEP IT UP TO DATE!!!
    BasicBlock::Ptr retval = BasicBlock::instance(startVa, this);
    rose_addr_t va = startVa;
    while (1) {
        SgAsmInstruction *insn = discoverInstruction(va);
        if (insn==NULL)                                                 // case: no instruction available
            goto done;
        retval->append(insn);
        if (insn->isUnknown())                                          // case: "unknown" instruction
            goto done;

        // Give user chance to adjust basic block successors and/or pre-compute cached analysis results
        BasicBlockCallback::Results userResult;
        basicBlockCallbacks_.apply(true, BasicBlockCallback::Args(this, retval, userResult));

        BOOST_FOREACH (rose_addr_t successorVa, basicBlockConcreteSuccessors(retval)) {
            if (successorVa!=startVa && retval->instructionExists(successorVa)) { // case: successor is inside our own block
                retval->pop();
                goto done;
            }
        }

        if (userResult.terminate == BasicBlockCallback::TERMINATE_NOW)  // case: user wants us to terminate block here
            goto done;
        if (userResult.terminate == BasicBlockCallback::TERMINATE_PRIOR) {
            retval->pop();                                              // case: user wants to terminate at prior insn
            goto done;
        }
        
        if (basicBlockIsFunctionCall(retval))                           // case: bb looks like a function call
            goto done;
        BasicBlock::Successors successors = basicBlockSuccessors(retval);

        if (successors.size()!=1)                                       // case: not exactly one successor
            goto done;
        SValuePtr successorExpr = successors.front().expr();

        if (!successorExpr->is_number())                                // case: successor is indeterminate
            goto done;
        rose_addr_t successorVa = successorExpr->get_number();

        if (successorVa == startVa)                                     // case: successor is our own basic block
            goto done;

        if (findPlaceholder(successorVa)!=cfg_.vertices().end())        // case: successor is an existing block
            goto done;

        AddressUser addressUser;
        if (instructionExists(successorVa).assignTo(addressUser)) {     // case: successor is inside an existing block
            if (addressUser.basicBlock() != conflict.basicBlock())
                goto done;
        }

        va = successorVa;
    }
done:
    retval->freeze();
    return retval;
}

ControlFlowGraph::VertexNodeIterator
Partitioner::truncateBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, SgAsmInstruction *insn) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_not_null(insn);
    BasicBlock::Ptr bblock = placeholder->value().bblock();
    if (bblock==NULL) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " has no basic block for truncation");
    }
    if (insn==bblock->instructions().front())
        throw BasicBlockError(bblock, basicBlockName(bblock) + " cannot be truncated at its initial instruction");
    if (!bblock->instructionExists(insn)) {
        throw BasicBlockError(bblock, basicBlockName(bblock) +
                              " does not contain instruction \"" + unparseInstructionWithAddress(insn) + "\""
                              " for truncation");
    }

    // For now we do a niave approach; this could be faster [Robb P. Matzke 2014-08-02]
    detachBasicBlock(placeholder);                      // throw away the original block
    ControlFlowGraph::VertexNodeIterator newPlaceholder = insertPlaceholder(insn->get_address());
    BasicBlock::Ptr newBlock = discoverBasicBlock(placeholder); // rediscover original block, but terminate at newPlaceholder
    attachBasicBlock(placeholder, newBlock);            // insert new block at original placeholder and insert successor edge
    return newPlaceholder;
}

ControlFlowGraph::VertexNodeIterator
Partitioner::insertPlaceholder(rose_addr_t startVa) {
    ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(startVa);
    if (placeholder == cfg_.vertices().end()) {
        AddressUser addressUser;
        if (instructionExists(startVa).assignTo(addressUser)) {
            ControlFlowGraph::VertexNodeIterator conflictBlock = findPlaceholder(addressUser.basicBlock()->address());
            placeholder = truncateBasicBlock(conflictBlock, addressUser.insn());
            ASSERT_require(placeholder->value().address() == startVa);
        } else {
            placeholder = cfg_.insertVertex(CfgVertex(startVa));
            vertexIndex_.insert(startVa, placeholder);
            adjustPlaceholderEdges(placeholder);
            bblockAttached(placeholder);
        }
    }
    return placeholder;
}

void
Partitioner::attachBasicBlock(const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);
    ControlFlowGraph::VertexNodeIterator placeholder = insertPlaceholder(bblock->address()); // insert or find existing
    attachBasicBlock(placeholder, bblock);
}

void
Partitioner::attachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, const BasicBlock::Ptr &bblock) {
    ASSERT_require(placeholder != cfg_.vertices().end());
    ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
    ASSERT_not_null(bblock);

    if (placeholder->value().address() != bblock->address()) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " cannot hold " + basicBlockName(bblock));
    }
    if (placeholder->value().bblock() == bblock)
        return;                                         // nothing to do since basic block is already in the CFG
    if (placeholder->value().bblock() != NULL) {
        throw PlaceholderError(placeholder->value().address(),
                               "placeholder " + StringUtility::addrToString(placeholder->value().address()) +
                               " already holds a different basic block");
    }

    bblock->freeze();
    bool isFunctionCall = basicBlockIsFunctionCall(bblock);
    bool isFunctionReturn = basicBlockIsFunctionReturn(bblock);

    // Make sure placeholders exist for the concrete successors
    bool hadIndeterminate = false;
    typedef std::pair<ControlFlowGraph::VertexNodeIterator, CfgEdge> VertexEdgePair;
    std::vector<VertexEdgePair> successors;
    BOOST_FOREACH (const BasicBlock::Successor &successor, basicBlockSuccessors(bblock)) {
        EdgeType edgeType = E_NORMAL;
        if (isFunctionCall) {
            edgeType = E_FUNCTION_CALL;
        } else if (isFunctionReturn) {
            edgeType = E_FUNCTION_RETURN;
        }
        CfgEdge edge(edgeType);
        if (successor.expr()->is_number()) {
            successors.push_back(VertexEdgePair(insertPlaceholder(successor.expr()->get_number()), edge));
        } else if (!hadIndeterminate) {
            successors.push_back(VertexEdgePair(indeterminateVertex_, edge));
            hadIndeterminate = true;
        }
    }

    // Function calls get an additional return edge because we assume they return to the fall-through address. This edge
    // doesn't exist explicitly in the specimen, but we must add it to the partitioner's CFG because if the called function
    // does not exist (e.g., it's in a dynamically loaded library that is not present) then this edge is the only way to
    // indicate that the called function eventually returns.
    if (isFunctionCall)
        successors.push_back(VertexEdgePair(insertPlaceholder(bblock->fallthroughVa()), CfgEdge(E_CALL_RETURN)));

    // Make CFG edges
    cfg_.clearOutEdges(placeholder);
    BOOST_FOREACH (const VertexEdgePair &pair, successors)
        cfg_.insertEdge(placeholder, pair.first, pair.second);

    // Insert the basic block instructions
    placeholder->value().bblock(bblock);
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        aum_.insertInstruction(insn, bblock);
    }
    if (bblock->isEmpty())
        adjustNonexistingEdges(placeholder);

    // Insert the basic block static data
    BOOST_FOREACH (const DataBlock::Ptr &dblock, bblock->dataBlocks()) {
        attachDataBlock(dblock);
        dblock->incrementOwnerCount();
    }

    bblockAttached(placeholder);
}

AddressIntervalSet
Partitioner::basicBlockInstructionExtent(const BasicBlock::Ptr &bblock) const {
    AddressIntervalSet retval;
    if (bblock!=NULL) {
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
            retval.insert(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
    }
    return retval;
}

AddressIntervalSet
Partitioner::basicBlockDataExtent(const BasicBlock::Ptr &bblock) const {
    AddressIntervalSet retval;
    if (bblock!=NULL) {
        BOOST_FOREACH (const DataBlock::Ptr &dblock, bblock->dataBlocks())
            retval.insert(dblock->extent());
    }
    return retval;
}

BasicBlock::Successors
Partitioner::basicBlockSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    BasicBlock::Successors successors;

    if (bb->isEmpty() || bb->successors().getOptional().assignTo(successors))
        return successors;

    SgAsmInstruction *lastInsn = bb->instructions().back();
    RegisterDescriptor REG_IP = instructionProvider_->instructionPointerRegister();

    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Use our own semantics if we have them.
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        std::vector<Semantics::SValuePtr> worklist(1, Semantics::SValue::promote(ops->readRegister(REG_IP)));
        while (!worklist.empty()) {
            Semantics::SValuePtr pc = worklist.back();
            worklist.pop_back();

            // Special handling for if-then-else expressions
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

    } else {
        // We don't have semantics, so delegate to the SgAsmInstruction subclass (which might try some other semantics).
        bool complete = true;
#if 0 // [Robb P. Matzke 2014-08-16]
        // Look at the entire basic block to try to figure out the successors.  We already did something very similar above, so
        // if our try failed then this one probably will too.  In fact, this one will be even slower because it must reprocess
        // the entire basic block each time it's called because it is stateless, whereas ours above only needed to process each
        // instruction as it was appended to the block.
        std::set<rose_addr_t> successorVas = lastInsn->getSuccessors(bb->instructions(), &complete, &memoryMap_);
#else
        // Look only at the final instruction of the basic block.  This is probably quite fast compared to looking at a whole
        // basic block.
        std::set<rose_addr_t> successorVas = lastInsn->getSuccessors(&complete);
#endif
        BaseSemantics::RiscOperatorsPtr ops = newOperators();
        BOOST_FOREACH (rose_addr_t va, successorVas)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->number_(REG_IP.get_nbits(), va))));
        if (!complete)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::promote(ops->undefined_(REG_IP.get_nbits()))));
    }

    // We don't want parallel edges in the CFG, so remove duplicates.
    std::sort(successors.begin(), successors.end(), sortByExpression);
    for (size_t i=1; i<successors.size(); /*void*/) {
        if (successors[i-1].expr()->get_expression()->equivalent_to(successors[i].expr()->get_expression())) {
            ASSERT_require(successors[i-1].type() == successors[i].type());
            successors.erase(successors.begin()+i);
        } else {
            ++i;
        }
    }
    bb->successors() = successors;
    return successors;
}

std::set<rose_addr_t>
Partitioner::basicBlockGhostSuccessors(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    std::set<rose_addr_t> ghosts;

    if (bb->isEmpty() || bb->ghostSuccessors().getOptional().assignTo(ghosts))
        return ghosts;
    
    std::set<rose_addr_t> insnVas;
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
        insnVas.insert(insn->get_address());
    
    const BasicBlock::Successors &successors = basicBlockSuccessors(bb);
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
        bool complete = true;
        BOOST_FOREACH (rose_addr_t naiveVa, insn->getSuccessors(&complete)) {
            if (insnVas.find(naiveVa)==insnVas.end()) {
                bool found = false;
                BOOST_FOREACH (const BasicBlock::Successor &successor, successors) {
                    if (successor.expr()->is_number() && successor.expr()->get_number()==naiveVa) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    ghosts.insert(naiveVa);
            }
        }
    }
    bb->ghostSuccessors() = ghosts;
    return ghosts;
}

std::vector<rose_addr_t>
Partitioner::basicBlockConcreteSuccessors(const BasicBlock::Ptr &bb, bool *isComplete/*=NULL*/) const {
    ASSERT_not_null(bb);
    std::vector<rose_addr_t> retval;
    if (isComplete)
        *isComplete = true;
    BOOST_FOREACH (const BasicBlock::Successor &successor, basicBlockSuccessors(bb)) {
        if (successor.expr()->is_number()) {
            retval.push_back(successor.expr()->get_number());
        } else if (isComplete) {
            *isComplete = false;
        }
    }
    return retval;
}

bool
Partitioner::basicBlockIsFunctionCall(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->isFunctionCall().getOptional().assignTo(retval))
        return retval;                                  // already cached

    SgAsmInstruction *lastInsn = bb->instructions().back();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Is the block fall-through address equal to the value on the top of the stack?
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        const RegisterDescriptor REG_IP = instructionProvider_->instructionPointerRegister();
        const RegisterDescriptor REG_SP = instructionProvider_->stackPointerRegister();
        const RegisterDescriptor REG_SS = instructionProvider_->stackSegmentRegister();
        rose_addr_t returnVa = bb->fallthroughVa();
        BaseSemantics::SValuePtr returnExpr = ops->number_(REG_IP.get_nbits(), returnVa);
        BaseSemantics::SValuePtr sp = ops->readRegister(REG_SP);
        BaseSemantics::SValuePtr topOfStack = ops->undefined_(REG_IP.get_nbits());
        topOfStack = ops->readMemory(REG_SS, sp, topOfStack, ops->boolean_(true));
        BaseSemantics::SValuePtr z = ops->equalToZero(ops->add(returnExpr, ops->negate(topOfStack)));
        bool isRetAddrOnTopOfStack = z->is_number() ? (z->get_number()!=0) : false;
        if (!isRetAddrOnTopOfStack) {
            bb->isFunctionCall() = false;
            return false;
        }
        
        // If the only successor is also the fall-through address then this isn't a function call.  This case handles code that
        // obtains the code address in position independent code. For example, x86 "A: CALL B; B: POP EAX" where A and B are
        // consecutive instruction addresses.
        BasicBlock::Successors successors = basicBlockSuccessors(bb);
        if (1==successors.size() && successors[0].expr()->is_number() && successors[0].expr()->get_number()==returnVa) {
            bb->isFunctionCall() = false;
            return false;
        }

        // This appears to be a function call
        bb->isFunctionCall() = true;
        return true;
    }

    // We don't have semantics, so delegate to the SgAsmInstruction subclass.
    retval = lastInsn->isFunctionCallFast(bb->instructions(), NULL, NULL);
    bb->isFunctionCall() = retval;
    return retval;
}

bool
Partitioner::basicBlockIsFunctionReturn(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->isFunctionReturn().getOptional().assignTo(retval))
        return retval;                                  // already cached

    SgAsmInstruction *lastInsn = bb->instructions().back();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // This is a function return if the instruction pointer has the same value as the memory for one past the end of the
        // stack pointer.  The assumption is that a function return pops the return-to address off the top of the stack and
        // unconditionally branches to it.  It may pop other things from the stack as well.  Assuming stacks grow down. This
        // will not work for callee-cleans-up returns where the callee also pops off some arguments that were pushed before
        // the call.
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        const RegisterDescriptor REG_IP = instructionProvider_->instructionPointerRegister();
        const RegisterDescriptor REG_SP = instructionProvider_->stackPointerRegister();
        const RegisterDescriptor REG_SS = instructionProvider_->stackSegmentRegister();
        BaseSemantics::SValuePtr retAddrPtr = ops->add(ops->readRegister(REG_SP),
                                                       ops->negate(ops->number_(REG_IP.get_nbits(), REG_IP.get_nbits()/8)));
        BaseSemantics::SValuePtr retAddr = ops->undefined_(REG_IP.get_nbits());
        retAddr = ops->readMemory(REG_SS, retAddrPtr, retAddr, ops->boolean_(true));
        BaseSemantics::SValuePtr isEqual = ops->equalToZero(ops->add(retAddr, ops->negate(ops->readRegister(REG_IP))));
        retval = isEqual->is_number() ? (isEqual->get_number() != 0) : false;
        bb->isFunctionReturn() = retval;
        return retval;
    }

    // No semantics, so delegate to SgAsmInstruction subclasses
    retval = lastInsn->isFunctionReturnFast(bb->instructions());
    bb->isFunctionReturn() = retval;
    return retval;
}

BaseSemantics::SValuePtr
Partitioner::basicBlockStackDelta(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);

    BaseSemantics::SValuePtr delta;
    if (bb->stackDelta().getOptional().assignTo(delta))
        return delta;                                   // already cached

    if (bb->finalState() != NULL) {
        ASSERT_not_null(bb->initialState());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
        ASSERT_not_null(ops);
        RegisterDescriptor REG_SP = instructionProvider_->stackPointerRegister();
        BaseSemantics::SValuePtr sp0 = bb->initialState()->readRegister(REG_SP, ops.get());
        BaseSemantics::SValuePtr spN = bb->finalState()->readRegister(REG_SP, ops.get());
        delta = ops->add(spN, ops->negate(sp0));
    }
    bb->stackDelta() = delta;
    return delta;
}

std::vector<SgAsmInstruction*>
Partitioner::instructionsOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectBasicBlocks).instructions();
}

std::vector<BasicBlock::Ptr>
Partitioner::basicBlocks() const {
    std::vector<BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const ControlFlowGraph::VertexValue &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bblock = vertex.bblock())
                bblocks.push_back(bblock);
        }
    }
    std::sort(bblocks.begin(), bblocks.end(), sortBasicBlocksByAddress);
    return bblocks;
}

std::vector<BasicBlock::Ptr>
Partitioner::basicBlocksOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectBasicBlocks).basicBlocks();
}

BasicBlock::Ptr
Partitioner::basicBlockContainingInstruction(rose_addr_t insnVa) const {
    std::vector<BasicBlock::Ptr> bblocks = basicBlocksOverlapping(insnVa);
    BOOST_FOREACH (const BasicBlock::Ptr &bblock, bblocks) {
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
            if (insn->get_address() == insnVa)
                return bblock;
        }
    }
    return BasicBlock::Ptr();
}

SgAsmInstruction *
Partitioner::discoverInstruction(rose_addr_t startVa) const {
    return (*instructionProvider_)[startVa];
}

size_t
Partitioner::nDataBlocks() const {
    return dataBlocksOverlapping(aum_.hull()).size();
}

bool
Partitioner::dataBlockExists(const DataBlock::Ptr &dblock) const {
    if (dblock==NULL)
        return false;
    if (dblock->nAttachedOwners()>0)
        return true;
    BOOST_FOREACH (const DataBlock::Ptr &exists, dataBlocksSpanning(dblock->extent())) {
        if (exists==dblock)
            return true;
    }
    return false;
}

void
Partitioner::attachDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_not_null(dblock);
    if (!dataBlockExists(dblock)) {
        ASSERT_require(0==dblock->nAttachedOwners());
        aum_.insertDataBlock(OwnedDataBlock(dblock));
        dblock->freeze();
    }
}

DataBlock::Ptr
Partitioner::detachDataBlock(const DataBlock::Ptr &dblock) {
    if (dblock!=NULL) {
        if (dblock->nAttachedOwners()) {
            throw DataBlockError(dblock, dataBlockName(dblock) + " cannot be detached because it has " +
                                 StringUtility::plural(dblock->nAttachedOwners(), "basic block and/or function owners"));
        }
        aum_.eraseDataBlock(dblock);
    }
    return dblock;
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocksOverlapping(const AddressInterval &interval) const {
    return aum_.overlapping(interval, AddressUsers::selectDataBlocks).dataBlocks();
}

std::vector<DataBlock::Ptr>
Partitioner::dataBlocksSpanning(const AddressInterval &interval) const {
    return aum_.spanning(interval, AddressUsers::selectDataBlocks).dataBlocks();
}

AddressInterval
Partitioner::dataBlockExtent(const DataBlock::Ptr &dblock) const {
    AddressInterval extent;
    if (dblock!=NULL)
        extent = dblock->extent();
    return extent;
}

DataBlock::Ptr
Partitioner::findBestDataBlock(const AddressInterval &interval) const {
    DataBlock::Ptr existing;
    if (!interval.isEmpty()) {
        std::vector<DataBlock::Ptr> found = dataBlocksSpanning(interval);
        BOOST_REVERSE_FOREACH (const DataBlock::Ptr &dblock, found) { // choose the smallest one (highest address if tied)
            if (existing==NULL || dblock->size() < existing->size())
                existing = dblock;
        }
        if (existing==NULL) {
            found = dataBlocksOverlapping(AddressInterval(interval.least()));
            BOOST_REVERSE_FOREACH (const DataBlock::Ptr &dblock, found) { // choose the largest one (highest address if tied)
                if (existing==NULL || dblock->size() > existing->size())
                    existing = dblock;
            }
        }
    }
    return existing;
}

// FIXME[Robb P. Matzke 2014-08-12]: nBytes to be replaced by a data type
DataBlock::Ptr
Partitioner::attachFunctionDataBlock(const Function::Ptr &function, rose_addr_t startVa, size_t nBytes) {
    ASSERT_not_null(function);
    ASSERT_require(nBytes>0);
    AddressInterval needInterval = AddressInterval::baseSize(startVa, nBytes);
    DataBlock::Ptr dblock = findBestDataBlock(needInterval);
    if (dblock==NULL) {
        // Create a new data block since there is none at this location.
        dblock = DataBlock::instance(startVa, nBytes);
    } else if (!dblock->extent().isContaining(needInterval)) {
        // A data block exists in the CFG/AUM but it doesn't contain everything we want.  We can extend the extent of the
        // existing data block.  FIXME[Robb P. Matzke 2014-08-20]: This will eventually merge data types.
        OwnedDataBlock odb = aum_.dataBlockExists(dblock);
        ASSERT_require(odb.isValid());                  // we know it exists in the AUM because of findBestDataBlock above
        aum_.eraseDataBlock(dblock);                    // temporarily erase the data block from the AUM
        dblock->thaw();                                 // needs to be thawed and refrozen to change the size
        dblock->size(startVa+nBytes - dblock->address());
        dblock->freeze();
        aum_.insertDataBlock(odb);                      // insert it back into the AUM w/new size
    }
    attachFunctionDataBlock(function, dblock);
    return dblock;
}

void
Partitioner::attachFunctionDataBlock(const Function::Ptr &function, const DataBlock::Ptr &dblock) {
    ASSERT_not_null(function);
    ASSERT_not_null(dblock);

    if (functionExists(function)) {
        // The function is in the CFG/AUM, so make sure its data block is also in the CFG/AUM.  In any case, we need to add the
        // function as a data block owner.
        OwnedDataBlock odb = aum_.dataBlockExists(dblock);
        if (odb.isValid()) {
            aum_.eraseDataBlock(dblock);
            odb.insertOwner(function);                  // no-op if function is already an owner
        } else {
            dblock->freeze();
            odb = OwnedDataBlock(dblock, function);
        }
        aum_.insertDataBlock(odb);

        // Add the data block to the function.
        function->thaw();
        if (function->insertDataBlock(dblock))          // false if dblock is already in the function
            dblock->incrementOwnerCount();
        function->freeze();
    } else {
        function->insertDataBlock(dblock);
    }
}

Function::Ptr
Partitioner::findFunctionOwningBasicBlock(rose_addr_t bblockVa) const {
    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(bblockVa);
    return placeholder != cfg_.vertices().end() ? placeholder->value().function() : Function::Ptr();
}

Function::Ptr
Partitioner::findFunctionOwningBasicBlock(const BasicBlock::Ptr &bblock) const {
    return bblock==NULL ? Function::Ptr() : findFunctionOwningBasicBlock(bblock->address());
}

std::vector<Function::Ptr>
Partitioner::findFunctionsOwningBasicBlocks(const std::vector<rose_addr_t> &bblockVas) const {
    typedef std::vector<Function::Ptr> Functions;
    Functions functions;
    BOOST_FOREACH (rose_addr_t blockVa, bblockVas) {
        if (Function::Ptr function = findFunctionOwningBasicBlock(blockVa)) {
            Functions::iterator lb = std::lower_bound(functions.begin(), functions.end(), function, sortFunctionsByAddress);
            if (lb==functions.end() || (*lb)->address()!=function->address())
                functions.insert(lb, function);
        }
    }
    return functions;
}

std::vector<Function::Ptr>
Partitioner::findFunctionsOwningBasicBlocks(const std::vector<BasicBlock::Ptr> &bblocks) const {
    std::vector<rose_addr_t> bblockVas;
    BOOST_FOREACH (const BasicBlock::Ptr &bblock, bblocks) {
        if (bblock!=NULL)
            bblockVas.push_back(bblock->address());
    }
    return findFunctionsOwningBasicBlocks(bblockVas);
}

// We have a number of choices for the algorithm:
//  (1) iterate over all functions, compute each function's extent and compare it to the interval.  Probably not too fast since
//      most functions would probably be excluded (intervals are often small, even single addresses), and also not fast because
//      it computes the full function extent even when only part of it could cause the function to be selected.
//  (2) iterate over all functions, descending into basic blocks and data blocks.  This is faster because we don't have to
//      compute a full function extent and we can short circuit once we find an overlap.  But it requires looking up each basic
//      block by address, which will be O(n log(n)) in total.
//  (3) Use the fact that the interval is probably small and therefore the list of basic blocks and data blocks that overlap
//      with it is small, and that the list can be returned quite quickly from the AUM.  For each instruction and data block
//      returned by the AUM, look at its function ownership list and merge it into the return value.  This is the approach we
//      take here.
std::vector<Function::Ptr>
Partitioner::functionsOverlapping(const AddressInterval &interval) const {
    std::vector<Function::Ptr> functions;

    AddressUsers overlapping = aum_.overlapping(interval);
    BOOST_FOREACH (const AddressUser &user, overlapping.addressUsers()) {
        if (BasicBlock::Ptr bb = user.basicBlock()) {
            ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(bb->address());
            ASSERT_require(placeholder != cfg_.vertices().end());
            ASSERT_require(placeholder->value().bblock()==bb);
            if (Function::Ptr function = placeholder->value().function())
                insertUnique(functions, function, sortFunctionsByAddress);
        } else {
            ASSERT_not_null(user.dataBlock());
            BOOST_FOREACH (const Function::Ptr &function, user.dataBlockOwnership().owningFunctions())
                insertUnique(functions, function, sortFunctionsByAddress);
        }
    }
    return functions;
}

AddressIntervalSet
Partitioner::functionExtent(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    AddressIntervalSet retval;

    // Basic blocks and their data
    BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(bblockVa);
        if (placeholder != cfg_.vertices().end()) {
            if (BasicBlock::Ptr bblock = placeholder->value().bblock()) {
                retval.insertMultiple(basicBlockInstructionExtent(bblock));
                retval.insertMultiple(basicBlockDataExtent(bblock));
            } else {
                retval.insert(AddressInterval(bblockVa));// all we know is the placeholder address
            }
        }
    }

    // Data blocks owned by the function
    BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks())
        retval.insert(dataBlockExtent(dblock));

    return retval;
}

void
Partitioner::bblockAttached(const ControlFlowGraph::VertexNodeIterator &newVertex) {
    ASSERT_require(newVertex!=cfg_.vertices().end());
    ASSERT_require(newVertex->value().type() == V_BASIC_BLOCK);
    if (isReportingProgress_)
        reportProgress();
    rose_addr_t startVa = newVertex->value().address();
    BasicBlock::Ptr bblock = newVertex->value().bblock();

    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"attached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"attached basic block:\n";
                BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                    debug <<"  + " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        } else {
            debug <<"inserted basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::AttachedBasicBlock(this, startVa, bblock));
#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
}

void
Partitioner::bblockDetached(rose_addr_t startVa, const BasicBlock::Ptr &bblock) {
    Stream debug(mlog[DEBUG]);
    if (debug) {
        if (bblock) {
            if (bblock->isEmpty()) {
                debug <<"detached empty basic block at " <<StringUtility::addrToString(startVa) <<"\n";
            } else {
                debug <<"detached basic block:\n";
                BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                    debug <<"  - " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        } else {
            debug <<"erased basic block placeholder at " <<StringUtility::addrToString(startVa) <<"\n";
        }
    }

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::DetachedBasicBlock(this, startVa, bblock));
#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
}

void
Partitioner::checkConsistency() const {
#ifndef NDEBUG
    static const bool extraDebuggingOutput = false;
    using namespace StringUtility;
    Stream debug(mlog[DEBUG]);
    if (extraDebuggingOutput)
        debug <<"checking partitioner consistency...\n";
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASIC_BLOCK) {
            if (extraDebuggingOutput && debug) {
                debug <<"  basic block" <<(vertex.value().bblock()?"":" placeholder")
                      <<" " <<addrToString(vertex.value().address()) <<"\n";
                debug <<"    outgoing edges:";
                BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.outEdges()) {
                    const ControlFlowGraph::VertexNode &target = *edge.target();
                    switch (target.value().type()) {
                        case V_BASIC_BLOCK:
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
                ASSERT_require(bb->isFrozen());
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
                        BasicBlock::Ptr bb2 = aum_.instructionExists(insn);
                        ASSERT_not_null2(bb2,
                                         "instruction " + addrToString(insn->get_address()) + " in block " +
                                         addrToString(bb->address()) + " must be present in the AUM");
                        ASSERT_require2(bb2 == bb,
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must belong to correct basic block");
                        AddressInterval insnInterval = AddressInterval::baseSize(insn->get_address(), insn->get_size());
                        AddressUsers addressUsers = aum_.spanning(insnInterval);
                        ASSERT_require2(addressUsers.instructionExists(insn),
                                        "instruction " + addrToString(insn->get_address()) + " in block " +
                                        addrToString(bb->address()) + " must span its own address interval in the AUM");
                    }
                }
            } else {
                // Basic block placeholder
                ASSERT_require2(vertex.nOutEdges() == 1,
                                "placeholder " + addrToString(vertex.value().address()) +
                                " must have exactly one outgoing edge");
                ControlFlowGraph::ConstEdgeNodeIterator edge = vertex.outEdges().begin();
                ASSERT_require2(edge->target() == undiscoveredVertex_,
                                "placeholder " + addrToString(vertex.value().address()) +
                                " edge must go to a special vertex");
            }

            ASSERT_require2(vertexIndex_.exists(vertex.value().address()),
                            "bb/placeholder " + addrToString(vertex.value().address()) +
                            " must exist in the vertex index");

        } else {
            // Special vertices
            ASSERT_require2(vertex.nOutEdges()==0,
                            "special vertices must have no outgoing edges");
        }
    }
#endif
}

// class method
std::string
Partitioner::basicBlockName(const BasicBlock::Ptr &bblock) {
    return bblock==NULL ? "no basic block" : bblock->printableName();
}

// class method
std::string
Partitioner::dataBlockName(const DataBlock::Ptr &dblock) {
    return dblock==NULL ? "no data block" : dblock->printableName();
}

// class method
std::string
Partitioner::functionName(const Function::Ptr &function) {
    return function==NULL ? "no function" : function->printableName();
}

// class method
std::string
Partitioner::vertexName(const ControlFlowGraph::VertexNode &vertex) {
    switch (vertex.value().type()) {
        case V_BASIC_BLOCK: {
            std::string retval = StringUtility::addrToString(vertex.value().address()) +
                                 "<" + StringUtility::numberToString(vertex.id());
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (bb->isEmpty())
                    retval += ",X";                     // non-existing
            } else {
                retval += ",P";                         // placeholder
            }
            return retval + ">";
        }
        case V_INDETERMINATE:
            return "indeterminate<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_NONEXISTING:
            return "non-existing<" + StringUtility::numberToString(vertex.id()) + ">";
        case V_UNDISCOVERED:
            return "undiscovered<" + StringUtility::numberToString(vertex.id()) + ">";
    }
}

std::string
Partitioner::vertexName(const ControlFlowGraph::ConstVertexNodeIterator &vertex) const {
    if (vertex != cfg_.vertices().end())
        return vertexName(*vertex);
    return "no-vertex";
}

// class method
std::string
Partitioner::vertexNameEnd(const ControlFlowGraph::VertexNode &vertex) {
    if (vertex.value().type() == V_BASIC_BLOCK) {
        if (BasicBlock::Ptr bb = vertex.value().bblock()) {
            if (!bb->isEmpty()) {
                return vertexName(vertex) + ":" + StringUtility::addrToString(bb->instructions().back()->get_address());
            }
        }
    }
    return vertexName(vertex);
}

// class method
std::string
Partitioner::edgeNameDst(const ControlFlowGraph::EdgeNode &edge) {
    std::string retval;
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "<fcall>";
            break;
        case E_FUNCTION_XFER:
            retval += "<fxfer>";
            break;
        case E_FUNCTION_RETURN:
            retval += "<return>";
            break;
        case E_CALL_RETURN:
            retval += "<callret>";
            break;
    }
    return retval + vertexName(*edge.target());
}

std::string
Partitioner::edgeNameDst(const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeNameDst(*edge);
    return "no-edge";
}

// class method
std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::EdgeNode &edge) {
    const ControlFlowGraph::VertexNode &source = *edge.source();
    std::string retval = vertexNameEnd(source);
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "<fcall>";
            break;
        case E_FUNCTION_XFER:
            retval += "<fxfer>";
            break;
        case E_FUNCTION_RETURN:
            retval += "<return>";
            break;
        case E_CALL_RETURN:
            retval += "<callret>";
            break;
    }
    return retval;
}

std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeNameSrc(*edge);
    return "no-edge";
}

// class method
std::string
Partitioner::edgeName(const ControlFlowGraph::EdgeNode &edge) {
    std::string retval = vertexNameEnd(*edge.source()) + " ";
    switch (edge.value().type()) {
        case E_NORMAL:
            break;
        case E_FUNCTION_CALL:
            retval += "(fcall)";
            break;
        case E_FUNCTION_XFER:
            retval += "(fxfer)";
            break;
        case E_FUNCTION_RETURN:
            retval += "(return)";
            break;
        case E_CALL_RETURN:
            retval += "(callret)";
            break;
    }
    return retval + "-> " + vertexName(*edge.target());
}

std::string
Partitioner::edgeName(const ControlFlowGraph::ConstEdgeNodeIterator &edge) const {
    if (edge != cfg_.edges().end())
        return edgeName(*edge);
    return "no-edge";
}

void
Partitioner::dumpCfg(std::ostream &out, const std::string &prefix, bool showBlocks, bool computeProperties) const {
    AsmUnparser unparser;
    const std::string insnPrefix = prefix + "    ";
    unparser.insnRawBytes.fmt.prefix = insnPrefix.c_str();
    unparser.set_registers(instructionProvider_->registerDictionary());

    // Sort the vertices according to basic block starting address.
    std::vector<ControlFlowGraph::ConstVertexNodeIterator> sortedVertices;
    for (ControlFlowGraph::ConstVertexNodeIterator vi=cfg_.vertices().begin(); vi!=cfg_.vertices().end(); ++vi) {
        if (vi->value().type() == V_BASIC_BLOCK)
            sortedVertices.push_back(vi);
    }
    std::sort(sortedVertices.begin(), sortedVertices.end(), sortVerticesByAddress);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexNodeIterator &vertex, sortedVertices) {
        out <<prefix <<"basic block " <<vertexName(*vertex);
        if (vertex->value().function()) {
            if (vertex->value().function()->address() == vertex->value().address()) {
                out <<" entry block for " <<functionName(vertex->value().function());
            } else {
                out <<" owned by " <<functionName(vertex->value().function());
            }
        }
        out <<"\n";

        // Sort incoming edges according to source (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeNodeIterator> sortedInEdges;
        for (ControlFlowGraph::ConstEdgeNodeIterator ei=vertex->inEdges().begin(); ei!=vertex->inEdges().end(); ++ei)
            sortedInEdges.push_back(ei);
        std::sort(sortedInEdges.begin(), sortedInEdges.end(), sortEdgesBySrc);
        out <<prefix <<"  predecessors:";
        if (sortedInEdges.empty()) {
            out <<" none\n";
        } else {
            BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedInEdges)
                out <<" " <<edgeNameSrc(*edge);
            out <<"\n";
        }

        // Show instructions in execution order
        if (showBlocks) {
            if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                    out <<insnPrefix;                   // hexdump does not prefix the first line
                    unparser.unparse(out, insn);
                }
            }
        }

        // Show some basic block properties
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (bb->finalState()==NULL)
                out <<prefix <<"  semantics failed\n";

            // call semantics?
            out <<prefix <<"  is function call? ";
            if (computeProperties)
                basicBlockIsFunctionCall(bb);
            bool b=false;
            if (bb->isFunctionCall().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"not computed\n";
            }

            // return semantics?
            out <<prefix <<"  is function return? ";
            if (computeProperties)
                basicBlockIsFunctionReturn(bb);
            if (bb->isFunctionReturn().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"not computed\n";
            }
            
            // stack delta
            out <<prefix <<"  stack delta: ";
            if (computeProperties)
                basicBlockStackDelta(bb);
            BaseSemantics::SValuePtr delta;
            if (bb->stackDelta().getOptional().assignTo(delta) && delta!=NULL) {
                out <<*delta <<"\n";
            } else {
                out <<"not computed\n";
            }

            // may-return?
            out <<prefix <<"  may eventually return to caller? ";
            if (computeProperties)
                basicBlockOptionalMayReturn(vertex);
            if (bb->mayReturn().getOptional().assignTo(b)) {
                out <<(b ? "yes" : "no") <<"\n";
            } else {
                out <<"unknown\n";
            }
        }
        
        // Sort outgoing edges according to destination (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeNodeIterator> sortedOutEdges;
        for (ControlFlowGraph::ConstEdgeNodeIterator ei=vertex->outEdges().begin(); ei!=vertex->outEdges().end(); ++ei)
            sortedOutEdges.push_back(ei);
        std::sort(sortedOutEdges.begin(), sortedOutEdges.end(), sortEdgesByDst);
        out <<prefix <<"  successors:";
        if (sortedOutEdges.empty()) {
            out <<" none\n";
        } else {
            BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedOutEdges)
                out <<" " <<edgeNameDst(*edge);
            out <<"\n";
        }

        // Ghost edges
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            std::set<rose_addr_t> ghosts = basicBlockGhostSuccessors(bb);
            if (!ghosts.empty()) {
                out <<prefix <<"  ghost successors:";
                BOOST_FOREACH (rose_addr_t ghost, ghosts)
                    out <<" " <<StringUtility::addrToString(ghost);
                out <<"\n";
            }
        }
    }
}

// class method
std::string
Partitioner::cfgGraphVizVertex(const ControlFlowGraph::VertexNode &vertex, const std::string &attrs) {
    std::string s = StringUtility::numberToString(vertex.id());
    switch (vertex.value().type()) {
        case V_UNDISCOVERED:
            s += " [ label=\"undiscovered\" ];\n";
            break;
        case V_INDETERMINATE:
            s += " [ label=\"indeterminate\" ];\n";
            break;
        case V_NONEXISTING:
            s += " [ label=\"nonexisting\" ];\n";
            break;
        case V_BASIC_BLOCK:
            s += " [ label=\"" + StringUtility::addrToString(vertex.value().address());
            if (BasicBlock::Ptr bb = vertex.value().bblock())
                s += "\\n" + StringUtility::addrToString(bb->fallthroughVa()-1);
            s += "\" " + attrs + "];\n";
            break;
    }
    return s;
}
            
void
Partitioner::cfgGraphViz(std::ostream &out, const AddressInterval &restrict,
                         bool showNeighbors) const {
    out <<"digraph {\n";

    // Emit those vertices that we want to include in the output
    std::set<size_t> selected;
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type()==V_BASIC_BLOCK && restrict.isContaining(vertex.value().address())) {
            out <<cfgGraphVizVertex(vertex);
            selected.insert(vertex.id());
        }
    }

    // Edges, and additional vertices outside our range
    std::set<size_t> seen = selected;
    BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, cfg_.edges()) {
        const ControlFlowGraph::VertexNode &source = *edge.source();
        const ControlFlowGraph::VertexNode &target = *edge.target();

        // Should we show this edge?
        bool sourceSelected = selected.find(source.id()) != selected.end();
        bool targetSelected = selected.find(target.id()) != selected.end();
        if (!sourceSelected && !targetSelected)
            continue;
        if (!showNeighbors && (!sourceSelected || !targetSelected))
            continue;

        // Create the source vertex if we haven't already
        if (seen.find(source.id()) == seen.end()) {
            out <<cfgGraphVizVertex(source, "shape=plaintext style=filled fillcolor=yellow");
            seen.insert(source.id());
        }

        // Create the destination vertex if we haven't already
        if (seen.find(target.id()) == seen.end()) {
            out <<cfgGraphVizVertex(target, "shape=plaintext style=filled fillcolor=yellow");
            seen.insert(target.id());
        }

        // The edge
        std::string edgeAttrs;
        switch (edge.value().type()) {
            case E_NORMAL:
                break;
            case E_FUNCTION_CALL:
                edgeAttrs = "label=\"call\"";
                break;
            case E_FUNCTION_XFER:
                edgeAttrs = "label=\"fxfer\"";
                break;
            case E_FUNCTION_RETURN:
                edgeAttrs = "label=\"ret\"";
                break;
            case E_CALL_RETURN:
                edgeAttrs = "label=\"cret\"";
                break;
        }
        out <<source.id() <<" -> " <<target.id();
        if (!edgeAttrs.empty())
            out <<" [" + edgeAttrs + "]";
        out <<";\n";
    }

    out <<"}\n";
}

std::vector<Function::Ptr>
Partitioner::nextFunctionPrologue(rose_addr_t startVa) {
    while (memoryMap_.atOrAfter(startVa).require(MemoryMap::EXECUTABLE).next().assignTo(startVa)) {
        Sawyer::Optional<rose_addr_t> unmappedVa = aum_.leastUnmapped(startVa);
        if (!unmappedVa)
            return std::vector<Function::Ptr>();        // empty; no higher unused address
        if (startVa == *unmappedVa) {
            BOOST_FOREACH (const FunctionPrologueMatcher::Ptr &matcher, functionPrologueMatchers_) {
                if (matcher->match(this, startVa)) {
                        std::vector<Function::Ptr> newFunctions = matcher->functions();
                    ASSERT_forbid(newFunctions.empty());
                    return newFunctions;
                }
            }
            ++startVa;
        } else {
            startVa = *unmappedVa;
        }
    }
    return std::vector<Function::Ptr>();
}

DataBlock::Ptr
Partitioner::matchFunctionPadding(const Function::Ptr &function) {
    ASSERT_not_null(function);
    rose_addr_t anchor = function->address();
    BOOST_FOREACH (const FunctionPaddingMatcher::Ptr &matcher, functionPaddingMatchers_) {
        rose_addr_t paddingVa = matcher->match(this, anchor);
        if (paddingVa < anchor)
            return attachFunctionDataBlock(function, paddingVa, anchor-paddingVa);
    }
    return DataBlock::Ptr();
}

size_t
Partitioner::attachFunctions(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += attachFunction(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);

    size_t nNewBlocks = 0;
    Function::Ptr exists;
    if (functions_.getOptional(function->address()).assignTo(exists)) {
        if (exists != function)
            throw FunctionError(function, functionName(function) + " is already attached with a different function pointer");
        ASSERT_require(function->isFrozen());
    } else {
        // Insert function into the table, and make sure all its basic blocks see that they're owned by the function.
        functions_.insert(function->address(), function);
        nNewBlocks = attachFunctionBasicBlocks(function);

        // Attach function data blocks.
        BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks()) {
            attachDataBlock(dblock);
            dblock->incrementOwnerCount();
        }

        // Prevent the function connectivity from changing while the function is in the CFG.  Non-frozen functions
        // can have basic blocks added and erased willy nilly because the basic blocks don't need to know that they're owned by
        // a function.  But we can't have that when the function is one that's part of the CFG.
        function->freeze();
    }
    return nNewBlocks;
}

Function::Ptr
Partitioner::attachOrMergeFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    Function::Ptr exists;
    if (functions_.getOptional(function->address()).assignTo(exists)) {
        if (exists != function) {
            bool canMerge = true;
            for (int pass=0; pass<2 && canMerge; ++pass) { // first pass to check, second pass to make changes
                // FIXME[Robb P. Matzke 2014-08-23]: we could relax these: found could be a subset of exists
                if (0==pass) {
                    if (exists->basicBlockAddresses().size() != function->basicBlockAddresses().size()) {
                        canMerge = false;
                        break;
                    }
                    if (!std::equal(exists->basicBlockAddresses().begin(), exists->basicBlockAddresses().end(),
                                    function->basicBlockAddresses().begin())) {
                        canMerge = false;
                        break;
                    }
                    if (exists->dataBlocks().size() != function->dataBlocks().size()) {
                        canMerge = false;
                        break;
                    }
                    if (!std::equal(exists->dataBlocks().begin(), exists->dataBlocks().end(), function->dataBlocks().begin())) {
                        canMerge = false;
                        break;
                    }
                }

                // Function name. Use new function's name if existing function has no name.  If names are the same except for
                // the "@plt" part then use the version with the "@plt".
                if (exists->name() == function->name()) {
                    // nothing to do
                } else if (exists->name().empty()) {
                    if (pass>0)
                        exists->name(function->name());
                } else if (boost::ends_with(exists->name(), "@plt") && boost::starts_with(exists->name(), function->name())) {
                    // nothing to do
                } else if (boost::ends_with(function->name(), "@plt") && boost::starts_with(function->name(), exists->name())) {
                    if (pass>0)
                        exists->name(function->name());
                } else {
                    canMerge = false;
                    break;
                }
            }
            if (!canMerge) {
                throw FunctionError(function,
                                    functionName(function) + " cannot be merged with attached " + functionName(exists));
            }
        }
        return exists;
    }
    attachFunction(function);
    return function;
}
    
size_t
Partitioner::attachFunctionBasicBlocks(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += attachFunctionBasicBlocks(function);
    return nNewBlocks;
}

size_t
Partitioner::attachFunctionBasicBlocks(const Function::Ptr &function) {
    ASSERT_not_null(function);
    size_t nNewBlocks = 0;
    bool functionExists = functions_.exists(function->address());
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(blockVa);
        if (placeholder == cfg_.vertices().end()) {
            placeholder = insertPlaceholder(blockVa);
            ++nNewBlocks;
        }
        if (functionExists) {
            if (placeholder->value().function()!=NULL && placeholder->value().function()!=function) {
                throw FunctionError(function,
                                    functionName(function) + " basic block " + vertexName(*placeholder) +
                                    " is already owned by " + functionName(placeholder->value().function()));
            }
            placeholder->value().function(function);
        }
    }
    return nNewBlocks;
}

void
Partitioner::detachFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    if (functionExists(function->address()) != function)
        return;                                         // already detached

    // Unlink basic block ownership, but do not detach basic blocks from CFG/AUM
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = findPlaceholder(blockVa);
        ASSERT_require(placeholder != cfg_.vertices().end());
        ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
        ASSERT_require(placeholder->value().function() == function);
        placeholder->value().function(Function::Ptr());
    }

    // Unlink data block ownership, but do not detach data blocks from CFG/AUM unless ownership count hits zero.
    BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks()) {
        ASSERT_not_null(dblock);
        if (0==dblock->decrementOwnerCount())
            detachDataBlock(dblock);
    }

    // Unlink the function itself
    functions_.erase(function->address());
    function->thaw();
}

AddressUsageMap
Partitioner::aum(const Function::Ptr &function) const {
    AddressUsageMap retval;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(blockVa);
        if (placeholder != cfg_.vertices().end()) {
            ASSERT_require(placeholder->value().type() == V_BASIC_BLOCK);
            if (BasicBlock::Ptr bb = placeholder->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    retval.insertInstruction(insn, bb);
                BOOST_FOREACH (const DataBlock::Ptr &dblock, bb->dataBlocks())
                    retval.insertDataBlock(OwnedDataBlock(dblock, bb));
            }
        }
    }
    BOOST_FOREACH (const DataBlock::Ptr &dblock, function->dataBlocks())
        retval.insertDataBlock(OwnedDataBlock(dblock, function));

    return retval;
}

std::set<rose_addr_t>
Partitioner::ghostSuccessors() const {
    std::set<rose_addr_t> ghosts;
    BOOST_FOREACH (const CfgVertex &vertex, cfg_.vertexValues()) {
        if (vertex.type() == V_BASIC_BLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock()) {
                BOOST_FOREACH (rose_addr_t ghost, basicBlockGhostSuccessors(bb)) {
                    ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(ghost);
                    if (placeholder == cfg_.vertices().end())
                        ghosts.insert(ghost);
                }
            }
        }
    }
    return ghosts;
}

std::vector<Function::Ptr>
Partitioner::functions() const {
    std::vector<Function::Ptr> functions;
    functions.reserve(functions_.size());
    BOOST_FOREACH (const Function::Ptr &function, functions_.values())
        functions.push_back(function);
    ASSERT_require(isSorted(functions, sortFunctionsByAddress, true));
    return functions;
}

std::vector<Function::Ptr>
Partitioner::discoverCalledFunctions() const {
    std::vector<Function::Ptr> functions;
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASIC_BLOCK && !functions_.exists(vertex.value().address())) {
            BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.inEdges()) {
                if (edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER) {
                    rose_addr_t entryVa = vertex.value().address();
                    insertUnique(functions, Function::instance(entryVa), sortFunctionsByAddress);
                    break;
                }
            }
        }
    }
    return functions;
}

std::vector<Function::Ptr>
Partitioner::discoverFunctionEntryVertices() const {
    std::vector<Function::Ptr> functions = discoverCalledFunctions();
    BOOST_FOREACH (const Function::Ptr &knownFunction, functions_.values())
        insertUnique(functions, knownFunction, sortFunctionsByAddress);
    return functions;
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         EdgeList *inwardInterFunctionEdges /*out*/,
                                         EdgeList *outwardInterFunctionEdges /*out*/) {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBasicBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
    if (inwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, inwardIds)
            inwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    if (outwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, outwardIds)
            outwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    return retval;
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         ConstEdgeList *inwardInterFunctionEdges /*out*/,
                                         ConstEdgeList *outwardInterFunctionEdges /*out*/) const {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBasicBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
    if (inwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, inwardIds)
            inwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    if (outwardInterFunctionEdges) {
        BOOST_FOREACH (size_t id, outwardIds)
            outwardInterFunctionEdges->push_back(cfg_.findEdge(id));
    }
    return retval;
}

Sawyer::Optional<Partitioner::Thunk>
Partitioner::functionIsThunk(const Function::Ptr &function) const {
    if (function==NULL || 0==(function->reasons() & SgAsmFunction::FUNC_THUNK) || function->nBasicBlocks()!=1)
        return Sawyer::Nothing();

    // Find the basic block for the thunk
    BasicBlock::Ptr bblock = basicBlockExists(function->address());
    if (!bblock)
        bblock = discoverBasicBlock(function->address());
    if (!bblock)
        return Sawyer::Nothing();

    // Basic block should have only one successor, which must be concrete
    BasicBlock::Successors succs = basicBlockSuccessors(bblock);
    if (succs.size()!=1)
        return Sawyer::Nothing();
    std::vector<rose_addr_t> concreteSuccessors = basicBlockConcreteSuccessors(bblock);
    if (concreteSuccessors.size()!=1)
        return Sawyer::Nothing();

    // Make sure the successor is of type E_FUNCTION_XFER
    if (succs[0].type() != E_FUNCTION_XFER) {
#if 1 // DEBUGGING [Robb P. Matzke 2014-11-07]
        mlog[INFO] <<"ROBB: changing thunk edge " <<StringUtility::addrToString(function->address())
                   <<" -> " <<StringUtility::addrToString(concreteSuccessors[0]) <<" to E_FUNCTION_XFER\n";
#endif
        succs[0] = BasicBlock::Successor(succs[0].expr(), E_FUNCTION_XFER);
        bblock->successors_.set(succs);                 // okay even if bblock is locked since we only change edge type
    }

    return Thunk(bblock, concreteSuccessors.front());
}

size_t
Partitioner::discoverFunctionBasicBlocks(const Function::Ptr &function,
                                         std::vector<size_t> &inwardInterFunctionEdges /*out*/,
                                         std::vector<size_t> &outwardInterFunctionEdges /*out*/) const {
    ASSERT_not_null(function);
    if (function->isFrozen())
        throw FunctionError(function, functionName(function) +
                            " is frozen or attached to CFG/AUM when discovering basic blocks");

    Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"discoverFunctionBlocks(" <<functionName(function) <<")\n";
    size_t nConflictsOrig = inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size();

    // Thunks are handled specially. They only ever contain one basic block. As a side effect, the thunk's outgoing edge is
    // changed to type E_FUNCTION_XFER.
    if (functionIsThunk(function))
        return 0;

    typedef Sawyer::Container::Map<size_t /*vertexId*/, Function::Ownership> VertexOwnership;
    VertexOwnership ownership;                          // contains only OWN_EXPLICIT and OWN_PROVISIONAL entries

    // Find the vertices that this function already owns
    BOOST_FOREACH (rose_addr_t startVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(startVa);
        if (placeholder == cfg_.vertices().end()) {
            throw Exception("block " + StringUtility::addrToString(startVa) + " of " + functionName(function) +
                            " must exist in the CFG");
        }
        ownership.insert(placeholder->id(), Function::OWN_EXPLICIT);
        SAWYER_MESG(debug) <<"  explicitly owns vertex " <<placeholder->id() <<"\n";
    }

    // Find all the unowned vertices we can reach from the previously owned vertices following non-function-call edges and
    // excluding special target vertices.  The loop is guaranteed to never visit the same vertex more than once because, we
    // only add a vertex to the worklist if it is not in the ownership set, adding it to the worklist and ownership set at the
    // same time.  The loop is also therefore guaranteed to not visit an edge more than once.
    Sawyer::Container::Stack<size_t> worklist(ownership.keys());
    while (!worklist.isEmpty()) {
        const ControlFlowGraph::VertexNode &source = *cfg_.findVertex(worklist.pop());
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, source.outEdges()) {
            if (edge.value().type()!=E_FUNCTION_CALL && edge.value().type()!=E_FUNCTION_XFER && !edge.isSelfEdge()) {
                const ControlFlowGraph::VertexNode &target = *edge.target();
                if (target.value().type()==V_BASIC_BLOCK && !ownership.exists(target.id())) {
                    if (target.value().function()) {
                        // Some other function already owns this vertex.  The edge is therefore an inter-function edge which
                        // was not labeled as a function call.  If the edge is to a known function entry block then we'll
                        // assume this should have been a function call edge and not traverse it, otherwise we'll have to let
                        // the user decide what to do.
                        if (!functionExists(target.value().address()))
                            outwardInterFunctionEdges.push_back(edge.id());
                    } else {
                        SAWYER_MESG(debug) <<"  following edge " <<edgeName(edge) <<"\n";
                        ownership.insert(target.id(), Function::OWN_PROVISIONAL);
                        worklist.push(target.id());
                        SAWYER_MESG(debug) <<"    provisionally owns vertex " <<target.id() <<"\n";
                    }
                }
            }
        }
    }

    // For all the provisionally-owned vertices other than this function's entry vertex, check that they have incoming edges
    // only from this same function.  We explicitly do not check the vertices that were previously owned (the user may have
    // wanted them in the function regardless of problems).
    BOOST_FOREACH (const VertexOwnership::Node &node, ownership.nodes()) {
        if (node.value() == Function::OWN_PROVISIONAL) {
            const ControlFlowGraph::VertexNode &target = *cfg_.findVertex(node.key());
            SAWYER_MESG(debug) <<"  testing provisional vertex " <<vertexName(target) <<"\n";
            if (target.value().address() != function->address()) {
                BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, target.inEdges()) {
                    const ControlFlowGraph::VertexNode &source = *edge.source();
                    SAWYER_MESG(debug) <<"    testing edge " <<edgeName(edge);
                    if (!ownership.exists(source.id())) {
                        // This edge crosses a function boundary yet is not labeled as a function call.
                        inwardInterFunctionEdges.push_back(edge.id());
                        if (debug) {
                            if (source.value().function()) {
                                debug <<"; edge source is owned by " <<functionName(source.value().function()) <<"\n";
                            } else {
                                debug <<"; edge is not owned by any function\n";
                            }
                        }
                    } else {
                        SAWYER_MESG(debug) <<"; ok\n";
                    }
                }
            }
        }
    }

    // If there were no conflicts then add all the provisional vertices to this function. This does not modify the CFG.
    if (inwardInterFunctionEdges.empty() && outwardInterFunctionEdges.empty()) {
        BOOST_FOREACH (const VertexOwnership::Node &node, ownership.nodes()) {
            if (node.value() == Function::OWN_PROVISIONAL)
                function->insertBasicBlock(cfg_.findVertex(node.key())->value().address());
        }
    }

    return inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size() - nConflictsOrig;
}

std::set<rose_addr_t>
Partitioner::functionGhostSuccessors(const Function::Ptr &function) const {
    ASSERT_not_null(function);
    std::set<rose_addr_t> ghosts;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        if (BasicBlock::Ptr bb = basicBlockExists(blockVa)) {
            BOOST_FOREACH (rose_addr_t ghost, basicBlockGhostSuccessors(bb)) {
                ControlFlowGraph::ConstVertexNodeIterator placeholder = findPlaceholder(ghost);
                if (placeholder == cfg_.vertices().end())
                    ghosts.insert(ghost);
            }
        }
    }
    return ghosts;
}

FunctionCallGraph
Partitioner::functionCallGraph() const {
    FunctionCallGraph cg;
    BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, cfg_.edges()) {
        if (edge.source()->value().type()==V_BASIC_BLOCK && edge.target()->value().type()==V_BASIC_BLOCK) {
            Function::Ptr source = edge.source()->value().function();
            Function::Ptr target = edge.target()->value().function();
            if (source!=NULL && target!=NULL &&
                (source!=target || edge.value().type()==E_FUNCTION_CALL || edge.value().type()==E_FUNCTION_XFER))
                cg.insert(source, target);
        }
    }
    return cg;
}

SgAsmBlock*
Partitioner::buildBasicBlockAst(const BasicBlock::Ptr &bb, bool relaxed) const {
    ASSERT_not_null(bb);
    if (bb->isEmpty()) {
        if (relaxed) {
            mlog[WARN] <<"creating an empty basic block AST node at " <<StringUtility::addrToString(bb->address()) <<"\n";
        } else {
            return NULL;
        }
    }

    ControlFlowGraph::ConstVertexNodeIterator bblockVertex = findPlaceholder(bb->address());
    SgAsmBlock *ast = SageBuilderAsm::buildBasicBlock(bb->instructions());
    unsigned reasons = 0;

    // Is this block an entry point for a function?
    if (Function::Ptr function = functionExists(bb->address())) {
        if (function->address() == bb->address())
            reasons |= SgAsmBlock::BLK_ENTRY_POINT;
    }

    // Does this block have any predecessors?
    if (bblockVertex != cfg_.vertices().end() && bblockVertex->nInEdges()==0)
        reasons |= SgAsmBlock::BLK_CFGHEAD;

    ast->set_reason(reasons);
    ast->set_code_likelihood(1.0);                      // FIXME[Robb P. Matzke 2014-08-07]

    // Cache the basic block successors in the AST since we've already computed them.  If the basic block is in the CFG then we
    // can use the CFG's edges to initialize the AST successors since they are canonical. Otherwise we'll use the successors
    // from bb. In any case, we fill in the successor SgAsmIntegerValueExpression objects with only the address and not
    // pointers to SgAsmBlock IR nodes since we don't have all the blocks yet; the SgAsmBlock pointers will be initialized
    // higher up on the AST-building stack.
    if (bblockVertex != cfg_.vertices().end()) {
        bool isComplete = true;
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, bblockVertex->outEdges()) {
            const ControlFlowGraph::VertexNode &target = *edge.target();
            if (target.value().type() == V_INDETERMINATE || target.value().type() == V_NONEXISTING) {
                isComplete = false;
            } else {
                ASSERT_require(target.value().type() == V_BASIC_BLOCK);
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(target.value().address());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            }
        }
        ast->set_successors_complete(isComplete);
    } else {
        bool isComplete = true;
        BOOST_FOREACH (rose_addr_t successorVa, basicBlockConcreteSuccessors(bb, &isComplete)) {
            SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(successorVa);
            succ->set_parent(ast);
            ast->get_successors().push_back(succ);
        }
        ast->set_successors_complete(isComplete);
    }
    return ast;
}

SgAsmBlock*
Partitioner::buildDataBlockAst(const DataBlock::Ptr &dblock, bool relaxed) const {
    // Build the static data item
    SgUnsignedCharList rawBytes(dblock->size(), 0);
    size_t nRead = memoryMap_.at(dblock->address()).read(rawBytes).size();
    ASSERT_always_require(nRead==dblock->size());
    SgAsmStaticData *datum = SageBuilderAsm::buildStaticData(dblock->address(), rawBytes);

    // Build the data block IR node pointing to the static item
    return SageBuilderAsm::buildDataBlock(datum);
}

SgAsmFunction*
Partitioner::buildFunctionAst(const Function::Ptr &function, bool relaxed) const {
    ASSERT_not_null(function);

    // Build the child basic block IR nodes and remember all the data blocks
    std::vector<DataBlock::Ptr> dblocks = function->dataBlocks();
    std::vector<SgAsmBlock*> children;
    BOOST_FOREACH (rose_addr_t blockVa, function->basicBlockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator vertex = findPlaceholder(blockVa);
        if (vertex == cfg_.vertices().end()) {
            mlog[WARN] <<functionName(function) <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" does not exist in the CFG; no AST node created\n";
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (SgAsmBlock *child = buildBasicBlockAst(bb, relaxed))
                children.push_back(child);
            BOOST_FOREACH (const DataBlock::Ptr &dblock, bb->dataBlocks())
                insertUnique(dblocks, dblock, sortDataBlocks);
        } else {
            mlog[WARN] <<functionName(function) <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" is undiscovered in the CFG; no AST node created\n";
        }
    }
    if (children.empty()) {
        if (relaxed) {
            mlog[WARN] <<"creating an empty AST node for " <<functionName(function) <<"\n";
        } else {
            return NULL;
        }
    }

    // Build the child data block IR nodes.  The data blocks attached to the SgAsmFunction node are the union of the data
    // blocks owned by the function and the data blocks owned by each of its basic blocks.
    BOOST_FOREACH (const DataBlock::Ptr &dblock, dblocks) {
        if (SgAsmBlock *child = buildDataBlockAst(dblock, relaxed))
            children.push_back(child);
    }

    // Sort the children in the order usually used by ROSE
    std::sort(children.begin(), children.end(), sortBlocksForAst);

    unsigned reasons = function->reasons();

    // Is the function part of the CFG?
    ControlFlowGraph::ConstVertexNodeIterator entryVertex = findPlaceholder(function->address());
    if (entryVertex != cfg_.vertices().end())
        reasons |= SgAsmFunction::FUNC_GRAPH;
    
    // Is the function discontiguous?
    if (aum(function).extent().nIntervals() > 1)
        reasons |= SgAsmFunction::FUNC_DISCONT;

    // Is the function the target of a function call?
    if (entryVertex != cfg_.vertices().end()) {
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, entryVertex->inEdges()) {
            if (edge.value().type() == E_FUNCTION_CALL || edge.value().type() == E_FUNCTION_XFER) {
                reasons |= SgAsmFunction::FUNC_CALL_TARGET;
                break;
            }
        }
    }
    
    // Build the AST
    SgAsmFunction *ast = SageBuilderAsm::buildFunction(function->address(), children);
    ast->set_reason(reasons);
    ast->set_name(function->name());
    ast->set_may_return(SgAsmFunction::RET_UNKNOWN);    // FIXME[Robb P. Matzke 2014-08-07]
    return ast;
}

SgAsmBlock*
Partitioner::buildGlobalBlockAst(bool relaxed) const {
    // Create the children first
    std::vector<SgAsmFunction*> children;
    BOOST_FOREACH (const Function::Ptr &function, functions_.values()) {
        if (SgAsmFunction *func = buildFunctionAst(function, relaxed)) {
            children.push_back(func);
        }
    }
    if (children.empty()) {
        if (relaxed) {
            mlog[WARN] <<"building an empty global block\n";
        } else {
            return NULL;
        }
    }

    // Build the global block
    SgAsmBlock *global = new SgAsmBlock;
    BOOST_FOREACH (SgAsmFunction *function, children) {
        global->get_statementList().push_back(function);
        function->set_parent(global);
    }
    return global;
}

SgAsmBlock*
Partitioner::buildAst(SgAsmInterpretation *interp/*=NULL*/, bool relaxed) const {
    if (SgAsmBlock *global = buildGlobalBlockAst(relaxed)) {
        fixupAstPointers(global, interp);
        return global;
    }
    return NULL;
}

void
Partitioner::fixupAstPointers(SgNode *ast, SgAsmInterpretation *interp/*=NULL*/) const {
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmNode*> Index;

    // Build various indexes since ASTs have inherently linear search time.  We store just the starting address for all these
    // things because that's all we ever want to point to.
    struct Indexer: AstSimpleProcessing {
        Index insnIndex, bblockIndex, funcIndex;
        void visit(SgNode *node) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                insnIndex.insert(insn->get_address(), insn);
            } else if (SgAsmBlock *block = isSgAsmBlock(node)) {
                if (!block->get_statementList().empty() && isSgAsmInstruction(block->get_statementList().front()))
                    bblockIndex.insert(block->get_address(), block);
            } else if (SgAsmFunction *func = isSgAsmFunction(node)) {
                funcIndex.insert(func->get_entry_va(), func);
            }
        }
    } indexer;
    indexer.traverse(ast, preorder);

    // Build a list of memory-mapped sections in the interpretation.  We'll use this list to select the best-matching section
    // for a particular address via SgAsmGenericFile::best_section_by_va()
    SgAsmGenericSectionPtrList mappedSections;
    Index sectionIndex;
    if (interp) {
        BOOST_FOREACH (SgAsmGenericHeader *header, interp->get_headers()->get_headers()) {
            if (header->is_mapped())
                mappedSections.push_back(header);
            BOOST_FOREACH (SgAsmGenericSection *section, header->get_mapped_sections()) {
                if (!section->get_mapped_xperm())
                    mappedSections.push_back(section);
            }
        }
    }

    struct FixerUpper: AstSimpleProcessing {
        const Index &insnIndex, &bblockIndex, &funcIndex;
        const SgAsmGenericSectionPtrList &mappedSections;
        FixerUpper(const Index &insnIndex, const Index &bblockIndex, const Index &funcIndex,
                   const SgAsmGenericSectionPtrList &mappedSections)
            : insnIndex(insnIndex), bblockIndex(bblockIndex), funcIndex(funcIndex), mappedSections(mappedSections) {}
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                if (ival->get_baseNode()==NULL) {
                    rose_addr_t va = ival->get_absoluteValue();
                    SgAsmNode *base = NULL;
                    if (funcIndex.getOptional(va).assignTo(base) || bblockIndex.getOptional(va).assignTo(base) ||
                        insnIndex.getOptional(va).assignTo(base)) {
                        ival->makeRelativeTo(base);
                    } else if (SgAsmGenericSection *section = SgAsmGenericFile::best_section_by_va(mappedSections, va)) {
                        ival->makeRelativeTo(section);
                    }
                }
            }
        }
    } fixerUpper(indexer.insnIndex, indexer.bblockIndex, indexer.funcIndex, mappedSections);
    fixerUpper.traverse(ast, preorder);
}

} // namespace
} // namespace
} // namespace
