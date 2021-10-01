static const char *gPurpose = "query or adjust test dependencies";
static const char *gDescription =
    "This tool queries or adjusts the dependencies that can be chosen when running a test. Each dependency has a name, value, "
    "and optional description. The dependencies also have Boolean values to indicate whether it is enabled and/or supported. An "
    "enabled dependency is one that can be selected for a test; a supported dependency is one that is officially supported "
    "according to the ROSE documentation.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    std::string databaseUri;
    Format outputFormat = Format::PLAIN;
};

static Sawyer::Message::Facility mlog;

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis",
               "@prop{programName} [@v{switches}] names\n\n"
               "@prop{programName} [@v{switches}] list        [@v{name} [@v{value}]]\n\n"
               "@prop{programName} [@v{switches}] enable      @v{name} @v{value}\n\n"
               "@prop{programName} [@v{switches}] disable     @v{name} @v{value}\n\n"
               "@prop{programName} [@v{switches}] supported   @v{name} @v{value}\n\n"
               "@prop{programName} [@v{switches}] unsupported @v{name} @v{value}\n\n"
               "@prop{programName} [@v{switches}] comment     @v{name} @v{value} @v{comment}\n\n"
               "@prop{programName} [@v{switches}] insert      @v{name} @v{value}\n\n"
               "@prop{programName} [@v{switches}] delete      @v{name} @v{value}\n\n");


    SwitchGroup sg("Tool-specific switches");
    insertDatabaseSwitch(sg, settings.databaseUri);
    insertOutputFormatSwitch(sg, settings.outputFormat);

    return parser
        .with(Rose::CommandLine::genericSwitches())
        .with(sg)
        .parse(argc, argv)
        .apply()
        .unreachedArgs();
}

static void
incorrectUsage() {
    mlog[FATAL] <<"incorrect usage; see --help\n";
    exit(1);
}

static void
print(const Settings &settings, const DependencyList &deps) {
    using namespace Rose::StringUtility;
    switch (settings.outputFormat) {
        case Format::PLAIN: {
            FormattedTable::CellProperties propTrue;
            propTrue.foreground(Color::HSV_GREEN);
            FormattedTable::CellProperties propFalse;
            propFalse.foreground(Color::HSV_RED);

            FormattedTable table(deps.size(), 5);
            table.columnHeader(0, 0, "Name");
            table.columnHeader(0, 1, "Value");
            table.columnHeader(0, 2, "Enabled");
            table.columnHeader(0, 3, "Supported");
            table.columnHeader(0, 4, "Comment");
            for (size_t i = 0; i < deps.size(); ++i) {
                table.insert(i, 0, cEscape(deps[i].name));
                table.insert(i, 1, cEscape(deps[i].value));
                table.insert(i, 2, deps[i].enabled ? "enabled" : "disabled");
                table.cellProperties(i, 2, deps[i].enabled ? propTrue : propFalse);
                table.insert(i, 3, deps[i].supported ? "supported" : "unsupported");
                table.cellProperties(i, 3, deps[i].supported ? propTrue : propFalse);
                table.insert(i, 4, cEscape(deps[i].comment));
            }
            std::cout <<table;
            return;
        }

        case Format::YAML: {
            for (const Dependency &dep: deps) {
                std::cout <<"- name:      " <<yamlEscape(dep.name) <<"\n";
                std::cout <<"  value:     " <<yamlEscape(dep.value) <<"\n";
                std::cout <<"  enabled:   " <<(dep.enabled ? "yes" : "no") <<"\n";
                std::cout <<"  supported: " <<(dep.supported ? "yes" : "no") <<"\n";
                std::cout <<"  comment:   " <<yamlEscape(dep.comment) <<"\n";
            }
            return;
        }
    }
    ASSERT_not_reachable("invalid output format");
}

// Show only the names of dependencies
static void
listNames(const Settings &settings, DB::Connection db) {
    std::vector<std::string> names;
    for (auto row: db.stmt("select distinct name from dependencies order by name"))
        names.push_back(*row.get<std::string>(0));
    switch (settings.outputFormat) {
        case Format::PLAIN: {
            FormattedTable table(names.size(), 1);
            table.columnHeader(0, 0, "Name");
            for (size_t i = 0; i < names.size(); ++i)
                table.insert(i, 0, names[i]);
            std::cout <<table;
            return;
        }

        case Format::YAML:
            for (const std::string &name: names)
                std::cout <<"- " <<StringUtility::yamlEscape(name) <<"\n";
            return;
    }
    ASSERT_not_reachable("invalid output format");
}

