static const char *gPurpose = "find error messages in test output";
static const char *gDescription =
    "This tool performs various actions related to error messages produced by tests. Error messages are detected "
    "heuristically by scanning the saved test output with regular expressions, and the first detected error message "
    "is recorded in the \"first_error\" property of the test. The @v{action} argument is one of the following "
    "words:"

    "@named{clear}{Erase the \"first_error\" property for the test ID numbers given in the subsequent arguments. If "
    "no test IDs are specified, then the \"first_error\" is cleared for all tests.  This is useful when the regular "
    "expressions are updated and the test output needs to be re-scanned to find the first errors.}"

    "@named{update}{Search for the first error message in the output of failed commands and cache them in the "
    "\"first_error\" property.  This information is only calculated for tests that failed, and which produced output "
    "stored in the \"Final output\" attachment, and which don't have a cached error message already, and whose ID numbers "
    "are listed as additional arguments. If no test ID arguments are specified, then all tests are updated.}";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Escape.h>
#include <boost/lexical_cast.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] clear [@v{test_ids}...]\n\n"
               "@prop{programName} [@v{switches}] update [@v{test_ids}...]");

    SwitchGroup sg("Tool-specific switches");

    insertDatabaseSwitch(sg, settings.databaseUri);

    std::vector<std::string> args = parser
                                    .with(Rose::CommandLine::genericSwitches())
                                    .with(sg)
                                    .parse(argc, argv)
                                    .apply()
                                    .unreachedArgs();

    if (args.empty() || ("clear" != args[0] && "update" != args[0])) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return args;
}

// Parse strings as integers.
static std::vector<int>
parseIds(const std::vector<std::string> &strings, size_t startAt = 0) {
    std::vector<int> ids;
    ids.reserve(strings.size() - startAt);
    for (size_t i = startAt; i < strings.size(); ++i) {
        try {
            ids.push_back(boost::lexical_cast<int>(strings[i]));
        } catch (const boost::bad_lexical_cast&) {
            mlog[FATAL] <<"invalid test ID \"" <<StringUtility::cEscape(strings[i]) <<"\"\n";
            exit(1);
        }
    }
    return ids;
}

// Build an expression to limit test IDs
static std::string
sqlIdLimitation(const std::string &columnName, const std::vector<int> &testIds) {
    if (!testIds.empty()) {
        std::string sql;
        for (int id: testIds)
            sql += (sql.empty() ? " " + columnName + " in (" : ", ") + boost::lexical_cast<std::string>(id);
        sql += ")";
        return sql;
    } else {
        return " true ";
    }
}

// Clear all cached error information from the database.
static void
clearErrors(DB::Connection db, const std::vector<int> &testIds) {
    db.stmt("update test_results set first_error = null, blacklisted = null"
            " where " + sqlIdLimitation("test_results.id", testIds))
        .run();
}

