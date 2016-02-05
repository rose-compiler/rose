#include <rose.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>
#include <time.h>

using namespace Sawyer::Message::Common;

struct Settings {
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    bool dryRun;                                        // if true, don't modify the database
    bool latestTests;                                   // operate only on the latest version of ROSE in the database

    Settings()
#ifdef DEFAULT_DATABASE
        : databaseUri(DEFAULT_DATABASE),
#endif
          dryRun(false), latestTests(false)
        {}
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("update cache for latest errors");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{action}");
    parser.errorStream(mlog[FATAL]);

    parser.doc("Description",
               "This tool performs various actions related to error messages produced by tests.  The @v{action} is "
               "one of the following words:"

               "@named{list}{List error messages in order of how often they occur.}"

               "@named{clear}{Erase all cached error information from the test_results table. This is useful if the "
               "regular expressions that search for errors have changed and the error information needs to be "
               "recomputed from scratch.}"

               "@named{update}{Search for the first error message in the output of failed commands and cache that "
               "information in the test_results table.  This information is only calculated for tests that failed, "
               "and which produced output, and which don't have a cached error message already.}"

               "@named{missing}{List basic information about tests that have no cached error message but which "
               "failed and produced output. This information is useful in order to adjust the regular expressions "
               "that match error messages in the output.}"

               "@named{count-missing}{Similar to the \"missing\" action except show only the number of such tests.}");


    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("Uniform resource locator for the database." + SqlDatabase::uriDocumentation()));

    sg.insert(Switch("dry-run")
              .intrinsicValue(true, settings.dryRun)
              .doc("Presence of this switch causes the database to not be modified, although all the queries "
                   "are still run.  When this switch is present, the transaction is not committed."));

    sg.insert(Switch("latest")
              .intrinsicValue(true, settings.latestTests)
              .doc("Operate on only the latest version of ROSE in the database."));

    return parser.with(CommandlineProcessing::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
}

// Render Unix time as a string when printing a database table
struct TimeRenderer: SqlDatabase::Renderer<int> {
    std::string operator()(const int &value, size_t width) const ROSE_OVERRIDE {
        struct tm tm;
        time_t t = value;
        localtime_r(&t, &tm);

        char buf[256];
        sprintf(buf, "%04d-%02d-%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
        return buf;
    }
} timeRenderer;

// We can't assume we have C++11 lambdas, so use a regular function
static bool
isLineFeed(char ch) {
    return ch == '\n';
}

// Remove linefeeds and special characters from an error message when rendering a table
static std::string
oneLineEscaped(const std::string &value) {
    std::vector<std::string> lines;
    boost::split(lines, value, isLineFeed, boost::token_compress_on);
    BOOST_FOREACH (std::string &s, lines)
        boost::trim(s);
    std::string s = boost::join(lines, " ");
    return StringUtility::cEscape(s);
}

// Generate the 'where' expression that limits what tests are being considered.
// Also appends variable values to the 'args' vector.
static std::string
sqlWhereClause(const SqlDatabase::TransactionPtr &tx, const Settings &settings, std::vector<std::string> &args /*in,out*/) {
    std::vector<std::string> constraints;

    if (settings.latestTests) {
        // Constrain the tests to be only the latest version of ROSE present in the database.
        SqlDatabase::StatementPtr q = tx->statement("select distinct rose, rose_date"
                                                    " from test_results"
                                                    " order by rose_date desc"
                                                    " limit 1");
        SqlDatabase::Statement::iterator row = q->begin();
        if (row != q->end()) {
            constraints.push_back("rose = ?");
            args.push_back(row.get<std::string>(0));
        }
    }

    if (constraints.empty())
        constraints.push_back("true");

    return " where " + boost::join(constraints, " and ");
}

// Bind arguments to a statement
static void
sqlBindArgs(const SqlDatabase::StatementPtr &stmt, const std::vector<std::string> &args) {
    for (size_t i=0; i<args.size(); ++i)
        stmt->bind(i, args[i]);
}

// Clear all cached error information from the database.
static void
clearErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("update test_results set first_error = null" +
                                                sqlWhereClause(tx, settings, args));
    sqlBindArgs(q, args);
    q->execute();
}

