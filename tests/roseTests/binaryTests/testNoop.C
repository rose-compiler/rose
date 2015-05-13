// Looks for no-op sequences
#include <rose.h>
#include <BinaryNoOperation.h>
#include <Partitioner2/Engine.h>
#include <sawyer/CommandLine.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

struct Settings {};                                     // command-line settings.

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("looks for instruction sequences that are no-op equivalent")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("description",
             "Parses, disassembles and partitions the specimens given as positional arguments on the command-line, "
             "and then scans the instructions of each basic block individually to find all sequences of instructions "
             "that have no effect.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());

    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    return parser.with(gen).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();

    Settings settings;
    std::vector<std::string> fileNames = parseCommandLine(argc, argv, settings /*out*/);

    P2::Engine engine;
    engine.useSemantics(true);
    P2::Partitioner partitioner = engine.partition(fileNames);

    // Get a list of basic blocks to analyze, sorted by starting address.
    std::vector<P2::BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK)
            bblocks.push_back(vertex.value().bblock());
    }
    std::sort(bblocks.begin(), bblocks.end(), P2::sortBasicBlocksByAddress);
    

    // Analyze each basic block to find no-op equivalents
    NoOperation nopAnalyzer(engine.disassembler());
    BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, bblocks) {
        std::cout <<bblock->printableName() <<":\n";
        const std::vector<SgAsmInstruction*> &insns = bblock->instructions();
        NoOperation::IndexIntervals allSequences = nopAnalyzer.findNoopSubsequences(insns);
        NoOperation::IndexIntervals bigSequences = NoOperation::largestEarliestNonOverlapping(allSequences);
        std::vector<bool> isNoop = NoOperation::toVector(bigSequences, insns.size());

        std::cout <<"  Instructions:\n";
        for (size_t i=0; i<insns.size(); ++i) {
            std::cout <<"    [" <<std::setw(2) <<i <<"] "
                      <<(isNoop[i] ? " X " : "   ")
                      <<unparseInstructionWithAddress(insns[i]) <<"\n";
        }

        if (allSequences.size() > 1) {
            std::cout <<"  All no-op sequences:\n";
            BOOST_FOREACH (const NoOperation::IndexInterval &where, allSequences) {
                if (where.isSingleton()) {
                    std::cout <<"    " <<where.least() <<"\n";
                } else {
                    std::cout <<"    " <<where.least() <<" .. " <<where.greatest() <<"\n";
                }
            }
        }
    }
}
