static const char *gPurpose = "select random ROSE configuration";
static const char *gDescription =
    "Queries the database dependencies and chooses a random configuration.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

enum class OutputMode {
    SHELL,                                              // shell script variables "boost=1.59 build=autoconf compiler=gcc-4.8"
    HUMAN,                                              // one key-value pair per line of output for human consumption
    RMC                                                 // output RMC specification
};

struct Settings {
    OutputMode outputMode = OutputMode::HUMAN;          // what kind of output to produce
    bool onlySupported = false;                         // select only supported values
    bool listing = false;                               // show all values
    bool balanceFailures = true;                        // attempt to balance the number of failures
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    std::string os;                                     // restrict to deps supported by operating system
};

static Sawyer::Message::Facility mlog;

static void
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}]");

    SwitchGroup tool("Tool-specific switches");
    insertDatabaseSwitch(tool, settings.databaseUri);

    tool.insert(Switch("format", 'F')
                .argument("style", enumParser<OutputMode>(settings.outputMode)
                          ->with("human", OutputMode::HUMAN)
                          ->with("rmc", OutputMode::RMC)
                          ->with("shell", OutputMode::SHELL))
                .doc("Style of output. The possibilities are:"
                     "@named{human}{Output in a human-friendly format. This is the default.}"
                     "@named{rmc}{Output an RMC specification.}"
                     "@named{shell}{Output one line of space-separated key=value pairs.}"));

    Rose::CommandLine::insertBooleanSwitch(tool, "only-supported", settings.onlySupported,
                                           "Resulting configuration will contain only supported values.");

    Rose::CommandLine::insertBooleanSwitch(tool, "balance-failures", settings.balanceFailures,
                                           "Attempt to balance the number of failures across language sets by preferring "
                                           "to test those lanugages that have fewer total failures. Since the purpose of "
                                           "portability testing is to find and classify failures, concentrating effort in "
                                           "those areas were failures are less likely is appropriate.");

    Rose::CommandLine::insertBooleanSwitch(tool, "list", settings.listing,
                                           "Instead of showing single values, show a space-separated list of all values");

    tool.insert(Switch("os")
                .argument("name", anyParser(settings.os))
                .doc("Dependencies have an optional column that lists what operating systems can support the dependency. "
                     "Normally this column is ignored, but if this switch is used and the column is non-empty, then only "
                     "select the dependency value if the specified operating system name appears in that column."));

    if (!parser.with(Rose::CommandLine::genericSwitches()).with(tool).parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"invalid usage; see --help\n";
        exit(1);
    }
}

using FailuresPerLanguageSet = Sawyer::Container::Map<std::string /*langset*/, size_t /*nfailures*/>;

// Number of failures per language set
static FailuresPerLanguageSet
loadLanguageFailureCounts(const Settings &settings, DB::Connection db) {
    FailuresPerLanguageSet retval;
    auto stmt = db.stmt("select rmc_languages, count(*) as nfailures"
                        " from test_results"
                        " where rose_date = (select coalesce(max(rose_date),0) from test_results)"
                        " and status != 'end' and status != 'setup'"
                        " and blacklisted = ''"
                        " group by rmc_languages");
    for (auto row: stmt) {
        const std::string languageSet = *row.get<std::string>(0);
        const size_t nfailures = *row.get<size_t>(1);
        retval.insert(languageSet, nfailures);
    }
    return retval;
}

using DependencyMap = Sawyer::Container::Map<std::string /*name*/, DependencyList>;

// Load all dependendencies from the database
static DependencyMap
loadAllDependencies(const Settings &settings, DB::Connection db) {
    auto stmt = db.stmt("select " + dependencyColumns() + " from dependencies where enabled <> 0" +
                        std::string(settings.onlySupported ? " and supported > 0" : ""));
    DependencyMap retval;
    for (const Dependency &dep: loadDependencies(stmt)) {
        if (settings.os.empty() || dep.osNames.empty() || dep.osNames.find(settings.os) != dep.osNames.end())
            retval.insertMaybeDefault(dep.name).push_back(dep);
    }
    return retval;
}

