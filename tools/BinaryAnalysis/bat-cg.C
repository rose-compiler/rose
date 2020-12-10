static const char *purpose = "emits a function call graph";
static const char *description =
    "Given a BAT state for a binary specimen, generate a GraphViz or text function call graph on standard output. The BAT "
    "state file is a file created by another BAT tool, such as bat-ana. If the state file name is \"-\" (a single hyphen) "
    "or not specified then the state is read from standard input.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Diagnostics.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose
#include <Partitioner2/GraphViz.h>                      // rose
#include <Partitioner2/Partitioner.h>                   // rose
#include <rose_strtoull.h>                              // rose

#include <batSupport.h>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Stopwatch.h>
#include <string>
#include <vector>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

enum OutputFormat { FORMAT_GRAPHVIZ, FORMAT_TEXT, FORMAT_GEXF };

struct Settings {
    OutputFormat outputFormat;                          // style of output
    bool inliningImports;                               // inline imported functions when creating GraphViz output
    std::set<std::string> functionNames;                // restrict output to these function names, addresses,...
    std::set<rose_addr_t> addresses;                    // ...and/or these functions
    SerialIo::Format stateFormat;

    Settings()
        : outputFormat(FORMAT_TEXT), inliningImports(false), stateFormat(SerialIo::BINARY) {}
};

Sawyer::Message::Facility mlog;

