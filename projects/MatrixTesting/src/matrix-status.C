static const char *gPurpose = "report portability testing status";
static const char *gDescription =
    "To be written.";


#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <boost/format.hpp>
#include <boost/regex.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    std::string databaseUri;
    std::string roseVersion;                            // if non-empty, use this version instead of latest
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Symopsis", "@prop{programName} [@v{switches}]");

    SwitchGroup sg("Tool-specific switches");
    insertDatabaseSwitch(sg, settings.databaseUri);

    sg.insert(Switch("rose", 'b')
              .argument("committish", anyParser(settings.roseVersion))
              .doc("Query the database for the specified ROSE version rather than showing information for "
                   "the latest test result. When specifying a hash, only full hashes are supported, not "
                   "abbreviated hashes."));

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(sg)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

static const FormattedTable::CellProperties&
badCell() {
    static bool called;
    static FormattedTable::CellProperties p;
    if (!called) {
        p.foreground(Color::HSV_RED);
        called = true;
    }
    return p;
}

static const FormattedTable::CellProperties&
goodCell() {
    static bool called;
    static FormattedTable::CellProperties p;
    if (!called) {
        p.foreground(Color::HSV_GREEN);
        called = true;
    }
    return p;
}

static void
showNotices(const Settings &settings, DB::Connection db, const Sawyer::Container::Map<std::string, std::string> &props) {
    FormattedTable table;
    table.indentation("    ");

    boost::regex noticeRe("NOTICE_(\\d+)");
    for (auto node: props.nodes()) {
        boost::smatch found;
        if (boost::regex_match(node.key(), found, noticeRe)) {
            const size_t i = table.nRows();
            std::string name = found.str(1);
            if (8 == name.size())                       // yyyyddmm
                name = name.substr(0, 4) + "-" + name.substr(4, 2) + "-" + name.substr(6);

            table.insert(i, 0, name);
            table.insert(i, 1, node.value());
        }
    }

    if (table.nRows() > 0) {
        std::cout <<"================================================================================\n"
                  <<"=== Notices to ROSE developers                                               ===\n"
                  <<"================================================================================\n"
                  <<"\n"
                  <<table
                  <<"\n";
    }
}

static void
showSlaveConfig(const Settings &settings, DB::Connection db) {
    Sawyer::Container::Map<std::string, std::string> props;
    auto stmt = db.stmt("select name, value from slave_settings");
    for (auto row: stmt)
        props.insert(*row.get<std::string>(0), *row.get<std::string>(1));

    showNotices(settings, db, props);

    std::cout <<"================================================================================\n"
              <<"=== Slave configuration                                                      ===\n"
              <<"================================================================================\n"
              <<"This section contains information about what the testing slaves should be\n"
              <<"testing, and how.\n"
              <<"\n";

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, "Operational setting");
    table.columnHeader(0, 1, "Value");
    table.columnHeader(0, 2, "Key");

    int i = 0;
    Sawyer::Optional<std::string> testRepo = props.getOptional("TEST_REPOSITORY");
    table.insert(i, 0, "ROSE repository to be tested");
    table.insert(i, 2, "TEST_REPOSITORY");
    if (testRepo && !testRepo->empty()) {
        table.insert(i, 1, *testRepo);
    } else {
        table.insert(i, 1, "testing is disabled");
        table.cellProperties(i, 1, badCell());
    }
    ++i;

    Sawyer::Optional<std::string> testCommittish = props.getOptional("TEST_COMMITTISH");
    table.insert(i, 0, "ROSE commit or tag to be tested");
    table.insert(i, 2, "TEST_COMMITTISH");
    if (testCommittish && !testCommittish->empty()) {
        table.insert(i, 1, *testCommittish);
    } else {
        table.insert(i, 1, "testing is disabled");
        table.cellProperties(i, 1, badCell());
    }
    i += 2;

    table.insert(i, 0, "Testing tools repository to be used");
    table.insert(i, 1, props.getOptional("MATRIX_REPOSITORY").orElse("none"));
    table.insert(i, 2, "MATRIX_REPOSITORY");
    ++i;

    table.insert(i, 0, "Testing tools commit or tag");
    table.insert(i, 1, props.getOptional("MATRIX_COMMITTISH").orElse("none"));
    table.insert(i, 2, "MATRIX_COMMITTISH");
    ++i;

    table.insert(i, 0, "Testing environment version");
    table.insert(i, 1, props.getOptional("TEST_ENVIRONMENT_VERSION").orElse("none"));
    table.insert(i, 2, "TEST_ENVIRONMENT_VERSION");
    ++i;

    table.insert(i, 0, "Testing operational flags");
    table.insert(i, 1, props.getOptional("TEST_FLAGS").orElse("none"));
    table.insert(i, 2, "TEST_FLAGS");
    i += 2;

    table.insert(i, 0, "Operating systems to be tested");
    table.insert(i, 1, props.getOptional("TEST_OS").orElse("none"));
    table.insert(i, 2, "TEST_OS");

    std::cout <<table <<"\n\n";
}

