static const char *purpose = "list variables";
static const char *description =
    "Lists information about synthesized variables. These are variables discerned from the instructions rather than "
    "variables obtained from symbol tables or debugging information.";

#include <rose.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <batSupport.h>
#include <boost/filesystem.hpp>
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

struct Settings {
    SerialIo::Format stateFormat = SerialIo::BINARY;
    std::set<std::string> functionNames;
    std::set<Address> addresses;
};

Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup sel("Selection switches");
    sel.name("sel");

    sel.insert(Switch("function", 'f')
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to the specified functions. The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));

    sel.insert(Switch("containing", 'a')
               .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to functions that contain one of the specified addresses. This switch may occur "
                    "multiple times and multiple comma-separate addresses may be specified per occurrence."));


    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(sel).with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    return parser;
}

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");                        // read RBA file from standard input
    return specimen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

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

    P2::Partitioner::Ptr partitioner;
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

    // Select the functions to analyze
    std::vector<P2::Function::Ptr> selectedFunctions;
    if (!settings.functionNames.empty() || !settings.addresses.empty()) {
        selectedFunctions = Bat::selectFunctionsByNameOrAddress(partitioner->functions(), settings.functionNames, mlog[WARN]);
        for (const P2::Function::Ptr &f: Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses))
            P2::insertUnique(selectedFunctions, f, P2::sortFunctionsByAddress);
    } else {
        selectedFunctions = partitioner->functions();
    }
    if (selectedFunctions.empty())
        mlog[WARN] <<"no matching functions found\n";

    // Find and print the local variables
    auto variableFinder = Variables::VariableFinder::instance();
    for (const P2::Function::Ptr &function: selectedFunctions) {
        std::cout <<"local variables for " <<function->printableName() <<":\n";
        Variables::StackVariables lvars = variableFinder->findStackVariables(partitioner, function);
        Variables::print(lvars, partitioner, std::cout, "  ");
    }

    // Find and print global variables
    std::cout <<"global variables:\n";
    Variables::GlobalVariables gvars = variableFinder->findGlobalVariables(partitioner);
    Variables::print(gvars, partitioner, std::cout, "  ");
}
