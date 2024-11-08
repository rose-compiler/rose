static const char *purpose = "lists basic block addresses";
static const char *description =
    "Given binary specimen, list the addresses of all the basic blocks.  Each line of output contains "
    "the following white-space separated fields:"
    "@bullet{The entry address of the basic block. This is the address of the first instruction that's executed.}"
    "@bullet{The number of instructions in the basic block.}"
    "@bullet{The list of largest contiguous address intervals for the block. Most basic blocks are contiguous in memory, "
    "so this list usually has but one element. Each element is the lowest and highest address of the interval separated "
    "by white space.}";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format format = SerialIo::BINARY;
};

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.format));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(gen);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    return parser;
}

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
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

    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    P2::Partitioner::ConstPtr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.format);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    for (const P2::ControlFlowGraph::Vertex &vertex: partitioner->cfg().vertices()) {
        using namespace StringUtility;
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            P2::BasicBlock::Ptr bb = vertex.value().bblock();
            std::cout <<addrToString(bb->address()) <<" " <<std::setw(6) <<bb->nInstructions();
            AddressIntervalSet bbExtent = partitioner->basicBlockInstructionExtent(bb);
            for (const AddressInterval &interval: bbExtent.intervals())
                std::cout <<" " <<addrToString(interval.least()) <<" " <<addrToString(interval.greatest());
            std::cout <<"\n";
        }
    }
}
