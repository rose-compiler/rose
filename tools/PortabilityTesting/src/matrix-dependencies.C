static const char *gPurpose = "query or adjust test dependencies";
static const char *gDescription =
    "This tool queries or adjusts the dependencies that can be chosen when running a test. Each dependency has a name, value, "
    "and optional description. The dependencies also have Boolean values to indicate whether it is enabled and/or supported. An "
    "enabled dependency is one that can be selected for a test; a supported dependency is one that is officially supported "
    "according to the ROSE documentation.\n\n"

    "The following commands are understood:\n"

    "@named{@prop{programName} [@v{switches}] names}"
    "{Lists the valid configuration names, such as \"compiler\", \"boost\", etc.}"

    "@named{@prop{programName} [@v{switches}] list [@v{name} [@v{value}]]}"
    "{Lists the dependencies for the specified name and value. If the value is omitted, then all dependencies with the "
    "specified name are printed; and if the name is also omitted then all dependencies are printed.}"

    "@named{@prop{programName} [@v{switches}] status [@v{name} [@v{value}]]}"
    "{This is similar to the \"list\" command, except it shows some summary information about test results as well.}"

    "@named{@prop{programName} [@v{switches}] enable @v{name} @v{value} [@v{restriction}...]}"
    "{Enable the specified property and value. However, if one or more @v{restriction} expressions are specified, then they "
    "are added to the restriction list for the specified dependency value instead of setting the \"enabled\" bit."
    "Restrictions are either an operating system name and version separated by a colon (such as \"ubuntu:20.04\") or "
    "a relational expression whose left hand side is some other property name and whose right hand side is a value. "
    "The relational operators are the same as for the @man{rose-matrix-query}{1} command, namley: \"=\" tests for "
    "equality, \"/\" tests for inequality, \"~\" tests for substring, and \"^\" tests for absense of substring. These "
    "characters were chosen because they're friendly to type in command shell interactive sessions without quoting.}"

    "@named{@prop{programName} [@v{switches}] disable @v{name} @v{value} [@v{restriction}...]}"
    "{Disable the specified property and value. However, if one or more @v{restriction} expressions arespecified, then "
    "they are removed from the restricton list for the specified dependency value instead of clearing the \"enabled\" "
    "bit. See the \"enable\" subcommand for a description of the restriction syntax.}"

    "@named{@prop{programName} [@v{switches}] supported @v{name} @v{value}}"
    "{Set the property's \"supported\" bit.}"

    "@named{@prop{programName} [@v{switches}] unsupported @v{name} @v{value}}"
    "{Clear the property's \"supported\" bit.}"

    "@named{@prop{programName} [@v{switches}] comment @v{name} @v{value} @v{comment}}"
    "{Set the comment to the specified value. An empty string for the comment will clear the comment.}"

    "@named{@prop{programName} [@v{switches}] insert @v{name} @v{value}}"
    "{Create a new property.}"

    "@named{@prop{programName} [@v{switches}] delete @v{name} @v{value}}"
    "{Delete the specified dependency from the database.}";


