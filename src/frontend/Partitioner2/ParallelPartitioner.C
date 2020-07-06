#include <rosePublicConfig.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L
#include <sage3basic.h>
#include <Partitioner2/ParallelPartitioner.h>

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/WorkList.h>
#include <unordered_set>

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

size_t
InsnInfo::size() const {
    ASSERT_require(wasDecoded());
    // No lock necessary since size_ doesn't change after wasDecoded returns true.
    return size_;
}

AddressInterval
InsnInfo::hull() const {
    ASSERT_require(wasDecoded());
    // No lock necessary since va_ and size_ don't change after wasDecoded returns true.
    return size_ > 0 ? AddressInterval::baseSize(va_, size_) : AddressInterval();
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
        auto successors = partitioner().computedConcreteSuccessors(insnVa);
        for (rose_addr_t successorVa: successors.values()) {
            if (partitioner().createLinkedCfgVertices(insnVa, successorVa).createdTarget) {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(successorVa)
                                         <<": normal successor\n";
                partitioner().scheduleDecodeInstruction(successorVa);
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(successorVa)
                                         <<": normal successor exists\n";
            }
        }

        if (partitioner().isFunctionCall(insnVa)) {
            rose_addr_t fallThroughVa = insnVa + insn->get_size();
            if (partitioner().createLinkedCfgVertices(insnVa, fallThroughVa).createdTarget) {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(fallThroughVa)
                                         <<": assumed fcall-return\n";
                partitioner().scheduleDecodeInstruction(fallThroughVa);
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"edge " <<addrToString(insnVa) <<" -> " <<addrToString(fallThroughVa)
                                         <<": assumed fcall-return exists\n";
            }
        }
    }
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
    auto vertex = cfg_.findVertexKey(insnVa);
    if (vertex == cfg_.vertices().end()) {
        SAWYER_MESG(mlog[DEBUG]) <<"adding instruction " <<addrToString(insnVa) <<" to CFG\n";
        vertex = cfg_.insertVertex(std::make_shared<InsnInfo>(insnVa));
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
        aum_.insert(insnInfo->hull(), insnInfo->address());
        ASSERT_require(nExeVas_ > 0); // because we know the instruction we inserted is from executable memory
        progress_->update(aum_.size() / (double)nExeVas_);
    }
    return insnInfo;
}

InsnInfo::Ptr
Partitioner::existingInstruction(rose_addr_t insnVa) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto vertex = cfg_.findVertexKey(insnVa);
    return vertex == cfg_.vertices().end() ? std::shared_ptr<InsnInfo>() : vertex->value();
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
Partitioner::basicBlockEndingAt(rose_addr_t va, size_t maxInsns) {
    InsnInfo::List insns;
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        auto vertex = cfg_.findVertexKey(va);
        while (vertex != cfg_.vertices().end() && insns.size() < maxInsns) {
            insns.push_back(vertex->value());
            if (vertex->nInEdges() != 1)
                break;
            vertex = vertex->inEdges().begin()->source();
        }
    }
    std::reverse(insns.begin(), insns.end());
    return insns;
}

InsnInfo::List
Partitioner::basicBlockContaining(rose_addr_t va) const {
    InsnInfo::List insns;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto startVertex = cfg_.findVertexKey(va);

    // First scan backward, including startVertex
    for (auto vertex = startVertex; vertex != cfg_.vertices().end(); vertex = vertex->inEdges().begin()->source()) {
        insns.push_back(vertex->value());
        if (vertex->nInEdges() != 1)
            break;
    }
    std::reverse(insns.begin(), insns.end());

    // Then scan forward
    for (auto vertex = startVertex; vertex != cfg_.vertices().end(); vertex = vertex->outEdges().begin()->target()) {
        if (vertex->nOutEdges() != 1)
            break;
        insns.push_back(vertex->outEdges().begin()->target()->value());
    }

    return insns;
}

AddressSet
Partitioner::computedConcreteSuccessors(rose_addr_t insnVa) {
    switch (settings_.successorAccuracy) {
        case Accuracy::LOW: {
            // No need to cache this because it's already fast.
            if (InstructionPtr insn = existingInstructionAst(insnVa)) {
                bool complete = true;
                return insn->getSuccessors(complete/*out*/);
            } else {
                return AddressSet();
            }
        }

        case Accuracy::HIGH: {
            InsnInfo::List insns = basicBlockEndingAt(insnVa, settings_.maxAnalysisBBlockSize);
            if (insns.empty())
                return AddressSet();
            auto key = InsnInfo::hash(insns);

            // Are successors already cached for this set of instruction addresses?
            if (auto cached = insns.back()->cached().computedConcreteSuccessors.get(key))
                return *cached;

            // Calculate the successors and cache them.
            auto locked = lockInCache(insns);
            bool complete = true;
            AddressSet successors = locked.insns.front()->getSuccessors(locked.insns, complete/*out*/, memoryMap());
            insns.back()->cached().computedConcreteSuccessors.set(key, successors);
            return successors;
        }
    }
    ASSERT_not_reachable("invalid successor accuracy setting");
}

