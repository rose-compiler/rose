static const char *gPurpose = "query test results";
static const char *gDescription =
    "Queries a database to show the matrix testing results.  The arguments are column names (use \"list\" to "
    "get a list of valid column names). They can be in two forms: a bare column name causes the table to "
    "contain that column, but if the column name is followed by an equal sign and a value, then the table "
    "is restricted to rows that have that value for the column, and the constant-valued column is displayed "
    "above the table instead (if you also want it in the table, then also specify its bare name).  If no columns "
    "are specified then all of them are shown (the special \"all\" column does the same thing).  Since more than "
    "one test might match the selection criteria, the final column is \"count\" to say how many such rows "
    "are present in the database.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>
#include <Sawyer/Map.h>

#include <boost/any.hpp>
#include <boost/format.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    bool usingLocalTime = false;
    std::string sortField;
    Format outputFormat = Format::PLAIN;
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{columns}");

    SwitchGroup sg("Tool-specific switches");

    insertDatabaseSwitch(sg, settings.databaseUri);
    insertOutputFormatSwitch(sg, settings.outputFormat);

    sg.insert(Switch("localtime")
              .intrinsicValue(true, settings.usingLocalTime)
              .doc("Display and parse times using the local time zone. Warning: parsing local times uses the daylight "
                   "saving time mode as of the time of parsing, and parsing of times near the switch between standard "
                   "and daylight saving time can be ambiguous. The default is to use " +
                   std::string(settings.usingLocalTime ? "local time" : "GMT") + "."));

    sg.insert(Switch("sort")
              .argument("field", anyParser(settings.sortField))
              .doc("Sort the output according to the specified column. The column need not be a column that's being "
                   "displayed in the output. The sort is always increasing."));

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(sg)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

using DependencyNames = Sawyer::Container::Map<std::string /*key*/, std::string /*colname*/>;

static DependencyNames
loadDependencyNames(DB::Connection db) {
    DependencyNames retval;
    for (auto row: db.stmt("select distinct name from dependencies")) {
        const std::string key = *row.get<std::string>(0);
        retval.insert(key, "rmc_" + key);
    }

    // Additional key/column relationships
    retval.insert("id", "test.id");
    retval.insert("reporting_user", "auth_user.identity");
    retval.insert("reporting_time", "test.reporting_time");
    retval.insert("tester", "test.tester");
    retval.insert("os", "test.os");
    retval.insert("rose", "test.rose");
    retval.insert("rose_date", "test.rose_date");
    retval.insert("status", "test.status");
    retval.insert("duration", "test.duration");
    retval.insert("noutput", "test.noutput");
    retval.insert("nwarnings", "test.nwarnings");
    retval.insert("first_error", "test.first_error");
    retval.insert("count", "count");

    return retval;
}

