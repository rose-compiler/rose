#include <rose.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>
#include <time.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

struct Settings {
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    bool dryRun;                                        // if true, don't modify the database
    bool latestTests;                                   // operate only on the latest version of ROSE in the database

    Settings() 
        :
#ifdef DEFAULT_DATABASE
          databaseUri(DEFAULT_DATABASE),
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
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{action} [@v{args}...]");
    parser.errorStream(mlog[FATAL]);

    parser.doc("Description",
               "This tool performs various actions related to error messages produced by tests.  The @v{action} is "
               "one of the following words:"

               "@named{list}{List error messages in order of how often they occur. This action takes no arguments.}"

               "@named{clear}{Erase all cached error information from the test_results table. This is useful if the "
               "regular expressions that search for errors have changed and the error information needs to be "
               "recomputed from scratch. An optional list of test identification numbers can be provided in order "
               "to clear the cached errors from only those tests.}"

               "@named{update}{Search for the first error message in the output of failed commands and cache that "
               "information in the test_results table.  This information is only calculated for tests that failed, "
               "and which produced output, and which don't have a cached error message already.  An optional list of "
               "test identifiers can be provided as arguments, in which case the previously mentioned test list is "
               "further constrained to include only those tests mentioned as arguments.}"

               "@named{missing}{List basic information about tests that have no cached error message but which "
               "failed and produced output. This information is useful in order to adjust the regular expressions "
               "that match error messages in the output.}"

               "@named{count-missing}{Similar to the \"missing\" action except show only the number of such tests.}");


    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("Uniform resource locator for the database. This switch overrides the ROSE_MATRIX_DATABASE environment "
                   "variable. " + SqlDatabase::uriDocumentation()));

    sg.insert(Switch("dry-run")
              .intrinsicValue(true, settings.dryRun)
              .doc("Presence of this switch causes the database to not be modified, although all the queries "
                   "are still run.  When this switch is present, the transaction is not committed."));

    sg.insert(Switch("latest")
              .intrinsicValue(true, settings.latestTests)
              .doc("Operate on only the latest version of ROSE in the database."));

    return parser.with(Rose::CommandLine::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
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

    return " where " + boost::join(constraints, " and ") + " ";
}

// Bind arguments to a statement
static void
sqlBindArgs(const SqlDatabase::StatementPtr &stmt, const std::vector<std::string> &args) {
    for (size_t i=0; i<args.size(); ++i)
        stmt->bind(i, args[i]);
}

// Build an expression to limit test IDs
static std::string
sqlIdLimitation(const std::string &columnName, const std::vector<int> &testIds) {
    if (!testIds.empty()) {
        std::string sql;
        BOOST_FOREACH (int id, testIds)
            sql += (sql.empty() ? "" : ", ") + boost::lexical_cast<std::string>(id);
        sql = " " + columnName + " in (" + sql + ") ";
        return sql;
    } else {
        return " true ";
    }
}

// Clear all cached error information from the database.
static void
clearErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings, const std::vector<int> &testIds) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("update test_results set first_error = null" +
                                                sqlWhereClause(tx, settings, args) +
                                                "and" + sqlIdLimitation("test_results.id", testIds));
    sqlBindArgs(q, args);
    q->execute();
}

