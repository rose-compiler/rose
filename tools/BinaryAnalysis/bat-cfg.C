static const char *purpose = "emits control flow graphs";
static const char *description =
    "Given a binary specimen, generate GraphViz or text files representing control flow graphs.";

#include <batSupport.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>

#include <SgAsmInstruction.h>

#include <stringify.h>                                  // ROSE

#include <boost/algorithm/string/predicate.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace {

enum class OutputFormat { GRAPHVIZ, TEXT };
enum class Mode { GLOBAL_CFG, FUNCTION_CFG, UNSPECIFIED };

struct Settings {
    OutputFormat outputFormat = OutputFormat::TEXT;     // whether to generate GraphViz or plain text
    Mode mode = Mode::UNSPECIFIED;                      // type of graph output to produce
    std::string outputPrefix = "-";                     // string to add to each output file name
    bool usingSubgraphs = true;                         // group vertices into functions
    bool showingInstructions = true;                    // show instructions in each vertex
    bool showingReturnEdges = false;                    // show function return edges
    std::set<std::string> functionNames;                // restrict output to these function names, addresses
    SerialIo::Format stateFormat = SerialIo::BINARY;
};

Sawyer::Message::Facility mlog;

// Build a command-line switch parser bound to the specified settings.
Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    //---------- Generic switches ----------
    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    generic.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    //---------- Output switches ----------
    SwitchGroup cfg("Control flow graph switches");
    cfg.name("cfg");

    cfg.insert(Switch("format")
               .argument("m", enumParser(settings.outputFormat)
                         ->with("gv", OutputFormat::GRAPHVIZ)
                         ->with("text", OutputFormat::TEXT))
               .doc("Determines which format of output to produce. The choices are:"
                    "@named{gv}{Produce a GraphViz file.}"
                    "@named{text}{Produce a text file.}"
                    "The default is \"" + std::string(OutputFormat::GRAPHVIZ==settings.outputFormat?"gv":"text") + "\"."));

    cfg.insert(Switch("mode")
               .argument("graph-type", enumParser(settings.mode)
                         ->with("global", Mode::GLOBAL_CFG)
                         ->with("function", Mode::FUNCTION_CFG))
               .doc("Specifies the type of control flow graphs to emit. The choices are:"
                    "@named{global}{Generate a single global control flow graph.}"
                    "@named{function}{Generate one control flow graph per function.}"
                    "If the mode is not specified then \"global\" is used unless the @s{function} switch is specified."));

    cfg.insert(Switch("prefix")
               .argument("string", anyParser(settings.outputPrefix))
               .doc("When generating output files (rather than writing to standard output), the @v{string} is prepended "
                    "to each file name. This can be used to cause output to be written to a different directory (if "
                    "@v{string} ends with a slash), and/or to organize related output files by causing their names "
                    "to start with a common string (like \"@s{prefix} a.out-\").  As a special case, if the prefix is "
                    "a single hyphen, \"-\", and only one output file would be generated, the output is sent to standard "
                    "output instead of creating a file. The default is "
                    "\"" + StringUtility::cEscape(settings.outputPrefix) + "\"."));

    cfg.insert(Switch("subgraphs")
               .intrinsicValue(true, settings.usingSubgraphs)
               .doc("When producing a graph that contains vertices from more than one function, this switch will cause "
                    "them to be organized into one subgraph per function.  The @s{no-subgraphs} switch disables "
                    "subgraphs. The default is to " + std::string(settings.usingSubgraphs?"":"not ") + "use subgraphs."));
    cfg.insert(Switch("no-subgraphs")
               .key("subgraphs")
               .intrinsicValue(false, settings.usingSubgraphs)
               .hidden(true));

    cfg.insert(Switch("show-insns")
               .intrinsicValue(true, settings.showingInstructions)
               .doc("Show disassembled instructions in each basic block vertex rather than showing only the block starting "
                    "addresses. Emitting just addresses makes the GraphViz files much smaller but requires a separate "
                    "assembly listing to interpret the graphs.  The @s{no-show-insns} causes only addresses to be emitted.  The "
                    "default is to emit " + std::string(settings.showingInstructions?"instructions":"only addresses") + "."));
    cfg.insert(Switch("no-show-insns")
               .key("show-insns")
               .intrinsicValue(false, settings.showingInstructions)
               .hidden(true));

    cfg.insert(Switch("show-return-edges")
               .intrinsicValue(true, settings.showingReturnEdges)
               .doc("Show the function return edges in control flow graphs. These are the edges originating at a basic block "
                    "that serves as a function return and usually lead to the indeterminate vertex.  Including them in "
                    "multi-function graphs makes the graphs more complicated than they need to be for visualization. The "
                    "@s{no-show-return-edges} switch disables these edges. The default is to " +
                    std::string(settings.showingReturnEdges?"":"not ") + "show these edges."));
    cfg.insert(Switch("no-show-return-edges")
               .key("show-return-edges")
               .intrinsicValue(false, settings.showingReturnEdges)
               .hidden(true));

    cfg.insert(Switch("function", 'f')
               .argument("name_or_address", listParser(anyParser(settings.functionNames), ","))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .doc("Restricts output to the specified functions and causes the function mode to be used (one graph per "
                    "selected function). The @v{name_or_address} can be the name of a function as a string or the entry "
                    "address for the function as an decimal, octal, hexadecimal or binary number. If a value is ambiguous, "
                    "it's first treated as a name and if no function has that name it's then treated as an address. This "
                    "switch may occur multiple times and multiple comma-separated values may be specified per occurrence."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.with(cfg);
    parser.with(generic);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimen}]");
    parser.doc("Output",
               "If only a single graph is created, it can be sent to standard output by specifying \"@s{prefix} -\". Otherwise "
               "output will be written to files which are truncated if they previously existed.\n\n"

               "For per-function control flow graphs, one file will be created for each function and the file will be named "
               "\"@v{prefix}cfg-@v{address}[-@v{name}].dot\" where @v{prefix} is the string specified with the @s{prefix} "
               "switch, @v{address} is the entry address for the function, and @v{name} is the name of the function with "
               "special characters replaced by underscores.  If the function has no name then the function name and its "
               "preceding hyphen are not part of the file name.\n\n"

               "For global control flow graphs, the output file is named \"@v{prefix}cfg-global.dot\".");

    return parser;
}

