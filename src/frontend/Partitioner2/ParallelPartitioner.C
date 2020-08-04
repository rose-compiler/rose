#include <rosePublicConfig.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L
#include <sage3basic.h>
#include <Partitioner2/ParallelPartitioner.h>

#include <BinaryBestMapAddress.h>
#include <BinaryDataFlow.h>
#include <BinarySymbolicExpr.h>
#include <boost/format.hpp>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/WorkList.h>
#include <sstream>
#include <stringify.h>
#include <unordered_set>
#include <BinaryUnparserBase.h>

using namespace Sawyer::Message::Common;
using namespace Rose::StringUtility;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Experimental {
namespace ParallelPartitioner {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diagnostics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Sawyer::Message::Facility mlog;

void initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Partitioner2::ParallelPartitioner");
        mlog.comment("decoding machine language instructions");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InsnInfo
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Optional<size_t>
InsnInfo::size() const {
    // No lock necessary since size_ doesn't change after wasDecoded returns true.
    if (wasDecoded()) {
        return size_;
    } else {
        return Sawyer::Nothing();
    }
}

Sawyer::Optional<AddressInterval>
InsnInfo::hull() const {
    if (wasDecoded()) {
        // No lock necessary since va_ and size_ don't change after wasDecoded returns true.
        return size_ > 0 ? AddressInterval::baseSize(va_, size_) : AddressInterval();
    } else {
        return Sawyer::Nothing();
    }
}

FunctionReasons
InsnInfo::functionReasons() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return functionReasons_;
}

void
InsnInfo::functionReasons(FunctionReasons reasons) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    functionReasons_ = reasons;
}

void
InsnInfo::insertFunctionReasons(FunctionReasons reasons) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    functionReasons_.set(reasons);
}

void
InsnInfo::eraseFunctionReasons(FunctionReasons reasons) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    functionReasons_.clear(reasons);
}

bool
InsnInfo::wasDecoded() const {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  return wasDecoded_;
}

void
InsnInfo::setDecoded() {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  if (ast_)
      size_ = ast_->get_size();
  wasDecoded_ = true;
}

InstructionPtr
InsnInfo::setAstMaybe(const InstructionPtr &insn) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (!wasDecoded_) {
        if (insn) {
            ASSERT_require(insn->get_address() == va_);
            size_ = insn->get_size();
        }
        ast_ = insn;
        wasDecoded_ = true;
    }
    return ast_;
}

InstructionPtr
InsnInfo::ast() const {
  SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
  ASSERT_require(wasDecoded_);
  return ast_;
}

// class method
uint64_t
InsnInfo::hash(const List &insns) {
    Combinatorics::HasherFnv hasher;
    for (auto &insnInfo: insns)
        hasher.insert(insnInfo->address());
    return hasher.partial();
}

