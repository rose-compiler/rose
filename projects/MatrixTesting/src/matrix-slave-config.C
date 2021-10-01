static const char *gPurpose = "query or modify slave settings";
static const char *gDescription =
    "Each argument is either a property name or a property name followed by an equal sign and a value. When the "
    "equal sign is present, the property's value is updated in the database. In either case, a shell statement is "
    "emitted that assigns the property's current (or updated) value to a variable. The variable names are the same "
    "as the property names.  Arguments are processed in the order they occur, but all happen within a single "
    "database transaction.  If no arguments are specified, then all properties are listed.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_getline.h>

#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <boost/regex.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    bool unformatted = false;                           // if set, then don't escape special characters
    bool showingName = true;                            // show variable name and equal sign along with value?
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{property}[=@v{value}]...");

    SwitchGroup tool("Tool-specific switches");
    insertDatabaseSwitch(tool, settings.databaseUri);

    tool.insert(Switch("plain")
                .intrinsicValue(true, settings.unformatted)
                .doc("Don't escape special shell characters in the output."));

    tool.insert(Switch("value-only")
                .intrinsicValue(false, settings.showingName)
                .doc("Omit the property name and equal sign, and show only the value."));

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(tool)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

static void
showProperty(const Settings &settings, const std::string &name, const std::string &value) {
    if (settings.showingName)
        std::cout <<name <<"=";
    if (settings.unformatted) {
        std::cout <<value <<"\n";
    } else {
        std::cout <<StringUtility::bourneEscape(value) <<"\n";
    }
}

static bool
nameExists(DB::Connection db, const std::string &name) {
    return db.stmt("select count(*) from slave_settings where name = ?name")
        .bind("name", name)
        .get<size_t>()
        .orElse(0) != 0;
}

static std::string
readFile(const std::string &fileName) {
    std::ifstream in(fileName.c_str());
    if (!in) {
        mlog[FATAL] <<"cannot read file \"" <<StringUtility::cEscape(fileName) <<"\"\n";
        exit(1);
    }

    std::string retval;
    while (in)
        retval += rose_getline(in);

    return retval;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    size_t nErrors = 0;

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    auto db = DB::Connection::fromUri(settings.databaseUri);

    if (args.empty()) {
        for (auto row: db.stmt("select name, value from slave_settings order by name"))
            showProperty(settings, *row.get<std::string>(0), row.get<std::string>(1).orDefault());

    } else {
        boost::regex noticeRe("NOTICE-(\\d)+");
        boost::smatch noticeFound;

        for (const std::string &arg: args) {
            const size_t eq = arg.find('=');
            const std::string name = std::string::npos == eq ? arg : arg.substr(0, eq);
            std::string value = std::string::npos == eq ? "" : arg.substr(eq+1);

            // If the value starts with "@" followed by a file name, use the contents of the file as the value.
            if (!value.empty() && '@' == value[0] && boost::filesystem::exists(value.substr(1)))
                value = readFile(value.substr(1));

            if (std::string::npos == eq) {
                // This is only a query
                bool found = false;
                for (auto row: db.stmt("select value from slave_settings where name = ?name").bind("name", name)) {
                    ASSERT_forbid(found);
                    showProperty(settings, name, row.get<std::string>(0).orDefault());
                    found = true;
                }
                if (!found) {
                    mlog[ERROR] <<"property \"" <<StringUtility::cEscape(name) <<"\" is not defined\n";
                    ++nErrors;
                }

            } else if (boost::regex_match(name, noticeFound, noticeRe)) {
                // NOTICE-n properties are special because they don't have to exist, and setting them to an empty value deletes
                // them.
                if (value.empty()) {
                    db.stmt("delete from slave_settings where name = ?name")
                        .bind("name", name)
                        .run();
                } else if (nameExists(db, name)) {
                    db.stmt("update slave_settings set value = ?value where name = ?name")
                        .bind("name", name)
                        .bind("value", value)
                        .run();
                } else {
                    db.stmt("insert into slave_settings (name, value) values (?name, ?value)")
                        .bind("name", name)
                        .bind("value", value)
                        .run();
                }
                showProperty(settings, name, value);

            } else if (nameExists(db, name)) {
                db.stmt("update slave_settings set value = ?value where name = ?name")
                    .bind("name", name)
                    .bind("value", value)
                    .run();

            } else {
                mlog[ERROR] <<"property \"" <<StringUtility::cEscape(name) <<"\" is not defined\n";
                ++nErrors;
            }
        }
    }
}
