static const char *purpose = "update database with test result";
static const char *description =
    "Reads a file containing test results in the form of @v{key}=@v{value} lines and uses them to update "
    "the specified database. The list of valid keys can be obtained by running \"rose-matrix-query-table "
    "list\".  White space is allowed on either side of the equal sign, and white space is stripped from "
    "both the @v{key} and @v{value}. Blank lines and lines whose first non-space character is \"#\" are "
    "ignored.";

#include <rose.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <CommandLine.h>
#include <cstdio>
#include <cstring>
#include <LinearCongruentialGenerator.h>
#include <rose_getline.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Set.h>
#include <SqlDatabase.h>
#include <sys/utsname.h>
#include <unistd.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

struct Settings {
    bool dryRun;
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings(): dryRun(false)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

typedef Sawyer::Container::Map<std::string /*colname*/, std::string /*value*/> KeyValuePairs;
typedef Sawyer::Container::Map<std::string /*key*/, std::string /*colname*/> DependencyNames;

static Sawyer::Message::Facility mlog;

static boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("URI specifying which database to use. This switch overrides the ROSE_MATRIX_DATABASE environment variable. " +
                   SqlDatabase::uriDocumentation()));

    sg.insert(Switch("dry-run")
              .intrinsicValue(true, settings.dryRun)
              .doc("Do everything but update the database.  When this switch is present, the database is accessed "
                   "like normal, but the final COMMIT is skipped, causing the database to roll back to its initial "
                   "state."));

    Parser parser;
    parser.purpose(purpose);
    parser.errorStream(mlog[FATAL]);
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file}");
    parser.doc("Description", description);
    parser.doc("Output",
               "Emits the new test ID to standard output on success.");
    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(sg);

    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (args.size() != 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return args[0];
}

static DependencyNames
loadDependencyNames(const SqlDatabase::TransactionPtr &tx) {
    DependencyNames retval;
    SqlDatabase::StatementPtr q = tx->statement("select distinct name from dependencies");
    for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row) {
        std::string key = row.get<std::string>(0);
        retval.insert(key, "rmc_"+key);
    }
    return retval;
}

static void
extraDependencies(DependencyNames &depnames /*in,out*/) {
    depnames.insert("duration", "duration");
    depnames.insert("noutput", "noutput");
    depnames.insert("nwarnings", "nwarnings");
    depnames.insert("os", "os");
    depnames.insert("rose", "rose");
    depnames.insert("rose_date", "rose_date");
    depnames.insert("status", "status");
    depnames.insert("tester", "tester");
}

static KeyValuePairs
parseFile(const boost::filesystem::path &fileName, const DependencyNames &depnames) {
    KeyValuePairs retval;

    std::ifstream in(fileName.native().c_str());
    if (!in.good()) {
        mlog[FATAL] <<"cannot open " <<fileName <<"\n";
        exit(1);
    }

    for (size_t lineNumber = 1; in; ++lineNumber) {
        std::string line = rose_getline(in);
        boost::trim(line);
        if (line.empty() || '#' == line[0])
            continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) {
            mlog[FATAL] <<StringUtility::cEscape(fileName.native()) <<":" <<lineNumber <<": "
                        <<"not a key=value pair: \"" <<StringUtility::cEscape(line) <<"\"\n";
            exit(1);
        }
        std::string key = boost::trim_copy(line.substr(0, eq));
        std::string val = boost::trim_copy(line.substr(eq+1));
        if (key.empty() || val.empty()) {
            mlog[FATAL] <<StringUtility::cEscape(fileName.native()) <<":" <<lineNumber <<": "
                        <<"key and value must be non-empty: \"" <<StringUtility::cEscape(line) <<"\"\n";
            exit(1);
        }
        if (!depnames.exists(key)) {
            mlog[FATAL] <<StringUtility::cEscape(fileName.native()) <<":" <<lineNumber <<": "
                        <<"key \"" <<StringUtility::cEscape(key) <<"\" is not recognized\n";
            exit(1);
        }
        if (retval.exists(depnames[key])) {
            mlog[WARN] <<StringUtility::cEscape(fileName.native()) <<":" <<lineNumber <<": "
                       <<"key \"" <<StringUtility::cEscape(key) <<"\" specified more than once\n";
            exit(1);
        }
        if (val.size() >= 2 && '\'' == val[0] && '\'' == val[val.size()-1])
            val = val.substr(1, val.size()-2);

        retval.insert(depnames[key], val);
    }
    return retval;
}

void
check_exists(const KeyValuePairs &kvpairs, const std::string &key) {
    if (!kvpairs.exists(key)) {
        mlog[FATAL] <<"key is required: \"" <<StringUtility::cEscape(key) <<"\"\n";
        exit(1);
    }
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
    if (tx == NULL)
        return -1;
    std::string userName = getUserName();
    SqlDatabase::StatementPtr q = tx->statement("select id from auth_identities where identity = ?")->bind(0, userName);
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
getTester() {
    return getUserName() + " running " + Sawyer::CommandLine::Parser().programName();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    boost::filesystem::path fileName = parseCommandLine(argc, argv, settings);

    // Query the database to find valid keys
    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what();
        exit(1);
    }

    DependencyNames dependencyNames = loadDependencyNames(tx);
    extraDependencies(dependencyNames);

    // Parse and validate the key/val file
    KeyValuePairs kvpairs = parseFile(fileName, dependencyNames);

    // Some keys are required
    check_exists(kvpairs, "rose");
    check_exists(kvpairs, "status");
    check_exists(kvpairs, "tester");
    check_exists(kvpairs, "os");

    // Some keys can be set automatically
    kvpairs.insertMaybe("reporting_user", StringUtility::numberToString(getUserId(tx)));
    kvpairs.insertMaybe("reporting_time", StringUtility::numberToString(time(NULL)));
    kvpairs.insertMaybe("tester", getTester());

    // Debugging
    if (mlog[TRACE]) {
        BOOST_FOREACH (const KeyValuePairs::Node &node, kvpairs.nodes())
            mlog[TRACE] <<node.key() <<"=" <<node.value() <<"\n";
    }

    // Generate SQL to insert this information.
    int idx = 0;
    if (tx != NULL) {
        SqlDatabase::StatementPtr insert = tx->statement("insert into test_results (" +
                                                         StringUtility::join(", ", kvpairs.keys()) +
                                                         ") values (" +
                                                         StringUtility::join(", ",
                                                                             std::vector<std::string>(kvpairs.size(), "?")) +
                                                         ") returning id");
        BOOST_FOREACH (const std::string &val, kvpairs.values())
            insert->bind(idx++, val);
        idx = insert->execute_int();
    }

    if (settings.dryRun) {
        if (idx >= 0) {
            mlog[WARN] <<"test record #" <<idx <<" not inserted (running with --dry-run)\n";
        } else {
            mlog[WARN] <<"test record not inserted (no database and/or running with --dry-run)\n";
        }
    } else {
        tx->commit();
        mlog[INFO] <<"inserted test record #" <<idx <<"\n";
        std::cout <<idx <<"\n";
    }
}