// class method
bool
InsnInfo::addressOrder(const Ptr &a, const Ptr &b) {
    if (a == nullptr || b == nullptr)
        return a == nullptr && b != nullptr;
    return a->address() < b->address();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CfgEdge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, const CfgEdge &edge) {
    namespace Stringify = stringify::Rose::BinaryAnalysis::Partitioner2;
    out <<edge.types_.toString(Stringify::EdgeType(), static_cast<const char*(*)(int64_t)>(&Stringify::EdgeType));
    return out;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkItem
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
WorkItem::operator<(const WorkItem &other) const {
    if (priority_ != other.priority_)
        return priority_ < other.priority_;
    return sort_ < other.sort_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DecodeInstruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DecodeInstruction::run() {
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"decoding instruction at " <<addrToString(insnVa) <<"\n";

    InsnInfo::Ptr insnInfo = partitioner().existingInstruction(insnVa);
    if (!insnInfo) {
        SAWYER_MESG(mlog[DEBUG]) <<"cfg node at " <<addrToString(insnVa) <<" has been erased\n";
        return;
    }

    if (insnInfo->wasDecoded()) {
        SAWYER_MESG(mlog[DEBUG]) <<"insn at " <<addrToString(insnVa) <<" is already decoded\n";
        return;
    }

    // Decode the instruction and add it (or a null pointer) to the node in the CFG.
    InstructionPtr insn = partitioner().decodeInstruction(insnVa);
    if (partitioner().makeInstruction(insnVa, insn)->ast() != insn) {
        SAWYER_MESG(mlog[DEBUG]) <<"another thread just decoded " <<addrToString(insnVa) <<"\n";
        return;
    }

    // We're the thread responsible for decoding the instruction, so we're also responsible for making sure that instruction's
    // CFG successors exist. Beware: other threads might be adding these same vertices if they're reachable from other
    // instructions as well.
    if (insn) {
        auto successors = partitioner().computedConcreteSuccessors(insnVa, Accuracy::DEFAULT);
        bool isFunctionCall = partitioner().isFunctionCall(insnVa, Accuracy::DEFAULT);

        if (isFunctionCall) {
            // Add the edge from the call source to the return target.
            // FIXME[Robb Matzke 2020-07-08]: may-return analysis needed.
            rose_addr_t fallThroughVa = insnVa + insn->get_size();
            if (partitioner().createLinkedCfgVertices(insnVa, fallThroughVa, E_CALL_RETURN).createdTarget) {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(fallThroughVa)
                                         <<": assumed fcall-return\n";
                partitioner().scheduleDecodeInstruction(fallThroughVa);
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(fallThroughVa)
                                         <<": assumed fcall-return exists\n";
            }
        }

        // Create all the CFG edges emanating from this vertex.
        EdgeType edgeType = isFunctionCall ? E_FUNCTION_CALL : E_NORMAL;
        for (rose_addr_t successorVa: successors.values()) {
            if (partitioner().createLinkedCfgVertices(insnVa, successorVa, edgeType).createdTarget) {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(successorVa)
                                         <<": normal successor\n";
                partitioner().scheduleDecodeInstruction(successorVa);
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(successorVa)
                                         <<": normal successor exists\n";
            }
        }
    }

#if 0 // DEBUGGING [Robb Matzke 2020-07-10]
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"control flow graph after inserting:\n";
        partitioner().printInsnCfg(mlog[DEBUG]);
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NextUnusedRegion
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
NextUnusedRegion::run() {
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"find unused executable addresses in " <<addrToString(where) <<"\n";

    AddressIntervalSet found = partitioner().unusedExecutableVas(where);
    for (AddressInterval interval: found.intervals()) {
        SAWYER_MESG(mlog[DEBUG]) <<"next unused executable region is " <<addrToString(interval) <<"\n";
        if (interval.size() >= partitioner().settings().minHoleSearch) {
            partitioner().makeInstruction(interval.least());
            partitioner().scheduleDecodeInstruction(interval.least());
            if (interval.least() < interval.greatest())
                partitioner().scheduleNextUnusedRegion(AddressInterval::hull(interval.least() + 1, interval.greatest()));
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scheduler
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
WorkItemSorter::operator()(const std::shared_ptr<WorkItem> &a, const std::shared_ptr<WorkItem> &b) const {
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    return *a < *b;
}

void
Scheduler::insert(const Item &item) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    queue_.push(item);
}

bool
Scheduler::isEmpty() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return queue_.empty();
}

Scheduler::Item
Scheduler::next() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_forbid(queue_.empty());
    Item retval = queue_.top();
    queue_.pop();
    return retval;
}

// std::priority_queue (and other adaptors) have a protected member "c" which is the underlying container that's being
// adapted.
template <class Item, class Container, class Comparator>
static const Container&
adaptorContainer(const std::priority_queue<Item, Container, Comparator>& queue) {
    struct SubQueue: private std::priority_queue<Item, Container, Comparator> {
        static const Container&
        container(const std::priority_queue<Item, Container, Comparator>& queue) {
            return queue.*&SubQueue::c;
        }
    };
    return SubQueue::container(queue);
}

void
Scheduler::reportStatus(std::ostream &out) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    const Container &items = adaptorContainer(queue_);
    out <<StringUtility::plural(items.size(), " work items") <<" pending\n";

    static const size_t nShow = 20; // max number of tasks to show
    Queue q = queue_; // yep, slow but this is only for debugging
    for (size_t i = 0; i < nShow && !q.empty(); ++i) {
        out <<(boost::format("  %2d: %s\n") % i % q.top()->title());
        q.pop();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Partitioner::Partitioner(const MemoryMap::Ptr &memory, Disassembler *decoder, const Settings &settings)
    : settings_(settings), nExeVas_(0), isRunning_(false) {
    insnCache_ = std::make_shared<InstructionCache>(memory, decoder);

    // For progress reporting, count the total bytes of executable memory.
    progress_ = Progress::instance();
    for (auto &node: memory->nodes()) {
        if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
            nExeVas_ += node.key().size();
    }
}

Accuracy
Partitioner::choose(Accuracy a, Accuracy b) {
    if (a != Accuracy::DEFAULT)
        return a;
    if (b != Accuracy::DEFAULT)
        return b;
    return Accuracy::LOW;
}

MemoryMap::Ptr
Partitioner::memoryMap() const {
    return instructionCache().memoryMap();
}

size_t
Partitioner::nDecodedAddresses() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return aum_.size();
}

Progress::Ptr
Partitioner::progress() const {
    // No lock needed since the pointer never changes
    return progress_;
}

void
Partitioner::statusReports() {
    while (!progress_->isFinished()) {
        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        scheduler_.reportStatus(std::cerr);
    }
}

AddressIntervalSet
Partitioner::unusedExecutableVas(AddressInterval where) const {
    // The return value is the gaps in the AUM, intersected with the executable memory addresses, intersected with this
    // function's argument. But we want to be much smarter than computing all three and then intersecting them because that
    // would take a long time.
    AddressIntervalSet retval;
    if (where.isEmpty())
        return retval;

    AddressInterval unused;             // bucket of unused addresses, subset of "where"
    AddressInterval executable;         // bucket of executable addresses, subset of "where"

    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);

    while (true) {
        // Fill the bucket of unused addresses
        if ((unused & where).isEmpty()) {
            unused = aum_.firstUnmapped(where.least()) & where;
            if (unused.isEmpty())
                return retval;
        }

        // Fill the bucket of executable addresses
        if ((executable & where).isEmpty()) {
            executable = memoryMap()->atOrAfter(unused.least()).require(MemoryMap::EXECUTABLE).available() & where;
            if (executable.isEmpty())
                return retval;
        }

        // Add overlapping unused and executable to the return value and advance.
        AddressInterval both = unused & executable;
        if (!both.isEmpty())
            retval.insert(both);
        rose_addr_t lastSearched = std::min(unused.greatest(), executable.greatest());
        if (lastSearched == where.greatest())
            return retval;
        where = AddressInterval::hull(lastSearched + 1, where.greatest());
    }
    return retval;
}

InstructionPtr
Partitioner::decodeInstruction(rose_addr_t insnVa) {
    return instructionCache().get(insnVa);
}

InsnInfo::Ptr
Partitioner::makeInstruction(rose_addr_t insnVa) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto vertex = insnCfg_.findVertexKey(insnVa);
    if (vertex == insnCfg_.vertices().end()) {
        SAWYER_MESG(mlog[DEBUG]) <<"adding instruction " <<addrToString(insnVa) <<" to CFG\n";
        vertex = insnCfg_.insertVertex(std::make_shared<InsnInfo>(insnVa));
    }
    return vertex->value();
}

InsnInfo::Ptr
Partitioner::makeInstruction(rose_addr_t insnVa, const InstructionPtr &insn /*null*/) {
    ASSERT_require(!insn || insn->get_address() == insnVa);
    InsnInfo::Ptr insnInfo = makeInstruction(insnVa);
    ASSERT_not_null(insnInfo);
    if (insnInfo->setAstMaybe(insn) == insn) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        aum_.insert(insnInfo->hull().get(), insnInfo->address());
        ASSERT_require(nExeVas_ > 0); // because we know the instruction we inserted is from executable memory
        progress_->update(aum_.size() / (double)nExeVas_);
    }
    return insnInfo;
}

