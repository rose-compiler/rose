#include "Partitioner2.h"
#include "SymbolicSemantics2.h"
#include "DispatcherM68k.h"
#include "Diagnostics.h"

#if 1 // DEBUGGING [Robb P. Matzke 2014-08-02]
#include "AsmUnparser_compat.h"
#endif

#include <boost/foreach.hpp>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stack.h>

// Defining this will cause the partitioner to continuously very that the CFG and AUM are consistent.  Doing so will impose a
// substantial slow-down.  Defining this has little effect if NDEBUG or SAWYER_NDEBUG is also defined.
#undef ROSE_PARTITIONER_EXPENSIVE_CHECKS

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
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Sort by the successor expressions. If both expressions are concrete then disregard their widths and treat them as unsigned
// when comparing.
static bool
sortByExpression(const Partitioner::BasicBlock::Successor &a, const Partitioner::BasicBlock::Successor &b) {
    if (a.expr()->is_number() && b.expr()->is_number())
        return a.expr()->get_number() < b.expr()->get_number();
    if (a.expr()->is_number() || b.expr()->is_number())
        return a.expr()->is_number();                   // concrete values are less than abstract expressions
    return a.expr()->get_expression()->structural_compare(b.expr()->get_expression()) < 0;
}

void
Partitioner::BasicBlock::init(const Partitioner *partitioner) {
    initDiagnostics();
#if 1
    // Try to use our own semantics
    if (usingDispatcher_) {
        if (dispatcher_ = partitioner->newDispatcher()) {
            usingDispatcher_ = true;
            initialState_ = dispatcher_->get_operators()->get_state(); // points to dispatcher's state
            BaseSemantics::RegisterStateGeneric::promote(initialState_->get_register_state())->initialize_large();
            initialState_ = initialState_->clone(); // make a copy so processInstruction doesn't change it
            optionalPenultimateState_ = initialState_->clone(); // one level of undo information
        } else {
            usingDispatcher_ = false;
        }
    }
#else
    // Rely on other methods to get basic block characteristics
    usingDispatcher_ = false;
#endif
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

BaseSemantics::StatePtr
Partitioner::BasicBlock::finalState() {
    if (usingDispatcher_ && dispatcher_!=NULL)
        return dispatcher_->get_operators()->get_state();
    return BaseSemantics::StatePtr();
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
    optionalPenultimateState_ = usingDispatcher_ ?
                                dispatcher_->get_operators()->get_state()->clone() :
                                BaseSemantics::StatePtr();
    clearCache();
    insns_.push_back(insn);
    if (usingDispatcher_) {
        try {
            dispatcher_->processInstruction(insn);
        } catch (...) {
            usingDispatcher_ = false;                   // an error turns off semantics for the remainder of the basic block
        }
    }
}

void
Partitioner::BasicBlock::pop() {
    ASSERT_forbid2(isFrozen(), "basic block must be modifiable to pop an instruction");
    ASSERT_forbid2(insns_.empty(), "basic block must have at least one instruction to pop");
    ASSERT_require2(optionalPenultimateState_, "only one level of undo is possible");
    clearCache();
    insns_.pop_back();

    if (BaseSemantics::StatePtr ps = *optionalPenultimateState_) {
        // If we didn't save a previous state it means that we didn't call processInstruction during the append, and therefore
        // we don't need to update the dispatcher (it's already out of date anyway).  Otherwise the dispatcher state needs to
        // be re-initialized by transferring ownership of the previous state into the partitioner.
        dispatcher_->get_operators()->set_state(ps);
        optionalPenultimateState_ = Sawyer::Nothing();
    }
}

rose_addr_t
Partitioner::BasicBlock::fallthroughVa() const {
    ASSERT_require(!insns_.empty());
    return insns_.back()->get_address() + insns_.back()->get_size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      InsnBlockPair
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Partitioner::InsnBlockPair::print(std::ostream &out) const {
    ASSERT_not_null(insn_);
    if (bblock_ != NULL) {
        out <<"{" <<unparseInstructionWithAddress(insn_) <<" in " <<StringUtility::addrToString(bblock_->address()) <<"}";
    } else {
        out <<unparseInstructionWithAddress(insn_);
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
            ++current;
        }
    }
    return true;
}

void
Partitioner::InsnBlockPairs::print(std::ostream &out) const {
    BOOST_FOREACH (const InsnBlockPair &ibpair, pairs_)
        out <<" " <<ibpair;
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

Sawyer::Container::IntervalSet<AddressInterval>
Partitioner::AddressUsageMap::extent() const {
    Sawyer::Container::IntervalSet<AddressInterval> retval;
    BOOST_FOREACH (const Map::Interval &interval, map_.keys())
        retval.insert(interval);
    return retval;
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

void
Partitioner::AddressUsageMap::print(std::ostream &out, const std::string &prefix) const {
    using namespace StringUtility;
    BOOST_FOREACH (const Map::Node &node, map_.nodes())
        out <<prefix <<"[" <<addrToString(node.key().least()) <<"," <<addrToString(node.key().greatest())
            <<"] =" <<node.value() <<"\n";
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
            <<" " <<StringUtility::plural(partitioner_->nBasicBlocks(), "blks")
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
    if (!bar) {
        if (0==progressTotal_) {
            BOOST_FOREACH (const MemoryMap::Segments::Node &node, memoryMap_.segments().nodes()) {
                if (0 != (node.value().get_mapperms() & MemoryMap::MM_PROT_EXEC))
                    progressTotal_ += node.key().size();
            }
        }
        bar = new Sawyer::ProgressBar<size_t, ProgressBarSuffix>(progressTotal_, mlog[INFO], "cfg");
    }
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
        if (vertex.type() == V_BASICBLOCK) {
            if (BasicBlock::Ptr bb = vertex.bblock())
                nInsns += bb->nInsns();
        }
    }
    return nInsns;
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
            aum_.erase(insn);
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
            goto done;
        retval->append(insn);
        if (insn->is_unknown())                                         // case: "unknown" instruction
            goto done;

        BOOST_FOREACH (rose_addr_t successorVa, bblockConcreteSuccessors(retval)) {
            if (successorVa!=startVa && retval->instructionExists(successorVa)) { // case: successor is inside our own block
                retval->pop();
                goto done;
            }
        }
        
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
            goto done;

        if (placeholderExists(successorVa)!=cfg_.vertices().end())      // case: successor is an existing block
            goto done;

        InsnBlockPair ibpair;
        if (instructionExists(successorVa).assignTo(ibpair)) {          // case: successor is inside an existing block
            if (ibpair.bblock() != conflict.bblock())
                goto done;
        }

        va = successorVa;
    }
done:
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
            bblockInserted(placeholder);
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
    ASSERT_require2(bb->isFrozen(), "basic block must be frozen before inserting into CFG");
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
    if (isFunctionCall) {
        successors.push_back(VertexEdgePair(insertPlaceholder(bb->fallthroughVa()), CfgEdge(E_FRET)));
    }

    // Make CFG edges
    cfg_.clearOutEdges(placeholder);
    BOOST_FOREACH (const VertexEdgePair &pair, successors)
        cfg_.insertEdge(placeholder, pair.first, pair.second);

    // Insert the basicblock
    placeholder->value().bblock(bb);
    BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
        aum_.insert(InsnBlockPair(insn, bb));
    }
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
    RegisterDescriptor REG_IP = instructionProvider_.instructionPointerRegister();

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
        Disassembler::AddressSet successorVas = firstInsn->get_successors(bb->instructions(), &complete, &memoryMap_);
        BOOST_FOREACH (rose_addr_t va, successorVas)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::instance(REG_IP.get_nbits(), va)));
        if (!complete)
            successors.push_back(BasicBlock::Successor(Semantics::SValue::instance(REG_IP.get_nbits())));
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

    return bb->cacheSuccessors(successors);
}

