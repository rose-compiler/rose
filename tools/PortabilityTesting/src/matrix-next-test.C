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

// Load all dependendencies from the database, pruning away those that don't match the operating system.
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
    ASSERT_forbid(choices.empty());
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"choose language set from (";
        for (const Dependency &choice: choices)
            mlog[DEBUG] <<" " <<choice.value;
        mlog[DEBUG] <<" )\n";
    }

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
    SAWYER_MESG(mlog[DEBUG]) <<"  returning languages = " <<wchoices[idx] <<"\n";
    return wchoices[idx];
}

struct HasValue {
    const std::string &value;
    HasValue(const std::string &value)
        : value(value) {}
    bool operator()(const Dependency &d) const {
        return d.value == value;
    }
};


// Adjust dependencies by eliminating all language sets but the specified one.
static void
restrictLanguages(const Settings &settings, DependencyMap &dependencies /*in,out*/, const std::string &languages) {
    ASSERT_require(dependencies.exists("languages"));
    DependencyList &deps = dependencies["languages"];
    deps.erase(std::remove_if(deps.begin(), deps.end(), HasValue(languages)), deps.end());
    ASSERT_forbid(deps.empty());
}

struct TestRestrictions {
    const DependencyMap &dependencies;
    TestRestrictions(const DependencyMap &dependencies)
        : dependencies(dependencies) {}


    bool operator()(const Dependency &d) const {
        return !restrictionsPass(d);
    }

    bool restrictionsPass(const Dependency &d) const {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        // For the dependency whose values are under consideration (d) individually consider each of the dependency names to
        // which its constraints refer. E.g., when considering the "tup" dependency, there's probably constraings that
        // depend on the chosen value of the "languages" dependency since Tup only works for binary analysis.
        for (auto node: d.constraints.nodes()) {
            const std::string &otherDepName = node.key();
            const DependencyList &otherDepValues = dependencies.getOrDefault(otherDepName);
            if (otherDepValues.empty()) {
                mlog[ERROR] <<"no values for dependency \"" <<StringUtility::cEscape(otherDepName) <<"\"\n"
                            <<"when evaluating restrictions for \"" <<StringUtility::cEscape(d.name) <<"\""
                            <<" value \"" <<StringUtility::cEscape(d.value) <<"\"\n";
                return false;
            } else if (otherDepValues.size() > 1) {
                mlog[ERROR] <<"forward dependency reference from \"" <<StringUtility::cEscape(d.name) <<"\""
                            <<" to \"" <<StringUtility::cEscape(otherDepName) <<"\"\n"
                            <<"when evaluating restrictions for \"" <<StringUtility::cEscape(d.name) <<"\""
                            <<" value \"" <<StringUtility::cEscape(d.value) <<"\"\n";
                return false;
            } else {
                // If none of d's restrictions are satisfied for otherDepName and otherDepValue, then fail.
                const std::string otherDepValue = otherDepValues[0].value;
                SAWYER_MESG(debug) <<"  " <<d.name <<" = " <<d.value <<" has constraints referencing "
                                   <<otherDepName <<" = " <<otherDepValue <<"\n";
                bool pass = false;
                for (const Dependency::ConstraintRhs &rhs: node.value()) {
                    SAWYER_MESG(debug) <<"    constraint: " <<otherDepName <<rhs.comparison <<rhs.value;
                    if ("=" == rhs.comparison) {
                        if (otherDepValue == rhs.value) {
                            SAWYER_MESG(debug) <<" passes\n";
                            pass = true;
                            break;
                        } else {
                            SAWYER_MESG(debug) <<" fails\n";
                        }
                    } else if ("/" == rhs.comparison) {
                        if (otherDepValue != rhs.value) {
                            SAWYER_MESG(debug) <<" passes\n";
                            pass = true;
                            break;
                        } else {
                            SAWYER_MESG(debug) <<" fails\n";
                        }
                    } else if ("~" == rhs.comparison) {
                        if (boost::contains(otherDepValue, rhs.value)) {
                            SAWYER_MESG(debug) <<" passes\n";
                            pass = true;
                            break;
                        } else {
                            SAWYER_MESG(debug) <<" fails\n";
                        }
                    } else if ("^" == rhs.comparison) {
                        if (!boost::contains(otherDepValue, rhs.value)) {
                            SAWYER_MESG(debug) <<" passes\n";
                            pass = true;
                            break;
                        } else {
                            SAWYER_MESG(debug) <<" fails\n";
                        }
                    } else {
                        ASSERT_not_reachable("unknown comparison \"" + rhs.comparison + "\"");
                    }
                }
                if (!pass) {
                    SAWYER_MESG(debug) <<"  no constraints passed,"
                                       <<" therefore " <<d.name <<" = " <<d.value <<" cannot be selected\n";
                    return false;
                }
            }
        }
        SAWYER_MESG(debug) <<"  dependency " <<d.name <<" = " <<d.value <<" can be selected\n";
        return true;
    }
};