InsnInfo::Ptr
Partitioner::existingInstruction(rose_addr_t insnVa) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto vertex = insnCfg_.findVertexKey(insnVa);
    return vertex == insnCfg_.vertices().end() ? std::shared_ptr<InsnInfo>() : vertex->value();
}

InstructionPtr
Partitioner::existingInstructionAst(rose_addr_t insnVa) {
    if (auto insnInfo = existingInstruction(insnVa))
        return insnInfo->ast();
    return InstructionPtr();
}

InstructionCache&
Partitioner::instructionCache() const {
    // no lock necessary since this pointer can never change
    return *insnCache_;
}

Partitioner::LockInCache
Partitioner::lockInCache(const InsnInfo::List &insns) {
    LockInCache retval;
    retval.locks.reserve(insns.size());
    retval.insns.reserve(insns.size());
    for (auto &insnInfo: insns) {
        retval.locks.push_back(instructionCache().lock(insnInfo->address()));
        retval.insns.push_back(retval.locks.back().get());
    }
    return retval;
}

InsnInfo::List
Partitioner::basicBlockEndingAt(rose_addr_t va, size_t maxInsns) const {
    InsnInfo::List insns;
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        auto vertex = insnCfg_.findVertexKey(va);
        if (vertex != insnCfg_.vertices().end()) {
            std::set<rose_addr_t> seen;
            while (insns.size() < maxInsns) {
                if (!seen.insert(vertex->value()->address()).second)
                    break;
                insns.push_back(vertex->value());
                if (vertex->nInEdges() != 1)
                    break;
                vertex = vertex->inEdges().begin()->source();
                if (vertex->nOutEdges() != 1)
                    break;
            }
        }
    }
    std::reverse(insns.begin(), insns.end());
    return insns;
}

InsnInfo::List
Partitioner::basicBlockContaining(rose_addr_t va) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    InsnInfo::List retval;
    auto startVertex = insnCfg_.findVertexKey(va);
    std::set<rose_addr_t> seen;
    if (startVertex != insnCfg_.vertices().end()) {
        // Starting with the specified va, scan backward as far as possible.
        auto vertex = startVertex;
        while (true) {
            if (!seen.insert(vertex->value()->address()).second)
                break;
            retval.push_back(vertex->value());
            if (vertex->nInEdges() != 1)
                break;
            vertex = vertex->inEdges().begin()->source();
            if (vertex->nOutEdges() != 1)
                break;
        }
        std::reverse(retval.begin(), retval.end());

        // While possibele, scan forward from the specified va.
        vertex = startVertex;
        while (vertex->nOutEdges() == 1) {
            vertex = vertex->outEdges().begin()->target();
            if (vertex->nInEdges() != 1)
                break;
            if (!seen.insert(vertex->value()->address()).second)
                break;
            retval.push_back(vertex->value());
        }
    }
    return retval;
}