std::vector<rose_addr_t>
Partitioner::bblockConcreteSuccessors(const BasicBlock::Ptr &bb) const {
    std::vector<rose_addr_t> retval;
    BOOST_FOREACH (const BasicBlock::Successor &successor, bblockSuccessors(bb)) {
        if (successor.expr()->is_number())
            retval.push_back(successor.expr()->get_number());
    }
    return retval;
}

bool
Partitioner::bblockIsFunctionCall(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);
    bool retval = false;

    if (bb->isEmpty() || bb->cachedIsFunctionCall().assignTo(retval))
        return retval;

    SgAsmInstruction *lastInsn = bb->instructions().back();

    // Use our own semantics if we have them.
    if (BaseSemantics::StatePtr state = bb->finalState()) {
        // Is the block fall-through address equal to the value on the top of the stack?
        ASSERT_not_null(bb->dispatcher());
        BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
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

BaseSemantics::SValuePtr
Partitioner::bblockStackDelta(const BasicBlock::Ptr &bb) const {
    ASSERT_not_null(bb);

    BaseSemantics::SValuePtr delta;
    if ((delta = bb->cachedStackDelta()) != NULL)
        return delta;

    if (bb->finalState() == NULL)
        return Semantics::SValuePtr();

    ASSERT_not_null(bb->initialState());
    BaseSemantics::RiscOperatorsPtr ops = bb->dispatcher()->get_operators();
    ASSERT_not_null(ops);
    RegisterDescriptor REG_SP = instructionProvider_.stackPointerRegister();
    BaseSemantics::SValuePtr sp0 = bb->initialState()->readRegister(REG_SP, ops.get());
    BaseSemantics::SValuePtr spN = bb->finalState()->readRegister(REG_SP, ops.get());
    delta = ops->add(spN, ops->negate(sp0));
    return bb->cacheStackDelta(delta);
}

SgAsmInstruction *
Partitioner::discoverInstruction(rose_addr_t startVa) {
    return instructionProvider_[startVa];
}

void
Partitioner::bblockInserted(const ControlFlowGraph::VertexNodeIterator &newVertex) {
    using namespace StringUtility;
    if (isReportingProgress_)
        reportProgress();

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

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::InsertionArgs(this, newVertex));
#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
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

#if !defined(NDEBUG) && defined(ROSE_PARTITIONER_EXPENSIVE_CHECKS)
    checkConsistency();
#endif
    cfgAdjustmentCallbacks_.apply(true, CfgAdjustmentCallback::ErasureArgs(this, removedBlock));
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
                        InsnBlockPairs ibpairs = aum_.spanning(insnInterval);
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

            ASSERT_require2(vertexIndex_.exists(vertex.value().address()),
                            "bb/placeholder " + addrToString(vertex.value().address()) + " must exist in the vertex index");

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
Partitioner::functionName(const Function::Ptr &function) {
    if (function==NULL)
        return "no function";
    return "function " + StringUtility::addrToString(function->address());
}

// class method
std::string
Partitioner::vertexName(const ControlFlowGraph::VertexNode &vertex) {
    switch (vertex.value().type()) {
        case V_BASICBLOCK: {
            std::string retval = StringUtility::addrToString(vertex.value().address()) +
                                 "[" + StringUtility::numberToString(vertex.id());
            if (BasicBlock::Ptr bb = vertex.value().bblock()) {
                if (bb->isEmpty())
                    retval += ",X";                     // non-existing
            } else {
                retval += ",P";                         // placeholder
            }
            return retval + "]";
        }
        case V_INDETERMINATE:
            return "indeterminate[" + StringUtility::numberToString(vertex.id()) + "]";
        case V_NONEXISTING:
            return "non-existing[" + StringUtility::numberToString(vertex.id()) + "]";
        case V_UNDISCOVERED:
            return "undiscovered[" + StringUtility::numberToString(vertex.id()) + "]";
    }
}

// class method
std::string
Partitioner::vertexNameEnd(const ControlFlowGraph::VertexNode &vertex) {
    if (vertex.value().type() == V_BASICBLOCK) {
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
        case E_FCALL:
            retval += "[fcall]";
            break;
        case E_FRET:
            retval += "[fret]";
            break;
    }
    return retval + vertexName(*edge.target());
}

// class method
std::string
Partitioner::edgeNameSrc(const ControlFlowGraph::EdgeNode &edge) {
    const ControlFlowGraph::VertexNode &source = *edge.source();
    std::string retval = vertexNameEnd(source);
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

// class method
std::string
Partitioner::edgeName(const ControlFlowGraph::EdgeNode &edge) {
    std::string retval = vertexNameEnd(*edge.source()) + " ";
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
    return retval + "-> " + vertexName(*edge.target());
}

static bool
sortVerticesByAddress(const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &a,
                      const Partitioner::ControlFlowGraph::ConstVertexNodeIterator &b) {
    const Partitioner::CfgVertex &av = a->value();
    const Partitioner::CfgVertex &bv = b->value();
    if (av.type() != bv.type() || av.type() != Partitioner::V_BASICBLOCK)
        return av.type() < bv.type();
    return av.address() < bv.address();
}

static bool
sortEdgesBySrc(const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &a,
               const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->source(), b->source());
}

static bool
sortEdgesByDst(const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &a,
               const Partitioner::ControlFlowGraph::ConstEdgeNodeIterator &b) {
    return sortVerticesByAddress(a->target(), b->target());
}

void
Partitioner::dumpCfg(std::ostream &out, const std::string &prefix, bool showBlocks) const {
    // Sort the vertices according to basic block starting address.
    std::vector<ControlFlowGraph::ConstVertexNodeIterator> sortedVertices;
    for (ControlFlowGraph::ConstVertexNodeIterator vi=cfg_.vertices().begin(); vi!=cfg_.vertices().end(); ++vi) {
        if (vi->value().type() == V_BASICBLOCK)
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
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedInEdges)
            out <<" " <<edgeNameSrc(*edge);
        out <<"\n";

        // Show instructions in execution order
        if (showBlocks) {
            if (BasicBlock::Ptr bb = vertex->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    out <<prefix <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        }

        // Warn if instruction semantics failed
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (bb->finalState()==NULL)
                out <<prefix <<"  semantics failed\n";
        }

        // Stack delta
        if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            BaseSemantics::SValuePtr delta = bblockStackDelta(bb);
            if (delta == NULL) {
                out <<prefix <<"  stack delta: not available\n";
            } else {
                out <<prefix <<"  stack delta: " <<*delta <<"\n";
            }
        }
        
        // Sort outgoing edges according to destination (makes comparisons easier)
        std::vector<ControlFlowGraph::ConstEdgeNodeIterator> sortedOutEdges;
        for (ControlFlowGraph::ConstEdgeNodeIterator ei=vertex->outEdges().begin(); ei!=vertex->outEdges().end(); ++ei)
            sortedOutEdges.push_back(ei);
        std::sort(sortedOutEdges.begin(), sortedOutEdges.end(), sortEdgesByDst);
        out <<prefix <<"  successors:";
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeNodeIterator &edge, sortedOutEdges) {
            out <<" " <<edgeNameDst(*edge);
        }
        out <<"\n";
    }
}

