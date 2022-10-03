static const char *gPurpose = "query or adjust slave health";
static const char *gDescription =
    "When invoked with no arguments, or just the argument \"show\", this tool shows some recent information about slave machines. "
    "When invoked with at least one argument that isn't the word \"show\", it updates the database with new health informtion. "
    "That information includes the specified health state (first argument) and an optional comment and/or test ID.\n\n"

    "The following states are posted by the \"rose-matrix-slave-run\" script that manages resources on the machine that's "
    "performing the tests, such as installing the test tools themselves. They all start with the word \"run\", and are reported "
    "on a best effort basis.\n\n"

    "@named{run-system}{The operating system is being updated.}"
    "@named{run-rmc}{The RMC/Spock tools are being re-installed.}"
    "@named{run-tools}{The portability testing tools are being re-installed.}"
    "@named{run-restart}{The script is restarting because it has been updated.}"
    "@named{run-download}{The ROSE version to be tested is being downloaded.}"
    "@named{run-paused}{Testing has paused for some reason.}"
    "@named{run-test}{A particular test has started.}"
    "@named{run-error}{The run script has exited with an error. This isn't always possible to report.}"

    "The following states are posted by the \"rose-matrix-run\" command which is typically invoked by the "
    "\"rose-matrix-slave-run\" script. This command is reponsible for running the test, and assumes that all testing tools are "
    "installed already. Thus, health states are reported with a high degree of reliability.\n\n"

    "@named{image}{A Docker image is being constructed in which to run the test.}"
    "@named{container}{A Docker container is being started for a test phase.}"
    "@named{phase}{The next phase of testing has started. Each test is many phases.}"
    "@named{tested}{A test has completed and a new test ID is added to the health report.}"
    "@named{error}{The script has encountered an error. This isn't always possible to report.}";

#include <rose.h>
#include "matrixTools.h"

#include <rose_getline.h>
#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Rose/StringUtility/Escape.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#ifdef __linux__
#include <sys/statvfs.h>
#endif

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    uint64_t maxAge = 7*86400;                          // maximum age of report in seconds
    Sawyer::Optional<size_t> testId;                    // test ID for "test" events
    Sawyer::Optional<std::string> comment;              // comment for other events
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    Format outputFormat = Format::PLAIN;                // how to show results
    std::string slaveName;                              // override user@host slave name
};

static Sawyer::Message::Facility mlog;

// Parse command-line and return health state (first argument or "show")
static std::string
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] [show]\n\n"
               "@prop{programName} [@v{switches}] @v{status} [@v{comment}]\n\n"
               "@prop{programName} [@v{switches}] tested @v{test_id} [@v{comment}]\n\n");

    SwitchGroup tool("Tool-specific switches");
    insertDatabaseSwitch(tool, settings.databaseUri);
    insertOutputFormatSwitch(tool, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::YAML)
                                 .set(Format::HTML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));

    tool.insert(Switch("since")
                .argument("duration", Rose::CommandLine::durationParser(settings.maxAge))
                .doc("Show reports that happened recently, where the @v{duration} is how far back to go in time "
                     "from now. " + Rose::CommandLine::DurationParser::docString() + " The default is " +
                     Rose::CommandLine::durationParser()->toString(settings.maxAge) + "."));

    tool.insert(Switch("slave")
                .argument("name", anyParser(settings.slaveName))
                .doc("Specify a name explicitly for this slave. If no name is given, then the slave name is "
                     "@v{user}@@@v{host}."));

    std::vector<std::string> args = parser
                                    .with(Rose::CommandLine::genericSwitches())
                                    .with(tool)
                                    .parse(argc, argv)
                                    .apply()
                                    .unreachedArgs();
    std::string event;
    if (args.empty()) {
        event = "show";
    } else if (1 == args.size()) {
        event = args[0];
        settings.comment = "";
    } else if (2 == args.size()) {
        event = args[0];
        if ("test" == event)
            event = "tested";                           // backward compatibility [Robb Matzke 2021-10-10]
        if ("tested" == event) {
            settings.testId = boost::lexical_cast<unsigned>(args[1]);
            settings.comment = "";
        } else {
            settings.comment = args[1];
        }
    } else if (3 == args.size()) {
        event = args[0];
        settings.testId = boost::lexical_cast<unsigned>(args[1]);
        settings.comment = args[2];
    } else {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    boost::regex eventRe("([a-zA-Z]\\w*)(-[a-zA-Z]\\w*)*"); // hyphenated symbols
    if (!boost::regex_match(event, eventRe)) {
        mlog[FATAL] <<"invalid event name; see --help\n";
        exit(1);
    }
    return event;
}