Borrowed<CachedItem<Semantics::RiscOperatorsPtr, uint64_t>>
Partitioner::basicBlockSemantics(const InsnInfo::List &insns) {
    Semantics::RiscOperatorsPtr ops;
    ASSERT_forbid(insns.empty());

    // Do we already have semantics for some prefix of these instructions? A common case for this call is that we've already
    // computed the semantics for a basic block, but now we're adding one additional instruction to the block.
    InsnInfo::List prefix = insns;
    while (!prefix.empty()) {
        auto key = InsnInfo::hash(prefix);
        if (auto cached = prefix.back()->cached().semantics.take(key)) {
            ops = *cached;
            break;
        }
        prefix.pop_back();
    }

    // We might have already computed the semantics for this same requested block.
    if (prefix.size() == insns.size())
        return borrow(insns.back()->cached().semantics, InsnInfo::hash(insns), ops);

    // If we don't know the semantics for any prefix, then create a new initial state and start from the beginning. Otherwise,
    // we have two choices: we could copy the semantics and add the after-prefix instructions, or we could just take the state
    // and modify it. We do the latter because it's unlikely that anyone's going to ever again ask for semantics for only the
    // first part of this basic block--they'll always ask for the whole block.
    if (prefix.empty()) {
        SmtSolver::Ptr solver; // FIXME[Robb Matzke 2020-07-29]: use a solver for semantics? Probably not needed.
        const RegisterDictionary *regdict = instructionCache().decoder()->registerDictionary();
        ops = Semantics::RiscOperators::instance(regdict, solver, settings_.semanticMemoryParadigm);
        BaseSemantics::MemoryStatePtr mem = ops->currentState()->memoryState();
        if (auto ml = boost::dynamic_pointer_cast<Semantics::MemoryListState>(mem)) {
            ml->memoryMap(memoryMap());
        } else if (auto mm = boost::dynamic_pointer_cast<Semantics::MemoryMapState>(mem)) {
            mm->memoryMap(memoryMap());
        }
    } else {
        // FIXME[Robb Matzke 2020-07-29]: Is this going to cause problems? Is some other thread using the old state still?
        prefix.back()->cached().semantics.reset();
    }

    // Add the non-prefix instructions to the semantic state. If there's a semantic failure then erase the current state (it would
    // be wrong anyway. If there is no current state then there's no point in processing the instruction.
    if (ops->currentState()) {
        if (BaseSemantics::DispatcherPtr protoCpu = instructionCache().decoder()->dispatcher()) {
            BaseSemantics::DispatcherPtr cpu = protoCpu->create(ops);
            for (auto insn = insns.begin() + prefix.size(); insn != insns.end(); ++insn) {
                try {
                    cpu->processInstruction((*insn)->ast().lock().get());
                } catch (...) {
                    ops->currentState(Semantics::StatePtr());
                    break;
                }
            }
        } else {
            ops->currentState(Semantics::StatePtr());
        }
    }

    return borrow(insns.back()->cached().semantics, InsnInfo::hash(insns), ops);
}

std::vector<SymbolicExpr::Ptr>
Partitioner::splitSuccessors(const BaseSemantics::RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ops->currentState());
    std::vector<SymbolicExpr::Ptr> retval;
    const RegisterDescriptor IP = instructionCache().decoder()->instructionPointerRegister();
    BaseSemantics::SValuePtr dfltIp = ops->undefined_(IP.nBits());
    SymbolicExpr::Ptr ip = Semantics::SValue::promote(ops->peekRegister(IP, dfltIp))->get_expression();
    if (ip->isIntegerConstant() || ip->isIntegerVariable()) {
        retval.push_back(ip);
    } else if (SymbolicExpr::InteriorPtr inode = ip->isInteriorNode()) {
        if (inode->getOperator() == SymbolicExpr::OP_ITE) {
            if (inode->child(0)->isIntegerConstant())
                retval.push_back(inode->child(0));
            if (inode->child(1)->isIntegerConstant())
                retval.push_back(inode->child(1));
            if (retval.empty())
                retval.push_back(SymbolicExpr::makeIntegerVariable(IP.nBits()));
        } else if (inode->getOperator() == SymbolicExpr::OP_SET) {
            for (SymbolicExpr::Ptr member: inode->children()) {
                if (member->isIntegerConstant())
                    retval.push_back(member);
            }
            if (retval.empty())
                retval.push_back(SymbolicExpr::makeIntegerVariable(IP.nBits()));
        } else {
            retval.push_back(SymbolicExpr::makeIntegerVariable(IP.nBits()));
        }
    }
    return retval;
}

std::vector<SymbolicExpr::Ptr>
Partitioner::computeSuccessors(const InsnInfo::List &insns, Accuracy accuracy) {
    accuracy = choose(accuracy, settings_.successorAccuracy);
    std::vector<SymbolicExpr::Ptr> retval;
    const RegisterDescriptor IP = instructionCache().decoder()->instructionPointerRegister();

    if (Accuracy::HIGH == accuracy) {
        auto borrowedOps = basicBlockSemantics(insns);
        Semantics::RiscOperatorsPtr ops = borrowedOps.get().orElse(nullptr);
        if (ops && ops->currentState())
            return splitSuccessors(ops);
    }

    // Low accuracy (whether requested or because high accuracy is unavailable)
    if (!insns.empty()) {
        ASSERT_not_null(insns.back());
        bool complete = true;
        AddressSet succs = insns.back()->ast().lock().get()->getSuccessors(complete /*out*/);
        for (rose_addr_t va: succs.values())
            retval.push_back(SymbolicExpr::makeIntegerConstant(IP.nBits(), va));
        if (!complete)
            retval.push_back(SymbolicExpr::makeIntegerVariable(IP.nBits()));
    }

    ASSERT_forbid(retval.empty());
    return retval;
}



std::vector<SymbolicExpr::Ptr>
Partitioner::computeSuccessors(rose_addr_t insnVa, Accuracy accuracy) {
    accuracy = choose(accuracy, settings_.successorAccuracy);
    InsnInfo::List insns;
    switch (accuracy) {
        case Accuracy::HIGH: {
            InsnInfo::List insns = basicBlockEndingAt(insnVa, settings_.maxAnalysisBBlockSize);
            // Once we hit the max basic block size we avoid using high accuracy because it becomes inefficient. Here's why...
            // Blocks are typically extended one instruction at a time, and adding an instruction to a small block is efficient
            // because we just take the previous instruction's semantics and modify it with the new instruction, then cache it
            // at the new instruction.  But when the basic block size surpasses the max analysis size, then the InsnInfo::List
            // will no longer include the first instructions, and therefore the hash will no longer match the cached semantic
            // hash. This is correct behavior since the semantics of instructions 0..N is different than 1..N+1. However, it
            // means that the semantics for 1..N+1 need to be created from scratch by processing all N instructions. This
            // happens each time the basic block is extended by one instruction.  To avoid this bad behavior, if
            // basicBlockEndingAt returns a block of the maximum size (since we can't tell if the block could have been even
            // bigger) we switch to LOW accuracy mode which doesn't require semantics.
            return computeSuccessors(insns, insns.size() == settings_.maxAnalysisBBlockSize ? Accuracy::LOW : Accuracy::HIGH);
        }
        case Accuracy::LOW:
            if (InsnInfo::Ptr insnInfo = existingInstruction(insnVa)) {
                insns.push_back(insnInfo);
                return computeSuccessors(insns, Accuracy::LOW);
            } else {
                return {};
            }
        default:
            ASSERT_not_reachable("invalid accuracy");
    }
}