static void
showSlaveHealth(const Settings &settings, DB::Connection db) {
    std::cout <<"================================================================================\n"
              <<"=== Slave health                                                             ===\n"
              <<"================================================================================\n"
              <<"This section contains information about what testing slaves have run in the last\n"
              <<"week.\n"
              <<"\n";

    //                          0     1          2         3           4        5
    auto stmt = db.stmt("select name, timestamp, load_ave, free_space, test_id, event"
                        " from slave_health"
                        " where timestamp >= ?since"
                        " order by timestamp desc, name")
                .bind("since", time(NULL) - 7*86400);

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, "Slave Name");
    table.columnHeader(0, 1, "Latest Report from Slave");
    table.columnHeader(0, 2, "Load\nAverage");
    table.columnHeader(0, 3, "Free\nSpace");
    table.columnHeader(0, 4, "Latest\nTest ID");
    table.columnHeader(0, 5, "Latest\nEvent");
    for (auto row: stmt) {
        const size_t i = table.nRows();
        table.insert(i, 0, row.get<std::string>(0).orElse("none"));
        time_t whenReported = row.get<time_t>(1).orElse(0);
        table.insert(i, 1, timeToLocal(whenReported) + ", " + approximateAge(whenReported));
        table.insert(i, 2, (boost::format("%6.2f%%") % row.get<double>(2).orElse(0)).str());
        table.insert(i, 3, (boost::format("%.0f GB") % (row.get<size_t>(3).orElse(0) / 1024.0)).str());
        table.insert(i, 4, row.get<std::string>(4).orElse("none"));
        table.insert(i, 5, row.get<std::string>(5).orElse("none"));
    }
    std::cout <<table <<"\n\n";
}

// Show latest tested ROSE versions and return their commits, starting with the most recent
static std::vector<std::string>
showLatestTestedRoseVersions(const Settings &settings, DB::Connection db) {
    std::cout <<"================================================================================\n"
              <<"=== ROSE versions tested recently                                            ===\n"
              <<"================================================================================\n"
              <<"These are the ROSE versions that have test results in the last week.\n"
              <<"\n";

    auto stmt = db.stmt("select rose, rose_date, count(*), min(reporting_time), max(reporting_time)"
                        " from test_results"
                        " where reporting_time >= ?since"
                        " group by rose, rose_date"
                        " order by rose_date desc")
                .bind("since", time(NULL) - 7 * 86400);

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, "ROSE version");
    table.columnHeader(0, 1, "Commit date");
    table.columnHeader(0, 2, "Test\nCount");
    table.columnHeader(0, 3, "Earliest\nTest");
    table.columnHeader(0, 4, "Latest\nTest");
    std::vector<std::string> retval;
    for (auto row: stmt) {
        const size_t i = table.nRows();
        retval.push_back(row.get<std::string>(0).orElse("unknown"));
        table.insert(i, 0, retval.back());
        table.insert(i, 1, timeToLocal(row.get<time_t>(1).orElse(0)));
        table.insert(i, 2, row.get<std::string>(2).orElse("unknown"));
        table.insert(i, 3, timeToLocal(row.get<time_t>(3).orElse(0)));
        table.insert(i, 4, timeToLocal(row.get<time_t>(4).orElse(0)));
    }
    std::cout <<table <<"\n\n";
    return retval;
}