Sawyer::Optional<rose_addr_t>
Partitioner::nextFunctionPrologue(rose_addr_t startVa) {
    while (memoryMap_.next(startVa, MemoryMap::MM_PROT_EXEC).assignTo(startVa)) {
        Sawyer::Optional<rose_addr_t> unmappedVa = aum_.leastUnmapped(startVa);
        if (!unmappedVa)
            return Sawyer::Nothing();                   // no higher unused address
        if (startVa == *unmappedVa) {
            BOOST_FOREACH (const FunctionPrologueMatcher::Ptr &matcher, functionPrologueMatchers_) {
                if (matcher->match(this, startVa))
                    return matcher->functionVa();
            }
            ++startVa;
        } else {
            startVa = *unmappedVa;
        }
    }
    return Sawyer::Nothing();
}

size_t
Partitioner::insertFunctions(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += insertFunction(function);
    return nNewBlocks;
}

size_t
Partitioner::insertFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);

    size_t nNewBlocks = 0;
    Function::Ptr exists;
    if (functions_.getOptional(function->address()).assignTo(exists)) {
        ASSERT_require2(&exists==&function, "conflicting " + functionName(function) + " already exists");
        ASSERT_require(function->isFrozen());
    } else {
        // Insert function into the table, and make sure all its blocks see that they're owned by the function.
        functions_.insert(function->address(), function);
        nNewBlocks = insertFunctionBlocks(function);

        // Prevent the function connectivity from changing while the function is in the CFG.  Non-frozen functions
        // can have basic blocks added and erased willy nilly because the basic blocks don't need to know that they're owned by
        // a function.  But we can't have that when the function is one that's part of the CFG.
        function->freeze();
    }
    return nNewBlocks;
}