// Update the database by filling in test_results.first_error information for those tests that don't have a cached first error
// but which failed and have output.
static void
updateDatabase(const SqlDatabase::TransactionPtr &tx, const Settings &settings, const std::vector<int> &testIds) {
    std::vector<std::string> args;

    std::string outputSeparatorLine = "=================-================="; // important! '=' * 17 + '-' + '=' * 17
    std::string outputSeparatorRe = outputSeparatorLine + " [-_a-zA-Z0-9]+ " + outputSeparatorLine;

    std::string sawyerAssertionFailedRe = "\\[FATAL\\]: assertion failed:\n"
                                          "(.*\\[FATAL\\]:.*\n)*"; // file name, function, expression, message

    //---------------------------------------------------------------------------------------------------
    // Step 1: Search for error text in output and save it in the first_error_staging column.
    //---------------------------------------------------------------------------------------------------

    // Find and store the error messages into the first_error_staging column.
    SqlDatabase::StatementPtr q;
    q = tx->statement("update test_results test"
                      " set first_error_staging = substring("

                      // We want to search for error messages only in the last section of output (if
                      // there's more than one).
                      "regexp_replace(att.content, '.*" + outputSeparatorLine + "\\n(.*)', '\\1')"

                      "from '(?n)("
                      //----- regular expressions begin -----
                      "\\merror: .+"                            // general error
                      "|\\mERROR: [^0-9].*"                     // not error counts from cmake
                      "|" + sawyerAssertionFailedRe +
                      "|\\[(ERROR|FATAL) *\\].*"                // Sawyer error message
                      "|catastrophic error: *\\n.+"             // ROSE translator compile error
                      "|^.* \\[err\\]: terminated after .+"     // RTH timeout
                      "|^.* \\[err\\]: command died with .+"    // RTH_RUN failure
                      "|^.* \\[err\\]: +what\\(\\): .*"         // C++ exception
                      "|^.* \\[err\\]: .*Segmentation fault"    // shell output
                      "|^.* line [0-9]+:.*Segmentation fault.*" // shell output
                      "|^.*run-test: .*: command failed with exit status.*" // Tup testing
                      "|Assertion `.*'' failed\\.$"             // failed <cassert> assertion
                      "|^.*: undefined reference to `.*"        // GNU linker error
                      "|No space left on device"
                      "|relocation R_X86_64_32S against `.*'' can not be used when making a shared object.*"
                      "|31;1m\\d+ TESTS FAILED. See above list for details\\." // Markus' STL tests
                      "|^make\\[[0-9]+\\]: \\*\\*\\* No rule to make target.*"
                      "|^Makefile:[0-9]+: recipe for target ''.*'' failed"
                      "|^make\\[[0-9]+\\]: \\*\\*\\* \\[.+\\] Error [0-9]+"
                      "|\\*{7} HPCTOOLKIT .* FAILED \\*{9}"
                      "|^.*: line \\d+: .* command not found"   // shell script
                      "|^make: (.*): Command not found"         // GNU make
                      "|^Error:\\n  .*"                         // Tupfile user-defined error
                      "|^run-test: (.*): Result differs from precomputed answer" // Tup testing
                      "|line [0-9]+:.*/\\.libs/lt-.*: Invalid argument" // intermittent libtool failure

                      "|\\merror: ?\n.*"                        // ROSE error on next line
                      //----- regular expressions end -----
                      ")')"
                      " from attachments att" +
                      sqlWhereClause(tx, settings, args) +
                      "    and test.id = att.test_id"
                      "    and test.first_error is null"
                      "    and test.status <> 'end'"
                      "    and att.name = 'Final output'"
                      "    and " + sqlIdLimitation("test.id", testIds));
    sqlBindArgs(q, args);
    q->execute();

    //---------------------------------------------------------------------------------------------------
    // Step 2: Massage the first_error_staging values so similar errors have equal text.
    //---------------------------------------------------------------------------------------------------

    // Replace absolute file names "/foo/bar/baz" with "/.../baz"
    std::string fileNameChar    =  "[-+=_.a-zA-Z0-9]";
    std::string nonFileNameChar = "[^-+=_.a-zA-Z0-9]";

    // Replace temporary file names with "temp-file"
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(/tmp(/" + fileNameChar + "+)+)',"
                  "   'temp-file',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // Replace file paths with just the last component of the path
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(^|" + nonFileNameChar + ")((/" + fileNameChar + "+)+)/(" + fileNameChar + "+)',"
                  "   '\\1/.../\\4',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // Remove the process ID and time from Sawyer::Message prefixes: "a.out[123] 45.678" => "a.out[...] ..."
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(" + fileNameChar + ")\\[[0-9]+\\] [0-9]+\\.[0-9]+s ',"
                  "   '\\1[...] ... ',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // The shell likes to print process IDs also, so remove them. "deepDelete [err]: sh: line 1: 1518 Segmentation fault" =>
    // "deepDelete [err]: sh: line 1: ... Segmentation fault"
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(\\[err\\]: sh: line [0-9]+: )[0-9]+ ',"
                  "   '\\1... ',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // RMC/Spock installation errors have temporary file names that should be removed
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(installation script failed for .*); see .* for details',"
                  "   '\\1',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // spock-shell conflict messages include hashes that should be removed
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '([0-9])@[0-9a-f]{8}',"
                  "   '\\1',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    // spock-shell errors for previously failed installations contains temporary file name
    tx->statement("update test_results test"
                  " set first_error_staging = regexp_replace(first_error_staging,"
                  "   '(installation was previously attempted and failed).*(for .*);.*',"
                  "   '\\1 \\2',"
                  "   'g')"
                  " where test.first_error_staging is not null"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();
    
    //---------------------------------------------------------------------------------------------------
    // Step 3: Move the staging area values to their final home.
    //---------------------------------------------------------------------------------------------------

    tx->statement("update test_results test"
                  " set first_error = first_error_staging,"
                  "     first_error_staging = null"
                  " where test.first_error_staging is not null and test.first_error_staging <> ''"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();

    //---------------------------------------------------------------------------------------------------
    // Step 4: Unrelated to above, populate the 'blacklisted' column
    //---------------------------------------------------------------------------------------------------

    std::string blacklistPrefixRe = "configure: error: blacklisted: ";
    tx->statement("update test_results as test"
                  " set blacklisted ="
                  "   regexp_replace("
                  "     coalesce(substring(att.content from '(?n)" + blacklistPrefixRe + ".*'), ''),"
                  "     '" + blacklistPrefixRe + "',"
                  "     '')"
                  " from attachments as att " +
                  sqlWhereClause(tx, settings, args) +
                  " and test.id = att.test_id"
                  " and att.name = 'Final output'"
                  " and " + sqlIdLimitation("test.id", testIds))
        ->execute();
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

// List errors for specific test IDs
static void
listErrors(const SqlDatabase::TransactionPtr &tx, const Settings &settings, const std::vector<int> &testIds) {
    std::vector<std::string> args;
    SqlDatabase::StatementPtr q = tx->statement("select id, coalesce(first_error,'') from test_results" +
                                                sqlWhereClause(tx, settings, args) +
                                                "and " + sqlIdLimitation("id", testIds) +
                                                "order by id");
    sqlBindArgs(q, args);
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row) {
        int testId = row.get<int>(0);
        std::string message = row.get<std::string>(1);
        std::cout <<"configuration #" <<testId <<":\n"
                  <<"    " <<message <<"\n";
    }
}

// Parse strings as integers.
static std::vector<int>
parseIds(const std::vector<std::string> &strings, size_t startAt = 0) {
    std::vector<int> retval;
    for (size_t i=startAt; i<strings.size(); ++i) {
        try {
            retval.push_back(boost::lexical_cast<int>(strings[i]));
        } catch (const boost::bad_lexical_cast&) {
            mlog[FATAL] <<"invalid test ID \"" <<StringUtility::cEscape(strings[i]) <<"\"\n";
            exit(1);
        }
    }
    return retval;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    // Parse the command-line
    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what();
        exit(1);
    }

    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    if (args[0] == "clear") {
        std::vector<int> ids = parseIds(args, 1);
        clearErrors(tx, settings, ids);
    } else if (args[0] == "update") {
        std::vector<int> ids = parseIds(args, 1);
        updateDatabase(tx, settings, ids);
    } else if (args[0] == "missing") {
        if (args.size() != 1) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        listMissingErrors(tx, settings);
    } else if (args[0] == "count-missing") {
        if (args.size() != 1) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        countMissingErrors(tx, settings);
    } else if (args[0] == "list" && args.size()==1) {
        if (args.size() != 1) {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        listErrors(tx, settings);
    } else if (args[0] == "list") {
        std::vector<int> ids = parseIds(args, 1);
        listErrors(tx, settings, ids);
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