// Parses the command-line and returns the name of the input file, if any.
boost::filesystem::path
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup cg("Function call graph switches");
    cg.name("cg");

    cg.insert(Switch("format")
              .argument("m", enumParser(settings.outputFormat)
                        ->with("gv", FORMAT_GRAPHVIZ)
                        ->with("gexf", FORMAT_GEXF)
                        ->with("text", FORMAT_TEXT))
              .doc("Determines which format of output to produce. The choices are:"
                   "@named{gv}{Produce a GraphViz file.}"
                   "@named{gexf}{Produce a Graph Exchange XML Format file.}"
                   "@named{text}{Produce a text file.}"
                   "The default is \"" + std::string(FORMAT_GRAPHVIZ==settings.outputFormat?"gv":"text") + "\"."));

    cg.insert(Switch("inline-imports")
              .intrinsicValue(true, settings.inliningImports)
              .doc("For GraphViz output, inline imports into their callers and display the names of inlined functions in the "
                   "vertex.  This sometimes makes the output much cleaner.  Imported functions are identified by their names "
                   "only: any name ending with \".dll\" or \"@@plt\" is considered an imported function. This feature is "
                   "disabled with the @s{no-inline-imports} switch.  The default is to " +
                   std::string(settings.inliningImports?"":"not ") + "perform this inlining."));
    cg.insert(Switch("no-inline-imports")
              .key("inline-imports")
              .intrinsicValue(false, settings.inliningImports)
              .hidden(true));

    cg.insert(Switch("function")
              .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
              .explosiveLists(true)
              .whichValue(SAVE_ALL)
              .doc("For text output, restricts output to the specified functions. The @v{name_or_address} can be the name of "
                   "a function as a string or the entry address for the function as an decimal, octal, hexadecimal or binary "
                   "number. If a value is ambiguous, it's first treated as a name and if no function has that name it's then "
                   "treated as an address. This switch may occur multiple times and multiple comma-separated values may be "
                   "specified per occurrence."));

    cg.insert(Switch("containing")
              .argument("addresses", listParser(nonNegativeIntegerParser(settings.addresses), ","))
              .explosiveLists(true)
              .whichValue(SAVE_ALL)
              .doc("For text output, restricts output to functions that contain the specified address. This switch may "
                   "occur multiple times and multiple comma-separated addresses may be specified per occurrence."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(generic);
    parser.with(cg);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{BAT-input}]");

    ParserResult cmdline = parser.parse(argc, argv).apply();
    
    // Positional args
    std::vector<std::string> input = cmdline.unreachedArgs();
    if (input.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    // Optional args
    if (!settings.functionNames.empty()) {
        if (FORMAT_GRAPHVIZ == settings.outputFormat && cmdline.have("format")) {
            mlog[FATAL] <<"--function and --format=gv are mutually exclusive\n";
            exit(1);
        }
        settings.outputFormat = FORMAT_TEXT;
    }

    return input.empty() ? std::string("-") : input[0];
}

void
emitGraphViz(const P2::Partitioner &partitioner, const Settings &settings) {
    if (settings.inliningImports) {
        P2::GraphViz::CgInlinedEmitter gv(partitioner, boost::regex("(\\.dll|@plt)$"));
        gv.highlight(boost::regex("."));            // highlight anything with a name
        gv.defaultGraphAttributes().insert("overlap", "scale");
        gv.emitCallGraph(std::cout);
    } else {
        P2::GraphViz::CgEmitter gv(partitioner);
        gv.defaultGraphAttributes().insert("overlap", "scale");
        gv.emitCallGraph(std::cout);
    }
}

void
emitText(const P2::Partitioner &partitioner, std::vector<P2::Function::Ptr> &functions, const Settings &settings) {
    P2::FunctionCallGraph cg = partitioner.functionCallGraph(P2::AllowParallelEdges::YES);
    BOOST_FOREACH (P2::Function::Ptr function, functions) {
        std::cout <<function->printableName() <<"\n";
        if (cg.callers(function).empty()) {
            std::cout <<"  no callers\n";
        } else {
            BOOST_FOREACH (P2::Function::Ptr caller, cg.callers(function))
                std::cout <<"  from  " <<caller->printableName() <<"\n";
        }
        if (cg.callees(function).empty()) {
            std::cout <<"  no callees\n";
        } else {
            BOOST_FOREACH (P2::Function::Ptr callee, cg.callees(function))
                std::cout <<"  calls " <<callee->printableName() <<"\n";
        }
    }
}

void
emitGexf(const P2::Partitioner &partitioner, const Settings &settings) {
    P2::FunctionCallGraph functionCalls = partitioner.functionCallGraph(P2::AllowParallelEdges::NO);
    const P2::FunctionCallGraph::Graph &cg = functionCalls.graph();
    Sawyer::ProgressBar<size_t> progress(cg.nVertices() + cg.nEdges(), mlog[MARCH], "GEXF output");

    std::cout <<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
              <<"<gexf xmlns=\"http://www.gexf.net/1.2draft\" version=\"1.2\">\n"
              <<"  <meta lastmodifieddate=\"1970-01-01\">\n" // can't be accurate anyway, so use an obviously wrong value
              <<"    <creator>bat-cg</creator>\n"
              <<"    <description>function call graph</description>\n"
              <<"  </meta>\n"
              <<"  <graph mode=\"static\" defaultedgetype=\"directed\">\n";

    std::cout <<"    <nodes>\n";
    BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Vertex &vertex, cg.vertices()) {
        std::cout <<"      <node id=\"" <<vertex.id() <<"\""
                  <<" label=\"" <<StringUtility::addrToString(vertex.value()->address()) <<"\""
                  <<" function=\"" <<StringUtility::cEscape(vertex.value()->name()) <<"\"/>\n";
        ++progress;
    }
    std::cout <<"    </nodes>\n";

    std::cout <<"    <edges>\n";
    BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Edge &edge, cg.edges()) {
        std::cout <<"      <edge id=\"" <<edge.id() <<"\""
                  <<" source=\"" <<edge.source()->id() <<"\""
                  <<" target=\"" <<edge.target()->id() <<"\""
                  <<" weight=\"" <<std::max(edge.value().count(), (size_t)1) <<"\""
                  <<" label=\"";
        switch (edge.value().type()) {
            case P2::E_NORMAL: std::cout <<"normal"; break;
            case P2::E_CALL_RETURN: std::cout <<"cret"; break;
            case P2::E_FUNCTION_CALL: std::cout <<"fcall"; break;
            case P2::E_FUNCTION_XFER: std::cout <<"xfer"; break;
            case P2::E_FUNCTION_RETURN: std::cout <<"fret"; break;
            case P2::E_USER_DEFINED: std::cout <<"user"; break;
            default: std::cout <<"other"; break;
        }
        std::cout <<"\"/>\n";
        ++progress;
    }
    std::cout <<"    </edges>\n";

    std::cout <<"  </graph>\n"
              <<"</gexf>\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("operating on function call graphs");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, settings);
    P2::Engine engine;
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);

    switch (settings.outputFormat) {
        case FORMAT_GRAPHVIZ:
            emitGraphViz(partitioner, settings);
            break;
        case FORMAT_TEXT: {
            std::vector<P2::Function::Ptr> selectedFunctions = partitioner.functions();
            if (!settings.functionNames.empty() || !settings.addresses.empty()) {
                selectedFunctions = Bat::selectFunctionsByNameOrAddress(selectedFunctions, settings.functionNames, mlog[WARN]);
                std::vector<P2::Function::Ptr> more = Bat::selectFunctionsContainingInstruction(partitioner, settings.addresses);
                BOOST_FOREACH (const P2::Function::Ptr &f, more)
                    P2::insertUnique(selectedFunctions, f, P2::sortFunctionsByAddress);
                if (selectedFunctions.empty())
                        mlog[WARN] <<"no matching functions found\n";
            }
                
            emitText(partitioner, selectedFunctions, settings);
            break;
        }
        case FORMAT_GEXF:
            emitGexf(partitioner, settings);
            break;
        default:
            ASSERT_not_implemented("output format");
    }
}