// Print some general information about a ROSE version and return true if it exists in the database
static bool
showRoseVersion(const Settings &settings, DB::Connection db, const std::string &roseVersion) {

    auto commitTime = db.stmt("select rose_date from test_results where rose = ?rose limit 1")
                      .bind("rose", roseVersion)
                      .get<time_t>();

    if (commitTime) {
        std::cerr <<"ROSE version " <<roseVersion <<" committed " <<timeToLocal(*commitTime) <<"\n";
        auto row = db.stmt("select count(*), min(reporting_time), max(reporting_time)"
                           " from test_results"
                           " where rose = ?rose")
                   .bind("rose", roseVersion)
                   .begin();
        std::cerr <<"tested " <<StringUtility::plural(row->get<size_t>(0).orElse(0), "times")
                  <<" from " <<timeToLocal(row->get<time_t>(1).orElse(0))
                  <<" to " <<timeToLocal(row->get<time_t>(2).orElse(0)) <<"\n";
        return true;

    } else {
        mlog[ERROR] <<"ROSE version " <<roseVersion <<" does not exist in the database\n";
        return false;
    }
}

static void
showTestPhases(const Settings &settings, DB::Connection db) {
    std::cout <<"================================================================================\n"
              <<"=== Test phases                                                              ===\n"
              <<"================================================================================\n"
              <<"These are the phases performed by each test. If a phase fails, the status for\n"
              <<"the test is the name of the failed phase. The name \"end\" means that all\n"
              <<"phases of the test passed. The phases are executed in order until one fails\n"
              <<"or the end is reached.\n"
              <<"\n";

    auto stmt = db.stmt("select name, purpose"
                        " from test_names"
                        " where purpose is not null and purpose <> ''"
                        " order by position");

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, "Status");
    table.columnHeader(0, 1, "Purpose");
    for (auto row: stmt) {
        const std::string name = row.get<std::string>(0).orDefault();
        const size_t i = table.nRows();
        table.insert(i, 0, name);
        table.cellProperties(i, 0, "end" == name ? goodCell() : badCell());
        table.insert(i, 1, *row.get<std::string>(1));
    }
    std::cout <<table <<"\n";
}

static void
showTestResultsByField(const Settings &settings, DB::Connection db, const std::string &roseVersion,
                       const std::string &fieldName, const std::string fieldTitle) {
    auto stmt = db.stmt("select " + fieldName + ", status, count(*)"
                        " from test_results"
                        " left outer join test_names on (test_results.status = test_names.name)"
                        " where rose = ?rose"
                        " group by " + fieldName + ", status, test_names.position"
                        " order by " + fieldName + ", test_names.position")
                .bind("rose", roseVersion);

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, fieldTitle);
    table.columnHeader(0, 1, "Reported\nStatus");
    table.columnHeader(0, 2, "Test\nCount");
    std::string prevFieldValue;
    for (auto row: stmt) {
        const size_t i = table.nRows();
        const std::string fieldValue = row.get<std::string>(0).orElse("unknown");
        table.insert(i, 0, fieldValue == prevFieldValue ? "" : fieldValue);
        const std::string status = row.get<std::string>(1).orElse("unknown");
        table.insert(i, 1, status);
        table.cellProperties(i, 1, "end" == status ? goodCell() : badCell());
        table.insert(i, 2, row.get<std::string>(2).orElse("unknwon"));
        prevFieldValue = fieldValue;
    }
    std::cout <<table <<"\n";
}

// Show test results sorted in various ways
static void
showTestResults(const Settings &settings, DB::Connection db, const std::string &roseVersion) {
    std::cout <<"================================================================================\n"
              <<"=== Test results                                                             ===\n"
              <<"================================================================================\n"
              <<"These are the test results for ROSE version " <<roseVersion <<"\n"
              <<"\n";

    std::cout <<"Test results by analysis language:\n";
    showTestResultsByField(settings, db, roseVersion, "rmc_languages", "Analysis\nLanguages");

    std::cout <<"Test results by operating system:\n";
    showTestResultsByField(settings, db, roseVersion, "os", "Operating\nSystem");

    std::cout <<"Test results by host compiler:\n";
    showTestResultsByField(settings, db, roseVersion, "rmc_compiler", "Compiler");

    std::cout <<"Test results by Boost version:\n";
    showTestResultsByField(settings, db, roseVersion, "rmc_boost", "Boost\nVersion");
}

