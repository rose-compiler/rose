static const char *purpose = "list synthesized variables";
static const char *description =
    "Analyzes a binary in order to synthesize information about source-level variables, and then show that information.";

#include <rose.h>

#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <batSupport.h>

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;    // format of RBA file
    bool demangle = true;                               // demangle file names
};

static Sawyer::Message::Facility mlog;

// Parse the command line from `argc` and `argv`. Adjust `settings` to reflect the command-line switches. A non-switch,
// positional argument may be present, in which case it's returned as the name of the input file. If no positional argument is
// present, or if the positional argument is "-", then this function returns the name "-", indicating that standard input
// should be read.
static boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));
    Rose::CommandLine::insertBooleanSwitch(generic, "demangle", settings.demangle, "Demangle function names.");

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

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about variables");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    boost::filesystem::path inputName = parseCommandLine(argc, argv, settings);
    P2::Partitioner::Ptr partitioner = P2::Partitioner::instanceFromRbaFile(inputName, settings.stateFormat);

    auto analyzer = Variables::VariableFinder::instance();
    Variables::GlobalVariables gvars = analyzer->findGlobalVariables(partitioner);

    for (const Variables::GlobalVariable &gvar: gvars.values())
        std::cout <<gvar <<"\n";
}
