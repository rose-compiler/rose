//! [description]
static const char *purpose = "demonstrate a simple disassembler";
static const char *description =
    "This is a tutorial that demonstrates how to write a ROSE binary analysis tool.";
//! [description]

//! [rose include]
// Minimal number of header files for this tool
#include <rose.h>                                       // must be first
#include <Rose/BinaryAnalysis.h>                        // all of Rose::BinaryAnalysis
//! [rose include]

//! [namespaces]
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
//! [namespaces]

//! [mlog decl]
static Sawyer::Message::Facility mlog;
//! [mlog decl]

struct Settings {
    boost::filesystem::path outputFileName = "-";
};

static Sawyer::CommandLine::Parser
buildSwitchParser(Settings &settings) {
    //! [switch defn]
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("output", 'o')
                .argument("filename", anyParser(settings.outputFileName))
                .doc("Write the assembly listing to the specified file. If the @v{filename} is \"-\" then output is send to the "
                     "standard output stream."));
    //! [switch defn]

    //! [switch parser]
    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(tool);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen}");
    parser.errorStream(mlog[FATAL]);
    //! [switch parser]
    return parser;
}

static std::ostream&
outputFile(const Settings &settings) {
    static std::ofstream file;
    if (settings.outputFileName == "-") {
        return std::cout;
    } else if (file.is_open()) {
        return file;
    } else {
        file.open(settings.outputFileName.c_str());
        if (!file) {
            mlog[FATAL] <<"cannot open file " <<settings.outputFileName <<"\n";
            exit(1);
        }
        return file;
    }
}

int
main(int argc, char *argv[]) {
    //! [init]
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("binary analysis tutorial");
    //! [init]

    Settings settings;
    auto parser = buildSwitchParser(settings);

    //! [engine config]
    auto engine = P2::Engine::forge(argc, argv, parser /*in,out*/);
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    //! [engine config]

    //! [arg check]
    if (specimen.empty()) {
        mlog[FATAL] <<"no binary specimen specified; see --help\n";
        exit(1);
    }
    mlog[INFO] <<"using the " <<engine->name() <<" partitioning engine\n";
    //! [arg check]

    //! [partition]
    P2::Partitioner::Ptr partitioner = engine->partition(specimen);
    partitioner->memoryMap()->dump(mlog[INFO]);
    mlog[INFO] <<"architecture is " <<partitioner->architecture()->name() <<"\n";
    //! [partition]

    //! [unparse]
    Rose::BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner->unparser();
    unparser->unparse(outputFile(settings), partitioner, Rose::Progress::instance());
    //! [unparse]
}
