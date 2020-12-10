// Looks for no-op sequences
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>
#include <BinaryNoOperation.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

struct Settings {
    Sawyer::Optional<rose_addr_t> initialStackPointer;
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "finds instruction sequences that are no-op equivalent";
    std::string description =
         "Parses, disassembles and partitions the specimens given as positional arguments on the command-line, "
         "and then scans the instructions of each basic block individually to find all sequences of instructions "
        "that have no effect.";

    Parser parser = engine.commandLineParser(purpose, description);

    SwitchGroup sg("Tool-specific switches");
    sg.insert(Switch("stack")
              .argument("va", nonNegativeIntegerParser(settings.initialStackPointer))
              .doc("Concrete value for the initial stack pointer. Using a concrete value for the stack pointer "
                   "sometimes makes the no-op analysis more accurate as far as being able to reason about which "
                   "memory was recently popped from the top of the stack. On the other hand, it can also result "
                   "in false positives (e.g., if the initial stack pointer is word aligned then an instruction "
                   "like \"and esp, 8\" will be detected as a no-op)."));

    return parser.with(sg).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    Settings settings;
    P2::Engine engine;
    engine.usingSemantics(true); // test specimens contain opaque predicates
    engine.followingGhostEdges(false);
    engine.findingIntraFunctionCode(false);
    P2::Partitioner partitioner = engine.partition(parseCommandLine(argc, argv, engine, settings));

    // Get a list of basic blocks to analyze, sorted by starting address.
    std::vector<P2::BasicBlock::Ptr> bblocks;
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK)
            bblocks.push_back(vertex.value().bblock());
    }
    std::sort(bblocks.begin(), bblocks.end(), P2::sortBasicBlocksByAddress);
    

    // Analyze each basic block to find no-op equivalents
    NoOperation nopAnalyzer(engine.disassembler());
    nopAnalyzer.initialStackPointer(settings.initialStackPointer);
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
                      <<partitioner.unparse(insns[i]) <<"\n";
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

#endif
