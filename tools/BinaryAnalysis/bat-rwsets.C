static const char *purpose = "demonstrate read/write sets";
static const char *description = "To be written.";

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/ReadWriteSets.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/Initialize.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <batSupport.h>

#include <stringify.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace Symbolic = Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;

struct Settings {
    std::set<std::string> functionNames;
    size_t maxIterations = 100;
    ReadWriteSets::Settings rwsSettings;
};

static Sawyer::Message::Facility mlog;

static Sawyer::CommandLine::Parser
buildSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;


    SwitchGroup tool("Tool specific switches");
    tool.name("tool");

    tool.insert(Switch("function", 'f')
                .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
                .explosiveLists(true)
                .whichValue(SAVE_ALL)
                .doc("Restricts analysis to the specified functions. The @v{name_or_address} can be the name of a function ax "
                     "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                     "If a value is ambiguous, it's first treated as a name and if no functio has that name it's then "
                     "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                     "be specified per occurrence."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.errorStream(mlog[FATAL]);
    parser.with(tool);
    parser.with(ReadWriteSets::commandLineSwitches(settings.rwsSettings));
    parser.with(Rose::CommandLine::genericSwitches());
    return parser;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[]) {
    // Initialize
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("read-write sets");

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser parser = buildSwitchParser(settings);
    P2::Engine::Ptr engine = P2::Engine::forge(argc, argv, parser /*in,out*/);
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    mlog[INFO] <<"using the " <<engine->name() <<" partitioning engine\n";
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    P2::Partitioner::Ptr partitioner = engine->partition(specimen);

    // Choose functions on which to run the analysis.
    const std::vector<P2::Function::Ptr> functions = [&settings, &partitioner]() {
        if (settings.functionNames.empty()) {
            return partitioner->functions();
        } else {
            return Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        }
    }();
    mlog[INFO] <<"processing " <<StringUtility::plural(functions.size(), "functons") <<"\n";

    // Process each function
    for (const P2::Function::Ptr &function: functions) {
        auto rwSets = ReadWriteSets::instance(partitioner, settings.rwsSettings);
        rwSets->analyze(function);
        std::cout <<*rwSets;
    }
}
