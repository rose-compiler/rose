static const char *purpose = "list basic function information";
static const char *description =
    "Lists basic information about each function present in a binary analysis state.  The binary analysis state is read "
    "from the file specified on the command-line, or from standard input if the name is \"-\" (a single hyphen) or not "
    "specified. Standard input is supported only on those systems where it's opened in binary mode, such as Linux.";

#include <rose.h>
#include <CommandLine.h>                                        // rose
#include <Diagnostics.h>                                        // rose
#include <Partitioner2/Engine.h>                                // rose
#include <Partitioner2/Partitioner.h>                           // rose
#include <SqlDatabase.h>                                        // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

Sawyer::Message::Facility mlog;
SerialIo::Format stateFormat = SerialIo::BINARY;
bool demangle = false;


// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(stateFormat));
    Rose::CommandLine::insertBooleanSwitch(generic, "demangle", demangle, "Demangle function names.");

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return input.empty() ? boost::filesystem::path("-") : input[0];
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
printFunctions(const P2::Partitioner &p) {
    SqlDatabase::Table<std::string,                     // 0: entry address
                       std::string,                     // 1: lowest and highest address
                       std::string,                     // 2: number of basic blocks and instructions
                       std::string,                     // 3: number of data blocks and bytes
                       size_t,                          // 4: number of largest contiguous regions
                       std::string,                     // 5: number of callers and callees
                       std::string> table;              // 6: name
    table.headers("Entry VA", "Lowest/Highest VA", "BBlocks/Insns", "DBlocks/Bytes", "Contig",
                  "Callers/Callees", "Name");
    table.reprint_headers(50);

    P2::FunctionCallGraph cg = p.functionCallGraph(P2::AllowParallelEdges::NO);

    BOOST_FOREACH (const P2::Function::Ptr &function, p.functions()) {
        AddressIntervalSet fe = p.functionExtent(function);
        size_t nInsns = 0;
        BOOST_FOREACH (rose_addr_t bbva, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = p.basicBlockExists(bbva))
                nInsns += bb->nInstructions();
        }
        size_t nDBlockBytes = 0;
        BOOST_FOREACH (const P2::DataBlock::Ptr &dblock, function->dataBlocks())
            nDBlockBytes += dblock->size();

        std::string name = demangle ? function->demangledName() : function->name();
        table.insert(StringUtility::addrToString(function->address()),
                     toString(fe.hull()),
                     toString(function->basicBlockAddresses().size(), nInsns),
                     toString(function->dataBlocks().size(), nDBlockBytes),
                     fe.nIntervals(),
                     toString(cg.nCallsIn(function), cg.nCallsOut(function)),
                     StringUtility::cEscape(name));
    }
    table.print(std::cout);
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

    P2::Engine engine;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, stateFormat);

    printFunctions(partitioner);
}
