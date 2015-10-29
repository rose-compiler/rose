#include <rose.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Set.h>
#include <SqlDatabase.h>

using namespace Sawyer::Message::Common;

struct Settings {
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
#ifdef DEFAULT_DATABASE
        : databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("query test results table");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{columns}");
    parser.doc("Description",
               "Queries a database to show the matrix testing results.  The arguments are column names (use \"list\" to "
               "get a list of valid column names. They can be in two forms: a bare column name causes the table to "
               "contain that column, but if the column name is followed by an equal sign and a value, then the table "
               "is restricted to rows that have that value for the column, and the constant-valued column is displayed "
               "above the table instead (if you also want it in the table, then also specify its bare name).  If no columns "
               "are specified then all of them are shown.  Since more than one test might match the selection criteria, "
               "the table ends with a \"totals\" column to say how many such rows are present in the database.");
    parser.errorStream(mlog[FATAL]);

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("URI specifying which database to use."));

    return parser.with(CommandlineProcessing::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs();
}

typedef Sawyer::Container::Map<std::string /*key*/, std::string /*colname*/> DependencyNames;

static DependencyNames
loadDependencyNames(const SqlDatabase::TransactionPtr &tx) {
    DependencyNames retval;
    SqlDatabase::StatementPtr q = tx->statement("select distinct name from dependencies");
    for (SqlDatabase::Statement::iterator row=q->begin(); row!=q->end(); ++row) {
        std::string key = row.get<std::string>(0);
        retval.insert(key, "rmc_"+key);
    }

    // Additional key/column relationships
    retval.insert("id", "id");
    retval.insert("reporting_user", "reporting_user");
    retval.insert("reporting_time", "reporting_time");
    retval.insert("tester", "tester");
    retval.insert("os", "os");
    retval.insert("rose", "rose");
    retval.insert("rose_date", "rose_date");
    retval.insert("status", "status");
    retval.insert("duration", "duration");
    retval.insert("noutput", "noutput");
    retval.insert("nwarnings", "nwarnings");

    return retval;
}

static void
displayHorizontalLine(const std::vector<std::size_t> &maxWidth) {
    if (!maxWidth.empty()) {
        for (size_t i=0; i<maxWidth.size(); ++i)
            std::cout <<"+-" <<std::string(maxWidth[i], '-') <<"-";
        std::cout <<"+\n";
    }
}

static void
displayTableHeader(const std::vector<std::string> &keysSelected, const std::vector<size_t> &maxWidth) {
    if (!maxWidth.empty()) {
        displayHorizontalLine(maxWidth);
        for (size_t i=0; i<keysSelected.size(); ++i)
            std::cout <<"| " <<std::left <<std::setw(maxWidth[i]) <<keysSelected[i] <<" ";
        std::cout <<"|\n";
        displayHorizontalLine(maxWidth);
    }
}

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    mlog = Sawyer::Message::Facility("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    DependencyNames dependencyNames = loadDependencyNames(tx);

    // Parse positional command-line arguments
    Sawyer::Container::Set<std::string> keysSeen;
    std::vector<std::string> whereClauses, whereValues, columnsSelected, keysSelected;
    BOOST_FOREACH (const std::string &arg, args) {
        size_t eq = arg.find('=');
        if (eq != std::string::npos) {
            // Arguments of the form "key=value" mean restrict the table to that value of the key.  This key column will be
            // emitted above the table instead of within the table (since the column within the table would have one value
            // across all the rows.
            std::string key = boost::trim_copy(arg.substr(0, eq));
            std::string val = boost::trim_copy(arg.substr(eq+1));
            if (!dependencyNames.exists(key)) {
                mlog[FATAL] <<"invalid key \"" <<StringUtility::cEscape(key) <<"\"\n";
                exit(1);
            }
            std::cout <<"  " <<std::left <<std::setw(16) <<key <<" = \"" <<StringUtility::cEscape(val) <<"\"\n";
            whereClauses.push_back(dependencyNames[key] + " = ?");
            whereValues.push_back(val);
            keysSeen.insert(key);
        } else if (arg == "list") {
            std::cout <<"Valid column names are:\n";
            BOOST_FOREACH (const std::string &key, dependencyNames.keys())
                std::cout <<"  " <<key <<"\n";
            exit(0);
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
        BOOST_FOREACH (const DependencyNames::Node &node, dependencyNames.nodes()) {
            if (!keysSeen.exists(node.key())) {
                keysSelected.push_back(node.key());
                columnsSelected.push_back(node.value());
            }
        }
    }
    
    // Build the SQL statement
    std::string sql = "select";
    if (columnsSelected.empty()) {
        sql += " *";
    } else {
        sql += " " + StringUtility::join(", ", columnsSelected) + ", count(*) as subtotal";
    }
    sql += " from test_results";
    if (!whereClauses.empty())
        sql += " where " + StringUtility::join(" and ", whereClauses);
    if (!columnsSelected.empty()) {
        sql += " group by " + StringUtility::join(", ", columnsSelected);
        sql += " order by " + StringUtility::join(", ", columnsSelected);
    }
    SqlDatabase::StatementPtr query = tx->statement(sql);
    for (size_t i=0; i<whereValues.size(); ++i)
        query->bind(i, whereValues[i]);
    if (!columnsSelected.empty()) {
        keysSelected.push_back("totals");
        columnsSelected.push_back("count(*)");
    }
    
    // Run the query and save results so we can compute column sizes.
    std::vector<std::vector<std::string> > table;
    for (SqlDatabase::Statement::iterator row = query->begin(); row != query->end(); ++row) {
        table.push_back(std::vector<std::string>());
        for (size_t i=0; i<columnsSelected.size(); ++i)
            table.back().push_back(row.get<std::string>(i));
    }

    // Compute column widths
    std::vector<size_t> maxWidth(keysSelected.size(), 0);
    for (size_t i=0; i<keysSelected.size(); ++i) {
        maxWidth[i] = std::max(maxWidth[i], keysSelected[i].size());
    }
    for (size_t i=0; i<table.size(); ++i) {
        for (size_t j=0; j<table[i].size(); ++j)
            maxWidth[j] = std::max(maxWidth[j], table[i][j].size());
    }

    // Display the table
    for (size_t i=0; i<table.size(); ++i) {
        if (i % 50 == 0)
            displayTableHeader(keysSelected, maxWidth);
        for (size_t j=0; j<table[i].size(); ++j)
            std::cout <<"| " <<std::left <<std::setw(maxWidth[j]) <<table[i][j] <<" ";
        std::cout <<"|\n";
    }
    if (!table.empty())
        displayHorizontalLine(maxWidth);
}