// Parses the command-line and returns the name of the specimen to be analyzed.
std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser, Settings &settings) {
    std::vector<std::string> specimen = parser.parse(argc, argv).apply().unreachedArgs();
    if (specimen.empty())
        specimen.push_back("-");

    // Values for optional args
    if (Mode::UNSPECIFIED == settings.mode) {
        if (settings.functionNames.empty()) {
            settings.mode = Mode::GLOBAL_CFG;
        } else {
            settings.mode = Mode::FUNCTION_CFG;
        }
    } else if (!settings.functionNames.empty()) {
        mlog[FATAL] <<"--function and --mode=global are mutually exclusive\n";
        exit(1);
    }

    ASSERT_forbid(settings.mode == Mode::UNSPECIFIED);
    return specimen;
}

// Replaces characters that can't appear in a file name component with underscores.  If the whole return string would be
// underscores then return the empty string instead.
std::string
escapeFileNameComponent(const std::string &s) {
    std::string retval;
    bool hasNonUnderscore = false;
    for (char ch: s) {
        if (isalnum(ch) || strchr("@$%=+:,.", ch)) {
            retval += ch;
            hasNonUnderscore = true;
        } else {
            retval += '_';
        }
    }
    return hasNonUnderscore ? retval : std::string();
}

boost::filesystem::path
makeGraphVizFileName(const std::string &prefix, const std::string &p1, const P2::Function::Ptr &function) {
    if (prefix == "-")
        return "-";
    std::vector<std::string> parts;
    parts.push_back(p1);
    parts.push_back(StringUtility::addrToString(function->address()).substr(2));
    parts.push_back(escapeFileNameComponent(function->name()));
    parts.erase(std::remove(parts.begin(), parts.end(), std::string()), parts.end());
    return prefix + StringUtility::join("-", parts) + ".dot";
}

boost::filesystem::path
makeGraphVizFileName(const std::string &prefix, const std::string &base) {
    if (prefix == "-")
        return "-";
    return prefix + base;
}

