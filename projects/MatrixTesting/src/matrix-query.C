static const char *gPurpose = "query test results";
static const char *gDescription =
    "Queries a database to show the matrix testing results.  The arguments are column names (use \"list\" to "
    "get a list of valid column names). They can be in two forms: constraints and displays. A constraint argument is "
    "a column name, comparison operator, and value; and a display argument is column name and optional "
    "sorting direction.\n\n"

    "The relational operators for constraints are designed to work well with shell scripts. They are:"
    "@named{=}{The column on the left hand side must be equal to the value on the right hand side.}"
    "@named{/}{The column on the left hand side must be not-equal to the value on the right hand sie.}"
    "@named{+}{The column on the left hand size must be greater than or equal to the value on the right hand side.}"
    "@named{-}{The column on the left hand size must be less than or equal to the value on the right hand side.}"
    "@named{~}{The column on the left hand side must contain the string on the right hand side.}"
    "@named{^}{The column on the left hand side must not contain string value on the right hand side.}"
    "\n\n"

    "Commit hashes can be abbreviated in equal, unequal, less, and greater constraints, even if the hash as a "
    "\"+local\" suffix.\n\n"

    "Dates can be entered as \"YYYYMMDD\" or \"YYYY-MM-DD\" and match all time points within that day. Times are "
    "specified as a date followed by \"HHMMSS\" or \"HH:MM:SS\". The time can be separated from the date with "
    "white space or the letter \"T\". A time (or date alone) can be followed by a timezone abbreviation or the "
    "time standard \"UTC\". Time zones and standards can be all upper- or all lower-case and separated from the "
    "time (or date only) by white space. If no time zone or standard is specified, then either GMT or your "
    "local time zone is assumed, depending on whether @s{localtime} was specified. For example, today is "
    "\"2021-10-09\", \"20211009\", or \"20211009 UTC\", and the current time is \"20211009T190424\" (ISO 8601 "
    "format) or \"2021-10-09 15:04:24 EDT\" in a more human friendly format.\n\n"

    "A display column can be sorted by appending \".a\" or \".d\" to its name, meaning sort so values are ascending "
    "or descending, respectively. If multiple columns are sorted, their sort priority is based on their column number. "
    "That is, the left-most sorted column is sorted first, then the next column to the right, etc.";

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
    Format outputFormat = Format::PLAIN;
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    Sawyer::Optional<size_t> limit;                     // limit number of resulting rows
    bool deleteMatchingTests = false;                   // if true, delete the tests whose records match
    bool showAges = true;                               // when showing times, also say "about x days ago" or similar
    bool considerAll = false;                           // consider all tests instead of just the latest ROSE version
    bool dittoize = true;                               // use ditto markers (") when successive rows have same value
};

static Sawyer::Message::Facility mlog;
static const size_t DEFAULT_ROW_LIMIT = 100;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{columns}");

    SwitchGroup sg("Tool-specific switches");

    insertDatabaseSwitch(sg, settings.databaseUri);
    insertOutputFormatSwitch(sg, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::YAML)
                                 .set(Format::HTML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));

    sg.insert(Switch("all", 'a')
              .intrinsicValue(true, settings.considerAll)
              .doc("Consider all tests instead of just the latest ROSE version. Constraining the  \"rose\" or \"rose_date\" "
                   "columns also implies @s{all}."));

    sg.insert(Switch("localtime")
              .intrinsicValue(true, settings.usingLocalTime)
              .doc("Display and parse times using the local time zone. Warning: parsing local times uses the daylight "
                   "saving time mode as of the time of parsing, and parsing of times near the switch between standard "
                   "and daylight saving time can be ambiguous. The default is to use " +
                   std::string(settings.usingLocalTime ? "local time" : "GMT") + "."));

    sg.insert(Switch("limit", 'n')
              .argument("nrows", nonNegativeIntegerParser(settings.limit))
              .doc("Limit the number of rows returned by the query. The default is " +
                   boost::lexical_cast<std::string>(DEFAULT_ROW_LIMIT) + "."));

    sg.insert(Switch("delete")
              .intrinsicValue(true, settings.deleteMatchingTests)
              .doc("Delete the tests that were matched."));

    Rose::CommandLine::insertBooleanSwitch(sg, "show-age", settings.showAges,
                                           "Causes timestamps to also incude an approximate age. For instance, the "
                                           "age might be described as \"about 6 hours ago\".");

    Rose::CommandLine::insertBooleanSwitch(sg, "ditto", settings.dittoize,
                                           "Replace repeated values with dittos. This works by scanning each row of a "
                                           "table from left to right and if the column has the same value as the previous "
                                           "row then that column is replaced with a ditto mark. If the column is different "
                                           "than the previous row, then the scanning stops and no subsequent columns of "
                                           "the row are changed. Only the first contiguous group of sorted columns are "
                                           "considered.");

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(sg)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