// Show error messages
static void
showErrorsByField(const Settings &settings, DB::Connection db, const std::string &roseVersion,
                         const std::string &fieldName, const std::string &fieldTitle) {

    auto stmt = db.stmt("select " + fieldName + ", status, count(*) as count, first_error"
                        " from test_results"
                        " left outer join test_names on (test_results.status = test_names.name)"
                        " where rose = ?rose and status <> 'end' and first_error is not null and first_error <> ''"
                        " group by " + fieldName + ", status, test_names.position, first_error"
                        " order by " + fieldName + ", test_names.position, count desc"
                        " limit 20")
                .bind("rose", roseVersion);

    FormattedTable table;
    table.indentation("    ");
    table.columnHeader(0, 0, fieldTitle);
    table.columnHeader(0, 1, "Test\nStatus");
    table.columnHeader(0, 2, "Error\nCount");
    table.columnHeader(0, 3, "Error Message");
    std::string prevFieldValue, prevStatus;
    for (auto row: stmt) {
        const size_t i = table.nRows();
        const std::string fieldValue = row.get<std::string>(0).orElse("unknown");
        table.insert(i, 0, fieldValue == prevFieldValue ? "" : fieldValue);
        if (prevFieldValue != fieldValue) {
            prevFieldValue = fieldValue;
            prevStatus = "";
        }

        const std::string status = row.get<std::string>(1).orElse("unknown");
        table.insert(i, 1, status == prevStatus ? "" : status);
        prevStatus = status;

        table.insert(i, 2, *row.get<std::string>(2));
        table.insert(i, 3, *row.get<std::string>(3));
    }
    std::cout <<table <<"\n";
}

static void
showErrors(const Settings &settings, DB::Connection db, const std::string &roseVersion) {
    std::cout <<"================================================================================\n"
              <<"=== Error messages                                                           ===\n"
              <<"================================================================================\n"
              <<"These are the heuristically determined first error messages from testing\n"
              <<"ROSE version " <<roseVersion <<"\n"
              <<"\n";

    std::cout <<"Errors by analysis language:\n";
    showErrorsByField(settings, db, roseVersion, "rmc_languages", "Analysis\nLanguages");

    std::cout <<"Errors by operating system:\n";
    showErrorsByField(settings, db, roseVersion, "os", "Operation\nSystem");

    std::cout <<"Errors by host compiler:\n";
    showErrorsByField(settings, db, roseVersion, "rmc_compiler", "Compiler");

    std::cout <<"Errors by boost version:\n";
    showErrorsByField(settings, db, roseVersion, "rmc_boost", "Boost\nVersion");
}

