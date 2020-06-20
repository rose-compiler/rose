static const char *purpose = "lists basic block addresses";
static const char *description =
    "Given a BAT state for a binary specimen, list the addresses of all the basic blocks.  Each line of output contains "
    "the following white-space separated fields:"
    "@bullet{The entry address of the basic block. This is the address of the first instruction that's executed.}"
    "@bullet{The number of instructions in the basic block.}"
    "@bullet{The list of largest contiguous address intervals for the block. Most basic blocks are contiguous in memory, "
    "so this list usually has but one element. Each element is the lowest and highest address of the interval separated "
    "by white space.}";

#include <rose.h>
#include <CommandLine.h>                                        // rose
#include <Diagnostics.h>                                        // rose
#include <Partitioner2/Engine.h>                                // rose
#include <Partitioner2/Partitioner.h>                           // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;
Rose::BinaryAnalysis::SerialIo::Format format = Rose::BinaryAnalysis::SerialIo::BINARY;

// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(format));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(gen);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? std::string("-") : input[0];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about basic blocks");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    P2::Engine engine;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, format);

    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        using namespace StringUtility;
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertex.value().bblock();
            std::cout <<addrToString(bb->address()) <<" " <<std::setw(6) <<bb->nInstructions();
            BOOST_FOREACH (const AddressInterval &interval, partitioner.basicBlockInstructionExtent(bb).intervals())
                std::cout <<" " <<addrToString(interval.least()) <<" " <<addrToString(interval.greatest());
            std::cout <<"\n";
        }
    }
}