enum class ColumnType {
    STRING,
    INTEGER,
    TIME,
    DURATION,
    VERSION_OR_HASH
};

enum class Sorted {
    NONE,
    ASCENDING,
    DESCENDING
};

struct Binding {
    std::string name;
    std::string value;

    Binding() {}
    Binding(const std::string &name, const std::string &value)
        : name(name), value(value) {}
};

// Information about database columns
class Column {
    // Declaration stuff
    std::string tableTitle_;                            // title to use in the table column
    std::string key_;                                   // name that appears on the command-line
    std::string identifier_;                            // like key, but suitable as an identifier in YAML or SQL
    size_t sqlSuffix_ = 0;                              // suffix for SQL aliases
    std::string sql_;                                   // the value as written in SQL
    std::string doc_;                                   // description
    ColumnType type_ = ColumnType::STRING;              // type of value returned by the sqlExpression
    bool isAggregate_ = false;                          // the column value is aggregated over the selection group

    // Result stuff
    bool isDisplayed_ = false;                          // should we show this column in the results?
    std::string constraintExpr_;                        // part of a WHERE clause
    std::vector<Binding> constraintBindings_;           // values that need to be bound to the constraint
    Sorted sorted_ = Sorted::NONE;                      // how to sort a displayed result

public:
    Column() {}

    const std::string& tableTitle() const {
        return tableTitle_;
    }
    Column& tableTitle(const std::string &s) {
        tableTitle_ = s;
        return *this;
    }

    const std::string& key() const {
        return key_;
    }
    Column& key(const std::string &s) {
        key_ = s;
        return *this;
    }

    const std::string& identifier() const {
        return identifier_;
    }
    Column& identifier(const std::string &s) {
        identifier_ = s;
        return *this;
    }

    const std::string& sql() const {
        return sql_;
    }
    Column& sql(const std::string &s) {
        sql_ = s;
        return *this;
    }

    Column& sqlSuffix(size_t n) {
        sqlSuffix_ = n;
        return *this;
    }
    std::string sqlAlias() const {
        return identifier_ + boost::lexical_cast<std::string>(sqlSuffix_);
    }

    const std::string& doc() const {
        return doc_;
    }
    Column& doc(const std::string &s) {
        doc_ = s;
        return *this;
    }

    ColumnType type() const {
        return type_;
    }
    Column& type(ColumnType t) {
        type_ = t;
        return *this;
    }

    bool isAggregate() const {
        return isAggregate_;
    }
    Column& isAggregate(bool b) {
        isAggregate_ = b;
        return *this;
    }

    const std::string& constraintExpr() const {
        return constraintExpr_;
    }
    Column& constraintExpr(const std::string &s) {
        constraintExpr_ = s;
        return *this;
    }

    const std::vector<Binding>& constraintBindings() const {
        return constraintBindings_;
    }
    Column& constraintBinding(const std::string &name, const std::string &value) {
        constraintBindings_.push_back(Binding(name, value));
        return *this;
    }
    template<typename T>
    Column& constraintBinding(const std::string &name, const T& value) {
        constraintBindings_.push_back(Binding(name, boost::lexical_cast<std::string>(value)));
        return *this;
    }

    bool isDisplayed() const {
        return isDisplayed_;
    }
    Column& isDisplayed(bool b) {
        isDisplayed_ = b;
        return *this;
    }

    // Does this column appear in the list of columns after SELECT?
    bool isSelected() const {
        return isDisplayed_ || constraintExpr_.empty();
    }

    Sorted sorted() const {
        return sorted_;
    }
    Column& sorted(Sorted s) {
        sorted_ = s;
        return *this;
    }
    bool isSorted() const {
        return sorted_ != Sorted::NONE;
    }
};

// A mapping from command-line name (key) to database column
using ColumnMap = Sawyer::Container::Map<std::string, Column>;

// List of columns
using ColumnList = std::vector<Column>;