size_t
Partitioner::insertFunctionBlocks(const Functions &functions) {
    size_t nNewBlocks = 0;
    BOOST_FOREACH (const Function::Ptr function, functions.values())
        nNewBlocks += insertFunctionBlocks(function);
    return nNewBlocks;
}

size_t
Partitioner::insertFunctionBlocks(const Function::Ptr &function) {
    ASSERT_not_null(function);
    size_t nNewBlocks = 0;
    bool functionExists = functions_.exists(function->address());
    BOOST_FOREACH (rose_addr_t blockVa, function->bblockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = placeholderExists(blockVa);
        if (placeholder == cfg_.vertices().end()) {
            placeholder = insertPlaceholder(blockVa);
            ++nNewBlocks;
        }
        if (functionExists) {
            ASSERT_require2(NULL==placeholder->value().function() || placeholder->value().function()==function,
                            functionName(function) +
                            " basic block " + StringUtility::addrToString(placeholder->value().address()) +
                            " is already owned by " + functionName(placeholder->value().function()));
            placeholder->value().function(function);
        }
    }
    return nNewBlocks;
}

void
Partitioner::eraseFunction(const Function::Ptr &function) {
    ASSERT_not_null(function);
    ASSERT_require(functionExists(function->address()));
    ASSERT_require(functionExists(function->address()) == function);
    static const Function::Ptr NO_FUNCTION;
    BOOST_FOREACH (rose_addr_t blockVa, function->bblockAddresses()) {
        ControlFlowGraph::VertexNodeIterator placeholder = placeholderExists(blockVa);
        ASSERT_require(placeholder != cfg_.vertices().end());
        ASSERT_require(placeholder->value().type() == V_BASICBLOCK);
        ASSERT_require(placeholder->value().function() == function);
        placeholder->value().function(NO_FUNCTION);
    }
    functions_.erase(function->address());
    function->thaw();
}

