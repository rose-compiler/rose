#include <rose.h>                                       // must be first
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION

static const char *gPurpose = "query or modify a FLIR database";
static const char *gDescription =
    "This tool lists information contained in a fast library identification and recognition (FLIR)  database, or "
    "modifies it in some way.\n\n"

    "The commands are:"

    "@named{ls}{List the libraries contained in this database.}"

    "@named{ls @v{libhash}}{List the functions belonging to the specified library. Libraries are uniquely identified "
    "by their hashes.}"

    "@named{rm @v{libhash}}{Delete the specified library and all its functions. Libraries are uniquely identified "
    "by their hashes.}";


#include <Rose/BinaryAnalysis/LibraryIdentification.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/FormattedTable.h>

#include <batSupport.h>

#include <boost/format.hpp>

using namespace Sawyer::Message::Common;
using namespace Rose;
using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
using Flir = Rose::BinaryAnalysis::LibraryIdentification;

static Sawyer::Message::Facility mlog;

struct Settings {
    std::string connectUrl;                             // name of database
    Flir::Settings flir;                                // settings for fast library identification and recognition

    Settings() {
        flir.minFunctionInsns = 0;
    }
};

// Parse command line, adjust the settings, and return the positional arguments.
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup generic = Rose::CommandLine::genericSwitches();

    // Tool switches
    SwitchGroup tool("Tool specific switches");
    tool.name("tool");

    tool.insert(Switch("database", 'D')
                .argument("url", anyParser(settings.connectUrl))
                .doc("Name of an existing datatabase into which new library functions are added. See the \"Databases\" section for "
                     "information about how to specify database drivers and names."));

    // Parsing
    Parser parser = Rose::CommandLine::createEmptyParser(gPurpose, gDescription);
    parser.errorStream(mlog[FATAL]);
    parser.with(tool);
    parser.with(Flir::commandLineSwitches(settings.flir));
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{command} @v{arguments}...");
    parser.doc("Databases", Sawyer::Database::Connection::uriDocString());

    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
    if (settings.connectUrl.empty()) {
        mlog[FATAL] <<"a database name must be specified with --database/-D\n";
        exit(1);
    }
    return args;
}

static std::string
timeToLocal(time_t t) {
    struct tm tm;
    std::string tz;
    localtime_r(&t, &tm);
    tz = tm.tm_zone;
    return (boost::format("%04d-%02d-%02d %02d:%02d:%02d %s")
            % (tm.tm_year + 1900) % (tm.tm_mon + 1) % tm.tm_mday
            % tm.tm_hour % tm.tm_min % tm.tm_sec
            % tz).str();
}

int
main(int argc, char *argv[]) {
    // Initialization
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("fast library identification and recognition");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Command-line parsing
    Settings settings;
    std::vector<std::string> args = parseCommandLine(argc, argv, settings);

    // Create the analysis object and connect to the database
    Flir flir;
    flir.settings(settings.flir);
    flir.connect(settings.connectUrl);

    if (args.empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    } else if ("ls" == args[0] || "list" == args[0]) {
        if (args.size() == 1) {
            FormattedTable table;
            table.columnHeader(0, 0, "Library\nHash");
            table.columnHeader(0, 1, "Library\nName");
            table.columnHeader(0, 2, "Library\nVersion");
            table.columnHeader(0, 3, "Library\nArchitecture");
            table.columnHeader(0, 4, "Creation\nTime");
            table.columnHeader(0, 5, "Creation\nVersion");
            int i = 0;
            for (const Flir::Library::Ptr &lib: flir.libraries()) {
                table.insert(i, 0, lib->hash());
                table.insert(i, 1, lib->name());
                table.insert(i, 2, lib->version());
                table.insert(i, 3, lib->architecture());
                table.insert(i, 4, timeToLocal(lib->creationTime()));
                table.insert(i, 5, lib->creationVersion());
                ++i;
            }
            std::cout <<table;
        } else {
            FormattedTable table;
            table.columnHeader(0, 0, "Function\nHash");
            table.columnHeader(0, 1, "Function\nAddress");
            table.columnHeader(0, 2, "Function\nInsns");
            table.columnHeader(0, 3, "Function\nName");
            table.columnHeader(0, 4, "Creation\nTime");
            table.columnHeader(0, 5, "Creation\nVersion");
            table.columnHeader(0, 6, "Library\nHash");
            table.columnHeader(0, 7, "Library\nName");
            int row = 0;
            bool failed = false;
            for (size_t i = 1; i < args.size(); ++i) {
                if (Flir::Library::Ptr lib = flir.library(args[i])) {
                    for (const Flir::Function::Ptr &function: flir.functions(lib)) {
                        table.insert(row, 0, function->hash());
                        table.insert(row, 1, StringUtility::addrToString(function->address()));
                        table.insert(row, 2, function->nInsns());
                        table.insert(row, 3, function->name());
                        table.insert(row, 4, timeToLocal(function->creationTime()));
                        table.insert(row, 5, function->creationVersion());
                        table.insert(row, 6, lib->hash());
                        table.insert(row, 7, lib->name());
                        ++row;
                    }
                } else {
                    mlog[ERROR] <<"no such library with hash " <<args[i] <<"\n";
                    failed = true;
                }
            }
            std::cout <<table;
            if (failed)
                exit(1);
        }
    } else {
        mlog[FATAL] <<"unknown command \"" <<StringUtility::cEscape(args[0]) <<"\"\n";
        exit(1);
    }
}

#else // LibraryIdentification is not enable...

#include <Rose/Diagnostics.h>
#include <cstring>

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Sawyer::Message::Facility mlog;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");

    int exitStatus = 1;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--no-error-if-disabled"))
            exitStatus = 0;
    }

    mlog[Rose::Diagnostics::FATAL]
        <<"fast library identification and recognition (FLIR) is not enabled in this ROSE configuration\n";

    return exitStatus;
}

#endif
