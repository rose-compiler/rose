static const char *gPurpose = "query or modify slave settings";
static const char *gDescription =
    "This command queries or sets global test properties.\n\n"

    "When called with no arguments or a single argument \"list\" it lists all the properties that affect how a slave machine "
    "runs.\n\n"

    "When called with the single argument \"notices\" it shows all announcements that the testing administrator has defined. "
    "These intended for human consumption, not automated slaves.\n\n"

    "Otherwise, each argument is considered to be either a query or a setting. A query is a single key word to be looked up "
    "in the database, and a setting is a key word followed by an equal sign and a value. If the value begins with a \"@\", "
    "then the rest of the value is taken to be a file name and the value becomes the contents of the file. The file must "
    "contain only characters in the Latin-1 (ISO-8859-1) character set.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_getline.h>

#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/regex.hpp>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    Format outputFormat = Format::PLAIN;
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database
    bool valueOnly = false;                             // in Format::SHELL mode, print only the value
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);

    parser.doc("Synopsis",
               "@prop{programName} [list]\n\n"
               "@prop{programName} notices\n\n"
               "@prop{programName} [@v{switches}] @v{property}[=@v{value}]...\n\n");

    SwitchGroup tool("Tool-specific switches");
    insertDatabaseSwitch(tool, settings.databaseUri);
    insertOutputFormatSwitch(tool, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::YAML)
                                 .set(Format::HTML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));
    Rose::CommandLine::insertBooleanSwitch(tool, "value-only", settings.valueOnly,
                                           "Adjusts the @s{format}=shell formatting so only the value is printed. This "
                                           "allows this tool to be used in shell scripts that say things like: "
                                           "os=$(@prop{programName} -Fshell --value-only TEST_OS");

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(tool)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
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

static void
showNotices(const Settings &settings, DB::Connection db) {
    FormattedTable table;
    table.format(tableFormat(settings.outputFormat));

    const boost::regex dateRe("NOTICE_(\\d{4})(\\d\\d)(\\d\\d)");
    auto stmt = db.stmt("select name, value from slave_settings where name like 'NOTICE_%' order by name");
    for (auto row: stmt) {
        const std::string name = row.get<std::string>(0).orDefault();
        const std::string value = row.get<std::string>(1).orDefault();

        boost::smatch found;
        std::string shortName = name;
        if (boost::regex_match(name, found, dateRe)) {
            shortName = found.str(1) + "-" + found.str(2) + "-" + found.str(3);
        } else {
            ASSERT_require(boost::starts_with(name, "NOTICE_"));
            ASSERT_require(name.size() > 7);
            shortName = name.substr(7);
        }

        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- name: " <<StringUtility::yamlEscape(name) <<"\n"
                      <<"  value: " <<StringUtility::yamlEscape(value) <<"\n";
        } else if (Format::SHELL == settings.outputFormat) {
            if (settings.valueOnly) {
                std::cout <<value <<"\n";
            } else {
                std::cout <<name <<"=" <<StringUtility::bourneEscape(value) <<"\n";
            }
        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, shortName);
            table.insert(i, 1, value);
        }
    }

    if (Format::YAML != settings.outputFormat && Format::SHELL != settings.outputFormat)
        std::cout <<table;
}