void
emitGraphVizFunctionCfg(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function,
                        const Settings &settings) {
    P2::GraphViz::CfgEmitter gv(partitioner);
    gv.defaultGraphAttributes().insert("overlap", "scale");
    gv.useFunctionSubgraphs(false);                     // since we're dumping only one function
    gv.showInstructions(settings.showingInstructions);
    gv.showReturnEdges(settings.showingReturnEdges);
    gv.showInNeighbors(true);
    gv.showOutNeighbors(true);
    gv.emitFunctionGraph(out, function);
}

void
emitGraphVizGlobalCfg(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const Settings &settings) {
    P2::GraphViz::CfgEmitter gv(partitioner);
    gv.defaultGraphAttributes().insert("overlap", "scale");
    gv.useFunctionSubgraphs(settings.usingSubgraphs);
    gv.showInstructions(settings.showingInstructions);
    gv.showReturnEdges(settings.showingReturnEdges);
    gv.emitWholeGraph(out);
}

class VertexLabels {
    Sawyer::Container::GraphIteratorMap<P2::ControlFlowGraph::ConstVertexIterator, std::string> labels_;
    P2::Partitioner::ConstPtr partitioner_;             // not null
    P2::Function::Ptr function_;
    size_t nLocalBlocks_;

public:
    VertexLabels(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &f)
        : partitioner_(partitioner), function_(f), nLocalBlocks_(0) {
        ASSERT_not_null(partitioner);
    }
    
    std::string operator()(P2::ControlFlowGraph::ConstVertexIterator vertex) {
        std::string retval;
        if (labels_.find(vertex).assignTo(retval))
            return retval;

        switch (vertex->value().type()) {
            case P2::V_INDETERMINATE:
                retval = "indeterminate";
                break;
            case P2::V_NONEXISTING:
                retval = "nonexisting";
                break;
            case P2::V_UNDISCOVERED:
                retval = "undiscovered";
                break;
            case P2::V_BASIC_BLOCK: {
                P2::BasicBlock::Ptr bb = vertex->value().bblock();
                ASSERT_not_null(bb);
                const std::set<rose_addr_t> &bbVas = function_->basicBlockAddresses();
                if (bbVas.find(bb->address()) != bbVas.end()) // block local to this function?
                    retval = "B" + StringUtility::numberToString(++nLocalBlocks_) + " ";
                if (bb->address() == function_->address()) {
                    retval += "function entry point";
                } else if (P2::Function::Ptr f = partitioner_->functionExists(bb->address())) {
                    retval += f->printableName();
                } else {
                    retval += bb->printableName();
                }
                break;
            }
            case P2::V_USER_DEFINED:
                retval = "user defined";
                break;
            default:
                ASSERT_not_implemented("vertex type");
        }
        labels_.insert(vertex, retval);
        return retval;
    }
};

std::string
edgeTypeName(const P2::ControlFlowGraph::Edge &edge) {
    std::string retval = stringifyBinaryAnalysisPartitioner2EdgeType(edge.value().type(), "E_");
    for (char &ch: retval)
        ch = '_' == ch ? ' ' : tolower(ch);
    return retval;
}

void
emitTextFunctionCfg(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function,
                    const Settings &settings) {
    ASSERT_not_null(partitioner);
    out <<function->printableName() <<"\n";
    VertexLabels vertexLabels(partitioner, function);

    // Emit information about each basic block
    for (rose_addr_t bbVa: function->basicBlockAddresses()) {
        P2::ControlFlowGraph::ConstVertexIterator placeholder = partitioner->findPlaceholder(bbVa);
        if (!partitioner->cfg().isValidVertex(placeholder)) {
            out <<"  " <<StringUtility::addrToString(bbVa) <<": not present in CFG\n";
        } else {
            // Block label
            out <<"  " <<vertexLabels(placeholder) <<":\n";

            // Predecessors
            for (const P2::ControlFlowGraph::Edge &edge: placeholder->inEdges())
                out <<"    " <<edgeTypeName(edge) <<" edge from " <<vertexLabels(edge.source()) <<"\n";
            
            // Vertex content
            P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbVa);
            if (bb && settings.showingInstructions) {
                for (SgAsmInstruction *insn: bb->instructions())
                    out <<"      " <<insn->toString() <<"\n";
            }

            // Vertex properties
            if (partitioner->basicBlockIsFunctionCall(bb))
                out <<"    block is a function call\n";
            if (partitioner->basicBlockIsFunctionReturn(bb))
                out <<"    block is a function return\n";

            // Owning functions
            P2::FunctionSet owners = placeholder->value().owningFunctions();
            for (const P2::Function::Ptr &owner: owners.values()) {
                if (owner != function)
                    out <<"    also owned by " <<owner->printableName() <<"\n";
            }
            if (!owners.exists(function))
                out <<"    ERROR: not marked as owned by " <<function->printableName() <<"\n";

            // Successors
            for (const P2::ControlFlowGraph::Edge &edge: placeholder->outEdges())
                out <<"    " <<edgeTypeName(edge) <<" edge to " <<vertexLabels(edge.target()) <<"\n";
        }
    }
}