#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>
#include <Sawyer/Message.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

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
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{command} [@v{arguments}...]");

    SwitchGroup sg("Tool-specific switches");
    insertDatabaseSwitch(sg, settings.databaseUri);
    insertOutputFormatSwitch(sg, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::HTML)
                                 .set(Format::YAML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));

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
        case Format::PLAIN:
        case Format::HTML:
        case Format::CSV:
        case Format::SHELL: {
            FormattedTable::CellProperties propTrue;
            propTrue.foreground(Color::HSV_GREEN);
            FormattedTable::CellProperties propFalse;
            propFalse.foreground(Color::HSV_RED);

            FormattedTable table(deps.size(), 5);
            table.format(tableFormat(settings.outputFormat));
            table.columnHeader(0, 0, "Name");
            table.columnHeader(0, 1, "Value");
            table.columnHeader(0, 2, "Enabled");
            table.columnHeader(0, 3, "Supported");
            table.columnHeader(0, 4, "Operating\nSystems");
            table.columnHeader(0, 5, "Restrictions");
            table.columnHeader(0, 6, "Comment");
            for (size_t i = 0; i < deps.size(); ++i) {
                table.insert(i, 0, cEscape(deps[i].name));
                table.insert(i, 1, cEscape(deps[i].value));
                table.insert(i, 2, deps[i].enabled ? "enabled" : "disabled");
                table.cellProperties(i, 2, deps[i].enabled ? propTrue : propFalse);
                table.insert(i, 3, deps[i].supported ? "supported" : "unsupported");
                table.cellProperties(i, 3, deps[i].supported ? propTrue : propFalse);
                table.insert(i, 4, StringUtility::join(", ", deps[i].osNames));
                std::string s;
                for (const Dependency::Constraints::Node &node: deps[i].constraints.nodes()) {
                    if (!s.empty())
                        s += "\n";                      // each referenced dependency list on its own line
                    for (const Dependency::ConstraintRhs &rhs: node.value())
                        s += (s.empty() ? "" : " ") + node.key() + rhs.comparison + rhs.value;
                }
                table.insert(i, 5, s);
                table.insert(i, 6, cEscape(deps[i].comment));
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
    auto stmt = db.stmt("select name, description, column_name, position from dependency_attributes order by name");
    FormattedTable table;
    table.columnHeader(0, 0, "Name");
    table.columnHeader(0, 1, "Description");
    table.columnHeader(0, 2, "SQL Column");
    table.columnHeader(0, 3, "Position");

    for (auto row: stmt) {
        const std::string name = *row.get<std::string>(0);
        const std::string desc = row.get<std::string>(1).orElse("");
        const std::string col = *row.get<std::string>(2);
        const size_t position = *row.get<size_t>(3);

        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- name: " <<StringUtility::yamlEscape(name) <<"\n";
            std::cout <<"  description: " <<StringUtility::yamlEscape(desc) <<"\n";
            std::cout <<"  sql_column: " <<StringUtility::yamlEscape(col) <<"\n";
            std::cout <<"  position: " <<position <<"\n";
        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, name);
            table.insert(i, 1, desc);
            table.insert(i, 2, col);
            table.insert(i, 3, position);
        }
    }

    if (Format::YAML != settings.outputFormat) {
        table.format(tableFormat(settings.outputFormat));
        std::cout <<table;
    }
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
enable(const Settings&, DB::Connection db, const std::string &name, const std::string &value, bool b) {
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

// Add or remove restrictions from a dependency.
static void
enable(const Settings&, DB::Connection db, const std::string &name, const std::string &value, bool insert,
       std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end) {
    auto stmt = db.stmt("select " + dependencyColumns() +
                        " from dependencies"
                        " where name = ?name and value = ?value")
                .bind("name", name)
                .bind("value", value);
    DependencyList deps = loadDependencies(stmt);
    if (deps.empty()) {
        mlog[FATAL] <<"dependency \"" <<StringUtility::cEscape(name) <<"\" \"" <<StringUtility::cEscape(value) <<"\""
                    <<" does not exist\n";
        exit(1);
    }
    ASSERT_require(1 == deps.size());
    Dependency &dep = deps[0];

    boost::regex osRe("[a-zA-Z]\\w*:\\d+(\\.\\d+)*");
    boost::regex relRe("([a-zA-Z]\\w*)([=/~^])(.*)");
    bool changed = false;
    for (/*void*/; begin != end; ++begin) {
        boost::smatch found;
        const std::string arg = boost::trim_copy(*begin);
        if (boost::regex_match(arg, osRe)) {
            if (insert) {
                changed = dep.osNames.insert(arg).second || changed;
            } else {
                if (dep.osNames.erase(arg) > 0) {
                    changed = true;
                } else {
                    mlog[WARN] <<"os constraint \"" <<arg <<"\" does not exist for " <<dep.name <<" = " <<dep.value <<"\n";
                }
            }
        } else if (boost::regex_match(arg, found, relRe)) {
            const std::string otherDepName = found.str(1);
            if (otherDepName == dep.name) {
                mlog[FATAL] <<"dependency " <<dep.name <<" = " <<dep.value
                            <<" cannot be restricted in terms of itself: \"" <<StringUtility::cEscape(arg) <<"\"\n";
                exit(1);
            }
            const std::string relation = found.str(2);
            const std::string value = found.str(3);
            const Dependency::ConstraintRhs rhs(relation, value);
            std::vector<Dependency::ConstraintRhs> &rhSides = dep.constraints.insertMaybeDefault(otherDepName);
            auto found = std::find(rhSides.begin(), rhSides.end(), rhs);
            if (insert) {
                auto otherPos = db.stmt("select position from dependency_attributes where name = ?name")
                                .bind("name", otherDepName)
                                .get<int>();
                if (!otherPos) {
                    mlog[FATAL] <<"dependency \"" <<otherDepName <<"\" does not exist\n"
                                <<"when constraining " <<dep.name <<" = " <<dep.value <<"\n";
                    exit(1);
                }
                auto depPos = db.stmt("select position from dependency_attributes where name = ?name")
                              .bind("name", dep.name)
                              .get<int>();
                ASSERT_require(depPos);
                if (*depPos <= *otherPos) {
                    mlog[FATAL] <<"dependency " <<dep.name <<" = " <<dep.value <<" cannot be constrained by \"" <<arg <<"\"\n"
                                <<"because dependency \"" <<otherDepName <<"\" is selected later"
                                <<" (" <<*otherPos <<" versus " <<*depPos <<")\n";
                    exit(1);
                }

                if (found == rhSides.end()) {
                    rhSides.push_back(rhs);
                    changed = true;
                }
            } else {
                if (found != rhSides.end()) {
                    rhSides.erase(found);
                    changed = true;
                } else {
                    mlog[WARN] <<"constraint " <<arg <<" does not exist for " <<dep.name <<" = " <<dep.value <<"\n";
                }
            }
        }
    }

    if (changed)
        updateDependency(db, dep);
}

// Mark a specific dependency as supported or unsupported
static void
support(const Settings&, DB::Connection db, const std::string &name, const std::string &value, bool b) {
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
comment(const Settings&, DB::Connection db, const std::string &name, const std::string &value,
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
insert(const Settings&, DB::Connection db, const std::string &name, const std::string &value) {
    if (0 != db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[ERROR] <<"dependency already exists\n";
        exit(1);
    } else {
        db.stmt("insert into dependencies (name, value, enabled, supported, comment) "
                " values (?name, ?value, 0, 0, '')")
            .bind("name", name)
            .bind("value", value)
            .run();
    }
}

// Erase a dependency
static void
erase(const Settings&, DB::Connection db, const std::string &name, const std::string &value) {
    if (0 == db.stmt("select count(*) from dependencies where name = ?name and value = ?value")
        .bind("name", name).bind("value", value).get<int>().orElse(0)) {
        mlog[WARN] <<"nothing affected\n";
    } else {
        db.stmt("delete from dependencies where name = ?name and value = ?value")
            .bind("name", name)
            .bind("value", value)
            .run();
    }
}

static void
showStatus(const Settings &settings, DB::Connection db, const std::string &whereClause,
           const std::vector<std::pair<std::string, std::string>> &whereBindings) {
    // Get the current ROSE version
    const std::string roseVersion = db.stmt("select rose from test_results order by reporting_time desc limit 1")
                                    .get<std::string>().orDefault();
    if (roseVersion.empty()) {
        mlog[ERROR] <<"no test results yet\n";
        exit(1);
    }

    // Get rows from the dependencies table
    auto stmt = db.stmt("select name, value, restrictions, comment"
                        " from dependencies" +
                        whereClause +
                        " order by name, value");
    for (auto binding: whereBindings)
        stmt.bind(binding.first, binding.second);

    FormattedTable table;
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "Dependency\nName");
    table.columnHeader(0, 1, "Dependency\nValue");
    table.columnHeader(0, 2, "Restrictions");
    table.columnHeader(0, 3, "Pass");
    table.columnHeader(0, 4, "Fail");
    table.columnHeader(0, 5, "Grade");
    table.columnHeader(0, 6, "Comment");

    Color::Gradient pfColors;
    pfColors.insert(0.0, lighten(Color::HSV_RED, 0.4));
    pfColors.insert(0.7, lighten(Color::HSV_RED, 0.4));
    pfColors.insert(1.0, Color::HSV_GREEN);

    std::string prevName;
    for (auto row: stmt) {
        const std::string name = *row.get<std::string>(0);
        const std::string value = *row.get<std::string>(1);
        const std::string restrictions = row.get<std::string>(2).orElse("");
        const std::string comment = row.get<std::string>(3).orElse("");

        // Number of passed and failed tests
        size_t passFail[2] = {0, 0};
        auto stmt2 = db.stmt("select"
                             "   case when status = 'end' then 0 else 1 end as passed,"
                             "   count(*)"
                             " from test_results"
                             " where rose = ?rose and rmc_" + name + " = ?value"
                             " group by passed")
                     .bind("rose", roseVersion)
                     .bind("value", value);
        for (auto cr: stmt2)
            passFail[*cr.get<size_t>(0)] = *cr.get<size_t>(1);
        const size_t nPassed = passFail[0];
        const size_t nFailed = passFail[1];
        const size_t nTests = nPassed + nFailed;
        const size_t grade = (size_t)::round(100.0 * nPassed / nTests);

        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- name: " <<StringUtility::yamlEscape(name) <<"\n"
                      <<"  value: " <<StringUtility::yamlEscape(value) <<"\n"
                      <<"  restrictions: " <<StringUtility::yamlEscape(restrictions) <<"\n"
                      <<"  passed: " <<nPassed <<"\n"
                      <<"  failed: " <<nFailed <<"\n"
                      <<"  commit: " <<StringUtility::yamlEscape(comment) <<"\n";
            if (nTests > 0)
                std::cout <<"  grade: " <<grade <<"\n";
        } else {
            const size_t i = table.nRows();
            if (name == prevName) {
                table.insert(i, 0, "\"");
                table.cellProperties(i, 0, centered());
            } else {
                table.insert(i, 0, name);
                prevName = name;
            }
            table.insert(i, 1, value);
            table.insert(i, 2, restrictions);
            table.insert(i, 3, nPassed);
            table.insert(i, 4, nFailed);
            if (nTests > 0) {
                table.insert(i, 5, boost::lexical_cast<std::string>(grade) + "%");
                FormattedTable::CellProperties props;
                props.foreground(Color::HSV_BLACK);
                props.background(pfColors((double)nPassed / nTests));
                table.cellProperties(i, 5, props);
            }
            table.insert(i, 6, comment);
        }
    }

    if (Format::YAML != settings.outputFormat)
        std::cout <<table;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

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

    } else if ("status" == args[0]) {
        std::string whereClause = " where enabled > 0";
        std::vector<std::pair<std::string, std::string>> bindings;
        if (args.size() >= 2) {
            whereClause += " and name = ?name";
            bindings.push_back(std::make_pair("name", args[1]));
        }
        if (args.size() >= 3) {
            whereClause += " and value = ?value";
            bindings.push_back(std::make_pair("value", args[2]));
        }
        if (args.size() >= 4)
            incorrectUsage();
        showStatus(settings, db, whereClause, bindings);

    } else if ("enable" == args[0] || "enabled" == args[0] ||
               "disable" == args[0] || "disabled" == args[0]) {
        const bool enabled = "enable" == args[0] || "enabled" == args[0];
        if (args.size() == 3) {
            enable(settings, db, args[1], args[2], enabled);
        } else if (args.size() >= 4) {
            enable(settings, db, args[1], args[2], enabled, args.begin()+3, args.end());
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
