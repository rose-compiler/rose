static const char *gPurpose = "report portability testing status";
static const char *gDescription =
    "To be written.";


#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <time.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    std::string databaseUri;
    std::string roseVersion;                            // if non-empty, use this version instead of latest
    Format outputFormat = Format::PLAIN;
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
    insertOutputFormatSwitch(sg, settings.outputFormat, FormatFlags().set(Format::PLAIN).set(Format::HTML));

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
showSectionTitle(const Settings &settings, const std::string &title, const std::string &desc = "") {
    static const size_t width = 80;
    switch (settings.outputFormat) {
        case Format::PLAIN:
            std::cout <<std::string(width, '=') <<"\n"
                      <<"=== " <<StringUtility::leftJustify(title, width-8) <<"===\n"
                      <<std::string(width, '=') <<"\n";
            if (!desc.empty())
                std::cout <<desc <<"\n";
            std::cout <<"\n";
            break;

        case Format::HTML:
            std::cout <<"<hr/><h2>" <<title <<"</h2>\n";
            if (!desc.empty())
                std::cout <<"<p>" <<desc <<"</p>\n";
            break;

        default:
            ASSERT_not_reachable("invalid output format");
    }
}

static void
showSubsectionTitle(const Settings &settings, const std::string &title, const std::string &desc = "") {
    switch (settings.outputFormat) {
        case Format::PLAIN:
            std::cout <<title <<"\n";
            if (!desc.empty())
                std::cout <<desc <<"\n";
            break;
        case Format::HTML:
            std::cout <<"<h3>" <<title <<"</h3>\n";
            if (!desc.empty())
                std::cout <<"<p>" <<desc <<"</p>\n";
            break;
        default:
            ASSERT_not_reachable("invalid output format");
    }
}

static void
showNotices(const Settings &settings, DB::Connection db, const Sawyer::Container::Map<std::string, std::string> &props) {
    FormattedTable table;
    table.indentation("    ");
    table.format(Format::HTML == settings.outputFormat ? FormattedTable::Format::HTML : FormattedTable::Format::PLAIN);

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
        showSectionTitle(settings, "Notices to ROSE developers", "");
        std::cout <<table <<"\n";
    }
}

static void
showSlaveConfig(const Settings &settings, DB::Connection db) {
    Sawyer::Container::Map<std::string, std::string> props;
    auto stmt = db.stmt("select name, value from slave_settings");
    for (auto row: stmt)
        props.insert(*row.get<std::string>(0), *row.get<std::string>(1));

    showNotices(settings, db, props);
    showSectionTitle(settings, "Slave configuration",
                     "This section contains information about what the slaves should be\n"
                     "testing, and how. This is also the information you need if you want\n"
                     "to set up your own testing. See also, rose-matrix-slave-config.\n");

    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
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

    table.insert(i, 0, "Tools repository to be used");
    table.insert(i, 1, props.getOptional("MATRIX_REPOSITORY").orElse("none"));
    table.insert(i, 2, "MATRIX_REPOSITORY");
    ++i;

    table.insert(i, 0, "Tools commit or tag");
    table.insert(i, 1, props.getOptional("MATRIX_COMMITTISH").orElse("none"));
    table.insert(i, 2, "MATRIX_COMMITTISH");
    ++i;

    table.insert(i, 0, "Tools environment version");
    table.insert(i, 1, props.getOptional("TEST_ENVIRONMENT_VERSION").orElse("none"));
    table.insert(i, 2, "TEST_ENVIRONMENT_VERSION");
    ++i;

    table.insert(i, 0, "Tools operational flags");
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
    showSectionTitle(settings, "Slave health",
                     "This section contains information about what slaves have run in the last week.\n"
                     "Each slave typically reports many test results. See also, rose-matrix-slave-health.");

    const time_t since = time(NULL) - 7 * 86400;

    //                          0     1          2         3           4      5
    auto stmt = db.stmt("select name, timestamp, load_ave, free_space, event, test_id"
                        " from slave_health"
                        " where timestamp >= ?since"
                        " order by timestamp desc, name")
                .bind("since", since);

    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "Slave Name");
    table.columnHeader(0, 1, "Latest Report from Slave");
    table.columnHeader(0, 2, "Load\nAverage");
    table.columnHeader(0, 3, "Free\nSpace");
    table.columnHeader(0, 4, "Latest\nEvent");
    table.columnHeader(0, 5, "Latest\nTest ID");
    table.columnHeader(0, 6, "Tests");
    for (auto row: stmt) {
        const size_t i = table.nRows();
        const std::string slaveName = row.get<std::string>(0).orElse("none");
        table.insert(i, 0, slaveName);
        time_t whenReported = row.get<time_t>(1).orElse(0);
        table.insert(i, 1, timeToLocal(whenReported) + ", " + approximateAge(whenReported));
        table.insert(i, 2, (boost::format("%6.2f%%") % (100.0*row.get<double>(2).orElse(0))).str());
        table.insert(i, 3, (boost::format("%.0f GiB") % (row.get<size_t>(3).orElse(0) / 1024.0)).str());
        table.insert(i, 4, row.get<std::string>(4).orElse("none"));
        table.insert(i, 5, row.get<std::string>(5).orElse("none"));

        // This might be a little slow... The number of tests completed by this slave in the same time interval as above.
        size_t nTests = db.stmt("select count(*) from test_results where tester = ?tester and reporting_time > ?since")
                        .bind("tester", slaveName)
                        .bind("since", since)
                        .get<size_t>().orElse(0);
        if (nTests > 0) {
            table.insert(i, 6, boost::lexical_cast<std::string>(nTests));
        } else {
            table.insert(i, 6, "");
        }
    }
    std::cout <<table <<"\n\n";
}