static void
showSlaveConfig(const Settings &settings, DB::Connection db) {
    // Load all the properties up front because we want to show them in a particular order
    Sawyer::Container::Map<std::string, std::string> props;
    auto stmt = db.stmt("select name, value from slave_settings");
    for (auto row: db.stmt("select name, value from slave_settings where name not like 'NOTICE%'")) {
        const std::string name = row.get<std::string>(0).orDefault();
        const std::string value = row.get<std::string>(1).orDefault();
        if (Format::SHELL == settings.outputFormat) {
            if (settings.valueOnly) {
                std::cout <<value <<"\n";
            } else {
                std::cout <<name <<"=" <<StringUtility::bourneEscape(value) <<"\n";
            }
        } else if (Format::YAML == settings.outputFormat) {
            std::cout <<(props.isEmpty() ? "- " : "  ") <<name <<": " <<StringUtility::yamlEscape(value) <<"\n";
            props.insert(name, value);
        } else {
            props.insert(name, value);
        }
    }

    if (Format::SHELL != settings.outputFormat && Format::YAML != settings.outputFormat) {
        FormattedTable table;
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
}

static bool
showSingleValue(const Settings &settings, DB::Connection db, const std::string &name, FormattedTable &table) {
    bool found = false;
    auto stmt = db.stmt("select value from slave_settings where name = ?name").bind("name", name);
    for (auto row: stmt) {
        ASSERT_forbid(found);                           // duplicate entry
        found = true;
        const std::string value = row.get<std::string>(0).orDefault();

        if (Format::SHELL == settings.outputFormat) {
            if (settings.valueOnly) {
                std::cout <<value <<"\n";
            } else {
                std::cout <<name <<"=" <<StringUtility::bourneEscape(value) <<"\n";
            }

        } else if (Format::YAML == settings.outputFormat) {
            std::cout <<"- " <<name <<": " <<StringUtility::yamlEscape(value) <<"\n";

        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, name);
            table.insert(i, 1, value);
        }
    }

    if (!found)
        mlog[ERROR] <<"property \"" <<StringUtility::cEscape(name) <<"\" is not defined\n";

    return found;
}

// NOTICE_n properties are special because they don't have to exist, and setting them to an empty value deletes them.
static bool
setNotice(DB::Connection db, const std::string &givenName, const std::string &value) {
    const boost::regex noticeRe1("(?:NOTICE|notice)[-_](\\d{8})");                   // YYYYMMDD
    const boost::regex noticeRe2("(?:NOTICE|notice)[-_](\\d{4})-(\\d\\d)-(\\d\\d)"); // YYYY-MM-DD

    std::string name;
    boost::smatch found;
    if (boost::regex_match(givenName, found, noticeRe1)) {
        name = "NOTICE_" + found.str(1);
    } else if (boost::regex_match(givenName, found, noticeRe2)) {
        name = "NOTICE_" + found.str(1) + found.str(2) + found.str(3);
    } else {
        mlog[ERROR] <<"notice \"" <<StringUtility::cEscape(givenName) <<"\" is spelled wrong; see --help\n";
        return false;
    }

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
    return true;
}

static bool
setValue(DB::Connection db, const std::string &name, const std::string &value) {
    if (nameExists(db, name)) {
        db.stmt("update slave_settings set value = ?value where name = ?name")
            .bind("name", name)
            .bind("value", value)
            .run();

    } else {
        mlog[ERROR] <<"property \"" <<StringUtility::cEscape(name) <<"\" is not defined\n";
        return false;
    }
    return true;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    size_t nErrors = 0;

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    if (args.empty()) {
        showSlaveConfig(settings, db);

    } else if (args.size() == 1 && "notices" == args[0]) {
        showNotices(settings, db);

    } else {
        FormattedTable valuesToShow;
        valuesToShow.format(tableFormat(settings.outputFormat));
        valuesToShow.columnHeader(0, 0, "Key");
        valuesToShow.columnHeader(0, 1, "Value");

        for (const std::string &arg: args) {
            const size_t eq = arg.find('=');
            const std::string name = std::string::npos == eq ? arg : arg.substr(0, eq);
            std::string value = std::string::npos == eq ? "" : arg.substr(eq+1);

            // If the value starts with "@" followed by a file name, use the contents of the file as the value.
            if (!value.empty() && '@' == value[0] && boost::filesystem::exists(value.substr(1)))
                value = readFile(value.substr(1));

            if (std::string::npos == eq) {
                if (!showSingleValue(settings, db, name, valuesToShow))
                    ++nErrors;

            } else if (boost::starts_with(arg, "NOTICE") || boost::starts_with(arg, "notice")) {
                if (!setNotice(db, name, value))
                    ++nErrors;

            } else {
                if (!setValue(db, name, value))
                    ++nErrors;
            }
        }

        if (valuesToShow.nRows() > 0)
            std::cout <<valuesToShow;
    }

    return nErrors > 0 ? 1 : 0;
}