Partitioner::AddressUsageMap
Partitioner::aum(const Function::Ptr &function) const {
    AddressUsageMap retval;
    BOOST_FOREACH (rose_addr_t blockVa, function->bblockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = placeholderExists(blockVa);
        if (placeholder != cfg_.vertices().end()) {
            ASSERT_require(placeholder->value().type() == V_BASICBLOCK);
            if (BasicBlock::Ptr bb = placeholder->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
                    retval.insert(InsnBlockPair(insn, bb));
            }
        }
    }
    return retval;
}

Partitioner::Functions
Partitioner::discoverFunctionEntryVertices() const {
    Functions functions = functions_;                   // entry points we already know about

    // There are probably fewer vertices than edges, so iterate over them instead. This also allows us to check for duplicates
    // against "functions_", which is smaller (or at worst, equal in size) than the local "functions".
    BOOST_FOREACH (const ControlFlowGraph::VertexNode &vertex, cfg_.vertices()) {
        if (vertex.value().type() == V_BASICBLOCK && !functions_.exists(vertex.value().address())) {
            rose_addr_t entryVa = vertex.value().address();
            BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, vertex.inEdges()) {
                if (edge.value().type() == E_FCALL) {
                    functions.insert(entryVa, Function::instance(entryVa));
                    break;
                }
            }
        }
    }
    return functions;
}