static std::string
slaveName(const Settings &settings) {
    if (!settings.slaveName.empty())
        return settings.slaveName;

#ifdef __linux__
    char buf[64];

    std::string userName;
    if (const char *s = getenv("USER")) {
        userName = s;
    } else if (getlogin_r(buf, sizeof buf) == 0) {
        userName = buf;
    } else {
        userName = "unknown";
    }

    std::string hostName;
    if (-1 == gethostname(buf, sizeof buf)) {
        hostName = "unknown";
    } else {
        hostName = buf;
        if (-1 != getdomainname(buf, sizeof buf)) {
            std::string domainName = buf;
            if (!domainName.empty() && domainName != "(none)")
                hostName += "." + std::string(buf);
        }
    }

    // If we're on an AWS machine, the host name is the internal IP address. In order to connect to the machine
    // we need its public IP instead
    boost::regex awsHostnameRe("ip(-[1-9][0-9]*){4}");
    if (boost::regex_match(hostName, awsHostnameRe)) {
        if (FILE *f = popen("curl http://169.254.169.254/latest/meta-data/public-ipv4", "r")) {
            std::string line = rose_getline(f);
            pclose(f);
            boost::trim(line);
            if (!line.empty())
                hostName = line;
        }
    }

    return userName + "@" + hostName;
#else
    return "unknown@unknown";
#endif
}

// Return time stamp, seconds since Unix epoch.
static time_t
now() {
    return time(NULL);
}

// Return system load average. This should be the average over a reasonably long period of time. For instance, on Linux we can
// get a 15 minute average.
static double
cpuLoad() {
#ifdef __linux__
    double loadavg[3];
    if (-1 == getloadavg(loadavg, 3)) {
        return 1.0;
    } else {
        return loadavg[2] / boost::thread::hardware_concurrency();;
    }
#else
    return 0.0;
#endif
}

// Return amount of space available in the file system for the current working directory. Returns MiB.
static size_t
diskFreeSpace() {
#ifdef __linux__
    struct statvfs sb;
    if (-1 == statvfs(".", &sb))
        return 0;
    return round((double)sb.f_bavail * sb.f_bsize / (1024.0*1024.0));
#else
    return 0;
#endif
}