// Subsection: versions of rose tested in the last month and return their versions
static std::vector<std::string>
showLatestVersions(const Settings &settings, DB::Connection db) {
    const time_t since = time(NULL) - 30 * 86400;
    const std::string sinceHuman = timeToGmt(since).substr(0, 10);

    showSubsectionTitle(settings, "ROSE version tested recently",
                        "These are the ROSE versions that have test results in the last month.\n"
                        "This is similar to the command \"rose-matrix-query rose_date+" + sinceHuman +
                        " rose.d reporting_time.min reporting_time.max\"");

    //                          0     1          2         3                    4
    auto stmt = db.stmt("select rose, rose_date, count(*), min(reporting_time), max(reporting_time)"
                        " from test_results"
                        " where reporting_time >= ?since"
                        " group by rose, rose_date"
                        " order by rose_date desc")
                .bind("since", since);

    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "ROSE version");
    table.columnHeader(0, 1, "Commit date");
    table.columnHeader(0, 2, "Tests");
    table.columnHeader(0, 3, "Earliest\nTest");
    table.columnHeader(0, 4, "Latest\nTest");
    std::vector<std::string> retval;
    for (auto row: stmt) {
        const size_t i = table.nRows();
        retval.push_back(row.get<std::string>(0).orElse("unknown"));
        table.insert(i, 0, retval.back());
        table.insert(i, 1, timeToLocal(row.get<time_t>(1).orElse(0)));
        table.insert(i, 2, row.get<std::string>(2).orElse("unknown"));
        if (auto earliest = row.get<time_t>(3))
            table.insert(i, 3, timeToLocal(*earliest) + ", " + approximateAge(*earliest));
        if (auto latest = row.get<time_t>(4))
            table.insert(i, 4, timeToLocal(*latest) + ", " + approximateAge(*latest));
    }
    std::cout <<table <<"\n\n";
    return retval;
}

