static const char *purpose = "report slave health";
static const char *description =
    "Reports testing slave health to the database server.";

#include <rose.h>

#include <CommandLine.h>                                // rose
#include <SqlDatabase.h>                                // rose
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

#ifdef __linux__
#include <sys/statvfs.h>
#endif

using namespace Rose;
using namespace Sawyer::Message::Common;

static const unsigned maxAgeHours = 72;                 // limit which tests are shown by default

struct Settings {
    bool dryRun;                                        // if true, don't change database
    bool showingAll;                                    // do not limit to last 24 hours
    Sawyer::Optional<unsigned> testId;                  // test ID for "test" events
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
        : dryRun(false), showingAll(false)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

// Parse command-line and return event type, "start", "stop", or "test".
static std::string
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] [show]\n\n"
               "@prop{programName} [@v{switches}] start|stop\n\n"
               "@prop{programName} [@v{switches}] test TEST_ID");

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("database", 'd')
                .argument("uri", anyParser(settings.databaseUri))
                .doc("Uniform resource locator for the database. This switch overrides the ROSE_MATRIX_DATABASE environment "
                     "variable. " + SqlDatabase::uriDocumentation()));

    tool.insert(Switch("all")
                .intrinsicValue(true, settings.showingAll)
                .doc("Show all records, not just those that have arrived in the last " +
                     StringUtility::plural(maxAgeHours, "hours") + "."));

    tool.insert(Switch("dry-run", 'n')
                .intrinsicValue(true, settings.dryRun)
                .doc("Avoid changing the database. All normal actions are performed, but the final database transaction "
                     "is not committed. This can be used for testing."));

    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(tool);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
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
        mlog[FATAL] <<"unrecognized even type \"" <<StringUtility::cEscape(args[0]) <<"\"\n";
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
    if (settings.showingAll) {
        return "";
    } else {
        return " where timestamp >= " + boost::lexical_cast<std::string>(now() - maxAgeHours * 3600);
    }
}

class AgeRenderer: public SqlDatabase::Renderer<unsigned> {
public:
    std::string operator()(const unsigned &value, size_t width) const ROSE_OVERRIDE {
        time_t secondsAgo = time(NULL) - value;
        if (unsigned nYears = secondsAgo / (365*86400)) {
            return StringUtility::plural(nYears, "years") + " ago";
        } else if (unsigned nMonths = secondsAgo / (30*86400)) {
            return StringUtility::plural(nMonths, "months") + " ago";
        } else if (unsigned nWeeks = secondsAgo / (7*86400)) {
            return StringUtility::plural(nWeeks, "weeks") + " ago";
        } else if (unsigned nDays = secondsAgo / 86400) {
            return StringUtility::plural(nDays, "days") + " ago";
        } else if (unsigned nHours = secondsAgo / 3600) {
            return StringUtility::plural(nHours, "hours") + " ago";
        } else if (unsigned nMinutes = secondsAgo / 60) {
            return StringUtility::plural(nMinutes, "minutes") + " ago";
        } else {
            return "just now";
        }
    }
};

class PercentRenderer: public SqlDatabase::Renderer<double> {
public:
    std::string operator()(const double &value, size_t width) const ROSE_OVERRIDE {
        unsigned pct = round(100.0 * value);
        return boost::lexical_cast<std::string>(pct) + "%";
    }
};

class SizeRenderer: public SqlDatabase::Renderer<unsigned> {
public:
    std::string operator()(const unsigned &value, size_t width) const ROSE_OVERRIDE {
        if (unsigned gib = value / 1024) {
            return boost::lexical_cast<std::string>(gib) + " GiB";
        } else {
            return boost::lexical_cast<std::string>(value) + " MiB";
        }
    }
};

class DurationRenderer: public SqlDatabase::Renderer<Sawyer::Optional<unsigned> > {
public:
    std::string operator()(const Sawyer::Optional<unsigned> &value, size_t width) const ROSE_OVERRIDE {
        if (value) {
            if (unsigned nDays = *value / 86400) {
                unsigned nHours = (*value - nDays * 86400) / 3600;
                return StringUtility::plural(nDays, "days") + " " + StringUtility::plural(nHours, "hours");
            } else if (unsigned nHours = *value / 3600) {
                unsigned nMinutes = (*value - nHours * 3600) / 60;
                return StringUtility::plural(nHours, "hours ") + " " + StringUtility::plural(nMinutes, "minutes");
            } else if (unsigned nMinutes = *value / 60) {
                unsigned nSeconds = *value % 60;
                return StringUtility::plural(nMinutes, "minutes") + " " + StringUtility::plural(nSeconds, "seconds");
            } else {
                return StringUtility::plural(*value, "seconds");
            }
        } else {
            return "";
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    std::string event = parseCommandLine(argc, argv, settings);

    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what() <<"\n";
        exit(1);
    }

    if ("show" == event) {
        typedef SqlDatabase::Table<std::string, unsigned, double, unsigned, std::string,
                                   std::string, std::string, Sawyer::Optional<unsigned> > Table;
        Table table;
        table.headers("Slave name", "Report time", "CPU load", "Disk space", "Event",
                      "Operating system", "Phase", "Duration");
        table.renderers().r1 = new AgeRenderer;
        table.renderers().r2 = new PercentRenderer;
        table.renderers().r3 = new SizeRenderer;
        table.renderers().r7 = new DurationRenderer;

        SqlDatabase::StatementPtr stmt =
            tx->statement("select name, timestamp, load_ave, free_space, event, test_id"
                          " from slave_health" +
                          showingWhereClause(settings) +
                          " order by name");
        for (SqlDatabase::Statement::iterator row = stmt->begin(); row != stmt->end(); ++row) {
            std::string name = row.get_str(0);
            unsigned timestamp = row.get_u32(1);
            double loadAverage = row.get_dbl(2);
            unsigned freeSpace = row.get_u32(3);
            std::string event = row.get_str(4);
            unsigned testId = row.get_u32(5);
            std::string os;
            std::string status;
            Sawyer::Optional<unsigned> duration;
            
            if (testId) {
                SqlDatabase::StatementPtr testQuery =
                    tx->statement("select os, status, duration from test_results where id = ?")
                    ->bind(0, testId);
                SqlDatabase::Statement::iterator testRow = testQuery->begin();
                if (testRow != testQuery->end()) {
                    os = testRow.get_str(0);
                    status = testRow.get_str(1);
                    duration = testRow.get_u32(2);
                }
            }

            table.insert(Table::Tuple(name, timestamp, loadAverage, freeSpace, event, os, status, duration));
        }
        table.print(std::cout);

    } else {
        // We want only one record per slave, so first delete any old records, then insert the new one.
        tx->statement("delete from slave_health where name = ?")
            ->bind(0, slaveName())
            ->execute();

        tx->statement("insert into slave_health"
                      "        (name, timestamp, load_ave, free_space, event, test_id)"
                      " values (?,    ?,         ?,        ?,          ?,     ?)")
            ->bind(0, slaveName())
            ->bind(1, now())
            ->bind(2, cpuLoad())
            ->bind(3, diskFreeSpace())
            ->bind(4, event)
            ->bind(5, settings.testId.orElse(0))
            ->execute();

        if (!settings.dryRun)
            tx->commit();
    }
}
