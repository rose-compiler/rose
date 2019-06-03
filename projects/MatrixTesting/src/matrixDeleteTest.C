#include <rose.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

struct Settings {
    bool dryRun;                                        // if set, don't modify the database
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
        : dryRun(false)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("delete all trace of a test from the database");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Description", "Deletes the tests specified as arguments.");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{test_ids}...");
    parser.errorStream(mlog[FATAL]);

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("dry-run")
              .intrinsicValue(true, settings.dryRun)
              .doc("This switch prevents modification of the database, although all other checking (including "
                   "connecting to the database and issuing database commands) is performed."));

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("Uniform resource locator for the database. This switch overrides the ROSE_MATRIX_DATABASE environment "
                   "variable. " + SqlDatabase::uriDocumentation()));

    return parser.with(Rose::CommandLine::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog[INFO].enable();

    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    std::vector<std::string> testIdStrings = parseCommandLine(argc, argv, settings);
    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what();
        exit(1);
    }

    // Check that input arguments are all non-negative integers in order to prevent SQL injection
    BOOST_FOREACH (const std::string &s, testIdStrings)
        (void) boost::lexical_cast<unsigned>(s);
    std::string inClause = " in (" + StringUtility::join(", ", testIdStrings) + ")";

    // Count how many tests will be deleted
    int nDeleted = tx->statement("select count(*) from test_results where id" + inClause)->execute_int();

    // Delete attachments first, then the test records
    tx->statement("delete from attachments where test_id" + inClause)->execute();
    tx->statement("delete from test_results where id " + inClause)->execute();

    // Commit changes
    if (settings.dryRun) {
        mlog[INFO] <<"no tests deleted (--dry-run)\n";
    } else {
        tx->commit();
        mlog[INFO] <<"deleted " <<StringUtility::plural(nDeleted, "tests") <<"\n";
    }
}