static void
showLatestTests(const Settings &settings, DB::Connection db) {
    const time_t since = time(NULL) - 24 * 3600;
    const std::string sinceHuman = timeToGmt(since);

    showSubsectionTitle(settings, "Tests recently completed",
                        "These are the tests that completed in the last 24 hours. This is similar to the command\n"
                        "\"rose-matrix-query reporting_time+" + StringUtility::bourneEscape(sinceHuman) + " rose reporting_time.d"
                        " slave id os compiler languages build boost status\"");

    //                          0     1               2       3   4   5             6              7          8          9
    auto stmt = db.stmt("select rose, reporting_time, tester, id, os, rmc_compiler, rmc_languages, rmc_build, rmc_boost, status"
                        " from test_results"
                        " where reporting_time > ?since"
                        " order by reporting_time desc"
                        " limit 100")
                .bind("since", since);

    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "Rose\nVersion");
    table.columnHeader(0, 1, "When Reported");
    table.columnHeader(0, 2, "Slave Name");
    table.columnHeader(0, 3, "Test\nID");
    table.columnHeader(0, 4, "Operating\nSystem");
    table.columnHeader(0, 5, "Compiler");
    table.columnHeader(0, 6, "Analysis\nLanguages");
    table.columnHeader(0, 7, "Build\nSystem");
    table.columnHeader(0, 8, "Boost\nVersion");
    table.columnHeader(0, 9, "Status");

    for (auto row: stmt) {
        const size_t i = table.nRows();
        table.insert(i, 0, row.get<std::string>(0).orElse("unknown"));
        const time_t reportingTime = *row.get<time_t>(1);
        table.insert(i, 1, timeToLocal(reportingTime) + ", " + approximateAge(reportingTime));
        table.insert(i, 2, row.get<std::string>(2).orElse("unknown"));
        table.insert(i, 3, row.get<std::string>(3).orElse("unknown"));
        table.insert(i, 4, row.get<std::string>(4).orElse("unknown"));
        table.insert(i, 5, row.get<std::string>(5).orElse("unknown"));
        table.insert(i, 6, row.get<std::string>(6).orElse("unknown"));
        table.insert(i, 7, row.get<std::string>(7).orElse("unknown"));
        table.insert(i, 8, row.get<std::string>(8).orElse("unknown"));
        const std::string status = row.get<std::string>(9).orElse("unknown");
        table.insert(i, 9, row.get<std::string>(9).orElse("unknown"));
        table.cellProperties(i, 9, "end" == status ? goodCell() : badCell());
    }
    std::cout <<table <<"\n\n";
}

// Show latest tested ROSE versions and return their commits, starting with the most recent
static std::vector<std::string>
showLatestTestSummaries(const Settings &settings, DB::Connection db) {
    showSectionTitle(settings, "Recent tests");
    std::vector<std::string> retval = showLatestVersions(settings, db);
    showLatestTests(settings, db);
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
    showSectionTitle(settings, "Test phases",
                     "These are the phases performed by each test. If a phase fails, the status for\n"
                     "the test is the name of the failed phase. The name \"end\" means that all\n"
                     "phases of the test passed. The phases are executed in order until one fails\n"
                     "or the end is reached.");

    auto stmt = db.stmt("select name, purpose"
                        " from test_names"
                        " where purpose is not null and purpose <> ''"
                        " order by position");

    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
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
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, fieldTitle);
    table.columnHeader(0, 1, "Reported\nStatus");
    table.columnHeader(0, 2, "Tests");
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
    showSectionTitle(settings, "Test results",
                     "These are the test results for ROSE version " + roseVersion);

    showSubsectionTitle(settings, "Test results by analysis language",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " languages.a status count\"\n");
    showTestResultsByField(settings, db, roseVersion, "rmc_languages", "Analysis\nLanguages");

    showSubsectionTitle(settings, "Test results by operating system",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " os.a status count\"\n");
    showTestResultsByField(settings, db, roseVersion, "os", "Operating\nSystem");

    showSubsectionTitle(settings, "Test results by host compiler",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " compiler.a status count\"\n");
    showTestResultsByField(settings, db, roseVersion, "rmc_compiler", "Compiler");

    showSubsectionTitle(settings, "Test results by Boost version",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " boost.a status count\"\n");
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
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, fieldTitle);
    table.columnHeader(0, 1, "Test\nStatus");
    table.columnHeader(0, 2, "Tests");
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
    showSectionTitle(settings, "Error messages",
                     "These are the heuristically determined first error messages from testing\n"
                     "ROSE version " + roseVersion);

    showSubsectionTitle(settings, "Errors by analysis language",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " languages.a status count first_error\"\n");
    showErrorsByField(settings, db, roseVersion, "rmc_languages", "Analysis\nLanguages");

    showSubsectionTitle(settings, "Errors by operating system",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " os.a status count first_error\"\n");
    showErrorsByField(settings, db, roseVersion, "os", "Operation\nSystem");

    showSubsectionTitle(settings, "Errors by host compiler",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " compiler.a status count first_error\"\n");
    showErrorsByField(settings, db, roseVersion, "rmc_compiler", "Compiler");

    showSubsectionTitle(settings, "Errors by boost version",
                        "Similar to \"rose-matrix-query rose=" + roseVersion + " boost.a status count first_error\"\n");
    showErrorsByField(settings, db, roseVersion, "rmc_boost", "Boost\nVersion");
}