// Update the database by filling in test_results.first_error information for those tests that don't have a cached first error
// but which failed and have output.
static void
updateDatabase(DB::Connection db, const std::vector<int> &testIds) {
    std::vector<std::string> args;

    std::string outputSeparatorLine = "=================-================="; // important! '=' * 17 + '-' + '=' * 17
    std::string outputSeparatorRe = outputSeparatorLine + " [-_a-zA-Z0-9]+ " + outputSeparatorLine;

    std::string sawyerAssertionFailedRe = "\\[FATAL\\]: assertion failed:\n"
                                          "(.*\\[FATAL\\]:.*\n)*"; // file name, function, expression, message

    //---------------------------------------------------------------------------------------------------
    // Step 1: Search for error text in output and save it in the first_error_staging column.
    //---------------------------------------------------------------------------------------------------

    // Find and store the error messages into the first_error_staging column.
    SAWYER_MESG(mlog[DEBUG]) <<"scanning test output...\n";
    auto q = db.stmt("update test_results test"
                     " set first_error_staging = substring("

                     // We want to search for error messages only in the last section of output (if
                     // there's more than one).
                     "regexp_replace(convert_from(att.content, 'latin-1'), '.*" + outputSeparatorLine + "\\n(.*)', '\\1')"

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
                     "|tup error: (.*)"                        // such as 'FUSE did not appear to release all...'
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
                     "|^.*: error while loading shared libraries.*" // missing shared library
                     "|\\merror: ?\\n.*"                        // ROSE error on next line
                     "|^.*\\magainst undefined hidden symbol.*" // error from /usr/bin/ld

                     //----- regular expressions end -----
                     ")')"
                     " from attachments att" +
                     "   where test.id = att.test_id"
                     "   and (test.first_error is null or test.first_error = '')"
                     "   and test.status <> 'end'"
                     "   and att.name = 'Final output'"
                     "   and " + sqlIdLimitation("test.id", testIds))
             .run();

    //---------------------------------------------------------------------------------------------------
    // Step 2: Massage the first_error_staging values so similar errors have equal text.
    //---------------------------------------------------------------------------------------------------

    // Replace absolute file names "/foo/bar/baz" with "/.../baz"
    std::string fileNameChar    =  "[-+=_.a-zA-Z0-9]";
    std::string nonFileNameChar = "[^-+=_.a-zA-Z0-9]";
    SAWYER_MESG(mlog[DEBUG]) <<"normalizing detected error messages...\n";

    // If a test failed and there is no error message, then make up something generic
    db.stmt("update test_results test"
            " set first_error_staging = 'no error message detected'"
            " where test.status <> 'end'"
            "   and (test.first_error_staging is null or test.first_error_staging = '')"
            "   and " + sqlIdLimitation("test.id", testIds))
        .run();

    // Replace temporary file names with "temp-file"
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(/tmp(/" + fileNameChar + "+)+)',"
            "   'temp-file',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // Replace file paths with just the last component of the path
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(^|" + nonFileNameChar + ")((/" + fileNameChar + "+)+)/(" + fileNameChar + "+)',"
            "   '\\1/.../\\4',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // Remove the process ID and time from Sawyer::Message prefixes: "a.out[123] 45.678" => "a.out[...] ..."
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(" + fileNameChar + ")\\[[0-9]+\\] [0-9]+\\.[0-9]+s ',"
            "   '\\1[...] ... ',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // The shell likes to print process IDs also, so remove them. "deepDelete [err]: sh: line 1: 1518 Segmentation fault" =>
    // "deepDelete [err]: sh: line 1: ... Segmentation fault"
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(\\[err\\]: sh: line [0-9]+: )[0-9]+ ',"
            "   '\\1... ',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // RMC/Spock installation errors have temporary file names that should be removed
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(installation script failed for .*); see .* for details',"
            "   '\\1',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // spock-shell conflict messages include hashes that should be removed
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '([0-9])@[0-9a-f]{8}',"
            "   '\\1',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    // spock-shell errors for previously failed installations contains temporary file name
    db.stmt("update test_results test"
            " set first_error_staging = regexp_replace(first_error_staging,"
            "   '(installation was previously attempted and failed).*(for .*);.*',"
            "   '\\1 \\2',"
            "   'g')"
            " where test.first_error_staging is not null"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();

    //---------------------------------------------------------------------------------------------------
    // Step 3: Unrelated to above, populate the 'blacklisted' column
    //---------------------------------------------------------------------------------------------------

    SAWYER_MESG(mlog[DEBUG]) <<"updating blacklisted column...\n";
    std::string blacklistPrefixRe = "configure: error: blacklisted: ";
    db.stmt("update test_results as test"
            " set blacklisted ="
            "   regexp_replace("
            "     coalesce(substring(convert_from(att.content, 'latin-1') from '(?n)" + blacklistPrefixRe + ".*'), ''),"
            "     '" + blacklistPrefixRe + "',"
            "     '')"
            " from attachments as att " +
            " where test.id = att.test_id"
            "   and (test.first_error is null or test.first_error = '')"
            "   and att.name = 'Final output'"
            "   and blacklisted is null"
            "   and " + sqlIdLimitation("test.id", testIds))
        .run();

    //---------------------------------------------------------------------------------------------------
    // Step 4: Move the staging area values to their final home.
    //---------------------------------------------------------------------------------------------------

    SAWYER_MESG(mlog[DEBUG]) <<"finalizing error results...\n";
    db.stmt("update test_results test"
            " set first_error = first_error_staging,"
            "     first_error_staging = null"
            " where test.first_error_staging is not null and test.first_error_staging <> ''"
            " and " + sqlIdLimitation("test.id", testIds))
        .run();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    // Parse the command-line
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    if ("clear" == args[0]) {
        std::vector<int> ids = parseIds(args, 1);
        clearErrors(db, ids);
    } else {
        ASSERT_require("update" == args[0]);
        std::vector<int> ids = parseIds(args, 1);
        updateDatabase(db, ids);
    }
}