static void
listEntireSpace(const Settings &settings, const DependencyMap &dependencies) {
    for (const DependencyList &depList: dependencies.values()) {
        ASSERT_forbid(depList.empty());
        switch (settings.outputMode) {
            case OutputMode::HUMAN:
                std::cout <<std::left <<std::setw(16) <<depList[0].name <<":";
                for (const Dependency &dep: depList)
                    std::cout <<" " <<dep.value;
                std::cout <<"\n";
                break;
            case OutputMode::RMC:
                std::cout <<std::left <<std::setw(20) <<("rmc_"+depList[0].name);
                for (const Dependency &dep: depList)
                    std::cout <<" " <<StringUtility::bourneEscape(dep.value);
                std::cout <<"\n";
                break;
            case OutputMode::SHELL:
                std::cout <<std::left <<depList[0].name <<"=(";
                for (size_t i = 0; i < depList.size(); ++i)
                    std::cout <<(i?" ":"") <<StringUtility::bourneEscape(depList[i].value);
                std::cout <<")\n";
                break;
        }
    }
}

// Choose a language set (e.g., "c,c++") from the supplied list of choices, but instead of choosing uniformly,
// give precedence to those language sets that have the fewest number of failed tests. Portability testing is all
// about trying to find and classify the errors, so this causes us to look the hardest in languages that have the
// fewest errors.
static std::string
chooseLanguageSet(const FailuresPerLanguageSet &failuresPerLanguageSet, const DependencyList &choices) {
    // Find maximum number of failures for any language set
    size_t maxFailures = 0;
    for (const Dependency &choice: choices) {
        size_t n = failuresPerLanguageSet.getOrElse(choice.value, 0);
        maxFailures = std::max(maxFailures, n);
    }

    // Build a choice vector giving more weight to language sets with fewer errors. Every language will be
    // represented by at least one entry even if all its tests failed.
    using IndexInterval = Sawyer::Container::Interval<size_t>;
    using WeightedChoices = Sawyer::Container::IntervalMap<IndexInterval, std::string>;
    WeightedChoices wchoices;
    for (const Dependency &choice: choices) {
        size_t weight = maxFailures + 1 - failuresPerLanguageSet.getOrElse(choice.value, 0);
        wchoices.insert(IndexInterval::baseSize(wchoices.size(), weight), choice.value);
    }

    // Choose one
    size_t idx = Sawyer::fastRandomIndex(wchoices.size());
    return wchoices[idx];
}

static void
showRandomPoint(const Settings &settings, const DependencyMap &dependencies, const FailuresPerLanguageSet &failuresPerLanguageSet) {
    for (const DependencyList &depList: dependencies.values()) {
        ASSERT_forbid(depList.empty());
        std::string chosenValue;
        if (settings.balanceFailures && "languages" == depList[0].name) {
            chosenValue = chooseLanguageSet(failuresPerLanguageSet, depList);
        } else {
            size_t idx = Sawyer::fastRandomIndex(depList.size());
            chosenValue = depList[idx].value;
        }

        switch (settings.outputMode) {
            case OutputMode::HUMAN:
                std::cout <<std::left <<std::setw(16) <<depList[0].name <<": " <<chosenValue <<"\n";
                break;
            case OutputMode::RMC:
                std::cout <<std::left <<std::setw(20) <<("rmc_"+depList[0].name)
                          <<" " <<StringUtility::bourneEscape(chosenValue) <<"\n";
                break;
            case OutputMode::SHELL:
                std::cout <<" " <<depList[0].name <<"=" <<StringUtility::bourneEscape(chosenValue);
                break;
        }
    }
    if (settings.outputMode == OutputMode::SHELL)
        std::cout <<"\n";
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    Sawyer::fastRandomIndex(0, time(NULL) + getpid());

    Settings settings;
    parseCommandLine(argc, argv, settings);
    auto db = DB::Connection::fromUri(settings.databaseUri);

    DependencyMap dependencies = loadAllDependencies(settings, db);
    FailuresPerLanguageSet failuresPerLanguageSet = loadLanguageFailureCounts(settings, db);

    if (settings.listing) {
        listEntireSpace(settings, dependencies);
    } else {
        showRandomPoint(settings, dependencies, failuresPerLanguageSet);
    }
}
