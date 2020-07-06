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
    for (auto cfgVertex: p.cfg().vertices()) {
        if (auto addr = cfgVertex.value().optionalAddress()) {
            debug <<"insert starting point " <<StringUtility::addrToString(*addr) <<" from CFG\n";
            pp.makeInstruction(*addr);
            pp.scheduleDecodeInstruction(*addr);
        }
    }
    info <<"; done\n";

    info <<"inserting function prologue patterns";
    auto functions = p.nextFunctionPrologue(0);
    for (auto function: functions) {
        debug <<"insert staring point " <<StringUtility::addrToString(function->address())
              <<" from function prologue matcher\n";
        pp.makeInstruction(function->address());
        pp.scheduleDecodeInstruction(function->address());
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
    info <<"starting disassembly at " <<StringUtility::plural(pp.cfg().nVertices(), "addresses") <<"\n";

    info <<"disassembling with " <<StringUtility::plural(nThreads, "threads");
    Sawyer::Stopwatch timer;
    pp.run(nThreads);
    info <<"; took " <<timer <<" seconds\n";
    info <<"CFG now has " <<StringUtility::plural(pp.cfg().nVertices(), "instructions") <<"\n";
}

std::vector<PP::InsnInfo::Ptr>
insnsByAddr(PP::Partitioner &pp) {
    std::vector<PP::InsnInfo::Ptr> insns;
    insns.reserve(pp.cfg().nVertices());
    for (auto cfgVertex: pp.cfg().vertices())
        insns.push_back(cfgVertex.value());
    std::sort(insns.begin(), insns.end(), PP::InsnInfo::addressOrder);
    return insns;
}

std::vector<SgAsmInstruction*>
insnsByAddr(P2::Partitioner &p) {
    std::vector<SgAsmInstruction*> insns;
    for (auto &vertex: p.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertex.value().bblock();
            for (auto insn: bb->instructions())
                insns.push_back(insn);
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
                maxPrintedVa = cfgInsns.back()->hull().greatest();
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
    std::string green = "\033[38;2;" + Rose::Color::HSV(0.3, 1.0, 0.4).toAnsi();
    std::string red = "\033[38;2;" + Rose::Color::HSV(0.0, 1.0, 0.4).toAnsi();
    std::string endColor = "\033[0m";

    auto insns1 = insnsByAddr(pp);
    std::reverse(insns1.begin(), insns1.end()); // so we can pop_back instead of erase
    auto insns2 = insnsByAddr(p);
    std::reverse(insns2.begin(), insns2.end());

    while (!insns1.empty() || !insns2.empty()) {
        Sawyer::Optional<rose_addr_t> maxPrintedVa;
        if (!insns1.empty() && (insns2.empty() || insns1.back()->address() <= insns2.back()->get_address())) {
            auto insn1 = insns1.back();
            if (!insn1->wasDecoded()) {
                std::cout <<green <<addrToString(insn1->address()) <<": not decoded" <<endColor <<"\n";
                maxPrintedVa = insn1->address();
            } else if (auto insn = insn1->ast()) {
                std::cout <<green <<p.unparse(insn.lock().get()) <<endColor <<"\n";
                maxPrintedVa = insn1->hull().greatest();
            } else {
                std::cout <<green <<addrToString(insn1->address()) <<": invalid address" <<endColor <<"\n";
                maxPrintedVa = insn1->address();
            }
            insns1.pop_back();
        }

        if (maxPrintedVa) {
            while (!insns2.empty() && insns2.back()->get_address() <= *maxPrintedVa)
                insns2.pop_back();
        }

        if (!insns2.empty() && (insns1.empty() || insns2.back()->get_address() < insns1.back()->address())) {
            auto insn2 = insns2.back();
            ASSERT_not_null(insn2);
            std::cout <<red <<p.unparse(insn2) <<endColor <<"\n";
            insns2.pop_back();
        }
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
    mlog[MARCH] <<"disassembly is finished at " <<(100 * progress->reportLatest().first.completion) <<" percent\n";
}

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("parallel disassembly");

    // Get, parse, and load the specimen.
    mlog[INFO] <<"parsing container\n";
    P2::Engine engine;
    std::vector<std::string> specimenName = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    MemoryMap::Ptr memory = engine.loadSpecimens(specimenName);
    Disassembler *decoder = engine.obtainDisassembler();

    // Create the parallel partitioner
    PP::Settings ppSettings;
    ppSettings.successorAccuracy = PP::Accuracy::HIGH;
    ppSettings.functionCallDetectionAccuracy = PP::Accuracy::HIGH;
    ppSettings.minHoleSearch = 128;
    PP::Partitioner pp(memory, decoder, ppSettings);

#if 1
    mlog[INFO] <<"searching for starting points (serial)\n";
    P2::Partitioner p = engine.createPartitioner();
    engine.runPartitionerInit(p);
    initializeParallelPartitioner(pp, p);
#else
    mlog[INFO] <<"reading start points from standard input";
    initializeParallelPartitioner(pp, std::cin);
    mlog[INFO] <<"; done\n";
#endif

#if 0
    // Write starting points to stdout
    for (auto vertex: pp.cfg().vertices())
        std::cout <<StringUtility::addrToString(vertex.value()->address()) <<"\n";
#endif

    mlog[INFO] <<"starting parallel phase\n";
    Sawyer::ProgressBar<double> bar(100.0, mlog[MARCH], "disassembly");
    bar.suffix(" percent");
    boost::thread(asyncProgressReporting, pp.progress(), &bar).detach();
    runPartitioner(pp);

    mlog[INFO] <<"generating output\n";
#if 0
    printCfgInstructions(pp, p);
#elif 1
    pp.printCfg(std::cout);
#elif 1
    pp.transferResults(p);
    printAllInstructions(pp, p);
#else
    engine.doingPostAnalysis(false);
    P2::Partitioner p2 = engine.partition(specimenName);
    printInsnsFromBoth(pp, p2);
#endif
}
