static const char *gPurpose = "query test results";
static const char *gDescription =
    "Queries a database to show the matrix testing results.  The arguments are column names (use \"list\" to "
    "get a list of valid column names). They can be in two forms: constraints and displays. A constraint argument is "
    "a column name, comparison operator, and value; and a display argument is column name and optional "
    "sorting direction.\n\n"

    "The relational operators for constraints are designed to work well with shell scripts. They are:"
    "@named{=}{The column on the left hand side must be equal to the value on the right hand side.}"
    "@named{/}{The column on the left hand side must be not-equal to the value on the right hand sie.}"
    "@named{~}{The column on the left hand side must contain the string on the right hand side.}"
    "\n\n"

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
    std::string symbol_;                                // name to use as the value in YAML output and SQL aliases
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

    const std::string& symbol() const {
        return symbol_;
    }
    Column& symbol(const std::string &s) {
        symbol_ = s;
        return *this;
    }

    const std::string& sql() const {
        return sql_;
    }
    Column& sql(const std::string &s) {
        sql_ = s;
        return *this;
    }

    std::string sqlAlias() const {
        return isAggregate() ? symbol() : sql();
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
    auto stmt = db.stmt("select distinct name from dependencies");
    for (auto row: stmt) {
        const std::string key = *row.get<std::string>(0);
        Column c;

        std::string s = key;
        s[0] = toupper(s[0]);
        c.tableTitle(s);
        c.symbol(key);
        c.sql("test_results.rmc_" + key);
        c.type(ColumnType::STRING);
        c.doc("The " + key + " dependency");
        if ("build" == key) {
            c.tableTitle("Build\nSystem");
            c.doc("The build system used to build ROSE");
        } else if ("debug" == key) {
            c.doc("Whether compiler debug flags are used");
        } else if ("edg_compile" == key) {
            c.tableTitle("EDG\nCompile");
            c.doc("Whether to compile EDG or download an EDG binary");
        } else if ("languages" == key) {
            c.doc("Comma-separated list of analysis languages");
        } else if ("optimize" == key) {
            c.doc("Whether to enable compiler optimizations");
        }

        retval.insert(key, c);
    }

    // Other columns
    retval.insert("id",
                  Column().tableTitle("ID").symbol("id").sql("test_results.id")
                  .doc("Test ID number")
                  .type(ColumnType::INTEGER));
    retval.insert("reporting_user",
                  Column().tableTitle("Reporting\nUser").symbol("reporting_user").sql("auth_identities.identity")
                  .doc("User that reported the results")
                  .type(ColumnType::STRING));
    retval.insert("reporting_time",
                  Column().tableTitle("Reporting\nTime").symbol("reporting_time").sql("test_results.reporting_time")
                  .doc("Time at which results were reported")
                  .type(ColumnType::TIME));
    retval.insert("min_reporting_time",
                  Column().tableTitle("Earliest\nReport").symbol("min_reporting_time").sql("min(test_results.reporting_time)")
                  .doc("Earliest time at which results were reported")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("max_reporting_time",
                  Column().tableTitle("Latest\nReport").symbol("max_reporting_time").sql("max(test_results.reporting_time)")
                  .doc("Latest time at which results were reported")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("tester",
                  Column().tableTitle("Tester\nName").symbol("tester").sql("test_results.tester")
                  .doc("Name of testing slave")
                  .type(ColumnType::STRING));
    retval.insert("os",
                  Column().tableTitle("Operating\nSystem").symbol("os").sql("test_results.os")
                  .doc("Operating system")
                  .type(ColumnType::STRING));
    retval.insert("rose",
                  Column().tableTitle("ROSE\nVersion").symbol("rose").sql("test_results.rose")
                  .doc("ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH));
    retval.insert("min_rose",
                  Column().tableTitle("Min ROSE\nVersion").symbol("min_rose").sql("min(test_results.rose)")
                  .doc("Minimum ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH)
                  .isAggregate(true));
    retval.insert("max_rose",
                  Column().tableTitle("Max ROSE\nVersion").symbol("max_rose").sql("max(test_results.rose)")
                  .doc("Maximum ROSE version number or commit hash")
                  .type(ColumnType::VERSION_OR_HASH)
                  .isAggregate(true));
    retval.insert("rose_date",
                  Column().tableTitle("ROSE\nDate").symbol("rose_date").sql("test_results.rose_date")
                  .doc("ROSE version or commit date")
                  .type(ColumnType::TIME));
    retval.insert("min_rose_date",
                  Column().tableTitle("Earliest\nROSE").symbol("min_rose_date").sql("min(test_results.rose_date)")
                  .doc("Earliest ROSE version or commit date")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("max_rose_date",
                  Column().tableTitle("Latest\nROSE").symbol("max_rose_date").sql("max(test_results.rose_date)")
                  .doc("Latest ROSE version or commit date")
                  .type(ColumnType::TIME)
                  .isAggregate(true));
    retval.insert("status",
                  Column().tableTitle("Status").symbol("status").sql("test_results.status")
                  .doc("Result status of test; phase at which test failed")
                  .type(ColumnType::STRING));
    retval.insert("duration",
                  Column().tableTitle("Test\nDuration").symbol("duration").sql("test_results.duration")
                  .doc("How long the test took not counting slave setup")
                  .type(ColumnType::DURATION));
    retval.insert("avg_duration",
                  Column().tableTitle("Average\nDuration").symbol("avg_duration").sql("avg(test_results.duration)")
                  .doc("Average length of time taken for the selected tests")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("max_duration",
                  Column().tableTitle("Max\nDuration").symbol("max_duration").sql("max(test_results.duration)")
                  .doc("Maximum length of time taken for any selected test")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("min_duration",
                  Column().tableTitle("Min\nDuration").symbol("min_duration").sql("min(test_results.duration)")
                  .doc("Minimum length of time taken for any selected test")
                  .type(ColumnType::DURATION)
                  .isAggregate(true));
    retval.insert("output",
                  Column().tableTitle("Lines of\nOutput").symbol("output").sql("test_results.noutput")
                  .doc("Number of lines of output")
                  .type(ColumnType::INTEGER));
    retval.insert("avg_output",
                  Column().tableTitle("Avg Lines\nof Output").symbol("avg_output").sql("avg(test_results.noutput)")
                  .doc("Average number of lines of output for the selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("max_output",
                  Column().tableTitle("Max Lines\nof Output").symbol("max_output").sql("max(test_results.noutput)")
                  .doc("Maximum number of lines of output for any selected test")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("min_output",
                  Column().tableTitle("Min Lines\nof Output").symbol("min_output").sql("min(test_results.noutput)")
                  .doc("Minimum number of lines of output for any selected test")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("warnings",
                  Column().tableTitle("Number of\nWarnings").symbol("warnings").sql("test_results.nwarnings")
                  .doc("Number of heuristically detected warning messages")
                  .type(ColumnType::INTEGER));
    retval.insert("avg_warnings",
                  Column().tableTitle("Average\nWarnings").symbol("avg_warnings").sql("avg(test_results.nwarnings)")
                  .doc("Average number of heuristically detected warning messages for the selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("max_warnings",
                  Column().tableTitle("Max\nWarnings").symbol("max_warnings").sql("max(test_results.nwarnings)")
                  .doc("Maximum number of heuristically detected warning messages for any selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("min_warnings",
                  Column().tableTitle("Min\nWarnings").symbol("min_warnings").sql("min(test_results.nwarnings)")
                  .doc("Minimum number of heuristically detected warning messages for any selected tests")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));
    retval.insert("first_error",
                  Column().tableTitle("First Error").symbol("first_error").sql("test_results.first_error")
                  .doc("Heuristically detected first error message")
                  .type(ColumnType::STRING));
    retval.insert("count",
                  Column().tableTitle("Count").symbol("count").sql("count(*)")
                  .doc("Number of tests represented by each row of the result table")
                  .type(ColumnType::INTEGER)
                  .isAggregate(true));

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

// Adjust the column so it's constrained as specified by the comparison and value. The key is the name of the column
// from the command-line and is used in error messages.
static void
constrainColumn(const Settings &settings, Column &c, const std::string &key, const std::string &comparison,
                const std::string &value) {
    switch (c.type()) {
        case ColumnType::VERSION_OR_HASH:
            if ("=" == comparison || "/" == comparison) {
                // Special cases for comparing ROSE commit hashes to allow specifying abbreviated hashes.
                boost::regex partialKeyRe("([0-9a-f]{1,39})(\\+local)?");
                boost::smatch matches;
                if (boost::regex_match(value, matches, partialKeyRe)) {
                    const std::string sqlOp = "=" == comparison ? " like " : " not like ";
                    c.constraintExpr(c.sqlAlias() + sqlOp + "?" + c.symbol());
                    c.constraintBinding(c.symbol(), matches.str(1) + "%" + matches.str(2));
                } else {
                    const std::string sqlOp = "=" == comparison ? " = " : " <> ";
                    c.constraintExpr(c.sqlAlias() + sqlOp + "?" + c.symbol());
                    c.constraintBinding(c.symbol(), value);
                }
                break;
            }
            // fall through to string comparison
        case ColumnType::STRING:
            if ("=" == comparison || "/" == comparison) {
                const std::string sqlOp = "=" == comparison ? " = " : " <> ";
                c.constraintExpr(c.sqlAlias() + sqlOp + "?" + c.symbol());
                c.constraintBinding(c.symbol(), value);
            } else if ("~" == comparison) {
                const std::string sqlOp = "=" == comparison ? " like " : " not like ";
                c.constraintExpr(c.sqlAlias() + sqlOp + "?" + c.symbol());
                c.constraintBinding(c.symbol(), "%" + value + "%");
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;

        case ColumnType::INTEGER:
            if ("=" == comparison || "/" == comparison) {
                const std::string sqlOp = "=" == comparison ? " = " : " <> ";
                c.constraintExpr(c.sqlAlias() + " " + sqlOp + " ?" + c.symbol());
                c.constraintBinding(c.symbol(), value);
            } else {
                mlog[FATAL] <<"field \"" <<key <<"\" cannot be constrained with \"" <<comparison <<"\" operator\n";
                exit(1);
            }
            break;

        case ColumnType::TIME:
            // Allow for ranges of time to be used when only a date is specified
            if ("=" == comparison || "/" == comparison) {
                if (Sawyer::Optional<std::pair<time_t, time_t>> range = parseDateTime(settings, value)) {
                    if ("=" == comparison) {
                        c.constraintExpr(c.sqlAlias() + " >= ?min_" + c.symbol() + " and " +
                                         c.sqlAlias() + " <= ?max_" + c.symbol());
                    } else {
                        c.constraintExpr(c.sqlAlias() + " < ?min_" + c.symbol() + " or " +
                                         c.sqlAlias() + " > ?max_" + c.symbol());
                    }
                    c.constraintBinding("min_" + c.symbol(), range->first);
                    c.constraintBinding("max_" + c.symbol(), range->second);
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
            if ("=" == comparison || "/" == comparison) {
                const std::string sqlOp = "=" == comparison ? " = " : " <> ";
                c.constraintExpr(c.sqlAlias() + sqlOp + "?" + c.symbol());
                c.constraintBinding(c.symbol(), Rose::CommandLine::DurationParser::parse(value));
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
        if (c.symbol() == key && c.isDisplayed())
            return true;
    }
    return false;
}

// Find the column in the list of columns and return its index if it is present
static Sawyer::Optional<size_t>
find(const ColumnList &cols, const std::string &key) {
    for (size_t i = 0; i < cols.size(); ++i) {
        if (cols[i].symbol() == key)
            return i;
    }
    return Sawyer::Nothing();
}

// Display all columns that we haven't chosen to display yet and which don't have constraints
static void
displayMoreColumns(ColumnList &cols, const ColumnMap &decls) {
    for (const Column &decl: decls.values()) {
        if (!decl.isAggregate()) {
            if (auto idx = find(cols, decl.symbol())) {
                if (cols[*idx].constraintExpr().empty())
                    cols[*idx].isDisplayed(true);
            } else {
                cols.push_back(decl);
                cols.back().isDisplayed(true);
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
        if (!c.constraintExpr().empty())
            s += (s.empty() ? " where " : " and ") + c.constraintExpr();
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
            return Rose::CommandLine::DurationParser::toString(length);
        }
    }
}

// Returns the number of tests matched by the row (zero if unavailable)
static size_t
emitTable(const Settings &settings, FormattedTable &table, DB::Row &row, const ColumnList &cols,
          std::set<size_t> &testIds /*out*/) {
    const size_t i = table.nRows();
    size_t j = 0, nTests = 0;
    for (const Column &c: cols) {
        if (c.isDisplayed()) {
            if (auto value = row.get<std::string>(j))
                table.insert(i, j, formatValue(settings, c, *value));
        }

        if (c.isSelected()) {
            if (c.symbol() == "count") {
                nTests += row.get<size_t>(j).orElse(0);
            } else if (c.symbol() == "id") {
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
                std::cout <<(0 == j ? "- " : "  ") <<c.symbol() <<": " <<StringUtility::yamlEscape(*value);
                if (*value != formatted)
                    std::cout <<" # " <<formatted;
                std::cout <<"\n";
            }
        }

        if (c.isSelected()) {
            if (c.symbol() == "count") {
                nTests += row.get<size_t>(j).orElse(0);
            } else if (c.symbol() == "id") {
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
    table.columnHeader(0, 0, "Key[1]");
    table.columnHeader(0, 1, "Description");
    table.columnHeader(0, 2, "SQL expr");
    for (const Column &c: columnDecls.values()) {
        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- key: " <<StringUtility::yamlEscape(c.symbol()) <<"\n";
            std::cout <<"  sql: " <<StringUtility::yamlEscape(c.sql()) <<"\n";
            std::cout <<"  description: " <<StringUtility::yamlEscape(c.doc()) <<"\n";
        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, c.symbol());
            table.insert(i, 1, c.doc());
            table.insert(i, 2, c.sql());
        }
    }

    switch (settings.outputFormat) {
        case Format::PLAIN:
            std::cout <<table
                      <<"Footnote [1]: The key is what you use on the command-line, and also appears in YAML output\n";
            break;
        case Format::HTML:
            table.format(FormattedTable::Format::HTML);
            std::cout <<table
                      <<"<p>Footnote [1]: The key is what you use on the command-line, and also appears in YAML "
                      <<"output</p>\n";
            break;
        case Format::YAML:
            break;
        case Format::CSV:
        case Format::SHELL:
            table.format(tableFormat(settings.outputFormat));
            std::cout <<table;
            break;
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    auto db = DB::Connection::fromUri(settings.databaseUri);
    const ColumnMap columnDecls = loadColumns(db);

    // Parse positional command-line positional arguments. They are either constraints or column names to display.
    boost::regex nameRe("[_a-zA-Z][_a-zA-Z0-9]*");
    boost::regex exprRe("([_a-zA-Z][_a-zA-Z0-9]*)([=/~])(.*)");
    boost::regex nameSortRe("([_a-zA-Z][_a-zA-Z0-9]*)(\\.[ad])?");
    std::vector<Column> cols;

    for (const std::string &arg: args) {
        boost::smatch exprParts;
        if (boost::regex_match(arg, exprParts, exprRe)) {
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

            const Column &decl = columnDecls[key];
            cols.push_back(decl);
            SAWYER_MESG(mlog[INFO]) <<(boost::format("  %-16s %s \"%s\"\n") % key % comparison % StringUtility::cEscape(value));
            constrainColumn(settings, cols.back(), key, comparison, value);

        } else if ("list" == arg) {
            // List all the column information
            emitDeclarations(settings, columnDecls);
            exit(0);

        } else if ("all" == arg) {
            displayMoreColumns(cols, columnDecls);

        } else if (boost::regex_match(arg, exprParts, nameSortRe)) {
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
                const Column &decl = columnDecls[key];
                cols.push_back(decl);
                cols.back().isDisplayed(true);
                if (".a" == sorted) {
                    cols.back().sorted(Sorted::ASCENDING);
                } else if (".d" == sorted) {
                    cols.back().sorted(Sorted::DESCENDING);
                } else {
                    ASSERT_require("" == sorted);
                }
            }

        } else {
            mlog[FATAL] <<"unrecognized argument \"" <<StringUtility::cEscape(arg) <<"\"\n";
            exit(1);
        }
    }

    // If we're deleting records, then the test ID must be one of the things we're selecting. But don't select it
    // automatically--make the user do it so we know that they know what they're doing.
    if (settings.deleteMatchingTests) {
        if (!isDisplayed(cols, "id")) {
            mlog[FATAL] <<"the \"id\" field must be selected in order to delete tests\n";
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
        cols.push_back(columnDecls["count"]);
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
                    nTests += emitTable(settings, table, row, cols, testIds /*out*/);
                    break;
                case Format::YAML:
                    nTests += emitYaml(settings, row, cols, testIds /*out*/);
                    break;
            }
        }
    }
    switch (settings.outputFormat) {
        case Format::PLAIN:
        case Format::CSV:
        case Format::SHELL:
            table.format(tableFormat(settings.outputFormat));
            std::cout <<table
                      <<StringUtility::plural(nTests, "matching tests") <<"\n";
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