static void
showAdditionalCommands(const Settings &settings, DB::Connection db, const std::string &roseVersion) {
    std::cout <<"================================================================================\n"
              <<"=== Additional information                                                   ===\n"
              <<"================================================================================\n"
              <<"This section lists some useful information for showing additional results.\n"
              <<"\n";

    std::cout <<"1. Get test IDs having a certain error.\n\n"
              <<"   To get a list of test IDs that had a certain error message, use rose-matrix-query\n"
              <<"   and constrain the ROSE version, match (with the \"~\" operator) part of an error\n"
              <<"   message, and display the operating system (\"os\"), test status (\"status\"), and\n"
              <<"   test identification (\"id\").\n"
              <<"\n"
              <<"   $ rose-matrix-query rose=" <<abbreviatedVersion(roseVersion) <<" \\\n"
              <<"         first_error~'undefined reference' os status id\n"
              <<"\n"
              <<"   Use `rose-matrix-query --help` and `rose-matrix-query list` to get information\n"
              <<"   about how to use this tool.\n"
              <<"\n";

    std::cout <<"2. Get the output from a test.\n\n"
              <<"   To get the final few hundred lines of output from a test, you need to know the\n"
              <<"   test identification number, which you can get from `rose-matrix-query`, such as\n"
              <<"   the previous example. Then run `rose-matrix-output` with that ID number. Example:\n"
              <<"\n"
              <<"   $ rose-matrix-output 331483 |less\n"
              <<"\n";

    std::cout <<"3. View test attachments.\n\n"
              <<"   The `rose-matrix-output` shell script in the previous example simply runs the\n"
              <<"   `rose-matrix-attachments` tool to print the test output attachments. There are\n"
              <<"   often additional attachments for a test, such as a summary of each step that was\n"
              <<"   run during the test.\n"
              <<"\n"
              <<"   To get the list of attachments for a test, give just a test ID, as in:\n"
              <<"\n"
              <<"   $ rose-matrix-attachments 331483\n"
              <<"   [440904]    \"Commands\"\n"
              <<"   [440905]    \"Final output\"\n"
              <<"\n"
              <<"   To view the attachment called \"Commands\", run:\n"
              <<"\n"
              <<"   $ rose-matrix-attachments 331483 440904\n"
              <<"\n"
              <<"   Among other things, this has the list of ROSE dependencies and versions that were\n"
              <<"   employed by the test, the complete ROSE configure command-line, and the locations\n"
              <<"   of the most frequent compiler warning messages from compiling the ROSE library.\n"
              <<"\n";

    std::cout <<"4. See what dependencies are configured for testing.\n\n"
              <<"   Portability tests have two kinds of dependencies: (1) dependencies that are installed\n"
              <<"   system-wide on the operating system in which the test runs, such as bash, grep, perl,\n"
              <<"   the system C++ compiler, etc., and (2) dependencies that are installed per test, such as\n"
              <<"   Boost, Z3, Python, Doxygen, and non-system C++ compilers. The latter are managed by\n"
              <<"   RMC/Spock -- the Rose Meta-Configuration System.  The `rose-matrix-dependencies` tool\n"
              <<"   can show you what dependencies are being tested. A dependency that's \"enabled\" is\n"
              <<"   one for which tests are being requested. Some dependencies are marked as being\n"
              <<"   supported, which means errors with these dependencies are of particular interest to\n"
              <<"   the ROSE development team.\n"
              <<"\n"
              <<"   $ rose-matrix-dependencies names      # Show all the dependency categories\n"
              <<"   $ rose-matrix-dependencies list boost # Show information for one category\n"
              <<"\n";

    std::cout <<"5. Reproduce a build environment for debugging.\n\n"
              <<"   Users can easliy install RMC/Spock in their home directories in order to mostly replicate\n"
              <<"   the environment in which a test was run. You'll still have to install any system dependencies\n"
              <<"   manually, but any ROSE developer should already have these anyway. RMC/Spock is available\n"
              <<"   from https://github.com/matzke1/rmc-spock. Install or upgrade it with:\n"
              <<"\n"
              <<"   $ git clone https://github.com/matzke1/rmc-spock\n"
              <<"   $ (cd rmc-spock && ./scripts/bootstrap.sh)\n"
              <<"   $ rm -rf rmc-spock\n"
              <<"   $ export PATH=\"$HOME/.spock/bin\"\n"
              <<"   $ rm -rf ~/.spock # If you ever want to totally remove it\n"
              <<"\n"
              <<"   To reproduce a build environment, create a \"_build\" subdirectory at the top of your\n"
              <<"   ROSE source tree, then use the `rose-matrix-attachments` command mentioned earlier to\n"
              <<"   download the \".rmc-main.cfg\" file for the test whose build environment you want to\n"
              <<"   recreate. Then, run these commands:\n"
              <<"\n"
              <<"   $ cd _build\n"
              <<"   $ rmc # this installs dependencies and puts you in a subshell\n"
              <<"   $ rmc build # this runs ROSE's \"build\" script\n"
              <<"   $ rmc config # run ROSE's \"configure\" script; use -n to see the command without running it\n"
              <<"   $ rmc make # or run \"make\" or \"tup\" (depending on teh build system) manually\n"
              <<"\n";
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    const std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    if (settings.databaseUri.empty()) {
        mlog[FATAL] <<"no database specified\n";
        mlog[INFO] <<"You need to use the --database switch, or set your ROSE_MATRIX_DATABASE environment\n"
                   <<"variable. See the \"Testing\" section of https://toc.rosecompiler.org for the proper\n"
                   <<"setting.\n";
        exit(1);
    }
    auto db = DB::Connection::fromUri(settings.databaseUri);

    showSlaveConfig(settings, db);
    showSlaveHealth(settings, db);
    std::vector<std::string> roseCommits = showLatestTestedRoseVersions(settings, db);
    showTestPhases(settings, db);

    if (!settings.roseVersion.empty()) {
        roseCommits.clear();
        roseCommits.push_back(settings.roseVersion);
        std::cerr <<"The following output is limited to the version selected from the command line, namely\n";
        if (!showRoseVersion(settings, db, roseCommits.back()))
            exit(1); // user requested a version, but we can't show it; error was already printed
        std::cerr <<"\n\n";
    }
    if (roseCommits.empty())
        exit(0);

    showTestResults(settings, db, roseCommits.front());
    showErrors(settings, db, roseCommits.front());

    showAdditionalCommands(settings, db, roseCommits.front());
}
