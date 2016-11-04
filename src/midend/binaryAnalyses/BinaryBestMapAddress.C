#include <sage3basic.h>

#include <BinaryBestMapAddress.h>
#include <integerOps.h>
#include <Sawyer/Graph.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/ThreadWorkers.h>

using namespace rose::Diagnostics;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

namespace rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility BestMapAddress::mlog;

// class method
void
BestMapAddress::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::BestMapAddress", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

void
BestMapAddress::nBits(size_t n) {
    ASSERT_require(n > 0);
    ASSERT_require(n <= 8*sizeof(rose_addr_t));
    nBits_ = n;
}

rose_addr_t
BestMapAddress::mask() const {
    if (0 == nBits_)
        throw Exception("no address space size specified yet");
    return IntegerOps::genMask<rose_addr_t>(nBits_);
}

void
BestMapAddress::gatherAddresses(P2::Engine &engine) {
    Disassembler *dis = engine.obtainDisassembler();
    if (0 == nBits_) {
        if (!dis)
            throw Exception("no disassembler");
        nBits_ = dis->instructionPointerRegister().get_nbits();
    } else if (dis->get_wordsize() != nBits_) {
        throw Exception("mismatched address sizes");
    }

    // Disassemble and partition within the restricted region
    P2::Partitioner partitioner = engine.createPartitioner();
    engine.runPartitioner(partitioner);

    // Scan for and insert two lists of addresses
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            if (P2::BasicBlock::Ptr bb = vertex.value().bblock()) {

                // Entry address list
                if (vertex.value().isEntryBlock() && !bb->isEmpty())
                    entryVas_.insert(bb->address());

                // Target address list
                if (partitioner.basicBlockIsFunctionCall(bb)) {
                    BOOST_FOREACH (rose_addr_t target, partitioner.basicBlockConcreteSuccessors(bb)) {
                        if (target != bb->fallthroughVa())
                            targetVas_.insert(target);
                    }
                }
            }
        }
    }
}

// Describes *what* a worker thread works on
struct Task {
    rose_addr_t delta;
    size_t &result;

    Task(rose_addr_t delta, size_t &result)
        : delta(delta), result(result) {}
};

// Describes *how* the worker does its job
struct Worker {
    BestMapAddress *self;
    Sawyer::ProgressBar<size_t> &progress;

    Worker(BestMapAddress *self, Sawyer::ProgressBar<size_t> &progress)
        : self(self), progress(progress) {}

    void operator()(size_t taskId, const Task &task) {
        const rose_addr_t mask = IntegerOps::genMask<rose_addr_t>(self->nBits());
        size_t nMatches = 0;
        BOOST_FOREACH (rose_addr_t a, self->entryAddresses().values()) {
            if (self->targetAddresses().exists((a + task.delta) & mask))
                ++nMatches;
        }
        task.result = nMatches;
        ++progress;
    }
};

BestMapAddress&
BestMapAddress::analyze(const AddressInterval &restrictEntryAddresses, const AddressInterval &restrictTargetAddresses) {
    // Build a table that contains all possible deltas.
    const rose_addr_t mask = IntegerOps::genMask<rose_addr_t>(nBits_);
    std::set<rose_addr_t> deltaSet;
    maxMatches_ = 0;
    BOOST_FOREACH (rose_addr_t entryVa, entryVas_.values()) {
        if (restrictEntryAddresses.isContaining(entryVa)) {
            ++maxMatches_;
            BOOST_FOREACH (rose_addr_t targetVa, targetVas_.values()) {
                if (restrictTargetAddresses.isContaining(targetVa))
                    deltaSet.insert((targetVa - entryVa) & mask);
            }
        }
    }
    std::vector<rose_addr_t> deltas(deltaSet.begin(), deltaSet.end());

    // Build worker thread tasks, each of which determines how many call targets would resolve to function entries if the
    // function entries were shifted by some delta.
    std::vector<size_t> nMatches(deltas.size(), 0);
    Sawyer::Container::Graph<Task> tasks;
    for (size_t i=0; i<deltas.size(); ++i)
        tasks.insertVertex(Task(deltas[i], nMatches[i]));
    Sawyer::ProgressBar<size_t> progress(tasks.nVertices(), mlog[MARCH]);
    Sawyer::workInParallel(tasks, CommandlineProcessing::genericSwitchArgs.threads, Worker(this, progress));

    // Sort and cache the results by number of matches.
    upToDate_ = false;
    results_.clear();
    for (size_t i=0; i<deltas.size(); ++i)
        results_.insertMaybeDefault(nMatches[i]).push_back(deltas[i]);
    upToDate_ = true;

    return *this;
}

double
BestMapAddress::bestDeltaRatio() const {
    if (!upToDate_)
        throw Exception("call BestMapAddress::analyze first");
    if (results_.isEmpty())
        return NAN;
    return (double)results_.greatest() / maxMatches_;
}

const std::vector<rose_addr_t>&
BestMapAddress::bestDeltas() const {
    if (!upToDate_)
        throw Exception("call BestMapAddress::analyze first");
    static const std::vector<rose_addr_t> empty;
    return results_.isEmpty() ? empty : results_[results_.greatest()];
}

} // namespace
} // namespace