AddressSet
Partitioner::computedConcreteSuccessors(rose_addr_t insnVa, Accuracy accuracy) {
    accuracy = choose(accuracy, settings_.successorAccuracy);
    switch (accuracy) {
        case Accuracy::LOW: {
            if (InstructionPtr insn = existingInstructionAst(insnVa)) {
                bool complete = true;
                return insn->getSuccessors(complete/*out*/);
            } else {
                return AddressSet();
            }
        }

        case Accuracy::HIGH: {
            AddressSet retval;
            std::vector<SymbolicExpr::Ptr> symbolicSuccessors = computeSuccessors(insnVa, accuracy);
            for (SymbolicExpr::Ptr expr: symbolicSuccessors) {
                if (auto ival = expr->toUnsigned())
                    retval.insert(*ival);
            }
            return retval;
        }

        default:
            ASSERT_not_reachable("invalid accuracy");
    }
}

bool
Partitioner::isFunctionCall(rose_addr_t insnVa, Accuracy accuracy) {
    accuracy = choose(accuracy, settings_.functionCallDetectionAccuracy);
    switch (accuracy) {
        case Accuracy::LOW: {
            // No need to cache this because it's already fast.
            if (InstructionPtr insn = existingInstructionAst(insnVa)) {
                auto locked = insn.lock();
                std::vector<SgAsmInstruction*> tmp{locked.get()};
                return insn->isFunctionCallFast(tmp, nullptr, nullptr);
            } else {
                return false;
            }
        }

        case Accuracy::HIGH: {
            InsnInfo::List insns = basicBlockEndingAt(insnVa, settings_.maxAnalysisBBlockSize);
            if (insns.empty())
                return false;
            auto key = InsnInfo::hash(insns);

            // Is the result already available in the CFG?
            if (auto cached = insns.back()->cached().isFunctionCall.get(key))
                return *cached;

            auto locked = lockInCache(insns);
            bool retval = locked.insns.front()->isFunctionCallFast(locked.insns, nullptr, nullptr);
            insns.back()->cached().isFunctionCall.set(key, retval);
            return retval;
        }

        default:
            ASSERT_not_reachable("invalid accuracy");
    }
}

Partitioner::CreateLinkedCfgVertices
Partitioner::createLinkedCfgVertices(rose_addr_t srcVa, rose_addr_t tgtVa, const CfgEdge &edgeInfo) {
    CreateLinkedCfgVertices retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto src = insnCfg_.findVertexKey(srcVa);
    if (src == insnCfg_.vertices().end()) {
        retval.createdSource = true;
        retval.source = std::make_shared<InsnInfo>(srcVa);
        src = insnCfg_.insertVertex(retval.source);
    } else {
        retval.createdSource = false;
        retval.source = src->value();
    }

    auto tgt = insnCfg_.findVertexKey(tgtVa);
    if (tgt == insnCfg_.vertices().end()) {
        retval.createdTarget = true;
        retval.target = std::make_shared<InsnInfo>(tgtVa);
        tgt = insnCfg_.insertVertex(retval.target);
    } else {
        retval.createdTarget = false;
        retval.target = tgt->value();
    }

    bool edgeFound = false;
    for (auto &edge: src->outEdges()) {
        if (edge.target() == tgt) {
            edgeFound = true;
            retval.createdEdge = false;
            edge.value().merge(edgeInfo);
            break;
        }
    }
    if (!edgeFound) {
        retval.createdEdge = true;
        insnCfg_.insertEdge(src, tgt, edgeInfo);
    }
    return retval;
}

void
Partitioner::scheduleDecodeInstruction(rose_addr_t insnVa) {
    scheduler_.insert(std::make_shared<DecodeInstruction>(*this, insnVa));
}

void
Partitioner::scheduleNextUnusedRegion(const AddressInterval &where) {
    scheduler_.insert(std::make_shared<NextUnusedRegion>(*this, where));
}

void
Partitioner::run(size_t maxWorkers) {
    struct Resources {
        Partitioner &p;
        explicit Resources(Partitioner &p)
            : p(p) {
            p.isRunning(true);
        }
        ~Resources() {
            p.isRunning(false);
        }
    } r(*this);

    //boost::thread(asyncStatusReports, this).detach();
    Sawyer::processWorkList(scheduler_, maxWorkers, Worker());
    progress_->finished();
}

bool
Partitioner::isRunning() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return isRunning_;
}

void
Partitioner::isRunning(bool b) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_require(isRunning_ != b);
    isRunning_ = b;
}

const InsnCfg&
Partitioner::insnCfg() const {
    // No synchronization necessary since this function is documented as not thread safe. The assert is only a half-baked check
    // prone to races.
    ASSERT_forbid(isRunning());
    return insnCfg_;
}