// Create a map that declares all the possible database columns that can be queried.
static ColumnMap
loadColumns(DB::Connection db) {
    ColumnMap retval;

    // Columns that are ROSE dependencies
    auto stmt = db.stmt("select name, column_name, description from dependency_attributes");
    for (auto row: stmt) {
        const std::string key = *row.get<std::string>(0);
        const std::string columnName = *row.get<std::string>(1);
        auto doc = row.get<std::string>(2);

        Column c;

        // Default values
        std::string s = key;
        s[0] = toupper(s[0]);
        c.tableTitle(s);
        c.sql("test_results." + columnName);
        c.type(ColumnType::STRING);
        c.doc(doc.orElse("The " + key + " dependency"));

        // Specific values for some columns
        if ("build" == key) {
            c.tableTitle("Build\nSystem");
        } else if ("edg_compile" == key) {
            c.tableTitle("EDG\nCompile");
        }

        retval.insert(key, c);
    }

    // Other columns
    retval.insert("count",
                  Column().tableTitle("Count").sql("count(*)")
                  .doc("Number of tests represented by each row of the result table")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("duration",
                  Column().tableTitle("Test\nDuration").sql("test_results.duration")
                  .doc("How long the test took not counting slave setup")
                  .type(ColumnType::DURATION));
    retval.insert("duration.avg",
                  Column().tableTitle("Average\nDuration").sql("avg(test_results.duration)")
                  .doc("Average length of time taken for the selected tests")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("duration.max",
                  Column().tableTitle("Max\nDuration").sql("max(test_results.duration)")
                  .doc("Maximum length of time taken for any selected test")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("duration.min",
                  Column().tableTitle("Min\nDuration").sql("min(test_results.duration)")
                  .doc("Minimum length of time taken for any selected test")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("first_error",
                  Column().tableTitle("First Error").sql("test_results.first_error")
                  .doc("Heuristically detected first error message")
                  .type(ColumnType::STRING));
    retval.insert("id",
                  Column().tableTitle("ID").sql("test_results.id")
                  .doc("Test ID number")
                  .type(ColumnType::INTEGER));
    retval.insert("os",
                  Column().tableTitle("Operating\nSystem").sql("test_results.os")
                  .doc("Operating system")
                  .type(ColumnType::STRING));
    retval.insert("output",
                  Column().tableTitle("Lines of\nOutput").sql("test_results.noutput")
                  .doc("Number of lines of output")
                  .type(ColumnType::INTEGER));
    retval.insert("output.avg",
                  Column().tableTitle("Avg Lines\nof Output").sql("round(avg(test_results.noutput))")
                  .doc("Average number of lines of output for the selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("output.max",
                  Column().tableTitle("Max Lines\nof Output").sql("max(test_results.noutput)")
                  .doc("Maximum number of lines of output for any selected test")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("output.min",
                  Column().tableTitle("Min Lines\nof Output").sql("min(test_results.noutput)")
                  .doc("Minimum number of lines of output for any selected test")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("pf",
                  Column().tableTitle("P/F")
                  .sql("case when status = 'end' then 'pass' else 'fail' end")
                  .doc("The word \"pass\" if status is \"end\", otherwise the word \"fail\"")
                  .type(ColumnType::STRING));
    retval.insert("reporting_time",
                  Column().tableTitle("Reporting Time").sql("test_results.reporting_time")
                  .doc("Time at which results were reported")
                  .type(ColumnType::TIME));
    retval.insert("reporting_time.max",
                  Column().tableTitle("Latest Report").sql("max(test_results.reporting_time)")
                  .doc("Latest time at which results were reported")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("reporting_time.min",
                  Column().tableTitle("Earliest Report").sql("min(test_results.reporting_time)")
                  .doc("Earliest time at which results were reported")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("reporting_user",
                  Column().tableTitle("Reporting User").sql("auth_identities.identity")
                  .doc("User that reported the results")
                  .type(ColumnType::STRING));
    retval.insert("rose",
                  Column().tableTitle("ROSE\nVersion").sql("test_results.rose")
                  .doc("ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH));
    retval.insert("rose.max",
                  Column().tableTitle("Max ROSE\nVersion").sql("max(test_results.rose)")
                  .doc("Maximum ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH)
                  .isAggregate(true));
    retval.insert("rose.min",
                  Column().tableTitle("Min ROSE\nVersion").sql("min(test_results.rose)")
                  .doc("Minimum ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH)
                  .isAggregate(true));
    retval.insert("rose_date",
                  Column().tableTitle("ROSE Date").sql("test_results.rose_date")
                  .doc("ROSE version or commit date")
                  .type(ColumnType::TIME));
    retval.insert("rose_date.max",
                  Column().tableTitle("Latest\nROSE Date").sql("max(test_results.rose_date)")
                  .doc("Latest ROSE version or commit date")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("rose_date.min",
                  Column().tableTitle("Earliest\nROSE Date").sql("min(test_results.rose_date)")
                  .doc("Earliest ROSE version or commit date")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("slave",
                  Column().tableTitle("Slave Name").sql("test_results.tester")
                  .doc("Name of testing slave")
                  .type(ColumnType::STRING));
    retval.insert("status",
                  Column().tableTitle("Status").sql("test_results.status")
                  .doc("Result status of test; phase at which test failed")
                  .type(ColumnType::STRING));
    retval.insert("warnings",
                  Column().tableTitle("Number of\nWarnings").sql("test_results.nwarnings")
                  .doc("Number of heuristically detected warning messages")
                  .type(ColumnType::INTEGER));
    retval.insert("warnings.avg",
                  Column().tableTitle("Average\nWarnings").sql("round(avg(test_results.nwarnings))")
                  .doc("Average number of heuristically detected warning messages for the selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("warnings.max",
                  Column().tableTitle("Max\nWarnings").sql("max(test_results.nwarnings)")
                  .doc("Maximum number of heuristically detected warning messages for any selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("warnings.min",
                  Column().tableTitle("Min\nWarnings").sql("min(test_results.nwarnings)")
                  .doc("Minimum number of heuristically detected warning messages for any selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));

    for (ColumnMap::Node &node: retval.nodes()) {
        Column &c = node.value();
        if (c.key().empty())
            c.key(node.key());
        if (c.identifier().empty())
            c.identifier(boost::replace_all_copy(node.key(), ".", "_"));
    }

    return retval;
}

static std::string
findLatestRoseVersion(DB::Connection db) {
    return db.stmt("select rose from test_results order by reporting_time desc limit 1").get<std::string>().orElse("");
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
    boost::regex utcRe("\\s*(z|Z|u|U|utc|UTC|gmt|GMT)");
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

static std::string
sqlComparison(const std::string &s) {
    if ("=" == s) {
        return " = ";
    } else if ("/" == s) {
        return " <> ";
    } else if ("~" == s) {
        return " like ";
    } else if ("^" == s) {
        return " not like ";
    } else if ("-" == s) {
        return " <= ";
    } else if ("+" == s) {
        return " >= ";
    } else {
        ASSERT_not_reachable("unknown comparison operator \"" + s + "\"");
    }
}

// Adjust the column so it's constrained as specified by the comparison and value. The key is the name of the column
// from the command-line and is used in error messages.
static void
constrainColumn(const Settings &settings, Column &c, const std::string &key, const std::string &comparison,
                const std::string &value) {
    switch (c.type()) {
        case ColumnType::VERSION_OR_HASH: {
            boost::regex partialKeyRe("([0-9a-f]{1,39})(\\+local)?");
            boost::smatch found;
            if (boost::regex_match(value, found, partialKeyRe)) {
                // Special cases for comparing ROSE abbreviated commit hashes
                const std::string abbrHash = found.str(1);
                const std::string local = found.str(2);
                if ("=" == comparison || "/" == comparison) {
                    const std::string sqlOp = "=" == comparison ? " like " : " not like ";
                    c.constraintExpr(c.sql() + sqlOp + "?" + c.sqlAlias());
                    c.constraintBinding(c.sqlAlias(), abbrHash + "%" + local);
                    break;
                } else if ("-" == comparison) {
                    const std::string fullHash = abbrHash + std::string(40-abbrHash.size(), 'f') + local;
                    c.constraintExpr(c.sql() + " <= ?" + c.sqlAlias());
                    c.constraintBinding(c.sqlAlias(), fullHash);
                    break;
                } else if ("+" == comparison) {
                    const std::string fullHash = abbrHash + std::string(40-abbrHash.size(), '0') + local;
                    c.constraintExpr(c.sql() + " >= ?" + c.sqlAlias());
                    c.constraintBinding(c.sqlAlias(), fullHash);
                    break;
                }
            }
        }
            // fall through to string comparison
        case ColumnType::STRING:
            if ("=" == comparison || "/" == comparison || "-" == comparison || "+" == comparison) {
                const std::string sqlOp = sqlComparison(comparison);
                c.constraintExpr(c.sql() + sqlOp + "?" + c.sqlAlias());
                c.constraintBinding(c.sqlAlias(), value);
            } else if ("~" == comparison || "^" == comparison) {
                const std::string sqlOp = sqlComparison(comparison);
                c.constraintExpr(c.sql() + sqlOp + "?" + c.sqlAlias());
                c.constraintBinding(c.sqlAlias(), "%" + value + "%");
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;

        case ColumnType::INTEGER:
            if ("=" == comparison || "/" == comparison || "-" == comparison || "+" == comparison) {
                const std::string sqlOp = sqlComparison(comparison);
                c.constraintExpr(c.sql() + sqlOp + "?" + c.sqlAlias());
                c.constraintBinding(c.sqlAlias(), value);
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;

        case ColumnType::TIME:
            // Allow for ranges of time to be used when only a date is specified
            if ("=" == comparison || "/" == comparison || "-" == comparison || "+" == comparison) {
                if (Sawyer::Optional<std::pair<time_t, time_t>> range = parseDateTime(settings, value)) {
                    if ("=" == comparison) {
                        c.constraintExpr(c.sql() + " >= ?min_" + c.sqlAlias() + " and " +
                                         c.sql() + " <= ?max_" + c.sqlAlias());
                        c.constraintBinding("min_" + c.sqlAlias(), range->first);
                        c.constraintBinding("max_" + c.sqlAlias(), range->second);
                    } else if ("/" == comparison) {
                        c.constraintExpr(c.sql() + " < ?min_" + c.sqlAlias() + " or " +
                                         c.sql() + " > ?max_" + c.sqlAlias());
                        c.constraintBinding("min_" + c.sqlAlias(), range->first);
                        c.constraintBinding("max_" + c.sqlAlias(), range->second);
                    } else if ("-" == comparison) {
                        c.constraintExpr(c.sql() + " <= ?" + c.sqlAlias());
                        c.constraintBinding(c.sqlAlias(), range->second);
                    } else {
                        ASSERT_require("+" == comparison);
                        c.constraintExpr(c.sql() + " >= ?" + c.sqlAlias());
                        c.constraintBinding(c.sqlAlias(), range->first);
                    }
                } else {
                    mlog[FATAL] <<"invalid date/time value in constraint for \"" <<key <<"\": "
                                <<"\"" <<StringUtility::cEscape(value) <<"\"\n";
                    exit(1);
                }
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;

        case ColumnType::DURATION:
            if ("=" == comparison || "/" == comparison || "-" == comparison || "+" == comparison) {
                const std::string sqlOp = sqlComparison(comparison);
                c.constraintExpr(c.sql() + sqlOp + "?" + c.sqlAlias());
                auto parser = Rose::CommandLine::durationParser();
                c.constraintBinding(c.sqlAlias(), parser->parse(value));
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;
    }
}

// Number of columns being displayed
static size_t
nDisplayed(const ColumnList &cols) {
    size_t retval = 0;
    for (const Column &c: cols) {
        if (c.isDisplayed())
            ++retval;
    }
    return retval;
}

// Is the specified column alread selected for displaying in the results?
static bool
isDisplayed(const ColumnList &cols, const std::string &key) {
    for (const Column &c: cols) {
        if (c.key() == key && c.isDisplayed())
            return true;
    }
    return false;
}

// Find the column in the list of columns and return its index if it is present
static Sawyer::Optional<size_t>
find(const ColumnList &cols, const std::string &key) {
    for (size_t i = 0; i < cols.size(); ++i) {
        if (cols[i].key() == key)
            return i;
    }
    return Sawyer::Nothing();
}

static Column&
appendColumn(ColumnList &cols, const Column &col) {
    static Sawyer::Container::Map<std::string /*column_key*/, size_t /*count*/> suffixes;
    cols.push_back(col);
    cols.back().sqlSuffix(++suffixes.insertMaybe(col.key(), 0));
    return cols.back();
}

static void
showConstraint(const std::string &name, const std::string &comparison, const std::string &value) {
    SAWYER_MESG(mlog[INFO]) <<(boost::format("  %-16s %s \"%s\"\n") % name % comparison % StringUtility::cEscape(value));
}

// Constrain ROSE version if appropriate
static void
maybeConstrainRoseVersion(const Settings &settings, DB::Connection db, ColumnList &cols, const ColumnMap &decls) {
    if (settings.considerAll)
        return;

    for (const Column &c: cols) {
        if ((c.key() == "rose" || c.key() == "rose_date") && !c.constraintExpr().empty())
            return;
    }

    std::string roseVersion = findLatestRoseVersion(db);
    if (!roseVersion.empty()) {
        Column &c = appendColumn(cols, decls["rose"]);
        c.constraintExpr(c.sql() + " = ?" + c.sqlAlias());
        c.constraintBinding(c.sqlAlias(), roseVersion);
        showConstraint("rose", "=", roseVersion);
    }
}

// Display all columns that we haven't chosen to display yet and which don't have constraints
static void
displayMoreColumns(ColumnList &cols, const ColumnMap &decls) {
    for (const Column &decl: decls.values()) {
        if (!decl.isAggregate()) {
            if (auto idx = find(cols, decl.key())) {
                if (cols[*idx].constraintExpr().empty())
                    cols[*idx].isDisplayed(true);
            } else {
                appendColumn(cols, decl).isDisplayed(true);
            }
        }
    }
}

// Convert the list of displayed columns to an SQL column list for a SELECT statement.
static std::string
buildSelectClause(const ColumnList &cols) {
    std::string s;
    for (const Column &c: cols) {
        if (c.isSelected()) {
            s += (s.empty() ? "select " : ", ") + c.sql();
            if (c.sql() != c.sqlAlias())
                s += " as " + c.sqlAlias();
        }
    }
    return s;
}

// Build the GROUP BY clause
static std::string
buildGroupByClause(const ColumnList &cols) {
    std::string s;
    for (const Column &c: cols) {
        if (c.isDisplayed() && !c.isAggregate())
            s += (s.empty() ? " group by " : ", ") + c.sqlAlias();
    }
    return s;
}

// Build an optional WHERE clause for the constrainted columns
static std::string
buildWhereClause(const ColumnList &cols) {
    std::string s;
    for (const Column &c: cols) {
        if (!c.constraintExpr().empty() && !c.isAggregate())
            s += (s.empty() ? " where " : " and ") + c.constraintExpr();
    }
    return s;
}

// Build an optional HAVING clause for the constrained aggregate columns
static std::string
buildHavingClause(const ColumnList &cols) {
    std::string s;
    for (const Column &c: cols) {
        if (!c.constraintExpr().empty() && c.isAggregate())
            s += (s.empty() ? " having " : " and ") + c.constraintExpr();
    }
    return s;
}

// Build an optional ORDERED BY clause
static std::string
buildOrderedByClause(const ColumnList &cols) {
    std::string s;
    for (const Column &c: cols) {
        if (c.isDisplayed() && c.sorted() != Sorted::NONE) {
            s += (s.empty() ? " order by " : ", ") + c.sqlAlias();
            if (Sorted::DESCENDING == c.sorted())
                s += " desc";
        }
    }
    return s;
}

// Add variable bindings for constraints
static void
buildConstraintBindings(DB::Statement stmt, const ColumnList &cols) {
    for (const Column &c: cols) {
        for (const Binding &b: c.constraintBindings()) {
            stmt.bind(b.name, b.value);
            SAWYER_MESG(mlog[DEBUG]) <<"  where " <<b.name <<" = " <<b.value <<"\n";
        }
    }
}

static DB::Statement
buildStatement(const Settings &settings, DB::Connection db, const ColumnList &cols) {
    std::string sql = buildSelectClause(cols) +
                      " from test_results" +
                      " join auth_identities on test_results.reporting_user = auth_identities.id" +
                      buildWhereClause(cols) +
                      buildGroupByClause(cols) +
                      buildHavingClause(cols) +
                      buildOrderedByClause(cols);
    if (settings.limit) {
        sql += " limit " + boost::lexical_cast<std::string>(*settings.limit);
    } else {
        // +1 is so we can detect if there are more rows
        sql += " limit " + boost::lexical_cast<std::string>(DEFAULT_ROW_LIMIT + 1);
    }

    SAWYER_MESG(mlog[DEBUG]) <<"SQL: " <<sql <<"\n";
    DB::Statement stmt = db.stmt(sql);

    buildConstraintBindings(stmt, cols);
    return stmt;
}

static void
buildTableColumnHeaders(FormattedTable &table, const ColumnList &cols) {
    size_t j = 0;
    for (const Column &c: cols) {
        if (c.isDisplayed())
            table.columnHeader(0, j++, c.tableTitle());
    }
}

static std::string
formatValue(const Settings &settings, const Column &c, const std::string &value) {
    if (Format::SHELL == settings.outputFormat)
        return value;

    switch (c.type()) {
        case ColumnType::STRING:
        case ColumnType::INTEGER:
        case ColumnType::VERSION_OR_HASH:
            return value;

        case ColumnType::TIME: {
            time_t when = boost::lexical_cast<time_t>(value);
            std::string s = settings.usingLocalTime ? timeToLocal(when) : timeToGmt(when);
            if (settings.showAges)
                s += ", " + approximateAge(when);
            return s;
        }

        case ColumnType::DURATION: {
            uint64_t length = 0;
            try {
                length = boost::lexical_cast<uint64_t>(value);
            } catch (...) {
                length = ::round(boost::lexical_cast<double>(value));
            }
            return Rose::CommandLine::durationParser()->toString(length);
        }
    }
}

// Adjust table by replacing repeated values with ditto marks.
void
dittoize(FormattedTable &table, size_t beginCol, size_t endCol) {
    if (table.nRows() > 0 && table.nColumns() > 0) {
        ASSERT_require(beginCol < endCol);
        ASSERT_require(endCol <= table.nColumns());

        std::vector<std::string> prev(endCol - beginCol);
        for (size_t i = 0; i < table.nRows(); ++i) {
            for (size_t j = beginCol; j < endCol; ++j) {
                const std::string value = table.get(i, j);
                if (!value.empty() && value == prev[j - beginCol]) {
                    table.insert(i, j, "\"");
                    table.cellProperties(i, j, centered());
                } else {
                    for (size_t k = j; k < endCol; ++k)
                        prev[k - beginCol] = table.get(i, k);
                    break;
                }
            }
        }
    }
}

// Returns the number of tests matched by the row (zero if unavailable)
static size_t
emitToTable(const Settings &settings, FormattedTable &table, DB::Row &row, const ColumnList &cols,
          std::set<size_t> &testIds /*out*/) {
    const size_t i = table.nRows();
    size_t j = 0, nTests = 0;
    for (const Column &c: cols) {
        if (c.isDisplayed()) {
            if (auto value = row.get<std::string>(j)) {
                table.insert(i, j, formatValue(settings, c, *value));
                if (c.key() == "status") {
                    table.cellProperties(i, j, "end" == *value ? goodCell() : badCell());
                } else if (c.key() == "pf") {
                    table.cellProperties(i, j, "pass" == *value ? goodCell() : badCell());
                }
            } else {
                table.insert(i, j, "");
            }
        }

        if (c.isSelected()) {
            if (c.key() == "count") {
                nTests += row.get<size_t>(j).orElse(0);
            } else if (c.key() == "id") {
                testIds.insert(*row.get<size_t>(j));
            }
            ++j;
        }
    }
    return nTests;
}

// Returns the number of tests matched by the row (zero if unavailable)
static size_t
emitYaml(const Settings &settings, DB::Row &row, const ColumnList &cols, std::set<size_t> &testIds /*out*/) {
    size_t j = 0, nTests = 0;
    for (const Column &c: cols) {
        if (c.isDisplayed()) {
            if (auto value = row.get<std::string>(j)) {
                const std::string formatted = formatValue(settings, c, *value);
                std::cout <<(0 == j ? "- " : "  ") <<c.identifier() <<": " <<StringUtility::yamlEscape(*value);
                if (*value != formatted)
                    std::cout <<" # " <<formatted;
                std::cout <<"\n";
            } else {
                std::cout <<(0 == j ? "- " : "  ") <<c.identifier() <<": \"\"\n";
            }
        }

        if (c.isSelected()) {
            if (c.key() == "count") {
                nTests += row.get<size_t>(j).orElse(0);
            } else if (c.key() == "id") {
                testIds.insert(*row.get<size_t>(j));
            }
            ++j;
        }
    }
    return nTests;
}

static void
emitDeclarations(const Settings &settings, const ColumnMap &columnDecls) {
    FormattedTable table;
    table.columnHeader(0, 0, "Command Line");
    table.columnHeader(0, 1, "Description");
    table.columnHeader(0, 2, "YAML key");
    table.columnHeader(0, 3, "SQL expr");
    for (const Column &c: columnDecls.values()) {
        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- key: " <<StringUtility::yamlEscape(c.key()) <<"\n";
            std::cout <<"  yaml_key: " <<StringUtility::yamlEscape(c.identifier()) <<"\n";
            std::cout <<"  sql: " <<StringUtility::yamlEscape(c.sql()) <<"\n";
            std::cout <<"  description: " <<StringUtility::yamlEscape(c.doc()) <<"\n";
        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, c.key());
            table.insert(i, 1, c.doc());
            table.insert(i, 2, c.identifier());
            table.insert(i, 3, c.sql());
        }
    }

    if (Format::YAML != settings.outputFormat) {
        table.format(tableFormat(settings.outputFormat));
        std::cout <<table;
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);
    const ColumnMap columnDecls = loadColumns(db);

    // Patterns
    const std::string identifier = "(?:[a-zA-Z]\\w*)";
    const std::string qualifier = "(?:\\.(?:avg|max|min))";
    const std::string columnName = "(?:" + identifier + qualifier + "?)";
    const std::string sort = "(?:\\.[ad])";
    const std::string comparisonOperator = "[-+=/~^]";
    const std::string value = ".*";

    const boost::regex displayRe("(" + columnName + ")(" + sort + ")?");
    const boost::regex expressionRe("(" + columnName + ")(" + comparisonOperator + ")(" + value + ")");


    // Parse positional command-line positional arguments. They are either constraints or column names to display.
    std::vector<Column> cols;
     for (const std::string &arg: args) {
        boost::smatch exprParts;
        if (boost::regex_match(arg, exprParts, expressionRe)) {
            // Arguments of the form <KEY><OPERATOR><VALUE> constrain the results.  This key column will be emitted above the
            // table instead of within the table (since the column within the table would have one value across all the rows.
            const std::string key = exprParts.str(1);
            const std::string comparison = exprParts.str(2);
            const std::string value = exprParts.str(3);

            if (!columnDecls.exists(key)) {
                mlog[FATAL] <<"invalid column \"" <<StringUtility::cEscape(key) <<"\"\n"
                            <<"use \"list\" to get a list of valid column names\n";
                exit(1);
            }

            Column &c = appendColumn(cols, columnDecls[key]);
            showConstraint(key, comparison, value);
            constrainColumn(settings, c, key, comparison, value);

        } else if ("list" == arg) {
            // List all the column information
            emitDeclarations(settings, columnDecls);
            exit(0);

        } else if ("all" == arg) {
            displayMoreColumns(cols, columnDecls);

        } else if (boost::regex_match(arg, exprParts, displayRe)) {
            // A column name all by itself (or with a sort direction) means display the column
            const std::string key = exprParts.str(1);
            const std::string sorted = exprParts.str(2);

            if (!columnDecls.exists(key)) {
                mlog[FATAL] <<"invalid column \"" <<StringUtility::cEscape(key) <<"\"\n"
                            <<"use \"list\" to get a list of valid column names\n";
                exit(1);
            }
            if (isDisplayed(cols, key)) {
                mlog[WARN] <<"duplicate display request for \"" <<key <<"\" is ignored\n";
            } else {
                Column &c = appendColumn(cols, columnDecls[key]);
                c.isDisplayed(true);
                if (".a" == sorted) {
                    c.sorted(Sorted::ASCENDING);
                } else if (".d" == sorted) {
                    c.sorted(Sorted::DESCENDING);
                } else {
                    ASSERT_require("" == sorted);
                }
            }

        } else {
            mlog[FATAL] <<"unrecognized argument \"" <<StringUtility::cEscape(arg) <<"\"\n";
            exit(1);
        }
    }
    maybeConstrainRoseVersion(settings, db, cols, columnDecls);

    // If we're deleting records, then the test ID must be one of the things we're selecting. But don't select it
    // automatically--make the user do it so we know that they know what they're doing.
    if (settings.deleteMatchingTests) {
        if (!isDisplayed(cols, "id")) {
            mlog[FATAL] <<"the \"id\" field must be displayed in order to delete tests\n";
            exit(1);
        }
    }

    // If no columns are displayed, then select lots of them
    if (nDisplayed(cols) == 0) {
        displayMoreColumns(cols, columnDecls);
        ASSERT_require(nDisplayed(cols) != 0);
    }

    // Make sure counts is selected (even it not displayed) so we can report how many tests matched.
    if (auto idx = find(cols, "count")) {
    } else {
        appendColumn(cols, columnDecls["count"]);
    }

    // Run the query
    DB::Statement query = buildStatement(settings, db, cols);

    // Generate the output per row of result
    std::set<size_t> testIds;
    FormattedTable table;
    size_t nTests = 0, nRows = 0;
    if (Format::YAML != settings.outputFormat)
        buildTableColumnHeaders(table, cols);
    for (auto row: query) {
        if (++nRows > settings.limit.orElse(DEFAULT_ROW_LIMIT)) {
            // We got more rows that the default limit. Don't print this last row since it's just a guard to detect the
            // overflow.
        } else {
            switch (settings.outputFormat) {
                case Format::PLAIN:
                case Format::HTML:
                case Format::CSV:
                case Format::SHELL:
                    nTests += emitToTable(settings, table, row, cols, testIds /*out*/);
                    break;
                case Format::YAML:
                    nTests += emitYaml(settings, row, cols, testIds /*out*/);
                    break;
            }
        }
    }

    // Should we replace repeated values with ditto marks? If so, we need to find the first contiguous group of columns
    // that are sorted, and these will be the ones to consider for dittos.
    if (settings.dittoize) {
        Sawyer::Optional<size_t> firstSorted;
        size_t lastSorted = 0, tableColumn = 0;
        for (const Column &c: cols) {
            if (c.isSorted()) {
                if (!firstSorted) {
                    firstSorted = tableColumn;
                    lastSorted = tableColumn;
                } else {
                    lastSorted = tableColumn;
                }
            } else if (firstSorted) {
                break;
            }
            if (c.isDisplayed())
                ++tableColumn;
        }
        if (firstSorted && table.nRows() > 0) {
            ASSERT_require(*firstSorted <= lastSorted);
            ASSERT_require(lastSorted < table.nColumns());
            dittoize(table, *firstSorted, lastSorted+1);
        }
    }

    // Produce output
    switch (settings.outputFormat) {
        case Format::PLAIN:
            table.format(tableFormat(settings.outputFormat));
            std::cout <<table
                      <<StringUtility::plural(nTests, "matching tests") <<"\n";
            break;
        case Format::CSV:
        case Format::SHELL:
            table.format(tableFormat(settings.outputFormat));
            std::cout <<table;
            SAWYER_MESG(mlog[INFO]) <<StringUtility::plural(nTests, "matching tests") <<"\n";
            break;
        case Format::HTML:
            table.format(FormattedTable::Format::HTML);
            std::cout <<table
                      <<"<p>" <<StringUtility::plural(nTests, "matching tests") <<"</p>\n";
            break;
        case Format::YAML:
            SAWYER_MESG(mlog[INFO]) <<StringUtility::plural(nTests, "matching tests") <<"\n";
            break;
    }

    if (nRows > settings.limit.orElse(DEFAULT_ROW_LIMIT)) {
        SAWYER_MESG(mlog[WARN]) <<"results were terminated after "
                                <<StringUtility::plural(settings.limit.orElse(DEFAULT_ROW_LIMIT), "rows")
                                <<"; use --limit to see more\n";
    }

    // Delete tests
    if (settings.deleteMatchingTests && !testIds.empty()) {
        mlog[INFO] <<"deleting " <<StringUtility::plural(testIds.size(), "matching tests") <<"\n";
        std::string inClause;
        for (size_t id: testIds)
            inClause += (inClause.empty() ? " in (" : ", ") + boost::lexical_cast<std::string>(id);
        inClause += ")";

        // Delete attachments first, then test records
        db.stmt("delete from attachments where test_id " + inClause).run();
        db.stmt("delete from test_results where id " + inClause).run();
    }
}