size_t
Partitioner::discoverFunctionBlocks(const Function::Ptr &function,
                                    EdgeList *inwardInterFunctionEdges /*out*/,
                                    EdgeList *outwardInterFunctionEdges /*out*/) {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
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
Partitioner::discoverFunctionBlocks(const Function::Ptr &function,
                                    ConstEdgeList *inwardInterFunctionEdges /*out*/,
                                    ConstEdgeList *outwardInterFunctionEdges /*out*/) const {
    std::vector<size_t> inwardIds, outwardIds;
    size_t retval = discoverFunctionBlocks(function, inwardIds /*out*/, outwardIds /*out*/);
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
Partitioner::discoverFunctionBlocks(const Function::Ptr &function,
                                    std::vector<size_t> &inwardInterFunctionEdges /*out*/,
                                    std::vector<size_t> &outwardInterFunctionEdges /*out*/) const {
    ASSERT_not_null(function);
    ASSERT_forbid(function->isFrozen());
    Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"discoverFunctionBlocks(" <<functionName(function) <<")\n";
    size_t nConflictsOrig = inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size();

    typedef Sawyer::Container::Map<size_t /*vertexId*/, Function::Ownership> VertexOwnership;
    VertexOwnership ownership;                          // contains only OWN_EXPLICIT and OWN_PROVISIONAL entries

    // Find the vertices that this function already owns
    BOOST_FOREACH (rose_addr_t startVa, function->bblockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator placeholder = placeholderExists(startVa);
        ASSERT_require2(placeholder != cfg_.vertices().end(),
                        "block " + StringUtility::addrToString(startVa) + " of " + functionName(function) +
                        " must exist in the CFG");
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
            if (edge.value().type()!=E_FCALL && !edge.isSelfEdge()) {
                const ControlFlowGraph::VertexNode &target = *edge.target();
                if (target.value().type()==V_BASICBLOCK && !ownership.exists(target.id())) {
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
                function->insert(cfg_.findVertex(node.key())->value().address());
        }
    }

    return inwardInterFunctionEdges.size() + outwardInterFunctionEdges.size() - nConflictsOrig;
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

    ControlFlowGraph::ConstVertexNodeIterator bblockVertex = placeholderExists(bb->address());
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
                ASSERT_require(target.value().type() == V_BASICBLOCK);
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(target.value().address());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            }
        }
        ast->set_successors_complete(isComplete);
    } else {
        bool isComplete = true;
        BOOST_FOREACH (const BasicBlock::Successor &successor, bblockSuccessors(bb)) {
            if (successor.expr()->is_number()) {
                SgAsmIntegerValueExpression *succ = SageBuilderAsm::buildValueU64(successor.expr()->get_number());
                succ->set_parent(ast);
                ast->get_successors().push_back(succ);
            } else {
                isComplete = false;
            }
        }
        ast->set_successors_complete(isComplete);
    }
    return ast;
}

SgAsmFunction*
Partitioner::buildFunctionAst(const Function::Ptr &function, bool relaxed) const {
    // Build the child basic block IR nodes
    std::vector<SgAsmBlock*> children;
    BOOST_FOREACH (rose_addr_t blockVa, function->bblockAddresses()) {
        ControlFlowGraph::ConstVertexNodeIterator vertex = placeholderExists(blockVa);
        if (vertex == cfg_.vertices().end()) {
            mlog[WARN] <<functionName(function) <<" bblock "
                       <<StringUtility::addrToString(blockVa) <<" does not exist in the CFG; no AST node created\n";
        } else if (BasicBlock::Ptr bb = vertex->value().bblock()) {
            if (SgAsmBlock *child = buildBasicBlockAst(bb, relaxed))
                children.push_back(child);
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

    unsigned reasons = 0;

    // Is the function part of the CFG?
    ControlFlowGraph::ConstVertexNodeIterator entryVertex = placeholderExists(function->address());
    if (entryVertex != cfg_.vertices().end())
        reasons |= SgAsmFunction::FUNC_GRAPH;
    
    // Is the function discontiguous?
    if (aum(function).extent().nIntervals() > 1)
        reasons |= SgAsmFunction::FUNC_DISCONT;

    // Is the function the target of a function call?
    if (entryVertex != cfg_.vertices().end()) {
        BOOST_FOREACH (const ControlFlowGraph::EdgeNode &edge, entryVertex->inEdges()) {
            if (edge.value().type() == E_FCALL) {
                reasons |= SgAsmFunction::FUNC_CALL_TARGET;
                break;
            }
        }
    }
    
    // Build the AST
    SgAsmFunction *ast = SageBuilderAsm::buildFunction(function->address(), children);
    ast->set_reason(reasons);
    ast->set_name("");                                  // FIXME[Robb P. Matzke 2014-08-07]
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

std::ostream&
operator<<(std::ostream &out, const Partitioner::InsnBlockPair &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::InsnBlockPairs &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::AddressUsageMap &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::ControlFlowGraph::VertexNode &x) {
    out <<Partitioner::vertexName(x);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Partitioner::ControlFlowGraph::EdgeNode &x) {
    out <<Partitioner::edgeName(x);
    return out;
}

} // namespace
} // namespace
} // namespace
