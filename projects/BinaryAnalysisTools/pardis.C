#include <featureTests.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L

static const char *purpose = "experimental parallel disassembly";
static const char *description =
    "Simple tool to try some parallel disassembly ideas.";

// ROSE headers. Don't use <rose/...> because that's broken for programs distributed as part of ROSE.
#include <rose.h>                                       // must be first ROSE header
#include <Color.h>
#include <CommandLine.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/ParallelPartitioner.h>
#include <rose_strtoull.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace PP = Rose::BinaryAnalysis::Partitioner2::Experimental::ParallelPartitioner;

Sawyer::Message::Facility mlog;

// Basic initialization of parallel partitioner
void
initializeParallelPartitioner(PP::Partitioner &pp) {
    pp.scheduleNextUnusedRegion(pp.memoryMap()->hull());
}

// Initialize the parallel partitioner from a serial partitioner.
void
initializeParallelPartitioner(PP::Partitioner &pp, P2::Partitioner &p) {
    Sawyer::Stopwatch timer;
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    initializeParallelPartitioner(pp);

    info <<"inserting starting points from old partitioner";
#if 1
    for (auto function: p.functions()) {
        debug <<"inserting starting point " <<StringUtility::addrToString(function->address()) <<" as function\n";
        PP::InsnInfo::Ptr insnInfo = pp.makeInstruction(function->address());
        insnInfo->functionReasons(function->reasons());
        pp.scheduleDecodeInstruction(function->address());
    }
#else
    for (auto cfgVertex: p.cfg().vertices()) {
        if (auto addr = cfgVertex.value().optionalAddress()) {
            debug <<"insert starting point " <<StringUtility::addrToString(*addr) <<" from CFG\n";
            pp.makeInstruction(*addr);
            pp.scheduleDecodeInstruction(*addr);
        }
    }
#endif
    info <<"; done\n";

    info <<"inserting function prologue patterns";
    for (rose_addr_t searchVa = 0; true; ++searchVa) {
        auto functions = p.nextFunctionPrologue(searchVa, searchVa /*out*/);
        if (functions.empty())
            break;
        for (auto function: functions) {
            debug <<"insert staring point " <<StringUtility::addrToString(function->address())
                  <<" from function prologue matcher\n";
            pp.makeInstruction(function->address());
            pp.scheduleDecodeInstruction(function->address());
        }
        if (searchVa == p.memoryMap()->hull().greatest())
            break;
    }
    info <<"; done\n";
}

// Initialize the parallel partitioner by reading addresses from a file, one per line.
void
initailizeParallelPartitioner(PP::Partitioner &pp, std::istream &in) {
    initializeParallelPartitioner(pp);
    std::string line;
    while (std::getline(std::cin, line)) {
        rose_addr_t va = rose_strtoull(line.c_str(), nullptr, 0);
        pp.makeInstruction(va);
        pp.scheduleDecodeInstruction(va);
    }
}

// Run the partitioner
void
runPartitioner(PP::Partitioner &pp) {
    Sawyer::Message::Stream info(mlog[INFO]);
    size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    if (0 == nThreads)
        nThreads = boost::thread::hardware_concurrency();
    info <<"starting disassembly at " <<StringUtility::plural(pp.insnCfg().nVertices(), "addresses") <<"\n";

    info <<"disassembling with " <<StringUtility::plural(nThreads, "threads");
    Sawyer::Stopwatch timer;
    pp.run(nThreads);
    info <<"; took " <<timer <<" seconds\n";
    info <<"CFG now has " <<StringUtility::plural(pp.insnCfg().nVertices(), "instructions") <<"\n";
}

std::vector<PP::InsnInfo::Ptr>
insnsByAddr(PP::Partitioner &pp) {
    std::vector<PP::InsnInfo::Ptr> insns;
    insns.reserve(pp.insnCfg().nVertices());
    for (auto cfgVertex: pp.insnCfg().vertices())
        insns.push_back(cfgVertex.value());
    std::sort(insns.begin(), insns.end(), PP::InsnInfo::addressOrder);
    return insns;
}

std::vector<SgAsmInstruction*>
insnsByAddr(P2::Partitioner &p) {
    std::vector<SgAsmInstruction*> insns;
    for (auto &vertex: p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            if (P2::BasicBlock::Ptr bb = vertex.value().bblock()) {
                for (auto insn: bb->instructions())
                    insns.push_back(insn);
            }
        }
    }
    std::sort(insns.begin(), insns.end(), [](SgAsmInstruction *a, SgAsmInstruction *b) {
            return a->get_address() < b->get_address();
        });
    insns.erase(std::unique(insns.begin(), insns.end(),
                            [](SgAsmInstruction *a, SgAsmInstruction *b) {
                                return a->get_address() == b->get_address();
                            }),
                insns.end());
    return insns;
}

