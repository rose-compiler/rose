static const char *purpose = "shows calling conventions";
static const char *description =
    "To be written.";

#include <rose.h>
#include <BinaryCallingConvention.h>            // rose
#include <CommandLine.h>                        // rose
#include <Diagnostics.h>                        // rose
#include <Partitioner2/Engine.h>                // rose
#include <Partitioner2/Partitioner.h>           // rose

#include <batSupport.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <SqlDatabase.h>                        // rose

using namespace ::Rose;
using namespace ::Sawyer::Message::Common;
using namespace ::Rose::BinaryAnalysis;
namespace P2 = ::Rose::BinaryAnalysis::Partitioner2;

namespace {

typedef Sawyer::Container::Map<P2::Function::Ptr, std::vector<std::string> > FuncDefNames;
typedef Sawyer::Container::Map<std::string, size_t> DefNameCounts;
typedef Sawyer::Container::Map<size_t, std::vector<std::string> > CountDefNames;

Sawyer::Message::Facility mlog;

struct Settings {
    boost::filesystem::path outputFileName;
    std::set<std::string> functionNames;
    bool showingDictionary;
    bool showingRankedDefnNames;
    bool ignoreFailure;
    bool showingCcAnalysis;
    bool removeAnalysis;
    SerialIo::Format stateFormat;

    Settings()
        : showingDictionary(false), showingRankedDefnNames(false), ignoreFailure(false), showingCcAnalysis(false),
          removeAnalysis(false), stateFormat(SerialIo::BINARY) {}

    bool isSilent() const {
        return !showingRankedDefnNames;
    }
};

// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");
    parser.errorStream(mlog[FATAL]);

    SwitchGroup gen = Rose::CommandLine::genericSwitches();

    gen.insert(Switch("output", 'o')
               .argument("file", anyParser(settings.outputFileName))
               .doc("Write out a new state file with calling convention information.  The new file may have the same name "
                    "as the input file since the input file is read and closed before any output is produced. The special "
                    "name \"-\" (a single hyphen) causes output to be sent to standard output. The default is to not write "
                    "a new state file."));

    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup sel("Selection switches");
    sel.name("sel");

    sel.insert(Switch("function")
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts analysis to the specified functions The @v{name_or_address} can be the name of a function as "
                    "a string or the entry address for the function as a decimal, octal, hexadecimal or binary number. "
                    "If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                    "treated as an address. This switch may occur multiple times and multiple comma-separated values may "
                    "be specified per occurrence."));


    SwitchGroup cc("Calling convention switches");
    cc.name("cc");

    cc.insert(Switch("ranked-names")
              .intrinsicValue(true, settings.showingRankedDefnNames)
              .doc("Shows all matching calling convention definition names and the number of times each definition matched. "
                   "The total number of matches may be greater than the total number of functions since it's possible for "
                   "a function's calling convention to match more than one definition.  This output is the default if no "
                   "other output switches are specified."));

    Rose::CommandLine::insertBooleanSwitch(cc, "show-dictionary", settings.showingDictionary,
                                           "Show calling convention dictionary entries.");

    Rose::CommandLine::insertBooleanSwitch(cc, "ignore-failure", settings.ignoreFailure,
                                           "Ignore functions for which calling convention analysis was not run, or for "
                                           "which the analysis did not reach a fixed point, or for which no matching "
                                           "calling convention definition could be found.   If failures not not ignored "
                                           "then these situations are reported as \"not-run\", \"non-converging\", and "
                                           "\"no-match\", respecitively.");

    Rose::CommandLine::insertBooleanSwitch(cc, "show-analysis", settings.showingCcAnalysis,
                                           "Show per-function results of low-level calling convention analysis results. "
                                           "This is information about how the function itself behaves regardless of "
                                           "what calling convention is eventually determined.");

    Rose::CommandLine::insertBooleanSwitch(cc, "remove", settings.removeAnalysis,
                                           "Instead of computing calling conventions, remove calling convention analysis "
                                           "results. This is intended to be used with the @s{output} switch to remove "
                                           "analysis results from a state file.");

    std::vector<std::string> input = parser.with(gen).with(sel).with(cc).parse(argc, argv).apply().unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    if (settings.isSilent())
        settings.showingRankedDefnNames = true;

    return input.empty() ? std::string("-") : input[0];
}

// Full name for a calling convention
std::string
ccDefnBestName(const CallingConvention::Definition::Ptr &defn) {
    return defn->comment().empty() ? defn->name() : defn->comment();
}