// List all dependencies
static void
list(const Settings &settings, DB::Connection db) {
    auto stmt = db.stmt("select " + dependencyColumns() + " from dependencies order by name, value");
    print(settings, loadDependencies(stmt));
}

// List dependencies having the specified name
static void
list(const Settings &settings, DB::Connection db, const std::string &name) {
    auto stmt = db.stmt("select " + dependencyColumns() +
                        " from dependencies"
                        " where name = ?name"
                        " order by name, value")
                .bind("name", name);
    print(settings, loadDependencies(stmt));
}

// List dependencies having the specified name and value
static void
list(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value) {
    auto stmt = db.stmt("select " + dependencyColumns() +
                        " from dependencies"
                        " where name = ?name and value = ?value"
                        " order by name, value")
                .bind("name", name)
                .bind("value", value);
    print(settings, loadDependencies(stmt));
}

// Enable or disable the dependency with the specified name and value
static void
enable(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value, bool b) {
    if (0 == db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[WARN] <<"nothing affected\n";
    } else {
        db.stmt("update dependencies set enabled = ?enabled where name = ?name and value = ?value")
            .bind("name", name)
            .bind("value", value)
            .bind("enabled", b)
            .run();
    }
}

// Mark a specific dependency as supported or unsupported
static void
support(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value, bool b) {
    if (0 == db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[WARN] <<"nothing affected\n";
    } else {
        db.stmt("update dependencies set supported = ?supported where name = ?name and value = ?value")
            .bind("name", name)
            .bind("value", value)
            .bind("supported", b)
            .run();
    }
}

// Change the comment for a particular dependency
static void
comment(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value,
        const std::string &s) {
    if (0 == db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[WARN] <<"nothing affected\n";
    } else {
        db.stmt("update dependencies set comment = ?comment where name = ?name and value = ?value")
            .bind("name", name)
            .bind("value", value)
            .bind("comment", s)
            .run();
    }
}

// Insert a new dependency
static void
insert(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value) {
    if (0 != db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[ERROR] <<"dependency already exists\n";
        exit(1);
    } else {
        db.stmt("insert into dependencies (name, value, enabled, supported, comment) "
                " values (?name, ?value, true, true, ''")
            .bind("name", name)
            .bind("value", value)
            .run();
    }
}

// Erase a dependency
static void
erase(const Settings &settings, DB::Connection db, const std::string &name, const std::string &value) {
    if (0 == db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[WARN] <<"nothing affected\n";
    } else {
        db.stmt("delete dependencies where = ?name and value = ?value")
            .bind("name", name)
            .bind("value", value)
            .run();
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    auto db = DB::Connection::fromUri(settings.databaseUri);

    if (args.empty()) {
        incorrectUsage();

    } else if ("names" == args[0]) {
        if (args.size() == 1) {
            listNames(settings, db);
        } else {
            incorrectUsage();
        }

    } else if ("list" == args[0]) {
        if (args.size() == 1) {
            list(settings, db);
        } else if (args.size() == 2) {
            list(settings, db, args[1]);
        } else if (args.size() == 3) {
            list(settings, db, args[1], args[2]);
        } else {
            incorrectUsage();
        }

    } else if ("enable" == args[0] || "enabled" == args[0] ||
               "disable" == args[0] || "disabled" == args[0]) {
        const bool enabled = "enable" == args[0] || "enabled" == args[0];
        if (args.size() == 3) {
            enable(settings, db, args[1], args[2], enabled);
        } else {
            incorrectUsage();
        }

    } else if ("support" == args[0] || "supported" == args[0] ||
               "unsupport" == args[0] || "unsupported" == args[0]) {
        const bool supported = "support" == args[0] || "supported" == args[0];
        if (args.size() == 3) {
            support(settings, db, args[1], args[2], supported);
        } else {
            incorrectUsage();
        }

    } else if ("comment" == args[0]) {
        if (args.size() == 4) {
            comment(settings, db, args[1], args[2], args[3]);
        } else {
            incorrectUsage();
        }

    } else if ("insert" == args[0]) {
        if (args.size() == 3) {
            insert(settings, db, args[1], args[2]);
        } else {
            incorrectUsage();
        }

    } else if ("delete" == args[0]) {
        if (args.size() == 3) {
            erase(settings, db, args[1], args[2]);
        } else {
            incorrectUsage();
        }


    } else {
        incorrectUsage();
    }
}
