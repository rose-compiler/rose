#include <sage3basic.h>

#include <BinaryBestMapAddress.h>
#include <CommandLine.h>
#include <integerOps.h>
#include <Sawyer/Graph.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/ThreadWorkers.h>

using namespace Rose::Diagnostics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility BestMapAddress::mlog;

// class method
void
BestMapAddress::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("Rose::BinaryAnalysis::BestMapAddress", Diagnostics::destination);
        mlog.comment("computing mem mapping based on insns");
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
        nBits_ = dis->instructionPointerRegister().nBits();
    } else if (dis->wordSizeBytes()*8 != nBits_) {
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
    Progress::Ptr progress;
    Sawyer::ProgressBar<size_t> &progressBar;

    Worker(BestMapAddress *self, const Progress::Ptr &progress, Sawyer::ProgressBar<size_t> &progressBar)
        : self(self), progress(progress), progressBar(progressBar) {}

    void operator()(size_t taskId, const Task &task) {
        const rose_addr_t mask = IntegerOps::genMask<rose_addr_t>(self->nBits());
        size_t nMatches = 0;
        BOOST_FOREACH (rose_addr_t a, self->entryAddresses().values()) {
            if (self->targetAddresses().exists((a + task.delta) & mask))
                ++nMatches;
        }
        task.result = nMatches;
        ++progressBar;
        if (progress)
            progress->update(progressBar.ratio());
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
    Sawyer::ProgressBar<size_t> progressBar(tasks.nVertices(), mlog[MARCH]);
    progressBar.suffix(" comparisons");
    Sawyer::workInParallel(tasks, Rose::CommandLine::genericSwitchArgs.threads, Worker(this, progress_, progressBar));

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

MemoryMap::Ptr
BestMapAddress::align(const MemoryMap::Ptr &map, const P2::Engine::Settings &settings, const Progress::Ptr &progress) {
    Sawyer::Message::Stream info(mlog[INFO]);
    if (info) {
        info <<"memory map before aligning segments:\n";
        map->dump(info, "  ");
    }

    MemoryMap::Ptr nonExecutable = map->shallowCopy();
    nonExecutable->require(MemoryMap::EXECUTABLE).prune();
    MemoryMap::Ptr retval = nonExecutable->shallowCopy();

    // Process one executable segment (plus all non-executable) at a time.
    BestMapAddress::AddressSet entryAddresses;
    size_t nWork = 0, totalWork = 2 * map->nSegments();
    BOOST_FOREACH (const MemoryMap::Node &mmNode, map->nodes()) {
        nWork += 2;                                     // incremented early in case of "continue" statements

        // What to align
        const AddressInterval &interval = mmNode.key();
        const MemoryMap::Segment &segment = mmNode.value();
        if (0 == (segment.accessibility() & MemoryMap::EXECUTABLE))
            continue;
        MemoryMap::Ptr tmpMap = map->shallowCopy();
        tmpMap->require(MemoryMap::EXECUTABLE).prune();
        tmpMap->insert(interval, segment);
        if (info) {
            info <<"aligning executable segment using this map:\n";
            tmpMap->dump(info, "  ");
        }

        // Partitioning engine used by the BestMapAddress analysis.
        P2::Engine engine(settings);
        engine.memoryMap(tmpMap);
        engine.doingPostAnalysis(false);
        if (progress)
            engine.progress(progress);

        // Analyze the tmpMap using the specified partitioning engine.
        BestMapAddress mapAnalyzer;
        mapAnalyzer.progress(progress);
        {
            ProgressTask t(progress, "disassemble", (double)(nWork-1) / totalWork);
            mapAnalyzer.gatherAddresses(engine);
        }
        mlog[INFO] <<"found " <<StringUtility::plural(mapAnalyzer.entryAddresses().size(), "entry addresses") <<" and "
                   <<StringUtility::plural(mapAnalyzer.targetAddresses().size(), "target addresses") <<"\n";
        BOOST_FOREACH (rose_addr_t entryVa, entryAddresses.values())
            mapAnalyzer.insertEntryAddress(entryVa);
        mlog[INFO] <<"using " <<StringUtility::plural(mapAnalyzer.entryAddresses().size(), "total entry addresses") <<"\n";
        info <<"performing remap analysis";
        Sawyer::Stopwatch remapTime;
        {
            ProgressTask t(progress, "remap", (double)(nWork-0) / totalWork);
            mapAnalyzer.analyze();
        }
        std::vector<rose_addr_t> deltas = mapAnalyzer.bestDeltas();
        info <<"; took " <<remapTime <<" seconds\n";
        mlog[INFO] <<"found " <<StringUtility::plural(deltas.size(), "deltas") <<" with match ratio "
                    <<(100.0*mapAnalyzer.bestDeltaRatio()) <<"%\n";

        // Find the best deltas by which to shift the executable segment. Try deltas in the order given until we find one that
        // shifts the executable segment to an area of memory where it doesn't overlap with any non-executable segments.
        // static const rose_addr_t mask = mapAnalyzer.mask();
        bool remapped = false;
        size_t bestDelta = 0;
        const rose_addr_t mask = mapAnalyzer.mask();
        BOOST_FOREACH (rose_addr_t delta, deltas) {
            // Check for overflow: we don't want a delta that would split the executable segment between the highest addresses
            // and the lowest addresses.
            rose_addr_t newLo = (interval.least() + delta) & mask;
            rose_addr_t newHi = (interval.greatest() + delta) & mask;
            if (newHi < newLo)
                continue;                               // overflow
            const AddressInterval newInterval = AddressInterval::hull(newLo, newHi);
            if (nonExecutable->within(newInterval).exists())
                continue;                               // new location would overlap with data segments

            if (0 == delta) {
                info <<"segment " <<StringUtility::addrToString(interval.least()) <<" is good where it is.\n";
            } else {
                info <<"segment " <<StringUtility::addrToString(interval.least())
                     <<" should move to " <<StringUtility::addrToString(newInterval.least()) <<"\n";
            }
            retval->insert(newInterval, segment);
            remapped = true;
            bestDelta = delta;
            break;
        }

        if (!remapped) {
            mlog[WARN] <<"cannot find a valid destination address for \"" <<StringUtility::cEscape(segment.name()) <<"\""
                       <<"; leaving at " <<StringUtility::addrToString(interval.least()) <<"\n";
            retval->insert(interval, segment);
        }

        // Add adjusted entry addresses to the set of all entry addresses.  This is optional, but sometimes helps the remap
        // analysis by giving it more information.
        BOOST_FOREACH (rose_addr_t origEntryVa, mapAnalyzer.entryAddresses().values()) {
            rose_addr_t adjustedEntryVa = (origEntryVa + bestDelta) & mask;
            entryAddresses.insert(adjustedEntryVa);
        }
    }
    return retval;
}
} // namespace
} // namespace