// Update the database by filling in test_results.first_error information for those tests that don't have a cached first error
// but which failed and have output.
static void
updateDatabase(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("update test_results test"
                                                " set first_error = substring(att.content from '(?n)("
                                                //----- regular expressions begin -----
                                                "\\merror: .+"
                                                "|catastrophic error: *\\n.+"
                                                "|^.* \\[err\\]: terminated after .+"
                                                "|^.* \\[err\\]: command died with .+"
                                                //----- regular expressions end -----
                                                ")')"
                                                " from attachments att" +
                                                sqlWhereClause(tx, settings, args) + " and"
                                                "    test.id = att.test_id and"
                                                "    test.first_error is null and"
                                                "    test.status <> 'end' and"
                                                "    att.name = 'Final output'");
    sqlBindArgs(q, args);
    q->execute();
}

// List tests that are missing error information.
static void
listMissingErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("select test.id, test.rose_date, test.os, users.name, test.tester"
                                                " from test_results test"
                                                " join attachments att on test.id = att.test_id"
                                                " join users on test.reporting_user = users.uid" +
                                                sqlWhereClause(tx, settings, args) + " and"
                                                "    test.first_error is null and"
                                                "    test.status <> 'end' and"
                                                "    att.name = 'Final output'"
                                                " order by test.id");
    sqlBindArgs(q, args);
    SqlDatabase::Table<int,                             // 0: id
                       int,                             // 1: rose_date
                       std::string,                     // 2: os
                       std::string,                     // 3: reporting_user
                       std::string>                     // 4: tester
        table(q);
    if (!table.empty()) {
        table.headers("Id", "ROSE date", "OS", "Reporting user", "Tester");
        table.reprint_headers(50);
        table.renderers().r1 = &timeRenderer;
        table.print(std::cout);
    }
}

// Count how many tests are missing first_error information when it should be available.
static void
countMissingErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("select count(*)"
                                                " from test_results test"
                                                " join attachments att on test.id = att.test_id" +
                                                sqlWhereClause(tx, settings, args) + " and"
                                                "    test.first_error is null and"
                                                "    test.status <> 'end' and"
                                                "    att.name = 'Final output'");
    sqlBindArgs(q, args);
    int n = q->execute_int();
    std::cout <<n <<"\n";
}

// List the errors ordered by how common they are.
static void
listErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("select count(*) as n, status, test.first_error"
                                                " from test_results test" +
                                                sqlWhereClause(tx, settings, args) + " and"
                                                " test.first_error is not null"
                                                " group by status, test.first_error"
                                                " order by n desc");
    sqlBindArgs(q, args);
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
        int count = row.get<int>(0);
        std::string status = row.get<std::string>(1);
        std::string mesg = row.get<std::string>(2);

        printf("%6d %-16s %s\n", count, status.c_str(), oneLineEscaped(mesg).c_str());
    }
}

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    mlog = Sawyer::Message::Facility("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);

    // Parse the command-line
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    if (args.size() != 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    if (args[0] == "clear") {
        clearErrors(tx, settings);
    } else if (args[0] == "update") {
        updateDatabase(tx, settings);
    } else if (args[0] == "missing") {
        listMissingErrors(tx, settings);
    } else if (args[0] == "count-missing") {
        countMissingErrors(tx, settings);
    } else if (args[0] == "list") {
        listErrors(tx, settings);
    } else {
        mlog[FATAL] <<"unknown command \"" <<StringUtility::cEscape(args[0]) <<"\"; see --help\n";
        exit(1);
    }

    if (settings.dryRun) {
        mlog[WARN] <<"database was not modified (running with --dry-run)\n";
    } else {
        tx->commit();
    }
}