InsnCfg&
Partitioner::insnCfg() {
    ASSERT_forbid2(isRunning(), "not thread safe");
    return insnCfg_;
}

void
Partitioner::printInsnCfg(std::ostream &out) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    out <<"digraph cfg {\n";
    for (auto &vertex: insnCfg_.vertices()) {
        out <<"I" <<StringUtility::addrToString(vertex.value()->address()).substr(2) <<";\n";
    }
    for (auto &edge: insnCfg_.edges()) {
        out <<"I" <<StringUtility::addrToString(edge.source()->value()->address()).substr(3)
            <<" -> I" <<StringUtility::addrToString(edge.target()->value()->address()).substr(2) <<";\n";
    }
    out <<"}\n";
}

void
Partitioner::dumpInsnCfg(std::ostream &out, const Rose::BinaryAnalysis::Partitioner2::Partitioner &p) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    Unparser::Base::Ptr unparser = insnCache_->decoder()->unparser();
    Rose::BinaryAnalysis::Partitioner2::Partitioner emptyPartitioner{};

    for (auto vertex: insnCfg_.vertices()) {
        out <<"vertex #" <<vertex.id() <<" ";
        if (vertex.value()->wasDecoded()) {
            if (auto ast = vertex.value()->ast()) {
                out <<p.unparse(ast.lock().get()) <<"\n";
            } else {
                out <<addrToString(vertex.value()->address()) <<": no instruction\n";
            }
        } else {
            out <<addrToString(vertex.value()->address()) <<": not decoded yet\n";
        }

        for (auto edge: vertex.inEdges())
            out <<"  from " <<addrToString(edge.source()->value()->address())
                <<" via " <<edge.value() <<" #" <<edge.id() <<"\n";
        for (auto edge: vertex.outEdges())
            out <<"  to   " <<addrToString(edge.target()->value()->address())
                <<" via " <<edge.value() <<" #" <<edge.id() <<"\n";
    }
}

std::map<rose_addr_t /*insn*/, rose_addr_t /*bb*/>
Partitioner::calculateInsnToBbMap() const {
    ASSERT_forbid2(isRunning(), "not thread safe");
    std::map<rose_addr_t, rose_addr_t> retval;
    for (auto &vertex: insnCfg_.vertices()) {
        if (retval.find(vertex.value()->address()) == retval.end()) {
            InsnInfo::List bb = basicBlockContaining(vertex.value()->address());
            ASSERT_forbid(bb.empty());
            rose_addr_t bbVa = bb.front()->address();
            for (auto &insnInfo: bb)
                retval[insnInfo->address()] = bbVa;
        }
    }
    return retval;
}

std::vector<InsnInfo::List>
Partitioner::allBasicBlocks() const {
    ASSERT_forbid2(isRunning(), "not thread safe");
    std::vector<InsnInfo::List> retval;

    std::unordered_set<rose_addr_t> seen;
    for (auto &vertex: insnCfg_.vertices()) {
        if (!seen.insert(vertex.value()->address()).second)
            continue;

        InsnInfo::List bb = basicBlockContaining(vertex.value()->address());
        for (auto &insnInfo: bb)
            seen.insert(insnInfo->address());
        retval.push_back(bb);
    }
    return retval;
}

// class method
bool
Partitioner::addressOrder(const InsnInfo::List &a, const InsnInfo::List &b) {
    if (a.empty() || b.empty())
        return a.empty() && !b.empty();
    return InsnInfo::addressOrder(a.front(), b.front());
}

rose_addr_t
Partitioner::remap() {
    ASSERT_forbid2(isRunning(), "not thread safe");
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    debug <<"remapping memory\n";

    BestMapAddress bma;
    bma.settings().nThreads = 0;                        // all hardware threads
    bma.nBits(instructionCache().decoder()->instructionPointerRegister().nBits());
    for (auto vertex: insnCfg_.vertices()) {
        if (vertex.value()->functionReasons().isAnySet()) {
            bma.insertEntryAddress(vertex.value()->address());
        }
        for (auto edge: vertex.outEdges()) {
            if (edge.value().types().isAnySet(E_FUNCTION_CALL | E_FUNCTION_XFER))
                bma.insertTargetAddress(edge.target()->value()->address());
        }
    }

    std::vector<rose_addr_t> deltas = bma.analyze().bestDeltas();
    std::sort(deltas.begin(), deltas.end());
    if (!deltas.empty()) {
        if (debug) {
            Diagnostics::mfprintf(debug)("remap deltas (%6.2%% accuracy):", 100.0 * bma.bestDeltaRatio());
            for (rose_addr_t va: deltas)
                debug <<" " <<StringUtility::addrToString(va, bma.nBits());
            debug <<"\n";
        }
        return deltas[0];
    }

    return 0;
}