static void
showDependencies(const Settings &settings, DB::Connection db, const std::string &roseVersion) {
    showSectionTitle(settings, "Dependencies",
                     "These are the dependencies being tested at this time. The Pass, Fail, and Grade\n"
                     "columns are for ROSE " + roseVersion + ".");

    auto stmt = db.stmt("select name, value, restrictions, comment"
                        " from dependencies"
                        " where enabled > 0"
                        " order by name, value");
    FormattedTable table;
    table.indentation("    ");
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "Dependency\nName");
    table.columnHeader(0, 1, "Dependency\nValue");
    table.columnHeader(0, 2, "Restrictions");
    table.columnHeader(0, 3, "Pass");
    table.columnHeader(0, 4, "Fail");
    table.columnHeader(0, 5, "Grade");
    table.columnHeader(0, 6, "Comment");

    Color::Gradient pfColors;
    pfColors.insert(0.0, lighten(Color::HSV_RED, 0.4));
    pfColors.insert(0.7, lighten(Color::HSV_RED, 0.4));
    pfColors.insert(1.0, Color::HSV_GREEN);

    std::string prevName;
    for (auto row: stmt) {
        const size_t i = table.nRows();
        const std::string name = *row.get<std::string>(0);
        const std::string value = *row.get<std::string>(1);
        const std::string restrictions = row.get<std::string>(2).orElse("");
        const std::string comment = row.get<std::string>(3).orElse("");

        if (name != prevName) {
            table.insert(i, 0, name);
            prevName = name;
        }
        table.insert(i, 1, value);
        table.insert(i, 2, restrictions);

        size_t passFail[2] = {0, 0};
        auto stmt2 = db.stmt("select"
                             "   case when status = 'end' then 0 else 1 end as passed,"
                             "   count(*)"
                             " from test_results"
                             " where rose = ?rose and rmc_" + name + " = ?value"
                             " group by passed")
                     .bind("rose", roseVersion)
                     .bind("value", value);
        for (auto cr: stmt2)
            passFail[*cr.get<size_t>(0)] = *cr.get<size_t>(1);
        table.insert(i, 3, passFail[0]);
        table.insert(i, 4, passFail[1]);
        if (passFail[0] + passFail[1] > 0) {
            double passRate = (double)passFail[0] / (passFail[0] + passFail[1]);
            int percent = (int)::round(100.0 * passRate);
            table.insert(i, 5, boost::lexical_cast<std::string>(percent) + "%");
            FormattedTable::CellProperties props;
            props.foreground(Color::HSV_BLACK);
            props.background(pfColors(passRate));
            table.cellProperties(i, 5, props);
        }

        table.insert(i, 6, comment);
    }
    std::cout <<table <<"\n";
}

static void
showAdditionalCommands(const Settings &settings, DB::Connection db, const std::string &roseVersion) {
    const std::string moreInfoUrl = "https://lep.is/w/index.php/ROSE:Portability_testing_results";
    const std::string moreInfoLink = Format::HTML == settings.outputFormat ?
                                     "<a href=\"" + moreInfoUrl + "\">documentation</a>" :
                                     moreInfoUrl;

    showSectionTitle(settings, "Additional information",
                     "The data above is only the tip of the iceburg.\n"
                     "See " + moreInfoLink + " for the rest of the iceburg.\n");
}

static std::string
timestamp() {
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d GMT")
            % (tm.tm_year+1900) % (tm.tm_mon+1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec).str();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    if (boost::starts_with(Rose::CommandLine::versionString, "ROSE "))
        Rose::CommandLine::versionString = Rose::CommandLine::versionString.substr(5);

    Settings settings;
    const std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    if (Format::HTML == settings.outputFormat) {
        std::cout <<"<!DOCTYPE html>\n"
                  <<"<html>\n"
                  <<"<head>\n"
                  <<"  <title>ROSE Portability Testing</title>\n"
                  <<"</head>\n"
                  <<"<body>\n"
                  <<"<h1>ROSE Portability Testing</h1>\n"
                  <<"Updated at " <<timestamp() <<" from the <tt>rose-matrix-status</tt> version "
                  <<Rose::CommandLine::versionString <<".<br/>\n";
    }

    showSlaveConfig(settings, db);
    showSlaveHealth(settings, db);
    std::vector<std::string> roseCommits = showLatestTestSummaries(settings, db);
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
    showDependencies(settings, db, roseCommits.front());

    showAdditionalCommands(settings, db, roseCommits.front());

    if (Format::HTML == settings.outputFormat)
        std::cout <<"</body></html>\n";
}
