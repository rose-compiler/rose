static const char *purpose = "list basic function information";
static const char *description =
    "Lists basic information about each function present in a binary specimen.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/FunctionCallGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/FormattedTable.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
    bool demangle = false;
};

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));
    Rose::CommandLine::insertBooleanSwitch(generic, "demangle", settings.demangle, "Demangle function names.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
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

std::string
rjust(const std::string &s, size_t width) {
    if (s.size() >= width)
        return s;
    return std::string(width-s.size(), ' ') + s;
}

std::string
ljust(const std::string &s, size_t width) {
    if (s.size() >= width)
        return s;
    return s + std::string(width-s.size(), ' ');
}

std::string
toString(const AddressInterval &i) {
    if (i.isEmpty())
        return "empty";
    return StringUtility::addrToString(i.least()) + ", " + StringUtility::addrToString(i.greatest());
}

std::string
toString(size_t a) {
    return boost::lexical_cast<std::string>(a);
}

std::string
toString(size_t a, size_t b) {
    return rjust(toString(a), 7) + "/" + ljust(toString(b), 7);
}

// Print a pretty table with information about functions.
void
printFunctions(const P2::Partitioner::ConstPtr &partitioner, const Settings &settings) {
    FormattedTable table;
    table.columnHeader(0, 0, "Entry VA");
    table.columnHeader(0, 1, "Lowest/Highest VA");
    table.columnHeader(0, 2, "BBlocks/Insns");
    table.columnHeader(0, 3, "DBlocks/Bytes");
    table.columnHeader(0, 4, "Contig");
    table.columnHeader(0, 5, "Callers/Callees");
    table.columnHeader(0, 6, "Name");

    P2::FunctionCallGraph cg = partitioner->functionCallGraph(P2::AllowParallelEdges::NO);

    for (const P2::Function::Ptr &function: partitioner->functions()) {
        AddressIntervalSet fe = partitioner->functionExtent(function);
        size_t nInsns = 0;
        for (Address bbva: function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbva))
                nInsns += bb->nInstructions();
        }
        size_t nDBlockBytes = 0;
        for (const P2::DataBlock::Ptr &dblock: function->dataBlocks())
            nDBlockBytes += dblock->size();

        std::string name = settings.demangle ? function->demangledName() : function->name();

        const size_t i = table.nRows();
        table.insert(i, 0, StringUtility::addrToString(function->address()));
        table.insert(i, 1, toString(fe.hull()));
        table.insert(i, 2, toString(function->basicBlockAddresses().size(), nInsns));
        table.insert(i, 3, toString(function->dataBlocks().size(), nDBlockBytes));
        table.insert(i, 4, fe.nIntervals());
        table.insert(i, 5, toString(cg.nCallsIn(function), cg.nCallsOut(function)));
        table.insert(i, 6, StringUtility::cEscape(name));
    }
    std::cout <<table;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about functions");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);

    P2::Partitioner::ConstPtr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    printFunctions(partitioner, settings);
}