// SQL "where" clause for limiting output
std::string
showingWhereClause(const Settings &settings) {
    return " where timestamp >= " + boost::lexical_cast<std::string>(now() - settings.maxAge);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::string event = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    if ("show" == event) {
        const time_t earliest = settings.maxAge < (uint64_t)now() ? now() - settings.maxAge : 0;
        //                          0     1          2         3           4      5        6
        auto stmt = db.stmt("select name, timestamp, load_ave, free_space, event, test_id, comment"
                            " from slave_health"
                            " where timestamp >= ?ts"
                            " order by timestamp desc").bind("ts", earliest);

        FormattedTable table;
        table.columnHeader(0, 0, "Slave Name");
        table.columnHeader(0, 1, "Latest Report from Slave");
        table.columnHeader(0, 2, "Load\nAverage");
        table.columnHeader(0, 3, "Free\nSpace");
        table.columnHeader(0, 4, "Latest\nEvent");
        table.columnHeader(0, 5, "Event\nDetail");
        table.columnHeader(0, 6, "Latest\nTest ID");
        table.columnHeader(0, 7, "Latest\nOS");
        table.columnHeader(0, 8, "Latest\nStatus");
        table.columnHeader(0, 9, "Latest\nDuration");

        for (auto row: stmt) {
            using namespace Rose::StringUtility;
            const std::string name = row.get<std::string>(0).orElse("none");
            const time_t whenReported = row.get<time_t>(1).orElse(0);
            const std::string loadAverage = (boost::format("%6.2f") % (100.0*row.get<double>(2).orElse(0))).str();
            const std::string freeSpace = (boost::format("%.0f") % (row.get<size_t>(3).orElse(0) / 1024.0)).str();
            const std::string latestEvent = row.get<std::string>(4).orElse("none");
            const std::string latestId = row.get<std::string>(5).orElse("none");
            const std::string comment = row.get<std::string>(6).orElse("");

            if (Format::YAML == settings.outputFormat) {
                std::cout <<"- name: " <<yamlEscape(name) <<"\n";
                std::cout <<"  when: " <<whenReported <<" # " <<timeToLocal(whenReported)
                          <<", " <<approximateAge(whenReported) <<"\n";
                std::cout <<"  loadavg: " <<loadAverage <<" # percent\n";
                std::cout <<"  free_space: " <<freeSpace <<" # GiB\n";
                std::cout <<"  latest_event: " <<latestEvent <<"\n";
                std::cout <<"  comment: " <<comment <<"\n";
                std::cout <<"  latest_id: " <<latestId <<"\n";
            } else {
                const size_t i = table.nRows();
                table.insert(i, 0, name);
                table.insert(i, 1, timeToLocal(whenReported) + ", " + approximateAge(whenReported));
                table.insert(i, 2, loadAverage + "%");
                table.insert(i, 3, freeSpace + " GiB");
                table.insert(i, 4, latestEvent);
                table.insert(i, 5, comment);
                table.insert(i, 6, latestId);
            }

            if (auto testId = row.get<size_t>(5)) {
                auto test = db.stmt("select os, duration, status"
                                    " from test_results"
                                    " where id = ?id")
                            .bind("id", *testId);
                for (auto row: test) {
                    const std::string os = row.get<std::string>(0).orElse("");
                    const auto duration = row.get<uint64_t>(1);
                    const std::string durationStr = duration ? Rose::CommandLine::durationParser()->toString(duration) : "";
                    const std::string status = row.get<std::string>(2).orElse("");
                    if (Format::YAML == settings.outputFormat) {
                        std::cout <<"  os: " <<os <<"\n";
                        if (duration) {
                            std::cout <<"  duration: " <<*duration <<" # " <<durationStr <<"\n";
                            std::cout <<"  status: " <<status <<"\n";
                        }
                    } else {
                        table.insert(table.nRows()-1, 7, os);
                        table.insert(table.nRows()-1, 8, status);
                        table.insert(table.nRows()-1, 9, durationStr);
                    }
                    break;
                }
            }
        }
        if (Format::YAML != settings.outputFormat) {
            table.format(tableFormat(settings.outputFormat));
            std::cout <<table;
        }

    } else {
        // We want only one record per slave, but slaves can't delete things. Therefore either update or insert
        const std::string testName = settings.testId ? " test_id, " : "";
        const std::string testSet = settings.testId ? " test_id = ?test_id, " : "";
        const std::string testBind = settings.testId ? " ?test_id, " : "";

        const std::string commentName = settings.comment ? " comment, " : "";
        const std::string commentSet = settings.comment ? " comment = ?comment, " : "";
        const std::string commentBind = settings.comment ? " ?comment, " : "";

        DB::Statement stmt;

        if (db.stmt("select count(*) from slave_health where name = ?name")
            .bind("name", slaveName(settings)).get<size_t>().get()) {

            stmt = db.stmt("update slave_health set" +
                           testSet + commentSet +
                           "    name = ?name,"
                           "    timestamp = ?timestamp,"
                           "    load_ave = ?load_ave,"
                           "    free_space = ?free_space,"
                           "    event = ?event"
                           " where name = ?name");
        } else {
            stmt = db.stmt("insert into slave_health"
                           "        (" + testName + commentName + "name,   timestamp,  load_ave,  free_space,  event)"
                           " values (" + testBind + commentBind + " ?name, ?timestamp, ?load_ave, ?free_space, ?event)");
        }

        stmt
            .bind("name", slaveName(settings))
            .bind("timestamp", now())
            .bind("load_ave", cpuLoad())
            .bind("free_space", diskFreeSpace())
            .bind("event", event);
        if (settings.testId)
            stmt.bind("test_id", *settings.testId);
        if (settings.comment)
            stmt.bind("comment", *settings.comment);
        stmt.run();
    }
}
