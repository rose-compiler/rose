static const char *gPurpose = "show information about test phases";
static const char *gDescription =
    "Queries a database to show information about the phases that are defined for a test. Phases execute one at a time "
    "in a well defined sequence until a phase fails or the final phase of the sequence is executed. A test is considered "
    "to have passed if and only if its final phase passes. The final phase is always named \"end\". The last phase that "
    "was executed becomes the \"status\" of the test.";

#include <rose.h>
#include "matrixTools.h"

#include <Rose/CommandLine.h>
#include <Rose/FormattedTable.h>
#include <Sawyer/Database.h>

using namespace Rose;
using namespace Sawyer::Message::Common;
namespace DB = Sawyer::Database;

struct Settings {
    std::string databaseUri;
    Format outputFormat = Format::PLAIN;
};

static Sawyer::Message::Facility mlog;

static void
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}]");

    SwitchGroup sg("Tool-specific switches");

    insertDatabaseSwitch(sg, settings.databaseUri);
    insertOutputFormatSwitch(sg, settings.outputFormat,
                             FormatFlags()
                                 .set(Format::PLAIN)
                                 .set(Format::YAML)
                                 .set(Format::HTML)
                                 .set(Format::CSV)
                                 .set(Format::SHELL));

    std::vector<std::string> args = parser
                                    .with(Rose::CommandLine::genericSwitches())
                                    .with(sg)
                                    .parse(argc, argv)
                                    .apply()
                                    .unreachedArgs();

    if (!args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    parseCommandLine(argc, argv, settings);
    DB::Connection db = connectToDatabase(settings.databaseUri, mlog);

    FormattedTable table;
    table.format(tableFormat(settings.outputFormat));
    table.columnHeader(0, 0, "Phase");
    table.columnHeader(0, 1, "Description");

    auto stmt = db.stmt("select name, purpose from test_names where purpose is not null order by position");
    for (auto row: stmt) {
        const std::string name = *row.get<std::string>(0);
        const std::string desc = *row.get<std::string>(1);
        if (Format::YAML == settings.outputFormat) {
            std::cout <<"- " <<name <<": " <<StringUtility::yamlEscape(desc) <<"\n";
        } else {
            const size_t i = table.nRows();
            table.insert(i, 0, name);
            table.insert(i, 1, desc);
        }
    }

    if (Format::YAML != settings.outputFormat)
        std::cout <<table;
}