void
Partitioner::transferResults(Rose::BinaryAnalysis::Partitioner2::Partitioner &out) {
    ASSERT_forbid2(isRunning(), "not thread safe");

    // Create the basic blocks
    std::vector<InsnInfo::List> basicBlocks = allBasicBlocks();
    for (auto &insns: basicBlocks) {
        ASSERT_forbid(insns.empty());
        mlog[DEBUG] <<"attaching basic block " <<addrToString(insns.front()->address()) <<"\n";

        // A basic block could end with an instruction that doesn't exist (its address is unmapped or not executable). The
        // serial partitioner doesn't handle this, so we simply drop those instructions.
        while (!insns.empty() && !insns.back()->ast())
            insns.pop_back();
        if (insns.empty())
            continue;

        // Create the basic block for the serial partitioner.
        // FIXME[Robb Matzke 2020-07-09]: This seems to be very slow.
        auto bblock = BasicBlock::instance(insns.front()->address(), out);
        for (auto &insnInfo: insns)
            bblock->append(out, insnInfo->ast().take());

        // Create the basic block successors. The serial partitioner uses Semantics::SValues to store the address, so we need
        // to jump through some hoops since our edges don't store this information. Fortunately, most successors are constants
        // that we can obtain directly from the CFG edges. The only extra one we need is if there's a branch to an
        // indeterminate address.
        auto borrowedOps = basicBlockSemantics(insns);
        BaseSemantics::RiscOperatorsPtr ops = borrowedOps.get().orElse(nullptr);
        ASSERT_not_null(ops);
        const RegisterDescriptor IP = instructionCache().decoder()->instructionPointerRegister();
        auto vertex = insnCfg_.findVertexKey(insns.back()->address());
        ASSERT_require(vertex != insnCfg_.vertices().end());
        for (auto edge: vertex->outEdges()) {
            rose_addr_t targetVa = edge.target()->value()->address();
            BaseSemantics::SValuePtr targetExpr = ops->number_(IP.nBits(), targetVa);
            edge.value().types().each([targetExpr, bblock](EdgeType et) {
                    bblock->insertSuccessor(targetExpr, et);
                });
        }
        if (ops->currentState()) {
            std::vector<SymbolicExpr::Ptr> successors = splitSuccessors(ops);
            for (auto edge: successors) {
                if (!edge->isIntegerConstant()) {
                    BaseSemantics::SValuePtr targetExpr = ops->undefined_(IP.nBits());
                    bblock->insertSuccessor(targetExpr, E_NORMAL);
                }
            }
        } else {
            bool complete;
            insns.back()->ast().lock().get()->getSuccessors(complete /*out*/);
            if (!complete)
                bblock->insertSuccessor(ops->undefined_(IP.nBits()), E_NORMAL);
        }

        out.detachBasicBlock(bblock);
        out.attachBasicBlock(bblock);
    }

    // Create the functions
    std::map<rose_addr_t /*func*/, AddressSet /*insns*/> fa = assignFunctions();
    std::map<rose_addr_t /*insn*/, rose_addr_t /*bb*/> bb = calculateInsnToBbMap();
    for (auto node: fa) {
        rose_addr_t funcVa = node.first;
        const AddressSet insnVas = node.second;
        AddressSet bbVas;
        for (rose_addr_t insnVa: insnVas.values())
            bbVas.insert(bb.at(insnVa));

        InsnInfo::Ptr funcEntry = existingInstruction(funcVa);
        auto function = Function::instance(funcVa, funcEntry->functionReasons().vector());
        for (rose_addr_t bbVa: bbVas.values())
            function->insertBasicBlock(bbVa);
        out.attachOrMergeFunction(function);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function assignment ("Fa") assigns each instruction to a specific function.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Dataflow state stored at each CFG vertex. The state consists of all the function entry points that can reach this vertex
// via CFG edges, and the length of the shortest such path.
struct FaState {
    using Map = std::map<rose_addr_t /*funcEntryVa*/, size_t /*minPathLength*/>;
    Map reachableFrom;                                  // which function entry point can reach this vertex, and shortest path
    bool isEntryPoint;                                  // is this vertex a function entry point

    FaState()
        : isEntryPoint(false) {}

    // Mark a function entry point
    explicit FaState(rose_addr_t va)
        : isEntryPoint(true) {
        reachableFrom[va] = 0;
    }

    std::string toString() const {
        std::ostringstream ss;
        if (isEntryPoint)
            ss <<"entry-point";
        for (auto &node: reachableFrom)
            ss <<(ss.str().empty() ? "" : ", ") <<addrToString(node.first) <<"+" <<node.second;
        return ss.str();
    }
};

struct FaTransfer {
    FaState operator()(const InsnCfg&, size_t vertexId, const FaState &state) const {
        FaState next = state;
        for (auto &node: next.reachableFrom)
            ++node.second;
        return next;
    }

    std::string toString(const FaState &state) const {
        return state.toString();
    }
};

struct FaMerge {
    bool operator()(FaState &a, const FaState &b) const {
        bool changed = false;
        for (auto &bNode: b.reachableFrom) {
            auto result = a.reachableFrom.insert(bNode);
            if (result.second) {
                changed = true;
            } else if (bNode.second < result.first->second) {
                result.first->second = bNode.second;
                changed = true;
            }
        }
        ASSERT_forbid(a.reachableFrom.empty());
        return changed;
    }
};

struct FaEdgePredicate {
    bool operator()(const InsnCfg&, const InsnCfg::Edge &edge, const FaState&/*incoming*/, const FaState &outgoing) {
        // Don't follow edges that that are related to function calls.
        if (edge.value().types().isAnySet(E_FUNCTION_CALL | E_FUNCTION_RETURN | E_FUNCTION_XFER))
            return false;

        // Don't follow edges into a function entry point.
        return !outgoing.isEntryPoint;
    }
};

std::map<rose_addr_t /*funcVa*/, AddressSet /*insnVas*/>
Partitioner::assignFunctions() {
    ASSERT_forbid2(isRunning(), "not thread safe");
    Sawyer::Message::Stream debug(mlog[DEBUG]);
#if 1 // DEBUGGING [Robb Matzke 2020-07-30]
    debug.enable();
#endif
    debug <<"assigning blocks to functions...\n";
    Sawyer::Stopwatch timer;

    // Create the dataflow engine. For each CFG vertex that has a non-empty function reason, initialize its state in the data
    // flow and mark it as a starting point. As a side effect, if any vertex has an incoming edge that would cause the vertex
    // to be a function entry point, then mark it as such.
    using DfEngine = Rose::BinaryAnalysis::DataFlow::Engine<InsnCfg, FaState, FaTransfer, FaMerge, FaEdgePredicate>;
    FaTransfer xfer;
    DfEngine dfEngine(insnCfg_, xfer, FaMerge(), FaEdgePredicate());
    size_t nFunctions = 0;
    for (auto vertex: insnCfg_.vertices()) {
        if (vertex.value()->functionReasons().isClear(SgAsmFunction::FUNC_CALL_TARGET)) {
            for (auto edge: vertex.inEdges()) {
                if (edge.value().types().isAnySet(E_FUNCTION_CALL | E_FUNCTION_XFER))
                    vertex.value()->insertFunctionReasons(SgAsmFunction::FUNC_CALL_TARGET);
            }
        }

        if (!vertex.value()->functionReasons().isEmpty()) {
            dfEngine.insertStartingVertex(vertex.id(), FaState(vertex.value()->address()));
            ++nFunctions;
        }
    }
    debug <<"dataflow is starting with " <<StringUtility::plural(nFunctions, "functions") <<"\n";

    // Run until a fixed point is reached. We've set up the problem so that we're guaranteed to reach a fixed point. Once this
    // completes, every vertex reachable from a function entry point will have a non-empty set of function owners, and vertices
    // that are not reachable will have an empty set.
    dfEngine.runToFixedPoint();
    debug <<"dataflow reached fixed point after " <<StringUtility::plural(dfEngine.nIterations(), "steps") <<"\n";

    // FIXME[Robb Matzke 2020-07-08]: What to do about unowned instructions?  Maybe we don't care about them because we have
    // more work to do? Or should we create additional functions to which they should be assigned? If so, what addresses should
    // we use as the function entry points?
    if (mlog[ERROR] || debug) {
        size_t nUnassigned = 0;
        for (size_t i = 0; i < insnCfg_.nVertices(); ++i) {
            if (dfEngine.getInitialState(i).reachableFrom.empty() && dfEngine.getInitialState(i).isEntryPoint) {
                ++nUnassigned;
                //debug <<"CFG vertex " <<addrToString(insnCfg_.findVertex(i)->value()->address()) <<" is not in any function\n";
            }
        }
        if (nUnassigned > 0)
            mlog[ERROR] <<StringUtility::plural(nUnassigned, "instructions") <<" are not assigned to any function\n";
    }

    // Check how many functions are at addresses that are not mapped or not executable. If there's a lot, then it might be
    // because the specimen is mapped at the wrong address and all the function calls (which typically have absolute targets)
    // are therefore even pointing at instructions.
    if (mlog[WARN] || debug) {
        size_t nEmptyFuncs = 0;
        for (auto &vertex: insnCfg_.vertices()) {
            if (vertex.value()->functionReasons().isAnySet() && vertex.value()->wasDecoded() && !vertex.value()->ast())
                ++nEmptyFuncs;
        }
        double emptyRate = nFunctions > 0 ? 1.0 * nEmptyFuncs / nFunctions : 0.0;
        if (emptyRate > 0.05 /*arbitrary*/) {
            SAWYER_MESG_OR(mlog[WARN], debug)
                <<boost::format("possible specimen mapping error: %6.2f%% of functions are at invalid memory\n")
                %(100*emptyRate);
#if 0 // [Robb Matzke 2020-07-31]
            SAWYER_MESG_OR(mlog[WARN], debug) <<"looking for a better address at which to map the specimen...\n";
            if (rose_addr_t shift = remap()) {
                size_t nBits = instructionCache().decoder()->instructionPointerRegister().nBits();
                mlog[WARN] <<"results may improve if you shift the specimen by "
                           <<StringUtility::addrToString(shift, nBits) <<StringUtility::plural(shift, "bytes") <<" in memory\n";
            } else {
                mlog[WARN] <<"unable to find any better mapping address\n";
            }
#endif
        }
    }

    // Assign each vertex to the function with the closest entry point, if any.
    std::map<rose_addr_t /*funcVa*/, AddressSet /*insnVas*/> retval;
    for (size_t i = 0; i < insnCfg_.nVertices(); ++i) {
        const FaState &state = dfEngine.getInitialState(i);
        if (!state.reachableFrom.empty()) {
            std::vector<std::pair<rose_addr_t, size_t> > funcs(state.reachableFrom.begin(), state.reachableFrom.end());
            std::nth_element(funcs.begin(), funcs.begin(), funcs.end(),
                             [](std::pair<rose_addr_t, size_t> &a, std::pair<rose_addr_t, size_t> &b) {
                                 return a.second < b.second;
                             });
            rose_addr_t funcVa = funcs[0].first;
            rose_addr_t insnVa = insnCfg_.findVertex(i)->value()->address();
            retval[funcVa].insert(insnVa);
        }
    }

    debug <<"assigning blocks to functions; took " <<timer <<" seconds\n";
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace
} // namespace
#endif