bool
Partitioner::isFunctionCall(rose_addr_t insnVa) {
    switch (settings_.functionCallDetectionAccuracy) {
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
    }
    ASSERT_not_reachable("invalid function call accuracy setting");
}

Partitioner::CreateLinkedCfgVertices
Partitioner::createLinkedCfgVertices(rose_addr_t srcVa, rose_addr_t tgtVa) {
    CreateLinkedCfgVertices retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto src = cfg_.findVertexKey(srcVa);
    if (src == cfg_.vertices().end()) {
        retval.createdSource = true;
        retval.source = std::make_shared<InsnInfo>(srcVa);
        src = cfg_.insertVertex(retval.source);
    } else {
        retval.createdSource = false;
        retval.source = src->value();
    }

    auto tgt = cfg_.findVertexKey(tgtVa);
    if (tgt == cfg_.vertices().end()) {
        retval.createdTarget = true;
        retval.target = std::make_shared<InsnInfo>(tgtVa);
        tgt = cfg_.insertVertex(retval.target);
    } else {
        retval.createdTarget = false;
        retval.target = tgt->value();
    }

    bool edgeFound = false;
    for (auto &edge: src->outEdges()) {
        if (edge.target() == tgt) {
            edgeFound = true;
            retval.createdEdge = false;
            break;
        }
    }
    if (!edgeFound) {
        retval.createdEdge = true;
        cfg_.insertEdge(src, tgt);
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

const Cfg&
Partitioner::cfg() const {
    // No synchronization necessary since this function is documented as not thread safe. The assert is only a half-baked check
    // prone to races.
    ASSERT_forbid(isRunning());
    return cfg_;
}

Cfg&
Partitioner::cfg() {
    ASSERT_forbid2(isRunning(), "not thread safe");
    return cfg_;
}

void
Partitioner::printCfg(std::ostream &out) const {
    ASSERT_forbid2(isRunning(), "not thread safe");
    out <<"graph {\n";
    for (auto &vertex: cfg_.vertices()) {
        out <<"  node " <<vertex.id() <<"\n";
    }
    for (auto &edge: cfg_.edges()) {
        out <<" edge " <<edge.source()->id() <<" -> " <<edge.target()->id() <<"\n";
    }
    out <<"}\n";
}


std::vector<InsnInfo::List>
Partitioner::allBasicBlocks() const {
    ASSERT_forbid2(isRunning(), "not thread safe");
    std::vector<InsnInfo::List> retval;

    std::unordered_set<rose_addr_t> seen;
    for (auto &vertex: cfg_.vertices()) {
        if (!seen.insert(vertex.value()->address()).second)
            continue;

        InsnInfo::List bb = basicBlockContaining(vertex.value()->address());
        for (auto &insnInfo: bb)
            seen.insert(insnInfo->address());
        retval.push_back(bb);
    }

#if 1 // DEBUGGING
    {
        Sawyer::Container::Map<rose_addr_t/*insn*/, rose_addr_t/*bb*/> map;
        for (const InsnInfo::List &list: retval) {
            ASSERT_forbid(list.empty());
            for (const InsnInfo::Ptr &insn: list) {
                ASSERT_not_null(insn);
                ASSERT_require(insn->wasDecoded());
                ASSERT_forbid2(map.exists(insn->address()),
                               "insn " + addrToString(insn->address()) + " in bb " + addrToString(list.front()->address()) +
                               " was already seen in bb " + addrToString(map.get(insn->address())));
                map.insert(insn->address(), list.front()->address());
            }
        }
    }
#endif

    return retval;
}

// class method
bool
Partitioner::addressOrder(const InsnInfo::List &a, const InsnInfo::List &b) {
    if (a.empty() || b.empty())
        return a.empty() && !b.empty();
    return InsnInfo::addressOrder(a.front(), b.front());
}

void
Partitioner::transferResults(Rose::BinaryAnalysis::Partitioner2::Partitioner &out) const {
    ASSERT_forbid2(isRunning(), "not thread safe");
    std::vector<InsnInfo::List> basicBlocks = allBasicBlocks();

    for (auto &insns: basicBlocks) {
        ASSERT_forbid(insns.empty());
        mlog[DEBUG] <<"attaching basic block " <<addrToString(insns.front()->address()) <<"\n";
        auto bblock = BasicBlock::instance(insns.front()->address(), out);
        for (auto &insnInfo: insns)
            bblock->append(out, insnInfo->ast().take());
        out.detachBasicBlock(bblock);
        out.attachBasicBlock(bblock);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
} // namespace
#endif
