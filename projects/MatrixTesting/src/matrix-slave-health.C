static const char *gPurpose = "query or adjust slave health";
static const char *gDescription =
    "Queries the slave health reports for users, or sends slave health reports for slaves.";

#include <rose.h>
#include "matrixTools.h"

#include <rose_getline.h>
#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Rose/StringUtility/Escape.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/trim.hpp>

#ifdef __linux__
#include <sys/statvfs.h>
#endif

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    uint64_t maxAge = 7*86400;                          // maximum age of report in seconds
    Sawyer::Optional<size_t> testId;                    // test ID for "test" events
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    Format outputFormat = Format::PLAIN;                // how to show results
};

static Sawyer::Message::Facility mlog;

// Parse command-line and return event type, "start", "stop", or "test".
static std::string
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] [show]\n\n"
               "@prop{programName} [@v{switches}] start|stop\n\n"
               "@prop{programName} [@v{switches}] test TEST_ID");

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
                     Rose::CommandLine::DurationParser::toString(settings.maxAge) + "."));

    std::vector<std::string> args = parser
                                    .with(Rose::CommandLine::genericSwitches())
                                    .with(tool)
                                    .parse(argc, argv)
                                    .apply()
                                    .unreachedArgs();
    if (args.empty()) {
        return "show";
    } else if (args[0] == "test") {
        if (args.size() == 2) {
            settings.testId = boost::lexical_cast<unsigned>(args[1]);
        } else {
            mlog[FATAL] <<"incorrect usage; see --help\n";
            exit(1);
        }
        return "test";
    } else if (args.size() != 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    } else if (args[0] == "show" || args[0] == "start" || args[0] == "stop") {
        return args[0];
    } else {
        mlog[FATAL] <<"unrecognized event type \"" <<StringUtility::cEscape(args[0]) <<"\"\n";
        exit(1);
    }
}

static std::string
slaveName() {
#ifdef __linux__
    char buf[64];

    std::string userName;
    if (getlogin_r(buf, sizeof buf) == 0) {
        userName = buf;
    } else if (const char *s = getenv("USER")) {
        userName = s;
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
    if (settings.databaseUri.empty()) {
        mlog[FATAL] <<"no database specified\n";
        mlog[INFO] <<"You need to use the --database switch, or set your ROSE_MATRIX_DATABASE environment\n"
                   <<"variable. See the \"Testing\" section of https://toc.rosecompiler.org for the proper\n"
                   <<"setting.\n";
        exit(1);
    }
    auto db = DB::Connection::fromUri(settings.databaseUri);

    if ("show" == event) {
        const time_t earliest = settings.maxAge < now() ? now() - settings.maxAge : 0;
        auto stmt = db.stmt("select name, timestamp, load_ave, free_space, event, test_id"
                            " from slave_health"
                            " where timestamp >= ?ts"
                            " order by timestamp desc").bind("ts", earliest);

        FormattedTable table;
        table.columnHeader(0, 0, "Slave Name");
        table.columnHeader(0, 1, "Latest Report from Slave");
        table.columnHeader(0, 2, "Load\nAverage");
        table.columnHeader(0, 3, "Free\nSpace");
        table.columnHeader(0, 4, "Latest\nEvent");
        table.columnHeader(0, 5, "Latest\nTest ID");
        table.columnHeader(0, 6, "Operating\nSystem");
        table.columnHeader(0, 7, "Duration");
        for (auto row: stmt) {
            using namespace Rose::StringUtility;
            const std::string name = row.get<std::string>(0).orElse("none");
            const time_t whenReported = row.get<time_t>(1).orElse(0);
            const std::string loadAverage = (boost::format("%6.2f") % (100.0*row.get<double>(2).orElse(0))).str();
            const std::string freeSpace = (boost::format("%.0f") % (row.get<size_t>(3).orElse(0) / 1024.0)).str();
            const std::string latestEvent = row.get<std::string>(4).orElse("none");
            const std::string latestId = row.get<std::string>(5).orElse("none");
            if (Format::YAML == settings.outputFormat) {
                std::cout <<"- name: " <<yamlEscape(name) <<"\n";
                std::cout <<"  when: " <<whenReported <<" # " <<timeToLocal(whenReported)
                          <<", " <<approximateAge(whenReported) <<"\n";
                std::cout <<"  loadavg: " <<loadAverage <<" # percent\n";
                std::cout <<"  free_space: " <<freeSpace <<" # GiB\n";
                std::cout <<"  latest_event: " <<latestEvent <<"\n";
                std::cout <<"  latest_id: " <<latestId <<"\n";
            } else {
                const size_t i = table.nRows();
                table.insert(i, 0, name);
                table.insert(i, 1, timeToLocal(whenReported) + ", " + approximateAge(whenReported));
                table.insert(i, 2, loadAverage + "%");
                table.insert(i, 3, freeSpace + " GiB");
                table.insert(i, 4, latestEvent);
                table.insert(i, 5, latestId);
            }

            if (auto testId = row.get<size_t>(5)) {
                auto test = db.stmt("select os, duration"
                                    " from test_results"
                                    " where id = ?id")
                            .bind("id", *testId);
                for (auto row: test) {
                    const std::string os = row.get<std::string>(0).orElse("");
                    const auto duration = row.get<uint64_t>(1);
                    const std::string durationStr = duration ? Rose::CommandLine::DurationParser::toString(duration) : "";
                    if (Format::YAML == settings.outputFormat) {https://datatracker.ietf.org/doc/html/rfc4180
                        std::cout <<"  os: " <<os <<"\n";
                        if (duration)
                            std::cout <<"  duration: " <<*duration <<" # " <<durationStr <<"\n";
                    } else {
                        table.insert(table.nRows()-1, 6, os);
                        table.insert(table.nRows()-1, 7, durationStr);
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
        DB::Statement stmt;

        if (db.stmt("select count(*) from slave_health where name = ?name").bind("name", slaveName()).get<size_t>().get()) {
            stmt = db.stmt("update slave_health set"
                           "    name = ?name,"
                           "    timestamp = ?timestamp,"
                           "    load_ave = ?load_ave,"
                           "    free_space = ?free_space,"
                           "    event = ?event,"
                           "    test_id = ?test_id"
                           " where name = ?name");
        } else {
            stmt = db.stmt("insert into slave_health"
                           "        (name,   timestamp,  load_ave,  free_space,  event,  test_id)"
                           " values (?name, ?timestamp, ?load_ave, ?free_space, ?event, ?test_id)");
        }

        stmt
            .bind("name", slaveName())
            .bind("timestamp", now())
            .bind("load_ave", cpuLoad())
            .bind("free_space", diskFreeSpace())
            .bind("event", event)
            .bind("test_id", settings.testId.orElse(0))
            .run();
    }
}
