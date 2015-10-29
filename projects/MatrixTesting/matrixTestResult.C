#include <rose.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <cstdio>
#include <cstring>
#include <LinearCongruentialGenerator.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Set.h>
#include <SqlDatabase.h>
#include <sys/utsname.h>
#include <unistd.h>

using namespace Sawyer::Message::Common;

struct Settings {
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("update database with test result");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{key_value_pairs}");
    parser.doc("Description", "Adds a test result to the database.");
    parser.errorStream(mlog[FATAL]);

    SwitchGroup sg("Tool-specific switches");
    sg.insert(Switch("help", 'h')
              .doc("Show this documentation.")
              .action(showHelpAndExit(0)));

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("URI specifying which database to use."));

    return parser.with(CommandlineProcessing::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
}

typedef Sawyer::Container::Map<std::string /*key*/, std::string /*colname*/> DependencyNames;
typedef Sawyer::Container::Map<std::string /*colname*/, std::string /*value*/> KeyValuePairs;

static DependencyNames
loadDependencyNames(const SqlDatabase::TransactionPtr &tx) {
    DependencyNames retval;
    SqlDatabase::StatementPtr q = tx->statement("select distinct name from dependencies");
    for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row) {
        std::string key = row.get<std::string>(0);
        retval.insert(key, "rmc_"+key);
    }

    retval.insert("duration", "duration");
    retval.insert("noutput", "noutput");
    retval.insert("nwarnings", "nwarnings");
    retval.insert("os", "os");
    retval.insert("rose", "rose");
    retval.insert("rose_date", "rose_date");
    retval.insert("status", "status");
    retval.insert("tester", "tester");

    return retval;
}

static std::string
getUserName() {
    if (const char *s = getenv("LOGNAME"))
        return s;
    mlog[FATAL] <<"LOGNAME is not set\n";
    exit(1);
}

static int
getUserId(const SqlDatabase::TransactionPtr &tx) {
    std::string userName = getUserName();
    SqlDatabase::StatementPtr q = tx->statement("select uid from users where name = ?")->bind(0, userName);
    SqlDatabase::Statement::iterator row = q->begin();
    if (row == q->end()) {
        mlog[FATAL] <<"no such user: \"" <<StringUtility::cEscape(userName) <<"\"\n";
        exit(1);
    }
    int retval = row.get<int>(0);
    ++row;
    if (row != q->end()) {
        mlog[FATAL] <<"user \"" <<StringUtility::cEscape(userName) <<"\" is ambiguous\n";
        exit(1);
    }
    return retval;
}

static std::string
getOsName() {
    struct utsname uts;
    if (uname(&uts) == -1)
        return "";
    return std::string(uts.sysname) + " " + uts.release + " " + uts.machine;
}

static std::string
getTester() {
    return getUserName() + " running " + Sawyer::CommandLine::Parser().programName();
}

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    mlog = Sawyer::Message::Facility("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);

    Settings settings;
    std::vector<std::string> kvlist = parseCommandLine(argc, argv, settings);
    if (kvlist.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    // Parse and validate key-value pairs from the command-line
    KeyValuePairs kvpairs;
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    DependencyNames dependencyNames = loadDependencyNames(tx);
    BOOST_FOREACH (const std::string &kvpair, kvlist) {
        size_t eq = kvpair.find('=');
        if (eq == std::string::npos) {
            mlog[FATAL] <<"not a key=value pair: \"" <<StringUtility::cEscape(kvpair) <<"\"\n";
            exit(1);
        }
        std::string key = boost::trim_copy(kvpair.substr(0, eq));
        std::string val = boost::trim_copy(kvpair.substr(eq+1));
        if (key.empty() || val.empty()) {
            mlog[FATAL] <<"key and value must be non-empty: \"" <<StringUtility::cEscape(kvpair) <<"\"\n";
            exit(1);
        }
        if (!dependencyNames.exists(key)) {
            mlog[FATAL] <<"key \"" <<StringUtility::cEscape(key) <<"\" is not recognized\n";
            exit(1);
        }
        if (kvpairs.exists(dependencyNames[key])) {
            mlog[FATAL] <<"key \"" <<StringUtility::cEscape(key) <<"\" specified more than once\n";
            exit(1);
        }
        kvpairs.insert(dependencyNames[key], val);
    }

    // Some keys are required
    Sawyer::Container::Set<std::string> requiredKeys;
    requiredKeys.insert("rose");
    requiredKeys.insert("status");
    requiredKeys.insert("tester");
    BOOST_FOREACH (const std::string &key, requiredKeys.values()) {
        if (!kvpairs.exists(key)) {
            mlog[FATAL] <<"key is required: \"" <<StringUtility::cEscape(key) <<"\"\n";
            exit(1);
        }
    }
    if (mlog[TRACE]) {
        BOOST_FOREACH (const KeyValuePairs::Node &node, kvpairs.nodes())
            mlog[TRACE] <<node.key() <<"=" <<node.value() <<"\n";
    }

    // Things set automatically instead of from the command-line
    kvpairs.insert("reporting_user", StringUtility::numberToString(getUserId(tx)));
    kvpairs.insert("reporting_time", StringUtility::numberToString(time(NULL)));

    // Things set automatically if not specified on the command-line
    kvpairs.insertMaybe("os", getOsName());
    kvpairs.insertMaybe("tester", getTester());
        
    // Generate SQL to insert this information.
    SqlDatabase::StatementPtr insert = tx->statement("insert into test_results (" +
                                                     StringUtility::join(", ", kvpairs.keys()) +
                                                     ") values (" +
                                                     StringUtility::join(", ", std::vector<std::string>(kvpairs.size(), "?")) +
                                                     ") returning id");
    int idx = 0;
    BOOST_FOREACH (const std::string &val, kvpairs.values())
        insert->bind(idx++, val);
    idx = insert->execute_int();

    tx->commit();
    mlog[INFO] <<"inserted test record #" <<idx <<"\n";
}