static Sawyer::Optional<std::pair<time_t, time_t> >
parseDateTime(const Settings &settings, std::string s) {
    struct tm tmMin, tmMax;
    boost::smatch matches;

    time_t now = 0;
    time(&now);
    localtime_r(&now, &tmMin);
    tmMax = tmMin;

    boost::regex dateRe1("(\\d{4})-(\\d{2})-(\\d{2})(.*)");
    boost::regex dateRe2("(\\d{4})(\\d{2})(\\d{2})(.*)");
    if (boost::regex_match(s, matches, dateRe1) ||
        boost::regex_match(s, matches, dateRe2)) {
        tmMin.tm_year = tmMax.tm_year = boost::lexical_cast<int>(matches[1].str()) - 1900;
        tmMin.tm_mon  = tmMax.tm_mon  = boost::lexical_cast<int>(matches[2].str()) - 1;
        tmMin.tm_mday = tmMax.tm_mday = boost::lexical_cast<int>(matches[3].str());
        tmMin.tm_hour = 0; tmMax.tm_hour = 23;
        tmMin.tm_min  = 0; tmMax.tm_min = 59;
        tmMin.tm_sec  = 0; tmMax.tm_sec = 59;
        s = matches[4].str();
    } else {
        return Sawyer::Nothing();                       // date is required
    }

    boost::regex timeRe1("\\s*(?:T\\s*)?(\\d{2}):(\\d{2}):(\\d{2})(.*)");
    boost::regex timeRe2("\\s*(?:T\\s*)?(\\d{2})(\\d{2})(\\d{2})(.*)");
    if (boost::regex_match(s, matches, timeRe1) ||
        boost::regex_match(s, matches, timeRe2)) {
        tmMin.tm_hour = tmMax.tm_hour = boost::lexical_cast<int>(matches[1].str());
        tmMin.tm_min  = tmMax.tm_min  = boost::lexical_cast<int>(matches[2].str());
        tmMin.tm_sec  = tmMax.tm_sec  = boost::lexical_cast<int>(matches[3].str());
        s = matches[4].str();
    } else {
        // time is optional
    }

    bool usingLocalTime = true;
    boost::regex utcRe("\\s*(z|Z|u|U|utc|UTC)");
    if (s.empty()) {
        usingLocalTime = settings.usingLocalTime;
    } else if (boost::regex_match(s, utcRe)) {
        usingLocalTime = false;
    } else {
        // zone is not supported
        return Sawyer::Nothing();
    }

    time_t tmin, tmax;
    if (usingLocalTime) {
        tmin = timelocal(&tmMin);
        tmax = timelocal(&tmMax);
    } else {
        tmMin.tm_isdst = tmMax.tm_isdst = 0;
        tmin = timegm(&tmMin);
        tmax = timegm(&tmMax);
    }

    return std::make_pair(tmin, tmax);
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    auto db = DB::Connection::fromUri(settings.databaseUri);
    DependencyNames dependencyNames = loadDependencyNames(db);

    // Parse positional command-line arguments
    boost::regex nameRe("[_a-zA-Z][_a-zA-Z0-9]*");
    boost::regex exprRe("([_a-zA-Z][_a-zA-Z0-9]*)([=~])(.*)");
    Sawyer::Container::Set<std::string> keysSeen;
    std::vector<std::string> whereClauses, columnsSelected, keysSelected;
    Sawyer::Container::Map<std::string, boost::any> whereValues;
    for (const std::string &arg: args) {
        boost::smatch exprParts;
        if (boost::regex_match(arg, exprParts, exprRe)) {
            // Arguments of the form <KEY><OPERATOR><VALUE> mean restrict the table to that value of the key.  This key column
            // will be emitted above the table instead of within the table (since the column within the table would have one
            // value across all the rows.
            std::string key = exprParts.str(1);
            std::string comparison = exprParts.str(2);
            std::string val = exprParts.str(3);

            if (!dependencyNames.exists(key)) {
                mlog[FATAL] <<"invalid key \"" <<StringUtility::cEscape(key) <<"\"\n";
                exit(1);
            }
            if ("count" == key) {
                mlog[FATAL] <<"field \"count\" cannot be compared\n";
                exit(1);
            }

            if ("rose" == key && "=" == comparison) {
                // Special cases for comparing ROSE commit hashes to allow specifying abbreviated hashes.
                boost::regex partialKeyRe("([0-9a-f]{1,39})(\\+local)?");
                boost::smatch matches;
                if (boost::regex_match(val, matches, partialKeyRe)) {
                    whereClauses.push_back(dependencyNames[key] + " like ?rose");
                    val = matches[1].str() + "%" + matches[2].str();
                } else {
                    whereClauses.push_back(dependencyNames[key] + " = ?rose");
                }
                whereValues.insert(key, val);

            } else if ("reporting_time" == key || "rose_date" == key) {
                // Special case for dates to allow for ranges.
                if (comparison != "=") {
                    mlog[FATAL] <<"field \"" <<key <<"\" can only be compared with \"=\"\n";
                    exit(1);
                }
                if (Sawyer::Optional<std::pair<time_t, time_t> > range = parseDateTime(settings, val)) {
                    whereClauses.push_back(dependencyNames[key] + " >= ?min_" + key + " and " +
                                           dependencyNames[key] + " <= ?max_" + key);
                    whereValues.insert("min_" + key, range->first);
                    whereValues.insert("max_" + key, range->second);
                } else {
                    whereClauses.push_back(dependencyNames[key] + " = ?" + key);
                    whereValues.insert(key, val);
                }

            } else if ("~" == comparison) {
                // Substring comparison
                whereClauses.push_back(dependencyNames[key] + " like ?" + key);
                whereValues.insert(key, "%" + val + "%");

            } else {
                // Equality comparison
                whereClauses.push_back(dependencyNames[key] + " = ?" + key);
                whereValues.insert(key, val);
            }

            std::cerr <<"  " <<std::left <<std::setw(16) <<key <<" = \"" <<StringUtility::cEscape(val) <<"\"\n";
            keysSeen.insert(key);

        } else if (arg == "list") {
            std::cout <<"Column names:\n";
            for (const std::string &key: dependencyNames.keys())
                std::cout <<"  " <<key <<"\n";
            exit(0);

        } else if (arg == "all") {
            for (const DependencyNames::Node &node: dependencyNames.nodes()) {
                if (!keysSeen.exists(node.key())) {
                    keysSelected.push_back(node.key());
                    columnsSelected.push_back(node.value());
                    keysSeen.insert(node.key());
                }
            }

        } else if (!dependencyNames.exists(arg)) {
            // Arguments of the form "key" mean add that key as one of the table columns and sort the table by ascending
            // values.
            mlog[FATAL] <<"invalid key \"" <<StringUtility::cEscape(arg) <<"\"\n";
            exit(1);

        } else {
            keysSelected.push_back(arg);
            columnsSelected.push_back(dependencyNames[arg]);
            keysSeen.insert(arg);
        }
    }

    // If no columns are selected, then select lots of them
    if (keysSelected.empty()) {
        for (const DependencyNames::Node &node: dependencyNames.nodes()) {
            if (!keysSeen.exists(node.key())) {
                keysSelected.push_back(node.key());
                columnsSelected.push_back(node.value());
            }
        }
    }

    // Build the SQL statement. In order to suppress the "count" from the output and yet use it in the ORDER BY clause
    // we need to have a two-level query that follows the format:
    //   select SET1 from (select SET2, count(*) as count from ....) as tbl;
    // where SET1 is the list of column names provided by the user
    // where SET2 is SET1 - "count"
    std::string sql = "select";
    if (columnsSelected.empty()) {
        sql += " *";
    } else {
        for (size_t i = 0; i < columnsSelected.size(); ++i) {
            std::string name = boost::replace_all_copy(columnsSelected[i], ".", "_");
            sql += std::string(i?",":"") + " " + name;
        }
    }
    sql += " from (select";
    if (columnsSelected.empty()) {
        sql += " *";
    } else {
        size_t nColsEmitted = 0;
        for (size_t i = 0; i < columnsSelected.size(); ++i) {
            std::string alias = boost::replace_all_copy(columnsSelected[i], ".", "_");
            if (columnsSelected[i] != "count")
                sql += std::string(nColsEmitted++?",":"") + " " + columnsSelected[i] + " as " + alias;
        }
        sql += std::string(nColsEmitted?",":"") + " count(*) as count";
    }
    sql += " from test_results as test join auth_identities as auth_user on test.reporting_user = auth_user.id";
    if (!whereClauses.empty())
        sql += " where " + StringUtility::join(" and ", whereClauses);
    if (!columnsSelected.empty()) {
        for (size_t i=0, nEmit=0; i < columnsSelected.size(); ++i) {
            if (columnsSelected[i] != "count")
                sql += std::string(nEmit++?", ":" group by ") + columnsSelected[i];
        }
    }
    if (!settings.sortField.empty()) {
        std::string sortBy;
        if (dependencyNames.getOptional(settings.sortField).assignTo(sortBy)) {
            sql += " order by " + sortBy;
        } else {
            mlog[FATAL] <<"cannot sort by \"" <<StringUtility::cEscape(settings.sortField) <<"\": not a vaild field name\n";
            exit(1);
        }
    }
    sql += ") as tbl";

    auto query = db.stmt(sql);
    for (auto &node: whereValues.nodes()) {
        if (node.value().type() == typeid(std::string)) {
            query.bind(node.key(), boost::any_cast<std::string>(node.value()));
        } else if (node.value().type() == typeid(time_t)) {
            query.bind(node.key(), boost::any_cast<time_t>(node.value()));
        } else {
            ASSERT_not_reachable("type for " + node.key());
        }
    }

    // Run the query and save results so we can compute column sizes.
    FormattedTable table;
    for (size_t j = 0; j < keysSelected.size(); ++j)
        table.columnHeader(0, j, keysSelected[j]);
    for (auto row: query) {
        size_t i = table.nRows();
        for (size_t j = 0; j < columnsSelected.size(); ++j) {
            std::string value;
            if ("test.rose_date" == columnsSelected[j] ||
                "test.reporting_time" == columnsSelected[j]) {
                time_t t = row.get<unsigned long>(j).orElse(0);
                struct tm tm;
                std::string tz;
                if (settings.usingLocalTime) {
                    localtime_r(&t, &tm);
                    tz = tm.tm_zone;
                } else {
                    gmtime_r(&t, &tm);
                    tz = "UTC";
                }
                value = (boost::format("%04d-%02d-%02d %02d:%02d:%02d %s")
                         % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday
                         % tm.tm_hour % tm.tm_min % tm.tm_sec
                         % tz).str();
            } else {
                value = row.get<std::string>(j).orElse("null");
            }

            switch (settings.outputFormat) {
                case Format::PLAIN:
                    table.insert(i, j, value);
                    break;
                case Format::YAML:
                    std::cout <<(j?"  ":"- ") <<columnsSelected[j] <<": " <<StringUtility::yamlEscape(value) <<"\n";
                    break;
            }
        }
    }

    if (Format::PLAIN == settings.outputFormat)
        std::cout <<table;
}