void
emitTextGlobalCfg(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const Settings &settings) {
    ASSERT_not_null(partitioner);
    for (P2::Function::Ptr function: partitioner->functions())
        emitTextFunctionCfg(out, partitioner, function, settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("operating on control flow graphs");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Parse command-line
    Settings settings;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(settings);
    auto engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser, settings /*in,out*/);

    // Ingest specimen
    P2::Partitioner::Ptr partitioner;
    if (specimen.size() == 1 && (specimen[0] == "-" || boost::ends_with(specimen[0], ".rba"))) {
        try {
            partitioner = P2::Partitioner::instanceFromRbaFile(specimen[0], settings.stateFormat);
        } catch (const std::exception &e) {
            mlog[FATAL] <<"cannot load partitioner from " <<specimen[0] <<": " <<e.what() <<"\n";
            exit(1);
        }
    } else {
        partitioner = engine->partition(specimen);
    }
    ASSERT_not_null(partitioner);

    // Get a list of functions
    std::vector<P2::Function::Ptr> selectedFunctions = partitioner->functions();
    if (!settings.functionNames.empty()) {
        selectedFunctions = Bat::selectFunctionsByNameOrAddress(selectedFunctions, settings.functionNames, mlog[WARN]);
        if (selectedFunctions.empty())
            mlog[WARN] <<"no matching functions found\n";
    }

    // Generate output
    bool hadErrors = false;
    if (Mode::FUNCTION_CFG == settings.mode) {
        if (selectedFunctions.size() > 1 && settings.outputPrefix == "-") {
            mlog[FATAL] <<"refusing to send " <<selectedFunctions.size() <<" GraphViz objects to standard output\n"
                        <<"use the --prefix switch instead to specify output file names.\n";
            exit(1);
        }
        for (const P2::Function::Ptr &function: selectedFunctions) {
            boost::filesystem::path fileName = makeGraphVizFileName(settings.outputPrefix, "cfg", function);
            std::ofstream file;
            if (fileName != "-") {
                mlog[INFO] <<"generating CFG GraphViz file " <<fileName <<"\n";
                file.open(fileName.string().c_str());
                if (file.fail()) {
                    mlog[ERROR] <<"cannot create file " <<fileName <<"\n";
                    hadErrors = true;
                    continue;
                }
            }
            std::ostream &out = file.is_open() ? file : std::cout;
            switch (settings.outputFormat) {
                case OutputFormat::GRAPHVIZ:
                    emitGraphVizFunctionCfg(out, partitioner, function, settings);
                    break;
                case OutputFormat::TEXT:
                    emitTextFunctionCfg(out, partitioner, function, settings);
                    break;
                default:
                    ASSERT_not_implemented("output format");
            }
        }
    } else {
        ASSERT_require(Mode::GLOBAL_CFG == settings.mode);
        boost::filesystem::path fileName = makeGraphVizFileName(settings.outputPrefix, "cfg-global");
        std::ofstream file;
        if (fileName != "-") {
            mlog[INFO] <<"generating CFG GraphViz file " <<fileName <<"\n";
            file.open(fileName.string().c_str());
            if (file.fail()) {
                mlog[ERROR] <<"cannot create file " <<fileName <<"\n";
                exit(1);
            }
        }
        std::ostream &out = file.is_open() ? file : std::cout;
        switch (settings.outputFormat) {
            case OutputFormat::GRAPHVIZ:
                emitGraphVizGlobalCfg(out, partitioner, settings);
                break;
            case OutputFormat::TEXT:
                emitTextGlobalCfg(out, partitioner, settings);
                break;
            default:
                ASSERT_not_implemented("output format");
        }
    }

    return hadErrors ? 1 : 0;
}