// Print the CFG instructions in address order.
void
printCfgInstructions(PP::Partitioner &pp, P2::Partitioner &p) {
    for (auto &insnInfo: insnsByAddr(pp)) {
        LockedInstruction lock = insnInfo->ast().lock();
        SgAsmInstruction *insn = lock.get();
        std::cout <<p.unparse(insn) <<"\n";
    }
}

// Print all instructions linearly, and highlight the ones that appear in the CFG
void
printAllInstructions(PP::Partitioner &pp, P2::Partitioner &p) {
    using namespace Rose::StringUtility;
    std::string green = "\033[38;2;" + Rose::Color::HSV(0.3, 1.0, 0.4).toAnsi();
    std::string red = "\033[38;2;" + Rose::Color::HSV(0.0, 1.0, 0.4).toAnsi();
    std::string endColor = "\033[0m";

    auto cfgInsns = insnsByAddr(pp);
    std::reverse(cfgInsns.begin(), cfgInsns.end()); // so we can pop_back instead of erase

    // Where are the executable addresses?
    MemoryMap::Ptr memory = pp.memoryMap()->shallowCopy();
    memory->require(MemoryMap::EXECUTABLE).keep();
    AddressInterval where = memory->hull();

    while (!where.isEmpty() || !cfgInsns.empty()) {
        Sawyer::Optional<rose_addr_t> maxPrintedVa;

        // Print some instructions from the parallel disassembler
        while (!cfgInsns.empty() && (where.isEmpty() || cfgInsns.back()->address() <= where.least())) {
            rose_addr_t cfgVa = cfgInsns.back()->address();
            if (!cfgInsns.back()->wasDecoded()) {
                std::cout <<green <<addrToString(cfgVa) <<": not decoded" <<endColor <<"\n";
                maxPrintedVa = cfgInsns.back()->address();
            } else if (auto insn = cfgInsns.back()->ast()) {
                std::cout <<green <<p.unparse(insn.lock().get()) <<endColor <<"\n";
                maxPrintedVa = cfgInsns.back()->hull().get().greatest();
            } else {
                std::cout <<green <<addrToString(cfgVa) <<": invalid address" <<endColor <<"\n";
                maxPrintedVa = cfgInsns.back()->address();
            }
            cfgInsns.pop_back();
        }

        // Prune the region of old instructons to print based on new instructions already printed
        if (!where.isEmpty() && maxPrintedVa) {
            if (*maxPrintedVa >= where.greatest()) {
                where = AddressInterval();
            } else if (*maxPrintedVa >= where.least()) {
                where = AddressInterval::hull(*maxPrintedVa + 1, where.greatest());
            }
        }

        // Print linear instructions up to next CFG instruction
        while (!where.isEmpty() && (cfgInsns.empty() || where.least() < cfgInsns.back()->address())) {
            if (auto insn = pp.instructionCache().get(where.least())) {
                std::cout <<red <<p.unparse(insn.lock().get()) <<endColor <<"\n";
                maxPrintedVa = insn->get_address() + insn->get_size() - 1;
            } else {
                std::cout <<red <<addrToString(where.least()) <<": invalid address" <<endColor <<"\n";
                maxPrintedVa = where.least();
            }

            if (*maxPrintedVa >= where.greatest()) {
                where = AddressInterval();
            } else if (*maxPrintedVa >= where.least()) {
                where = AddressInterval::hull(*maxPrintedVa + 1, where.greatest());
            }
        }
    }
}

// Compare instructions from two different disassemblers, giving precedence to those from the first partitioner.
void
printInsnsFromBoth(PP::Partitioner &pp, P2::Partitioner &p) {
    using namespace Rose::StringUtility;
    const std::string green = "\033[38;2;" + Rose::Color::HSV(0.3, 1.0, 0.4).toAnsi();
    const std::string red = "\033[38;2;" + Rose::Color::HSV(0.0, 1.0, 0.4).toAnsi();
    const std::string endColor = "\033[0m";

    std::vector<PP::InsnInfo::Ptr> insns1 = insnsByAddr(pp);
    std::reverse(insns1.begin(), insns1.end()); // so we can pop_back instead of erase
    std::vector<SgAsmInstruction*> insns2 = insnsByAddr(p);
    std::reverse(insns2.begin(), insns2.end());

    while (!insns1.empty() || !insns2.empty()) {
        std::string color;
        SgAsmInstruction *insn = nullptr;
        rose_addr_t va = 0;
        LockedInstruction insnLock;

        if (insns1.empty()) {
            color = "S " + red;                         // serial partitioner only
            insn = insns2.back();
            va = insn->get_address();
            insns2.pop_back();
        } else if (insns2.empty()) {
            color = "P " + green;                       // parallel partitioner only
            insnLock = insns1.back()->ast().lock();
            insn = insnLock.get();
            va = insns1.back()->address();
            insns1.pop_back();
        } else if (insns2.back()->get_address() < insns1.back()->address()) {
            color = "S " + red;                         // serial partitioner only
            insn = insns2.back();
            va = insn->get_address();
            insns2.pop_back();
        } else if (insns1.back()->address() < insns2.back()->get_address()) {
            color = "P " + green;                       // parallel partitioner only
            insnLock = insns1.back()->ast().lock();
            insn = insnLock.get();
            va = insns1.back()->address();
            insns1.pop_back();
        } else {
            ASSERT_require(insns1.back()->address() == insns2.back()->get_address());
            color = "B ";                               // both partitioners
            insnLock = insns1.back()->ast().lock();
            insn = insnLock.get();
            va = insns1.back()->address();
            insns1.pop_back();
            insns2.pop_back();
        }

        std::cout <<color <<(insn ? p.unparse(insn) : addrToString(va)+": invalid memory") <<endColor <<"\n";
    }
}
    
