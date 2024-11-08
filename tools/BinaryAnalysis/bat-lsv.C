static const char *purpose = "list synthesized variables";
static const char *description =
    "Analyzes a binary in order to synthesize information about source-level variables, and then show that information.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Variables.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>

#include <boost/algorithm/string/predicate.hpp>
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

static Sawyer::CommandLine::Parser
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

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");
    return specimen;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("listing info about variables");
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

    auto analyzer = Variables::VariableFinder::instance();
    Variables::GlobalVariables gvars = analyzer->findGlobalVariables(partitioner);

    for (const Variables::GlobalVariable &gvar: gvars.values())
        std::cout <<gvar <<"\n";
}