// Matching definition names for all functions, or the name "unknown"
FuncDefNames
functionCcDefinitionNames(const P2::Partitioner &p, const Settings &settings, const std::vector<P2::Function::Ptr> &functions) {
    FuncDefNames retval;
    BOOST_FOREACH (P2::Function::Ptr f, functions) {
        SAWYER_MESG(mlog[DEBUG]) <<"finding matching definitions for " <<f->printableName() <<"\n";
        if (!f->callingConventionAnalysis().hasResults()) {
            SAWYER_MESG(mlog[DEBUG]) <<"  analysis was not run\n";
            if (!settings.ignoreFailure)
                retval.insertMaybeDefault(f).push_back("not-run");
        } else if (!f->callingConventionAnalysis().didConverge()) {
            SAWYER_MESG(mlog[DEBUG]) <<"  analysis did not converge\n";
            if (!settings.ignoreFailure)
                retval.insertMaybeDefault(f).push_back("non-convergent");
        } else {
            CallingConvention::Dictionary matches = p.functionCallingConventionDefinitions(f);
            if (matches.empty()) {
                SAWYER_MESG(mlog[DEBUG]) <<"  no matching definitions\n";
                if (!settings.ignoreFailure)
                    retval.insertMaybeDefault(f).push_back("no-match");
            } else {
                BOOST_FOREACH (const CallingConvention::Definition::Ptr &defn, matches) {
                    SAWYER_MESG(mlog[DEBUG]) <<"  matched: " <<*defn <<"\n";
                    retval.insertMaybeDefault(f).push_back(ccDefnBestName(defn));
                }
            }
        }
    }
    return retval;
}

// How many times does each definition name occur?
DefNameCounts
countDefinitionNames(const FuncDefNames &fdns) {
    DefNameCounts retval;
    BOOST_FOREACH (const FuncDefNames::Node &node, fdns.nodes()) {
        BOOST_FOREACH (const std::string &name, node.value())
            ++retval.insertMaybe(name, 0);
    }
    return retval;
}

// Rank calling convention definition names by their frequency
CountDefNames
rankDefinitionNames(const DefNameCounts &dncs) {
    CountDefNames retval;
    BOOST_FOREACH (const DefNameCounts::Node &node, dncs.nodes())
        retval.insertMaybeDefault(node.value()).push_back(node.key());
    return retval;
}

// Show a table that lists all calling convention definitions according to how often they appear.
void
show(const CountDefNames &cdns) {
    SqlDatabase::Table<size_t, std::string> table;
    table.headers("Count", "CC Name");
    BOOST_REVERSE_FOREACH (const CountDefNames::Node &node, cdns.nodes()) {
        BOOST_FOREACH (const std::string &ccname, node.value())
            table.insert(node.key(), ccname);
    }
    table.print(std::cout);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("analyzing calling conventions");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, settings);
    
    // Read the state file
    P2::Engine engine;
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);

    // Create the output state file early so that the user will get an error early if the file can't be created. The alternative
    // is to wait until after all calling convention analysis is completed, which could be a while!
    if (!settings.outputFileName.empty())
        Bat::checkRbaOutput(settings.outputFileName, mlog);

    // Output the dictionary if requested. This is fast, so do it before the cc analysis starts.
    if (settings.showingDictionary) {
        BOOST_FOREACH (const CallingConvention::Definition::Ptr &ccdef, partitioner.instructionProvider().callingConventions())
            std::cout <<"cc definition: " <<*ccdef <<"\n";
    }

    // Select functions on which to operate
    std::vector<P2::Function::Ptr> selectedFunctions = partitioner.functions();
    if (!settings.functionNames.empty())
        selectedFunctions = Bat::selectFunctionsByNameOrAddress(selectedFunctions, settings.functionNames, mlog[WARN]);
    
    // Obtain or remove calling convention analysis results.
    if (settings.removeAnalysis) {
        BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions)
            function->callingConventionAnalysis().clearResults();
    } else {
        // If the input state file already has calling convention analysis then nothing really happens here.
        // The defaultCc obtained here is the same one that P2::Engine uses when calling the CC analysis.
        const CallingConvention::Dictionary &ccDict = partitioner.instructionProvider().callingConventions();
        CallingConvention::Definition::Ptr defaultCc;
        if (!ccDict.empty())
            defaultCc = ccDict[0];
        if (settings.functionNames.empty()) {
            partitioner.allFunctionCallingConventionDefinition(defaultCc); // faster than a loop since this runs in parallel
        } else {
            BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions)
                partitioner.functionCallingConvention(function, defaultCc);
        }
    }

    // Write to the output state if desired.
    if (!settings.outputFileName.empty())
        engine.savePartitioner(partitioner, settings.outputFileName, settings.stateFormat);
    if (settings.removeAnalysis)
        return 0;

    //---- The rest of this file is for output ----

    // Accumulate some statistics
    FuncDefNames funcDefNames = functionCcDefinitionNames(partitioner, settings, selectedFunctions);
    DefNameCounts defNameCounts = countDefinitionNames(funcDefNames);
    CountDefNames countDefNames = rankDefinitionNames(defNameCounts);

    if (settings.showingCcAnalysis) {
        BOOST_FOREACH (const P2::Function::Ptr &function, selectedFunctions) {
            std::cout <<function->printableName() <<": " <<function->callingConventionAnalysis() <<"\n";
            std::cout <<"   matches these definitions:";
            BOOST_FOREACH (const std::string &s, funcDefNames[function])
                std::cout <<" " <<s;
            std::cout <<"\n";
        }
    }
    
    if (settings.showingRankedDefnNames) {
        show(countDefNames);
    }
}
