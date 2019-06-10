static const char *purpose = "query or modify slave settings";
static const char *description =
    "Each argument is either a property name or a property name followed by an equal sign and a value. When the "
    "equal sign is present, the property's value is updated in the database. In either case, a shell statement is "
    "emitted that assigns the property's current (or updated) value to a variable. The variable names are the same "
    "as the property names.  Arguments are processed in the order they occur, but all happen within a single "
    "database transaction.  If no arguments are specified, then all properties are listed.";

#include <rose.h>

#include <CommandLine.h>                                // rose
#include <SqlDatabase.h>                                // rose

#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

struct Settings {
    bool unformatted;                                   // if set, then don't escape special characters
    bool showingName;                                   // show variable name and equal sign along with value?
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
        : unformatted(false), showingName(true)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{property}[=@v{value}]...");

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("database", 'd')
                .argument("uri", anyParser(settings.databaseUri))
                .doc("Uniform resource locator for the database. This switch overrides the ROSE_MATRIX_DATABASE environment "
                     "variable. " + SqlDatabase::uriDocumentation()));

    tool.insert(Switch("plain")
                .intrinsicValue(true, settings.unformatted)
                .doc("Don't escape special shell characters in the output."));

    tool.insert(Switch("value-only")
                .intrinsicValue(false, settings.showingName)
                .doc("Omit the property name and equal sign, and show only the value."));

    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(tool);
    return parser.parse(argc, argv).apply().unreachedArgs();
}

static std::string
shellEscape(const std::string &s) {
    std::string escaped;
    bool needQuote = false;
    BOOST_FOREACH (char ch, s) {
        if ('\'' == ch) {
            escaped += "'\"'\"'";
            needQuote = true;
        } else if (!isalnum(ch) && !strchr("_-+=:,./", ch)) {
            escaped += ch;
            needQuote = true;
        } else {
            escaped += ch;
        }
    }
    if (needQuote)
        escaped = "'" + escaped + "'";
    return escaped;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    size_t nErrors = 0;

    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);

    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what() <<"\n";
        exit(1);
    }
        
    if (args.empty()) {
        // list all properties and their values
        SqlDatabase::StatementPtr stmt = tx->statement("select name, value from slave_settings order by name");
        for (SqlDatabase::Statement::iterator row = stmt->begin(); row != stmt->end(); ++row) {
            if (settings.showingName)
                std::cout <<row.get_str(0) <<"=";
            if (settings.unformatted) {
                std::cout <<row.get_str(1) <<"\n";
            } else {
                std::cout <<shellEscape(row.get_str(1)) <<"\n";
            }
        }
    } else {
        BOOST_FOREACH (const std::string &arg, args) {
            size_t eq = arg.find('=');
            std::string property = std::string::npos == eq ? arg : arg.substr(0, eq);
            std::string value;

            // Get the property
            SqlDatabase::StatementPtr stmt =
                tx->statement("select value from slave_settings where name = ?")->bind(0, property);
            SqlDatabase::Statement::iterator row = stmt->begin();
            if (row == stmt->end()) {
                mlog[ERROR] <<"property \"" <<StringUtility::cEscape(args[0]) <<"\" is not defined\n";
                ++nErrors;
            } else {
                if (eq != std::string::npos) {
                    value = arg.substr(eq+1);
                    tx->statement("update slave_settings set value = ? where name = ?")
                        ->bind(0, value)
                        ->bind(1, property)
                        ->execute();
                } else {
                    value = row.get_str(0);
                }

                if (settings.showingName)
                    std::cout <<property <<"=";
                if (settings.unformatted) {
                    std::cout <<value <<"\n";
                } else {
                    std::cout <<shellEscape(value) <<"\n";
                }
            }
        }
    }

    tx->commit();
}