// From all available choices, choose one set of dependencies.
static void
chooseOneSet(const Settings &settings, DB::Connection db, DependencyMap &dependencies /*in,out*/) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // We need to process the dependencies in a particular order defined in the database
    for (auto row: db.stmt("select name from dependency_attributes order by position")) {
        const std::string name = *row.get<std::string>(0);
        if (!dependencies.exists(name)) {
            mlog[FATAL] <<"no possible values for " <<name <<" dependency\n";
            exit(1);
        }

        DependencyList &deps = dependencies[name];

        if (debug) {
            debug <<"choosing from " <<name <<", candidate values are:";
            for (const Dependency &d: deps) {
                debug <<" " <<d.value;
                ASSERT_require(d.name == name);
            }
            debug <<"\n";
        }

        // First remove dependencies whose restrictions fail to select it.
        deps.erase(std::remove_if(deps.begin(), deps.end(), TestRestrictions(dependencies)), deps.end());
        if (deps.empty()) {
            mlog[FATAL] <<"all restrictions failed for dependencies \"" <<StringUtility::cEscape(name) <<"\"\n";
            mlog[FATAL] <<"values already chosen are:\n";
            for (auto &node: dependencies.nodes()) {
                if (node.key() == name) {
                    break;
                } else {
                    mlog[FATAL] <<"  \"" <<StringUtility::cEscape(node.key()) <<"\" ="
                                <<" \"" <<StringUtility::cEscape(node.value()[0].value) <<"\"\n";
                    exit(1);
                }
            }
        }

        // Then randomly select one of the remaining values, removing the rest
        const size_t i = Sawyer::fastRandomIndex(deps.size());
        if (debug) {
            debug <<"  choosing randomly from " <<name <<" = {";
            for (const Dependency &d: deps)
                debug <<" " <<d.value;
            debug <<" }[" <<i <<"] = " <<deps[i].value <<"\n";
        }
        std::swap(deps[0], deps[i]);
        deps.erase(deps.begin() + 1, deps.end());
    }
}

// Show the selected dependencies. There must be exactly one value per name.
static void
showDependencies(const Settings &settings, const DependencyMap &dependencies) {
    for (const DependencyList &deps: dependencies.values()) {
        ASSERT_require(deps.size() == 1);
        const Dependency &d = deps[0];
        switch (settings.outputMode) {
            case OutputMode::HUMAN:
                std::cout <<(boost::format("%-16s: %s\n") % d.name % d.value);
                break;
            case OutputMode::RMC:
                std::cout <<(boost::format("rmc_%-20s %s\n") % d.name % StringUtility::bourneEscape(d.value));
                break;
            case OutputMode::SHELL:
                std::cout <<" " <<d.name <<"=" <<StringUtility::bourneEscape(d.value);
                break;
        }
    }
    if (OutputMode::SHELL == settings.outputMode)
        std::cout <<"\n";
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    Sawyer::fastRandomIndex(0, time(NULL) + getpid());

    Settings settings;
    parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    DependencyMap dependencies = loadAllDependencies(settings, db);

    if (settings.listing) {
        listEntireSpace(settings, dependencies);
    } else {
        if (settings.balanceFailures) {
            FailuresPerLanguageSet failuresPerLanguageSet = loadLanguageFailureCounts(settings, db);
            std::string languageSet = chooseLanguageSet(failuresPerLanguageSet, dependencies["languages"]);
            restrictLanguages(settings, dependencies /*in,out*/, languageSet);
        }
        chooseOneSet(settings, db, dependencies /*in,out*/);
        showDependencies(settings, dependencies);
    }
}