void
asyncProgressReporting(Progress::Ptr &progress, Sawyer::ProgressBar<double> *bar) {
    ASSERT_not_null(bar);
    progress->reportRegularly(boost::chrono::seconds(1),
                              [&bar](const Progress::Report &rpt, double age) -> bool {
                                  bar->value(100.0 * rpt.completion);
                                  return true; // keep listening until task is finished
                              });
    mlog[MARCH] <<"disassembly is finished at " <<(100 * progress->reportLatest().first.completion) <<" percent coverage\n";
}

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("parallel disassembly");
    Sawyer::Stopwatch timer;

    // Get, parse, and load the specimen.
    mlog[INFO] <<"parsing container\n";
    P2::Engine engine;
    std::vector<std::string> specimenName = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    MemoryMap::Ptr memory = engine.loadSpecimens(specimenName);
    Disassembler *decoder = engine.obtainDisassembler();

    // Create the parallel partitioner
    PP::Settings ppSettings;
    ppSettings.maxAnalysisBBlockSize = 20;
    ppSettings.successorAccuracy = PP::Accuracy::HIGH;
    ppSettings.functionCallDetectionAccuracy = PP::Accuracy::HIGH;
    ppSettings.minHoleSearch = 128;
    PP::Partitioner pp(memory, decoder, ppSettings);

#if 1 // [Robb Matzke 2020-07-30]
    mlog[INFO] <<"searching for starting points (serial)\n";
    P2::Partitioner p = engine.createPartitioner();
    engine.runPartitionerInit(p);
    initializeParallelPartitioner(pp, p);
#elif 1
    mlog[INFO] <<"adding memory starting points\n";
    initializeParallelPartitioner(pp);
    P2::Partitioner p = engine.createPartitioner();
#else
    mlog[INFO] <<"reading start points from standard input";
    initializeParallelPartitioner(pp, std::cin);
    mlog[INFO] <<"; done\n";
#endif

#if 0
    // Write starting points to stdout
    for (auto vertex: pp.insnCfg().vertices())
        std::cout <<StringUtility::addrToString(vertex.value()->address()) <<"\n";
#endif

    mlog[INFO] <<"parallel disassembly phase";
    Sawyer::ProgressBar<double> bar(100.0, mlog[MARCH], "disassembly");
    bar.suffix(" percent");
    boost::thread(asyncProgressReporting, pp.progress(), &bar).detach();
    timer.restart();
    runPartitioner(pp);
    mlog[INFO] <<"; took " <<timer <<" seconds\n";
    //pp.dumpInsnCfg(std::cerr, p);

#if 0 // [Robb Matzke 2020-07-30]
    std::map<rose_addr_t, AddressSet> functions = pp.assignFunctions();
    for (auto node: functions) {
        std::cout <<"Function " <<StringUtility::addrToString(node.first) <<"\n";
        for (rose_addr_t va: node.second.values()) {
            std::cout <<"  " <<StringUtility::addrToString(va) <<"\n";
        }
    }
#endif

#if 0
    mlog[INFO] <<"generating output\n";
    printCfgInstructions(pp, p);
#elif 0
    mlog[INFO] <<"generating output\n";
    pp.printCfg(std::cout);
#elif 1
    mlog[INFO] <<"transfering results to serial partitioner";
    timer.restart();
    pp.transferResults(p);
    mlog[INFO] <<"; took " <<timer <<" seconds\n";
    mlog[INFO] <<"generating output\n";
    //printInsnsFromBoth(pp, p);
    engine.savePartitioner(p, "x.rba");

#else
    mlog[INFO] <<"running serial partitioner";
    timer.restart();
    engine.doingPostAnalysis(false);
    P2::Partitioner p2 = engine.partition(specimenName);
    mlog[INFO] <<"; took " <<timer <<" seconds\n";

    mlog[INFO] <<"generating output\n";
    printInsnsFromBoth(pp, p2);
#endif
}

#else

#include <iostream>

int main(int argc, char *argv[]) {
    std::cerr <<argv[0] <<": not supported in this ROSE configuration\n";
    return 1;
}

#endif
