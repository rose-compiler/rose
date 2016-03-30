#include <rose.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <cstring>
#include <LinearCongruentialGenerator.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <SqlDatabase.h>

using namespace Sawyer::Message::Common;

enum OutputMode {
    OUTPUT_SHELL,                                       // shell script variables "boost=1.59 build=autoconf compiler=gcc-4.8"
    OUTPUT_HUMAN,                                       // one key-value pair per line of output for human consumption
    OUTPUT_RMC,                                         // output RMC specification
    OUTPUT_OVERRIDES                                    // output entire space as "OVERRIDE_*='VALUES...'"
};

struct Settings {
    OutputMode outputMode;                              // what kind of output to produce
    std::string databaseUri;                            // e.g., postgresql://user:password@host/database

    Settings()
        : outputMode(OUTPUT_HUMAN)
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
              .doc("Uniform resource locator for the database." + SqlDatabase::uriDocumentation()));

    sg.insert(Switch("format", 'f')
              .argument("style", enumParser<OutputMode>(settings.outputMode)
                        ->with("human", OUTPUT_HUMAN)
                        ->with("rmc", OUTPUT_RMC)
                        ->with("shell", OUTPUT_SHELL)
                        ->with("overrides", OUTPUT_OVERRIDES))
              .doc("Style of output. The possibilities are:"
                   " @named{human}{Output in a human-friendly format. This is the default.}"
                   " @named{rmc}{Output an RMC specification.}"
                   " @named{shell}{Output one line of space-separated key=value pairs.}"
                   " @named{overrides}{Output entire configuration space as shell OVERRIDE variables.}"));

    if (!parser.with(CommandlineProcessing::genericSwitches()).with(sg).parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"invalid usage; see --help\n";
        exit(1);
    }
}

typedef Sawyer::Container::Map<std::string /*dependency*/, std::vector<std::string>/*versions*/> Dependencies;

// Load all dependendencies from the database
static Dependencies
loadAllDependencies(const SqlDatabase::TransactionPtr &tx) {
    Dependencies dependencies;
    SqlDatabase::StatementPtr q = tx->statement("select name, value from dependencies where enabled <> 0");
    for (SqlDatabase::Statement::iterator row = q->begin(); row != q->end(); ++row)
        dependencies.insertMaybeDefault(row.get<std::string>(0)).push_back(row.get<std::string>(1));
    return dependencies;
}

// Load all dependencies from a file. Each line of the file should be a line like what's output by the "--format=overrides"
// switch.
static Dependencies
loadAllDependencies(const std::string &fileName) {
    Dependencies dependencies;
    char buf[4096];
    unsigned lineNum = 0;
    boost::regex varValRe("OVERRIDE_([A-Z_]+)='?(.*?)'?");
    std::ifstream in(fileName.c_str());
    while (1) {
        in.getline(buf, sizeof buf);
        if (in.eof())
            break;
        if (!in.good()) {
            mlog[FATAL] <<fileName <<":" <<lineNum <<": read failed\n";
            exit(1);
        }

        ++lineNum;
        std::string s = buf;
        boost::smatch captures;
        if (!boost::regex_match(s, captures, varValRe)) {
            mlog[FATAL] <<fileName <<":" <<lineNum <<": invalid dependency info: \"" <<StringUtility::cEscape(buf) <<"\"\n";
            exit(1);
        }
        std::string name = boost::to_lower_copy(captures.str(1));
        dependencies.insert(name, StringUtility::split(" ", captures.str(2)));
    }
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
listEntireSpace(const Dependencies &dependencies) {
    BOOST_FOREACH (const Dependencies::Node &node, dependencies.nodes())
        std::cout <<"OVERRIDE_" <<boost::to_upper_copy(node.key()) <<"=" <<shellEscape(boost::join(node.value(), " ")) <<"\n";
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
            default:
                ASSERT_not_reachable("output format not handled");
        }
    }
    if (settings.outputMode == OUTPUT_SHELL)
        std::cout <<"\n";
}

int
main(int argc, char *argv[]) {
    Sawyer::initializeLibrary();
    mlog = Sawyer::Message::Facility("tool");
    Sawyer::Message::mfacilities.insertAndAdjust(mlog);

    Settings settings;
    parseCommandLine(argc, argv, settings);

    Dependencies dependencies;
    if (boost::starts_with(settings.databaseUri, "file://")) {
        std::string fileName = settings.databaseUri.substr(7);
        dependencies = loadAllDependencies(fileName);
    } else {
        SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(settings.databaseUri)->transaction();
        dependencies = loadAllDependencies(tx);
    }

    if (OUTPUT_OVERRIDES == settings.outputMode) {
        listEntireSpace(dependencies);
    } else {
        showRandomPoint(settings, dependencies);
    }
}
