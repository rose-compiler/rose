#include <rose.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <CommandLine.h>
#include <cstring>
#include <LinearCongruentialGenerator.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

enum OutputMode {
    OUTPUT_SHELL,                                       // shell script variables "boost=1.59 build=autoconf compiler=gcc-4.8"
    OUTPUT_HUMAN,                                       // one key-value pair per line of output for human consumption
    OUTPUT_RMC                                          // output RMC specification
};

struct Settings {
    OutputMode outputMode;                              // what kind of output to produce
    bool onlySupported;                                 // select only supported values
    bool listing;                                       // show all values
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
        : outputMode(OUTPUT_HUMAN), onlySupported(false), listing(false)
#ifdef DEFAULT_DATABASE
          , databaseUri(DEFAULT_DATABASE)
#endif
        {}
};

static Sawyer::Message::Facility mlog;

static void
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;

    parser.purpose("select random ROSE configuration");
    parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    parser.chapter(1, "ROSE Command-line Tools");
    parser.doc("Description", "Select a random configuration from all available configurations.");
    parser.doc("Synopsis", "@prop{programName} [@v{switches}]");
    parser.errorStream(mlog[FATAL]);

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("database", 'd')
              .argument("uri", anyParser(settings.databaseUri))
              .doc("Uniform resource locator for the database. This switch overrides the ROSE_MATRIX_DATABASE environment "
                   "variable. " + SqlDatabase::uriDocumentation()));

    sg.insert(Switch("format", 'f')
              .argument("style", enumParser<OutputMode>(settings.outputMode)
                        ->with("human", OUTPUT_HUMAN)
                        ->with("rmc", OUTPUT_RMC)
                        ->with("shell", OUTPUT_SHELL))
              .doc("Style of output. The possibilities are:"
                   " @named{human}{Output in a human-friendly format. This is the default.}"
                   " @named{rmc}{Output an RMC specification.}"
                   " @named{shell}{Output one line of space-separated key=value pairs.}"));

    sg.insert(Switch("only-supported")
              .intrinsicValue(true, settings.onlySupported)
              .doc("Resulting configuration will contain only supported values."));

    Rose::CommandLine::insertBooleanSwitch(sg, "list", settings.listing,
                                           "Instead of showing single values, show a space-separated list of all values");

    if (!parser.with(Rose::CommandLine::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"invalid usage; see --help\n";
        exit(1);
    }
}

typedef Sawyer::Container::Map<std::string /*dependency*/, std::vector<std::string>/*versions*/> Dependencies;

// Load all dependendencies from the database
static Dependencies
loadAllDependencies(const SqlDatabase::TransactionPtr &tx, const Settings &settings) {
    Dependencies dependencies;
    SqlDatabase::StatementPtr q = tx->statement("select name, value from dependencies where enabled <> 0" +
                                                std::string(settings.onlySupported?" and supported > 0":""));
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
        dependencies.insertMaybeDefault(row.get<std::string>(0)).push_back(row.get<std::string>(1));
    return dependencies;
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

static void
listEntireSpace(const Settings &settings, const Dependencies &dependencies) {
    BOOST_FOREACH (const Dependencies::Node &node, dependencies.nodes()) {
        switch (settings.outputMode) {
            case OUTPUT_HUMAN:
                std::cout <<std::left <<std::setw(16) <<node.key() <<":";
                BOOST_FOREACH (const std::string &value, node.value())
                    std::cout <<" " <<value;
                std::cout <<"\n";
                break;
            case OUTPUT_RMC:
                std::cout <<std::left <<std::setw(20) <<("rmc_"+node.key());
                BOOST_FOREACH (const std::string &value, node.value())
                    std::cout <<" " <<shellEscape(value);
                std::cout <<"\n";
                break;
            case OUTPUT_SHELL:
                std::cout <<std::left <<node.key() <<"=(";
                for (size_t i=0; i<node.value().size(); ++i)
                    std::cout <<(i?" ":"") <<shellEscape(node.value()[i]);
                std::cout <<")\n";
                break;
        }
    }
}

static void
showRandomPoint(const Settings &settings, const Dependencies &dependencies) {
    LinearCongruentialGenerator prand;
    BOOST_FOREACH (const Dependencies::Node &node, dependencies.nodes()) {
        size_t idx = prand() % node.value().size();
        switch (settings.outputMode) {
            case OUTPUT_HUMAN:
                std::cout <<std::left <<std::setw(16) <<node.key() <<": " <<node.value()[idx] <<"\n";
                break;
            case OUTPUT_RMC:
                std::cout <<std::left <<std::setw(20) <<("rmc_"+node.key()) <<" " <<shellEscape(node.value()[idx]) <<"\n";
                break;
            case OUTPUT_SHELL:
                std::cout <<" " <<node.key() <<"=" <<shellEscape(node.value()[idx]);
                break;
        }
    }
    if (settings.outputMode == OUTPUT_SHELL)
        std::cout <<"\n";
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    if (const char *dbUri = getenv("ROSE_MATRIX_DATABASE"))
        settings.databaseUri = dbUri;
    parseCommandLine(argc, argv, settings);

    SqlDatabase::TransactionPtr tx;
    try {
        tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
    } catch (const SqlDatabase::Exception &e) {
        mlog[FATAL] <<"cannot open database: " <<e.what() <<"\n";
        exit(1);
    }
        
    Dependencies dependencies = loadAllDependencies(tx, settings);

    if (settings.listing) {
        listEntireSpace(settings, dependencies);
    } else {
        showRandomPoint(settings, dependencies);
    }
}
