#include <rose.h>

#include <AsmUnparser_compat.h>
#include <Diagnostics.h>
#include <DwarfLineMapper.h>
#include <Partitioner2/CfgPath.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <rose_strtoull.h>
#include <sawyer/BiMap.h>
#include <sawyer/GraphTraversal.h>
#include <SymbolicMemory2.h>
#include <SymbolicSemantics2.h>
#include <YicesSolver.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace Sawyer::Container::Algorithm;
using namespace rose::BinaryAnalysis::InstructionSemantics2; // BaseSemantics, SymbolicSemantics
namespace P2 = Partitioner2;

Diagnostics::Facility mlog;
DwarfLineMapper srcMapper;

enum FollowCalls { SINGLE_FUNCTION, FOLLOW_CALLS };
enum PathSelection { NO_PATHS, FEASIBLE_PATHS, ALL_PATHS };

// Settings from the command-line
struct Settings {
    std::string isaName;                                // instruction set architecture name
    std::string beginVertex;                            // address or function name where paths should begin
    std::vector<std::string> endVertices;               // addresses or function names where paths should end
    std::vector<std::string> avoidVertices;             // vertices to avoid in any path
    std::vector<std::string> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    size_t expansionDepthLimit;                         // max function call depth when expanding function calls
    size_t vertexVisitLimit;                            // max times a vertex can appear in a path
    bool showInstructions;                              // show instructions in paths?
    bool showConstraints;                               // show path constraints?
    bool showFinalState;                                // show final machine state for each feasible path?
    bool showFunctionSubgraphs;                         // show functions in GraphViz output?
    std::string graphVizPrefix;                         // prefix for GraphViz file names
    PathSelection graphVizOutput;                       // which paths to dump to GraphViz files
    size_t maxPaths;                                    // max number of paths to find (0==unlimited)
    bool multiPathSmt;                                  // send multiple paths at once to the SMT solver?
    size_t maxExprDepth;                                // max depth when printing expressions
    bool showExprWidth;                                 // show expression widths in bits?
    bool debugSmtSolver;                                // turn on SMT debugging?
    Sawyer::Optional<rose_addr_t> initialStackPtr;      // concrete value to use for stack pointer register initial value
    Settings()
        : beginVertex("_start"), expansionDepthLimit(4), vertexVisitLimit(1), showInstructions(true),
          showConstraints(false), showFinalState(false), showFunctionSubgraphs(true),
          graphVizPrefix("path-"), graphVizOutput(NO_PATHS), maxPaths(1), multiPathSmt(false), maxExprDepth(4),
          showExprWidth(false), debugSmtSolver(false) {}
};
static Settings settings;

// This is a "register" that stores a description of the current path.  The major and minor numbers are arbitrary, but chosen
// so that they hopefully don't conflict with any real registers, which tend to start counting at zero.  Since we're using
// BaseSemantics::RegisterStateGeneric, we can use its flexibility to store extra "registers" without making any other changes
// to the architecture.
static const RegisterDescriptor REG_PATH(99, 0, 0, 1);

// This is the register where functions will store their return value.
static RegisterDescriptor REG_RETURN;

// Information stored per V_USER_DEFINED vertices.
struct FunctionSummary {
    P2::Function::Ptr function;
    FunctionSummary() {}
    explicit FunctionSummary(const P2::Function::Ptr &function): function(function) {}
};
typedef Sawyer::Container::Map<P2::ControlFlowGraph::ConstVertexIterator, FunctionSummary> FunctionSummaries;
static FunctionSummaries functionSummaries;

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[])
{
    using namespace Sawyer::CommandLine;

    //--------------------------- 
    Parser parser;
    parser
        .purpose("finds paths in control flow graph")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} --begin=@v{va} --end=@v{va} [@v{switches}] @v{specimen}")
        .doc("Description",
             "Parses, loads, disassembles, and partitions the specimen and then looks for control flow paths. A path "
             "is a sequence of edges in the global control flow graph beginning and ending at user-specified vertices. "
             "A vertex is specified as either the name of a function, or an address contained in a basic block. Addresses "
             "can be decimal, octal, or hexadecimal.")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());
    
    //--------------------------- 
    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    //--------------------------- 
    SwitchGroup dis("Disassembly switches");
    dis.insert(Switch("isa")
               .argument("architecture", anyParser(settings.isaName))
               .doc("Instruction set architecture. Specify \"list\" to see a list of possible ISAs."));

    //--------------------------- 
    SwitchGroup cfg("Control flow graph switches");
    cfg.insert(Switch("begin")
               .argument("name_or_va", anyParser(settings.beginVertex))
               .doc("CFG vertex where paths will start.  If @v{name_or_va} is an integer (decimal, octal, or hexadecimal "
                    "in the usual C/C++ syntax) then it is assumed to be the starting address of a basic block, or an "
                    "address contained in the block.  Otherwise @v{name_or_va} is assumed to be the name of a function "
                    "whose entry vertex is used.  If no name is specified then \"" +
                    StringUtility::cEscape(settings.beginVertex) + "\" is assumed."));

    cfg.insert(Switch("end")
               .argument("name_or_va", listParser(anyParser(settings.endVertices)))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("CFG vertex where paths will end.  This switch can be specified as often as desired (or a list argument "
                    "provided) in order to find multiple end-points, but at least one end-point must be specified. Paths will "
                    "end at the first end-point they encounter. End-point vertices are either addresses or functions as "
                    "described for the @s{begin} switch."));

    cfg.insert(Switch("avoid-vertex")
               .argument("name_or_va", listParser(anyParser(settings.avoidVertices)))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("A vertex that should be excluded from any path.  This switch may appear as often as desired in "
                    "order to avoid multiple vertices, or vertices can be specified in a comma-separated list.  A vertex "
                    "can be either an address or a function name (see @s{begin})."));

    cfg.insert(Switch("avoid-edge")
               .argument("edge", listParser(anyParser(settings.avoidEdges))->exactly(2))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("Edge where path search will end. Edges are specified as a pair of vertices separated by a comma: "
                    "the source and target vertices.  A vertex can be either an address or a function name (see "
                    "@s{begin})."));

    cfg.insert(Switch("expansion-depth")
               .argument("n", nonNegativeIntegerParser(settings.expansionDepthLimit))
               .doc("Maximum function call depth when expanding paths through function calls.  The default is " +
                    StringUtility::numberToString(settings.expansionDepthLimit) + "."));

    cfg.insert(Switch("vertex-visits")
               .argument("n", nonNegativeIntegerParser(settings.vertexVisitLimit))
               .doc("Maximum number of times a vertex can appear in a single path for some level of function call.  A value "
                    "of one means that the path will contain no back-edges. A value of two will typically allow one "
                    "iteration for each loop. The default is " +
                    StringUtility::numberToString(settings.vertexVisitLimit) + "."));

    cfg.insert(Switch("max-paths")
               .argument("n", nonNegativeIntegerParser(settings.maxPaths))
               .doc("Maximum number of feasible paths to find. Once this number of feasible paths are found the analysis "
                    "exits.  The default is " + StringUtility::numberToString(settings.maxPaths) + ". Setting this to "
                    "zero finds all possible feasible paths, subject to other constraints.\n"));

    cfg.insert(Switch("multi-path")
               .intrinsicValue(true, settings.multiPathSmt)
               .doc("Enables a mode of operation where information about multiple paths is sent to the SMT solver all "
                    "at once rather than invoking the SMT solver once per path.  When multi-path mode is enabled information "
                    "about which path is taken is not available--only the fact that some path was feasible is reported. The "
                    "initial conditions that cause some feasible path to be taken are always reported regardless of whether "
                    "feasibility is evaluated one path at a time, or multiple paths at once.  The @s{single-path} switch "
                    "is the inverse.  The default is @s{" + std::string(settings.multiPathSmt?"multi":"single") + "-path}."));
    cfg.insert(Switch("single-path")
               .key("multi-path")
               .intrinsicValue(false, settings.multiPathSmt)
               .doc("Enables a mode of operation where information about only one path at a time is sent to the SMT solver. "
                    "See @s{multi-path} for details."));

    cfg.insert(Switch("stack")
               .argument("va", nonNegativeIntegerParser(settings.initialStackPtr))
               .doc("Virtual address to use for the initial stack pointer.  If no stack address is specified then the "
                    "analysis uses a symbolic value.  One benefit of specifying a concrete value is that it helps "
                    "disambiguate stack variables from other variables."));

    //--------------------------- 
    SwitchGroup out("Output switches");
    out.insert(Switch("show-instructions")
               .intrinsicValue(true, settings.showInstructions)
               .doc("Cause instructions to be listed as part of each path. The @s{no-show-instructions} switch turns "
                    "this off. The default is to " + std::string(settings.showInstructions?"":"not ") + "show instructions."));
    out.insert(Switch("no-show-instructions")
               .key("show-instructions")
               .intrinsicValue(false, settings.showInstructions)
               .hidden(true));

    out.insert(Switch("show-constraints")
               .intrinsicValue(true, settings.showConstraints)
               .doc("List constraints for each path.  The @s{no-show-constraints} switch turns this off. The default is to " +
                    std::string(settings.showConstraints?"":"not ") + "show path constraints in the output.\n"));
    out.insert(Switch("no-show-constraints")
               .key("show-constraints")
               .intrinsicValue(false, settings.showConstraints)
               .hidden(true));

    out.insert(Switch("show-final-state")
               .intrinsicValue(true, settings.showFinalState)
               .doc("Show symbolic machine state at the end of each path.  The state contains the final values for registers "
                    "and memory accessed along the path.  The @s{no-show-final-state} switch turns this information off. "
                    "The default is to " + std::string(settings.showFinalState?"":"not ") + "show this information."));
    out.insert(Switch("no-show-final-state")
               .key("show-final-state")
               .intrinsicValue(false, settings.showFinalState)
               .hidden(true));

    out.insert(Switch("subgraphs")
               .intrinsicValue(true, settings.showFunctionSubgraphs)
               .doc("Organize basic blocks into subgraphs in the GraphViz output according to the function to which "
                    "the block belongs.  The @s{no-subgraphs} switch turns this feature off. The default is to " +
                    std::string(settings.showFunctionSubgraphs?"":"not ") + "show subgraphs."));
    out.insert(Switch("no-subgraphs")
               .key("subgraphs")
               .intrinsicValue(false, settings.showFunctionSubgraphs)
               .hidden(true));

    out.insert(Switch("gv-prefix")
               .argument("string", anyParser(settings.graphVizPrefix))
               .doc("File name prefix for graph-viz files.  The default is " +
                    (settings.graphVizPrefix.empty() ? std::string("an empty string") :
                     ("\"" + StringUtility::cEscape(settings.graphVizPrefix) + "\"")) + ". "
                    "The full file name is constructed by appending additional strings to the prefix, namely "
                    "a six-digit decimal path number, followed by a hyphen and six-digit decimal number of vertices "
                    "in the path, followed by \".dot\"."));

    out.insert(Switch("gv-paths")
               .argument("selector", enumParser(settings.graphVizOutput)
                         ->with("all", ALL_PATHS)
                         ->with("feasible", FEASIBLE_PATHS)
                         ->with("none", NO_PATHS))
               .doc("Whether to dump paths to GraphViz files.  The switch argument, @v{selector}, can be one of:"
                    "@named{\"all\"}{Dump all paths regardless of whether they're feasible paths. " +
                    std::string(ALL_PATHS==settings.graphVizOutput? "This is the default.":"") + "}"
                    "@named{\"feasible\"}{Dump only those paths that are determined to be feasible.  The path numbers "
                    "in the file names are still computed across all paths and so will not be consecutive. " +
                    std::string(FEASIBLE_PATHS==settings.graphVizOutput? "This is the default.":"") + "}"
                    "@named{\"none\"}{Do not dump any paths. " +
                    std::string(NO_PATHS==settings.graphVizOutput? "This is the default.":"") + "}"
                    "The file name algorithm is described in the @s{gv-prefix} switch documentation."));

    out.insert(Switch("max-expr-depth")
               .argument("n", nonNegativeIntegerParser(settings.maxExprDepth))
               .doc("Maximum depth to which symbolic expressions are printed in diagnostic messages.  If @v{n} is zero then "
                    "then entire expression is displayed. The default is " +
                    StringUtility::numberToString(settings.maxExprDepth) + "."));

    out.insert(Switch("show-expr-width")
               .intrinsicValue(true, settings.showExprWidth)
               .doc("Show width in bits for symbolic expressions.  This is useful, but can make the expressions overly "
                    "verbose and less readable.  The @s{no-show-expr-width} turns this off. The default is to " +
                    std::string(settings.showExprWidth ? "" : "not ") + "show expression widths."));
    out.insert(Switch("no-show-expr-width")
               .key("show-expr-width")
               .intrinsicValue(false, settings.showExprWidth)
               .hidden(true));

    out.insert(Switch("debug-smt")
               .intrinsicValue(true, settings.debugSmtSolver)
               .doc("Turns on debugging of the SMT solver.  The @s{no-debug-smt} switch disables debugging. The default "
                    "is to " + std::string(settings.debugSmtSolver ? "" : "not ") + "debug the solver."));
    out.insert(Switch("no-debug-smt")
               .key("debug-smt")
               .intrinsicValue(false, settings.debugSmtSolver)
               .hidden(true));

    return parser.with(gen).with(dis).with(cfg).with(out).parse(argc, argv).apply();
}

rose_addr_t
virtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK)
        return vertex->value().address();
    if (vertex->value().type() == P2::V_USER_DEFINED) {
        ASSERT_require(functionSummaries.exists(vertex));
        const FunctionSummary &summary = functionSummaries[vertex];
        ASSERT_not_null(summary.function);
        return summary.function->address();
    }
    ASSERT_not_reachable("invalid vertex type");
}

P2::ControlFlowGraph::ConstVertexIterator
vertexForInstruction(const P2::Partitioner &partitioner, const std::string &nameOrVa) {
    const char *s = nameOrVa.c_str();
    char *rest;
    errno = 0;
    rose_addr_t va = rose_strtoull(s, &rest, 0);
    if (*rest || errno!=0) {
        size_t nFound = 0;
        BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
            if (function->name() == nameOrVa) {
                va = function->address();
                ++nFound;
            }
        }
        if (0==nFound)
            return partitioner.cfg().vertices().end();
        if (nFound > 1)
            throw std::runtime_error("vertex \""+StringUtility::cEscape(nameOrVa)+"\" is ambiguous");
    }
    return partitioner.instructionVertex(va);
}

// Convert a pair of addresses to a CFG edge.  The source address can be a basic block or last instruction of the basic
// block.
P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner &partitioner,
                    const P2::ControlFlowGraph::ConstVertexIterator &source,
                    const P2::ControlFlowGraph::ConstVertexIterator &target) {
    if (source == partitioner.cfg().vertices().end() || target == partitioner.cfg().vertices().end())
        return partitioner.cfg().edges().end();         // sourceVa or targetVa is not an instruction starting address
    ASSERT_require(source->value().type() == P2::V_BASIC_BLOCK);
    ASSERT_require(target->value().type() == P2::V_BASIC_BLOCK);
    for (P2::ControlFlowGraph::ConstEdgeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target() == target)
            return edge;
    }
    return partitioner.cfg().edges().end();
}

P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner &partitioner, const std::string &sourceNameOrVa, const std::string &targetNameOrVa) {
    return edgeForInstructions(partitioner,
                               vertexForInstruction(partitioner, sourceNameOrVa),
                               vertexForInstruction(partitioner, targetNameOrVa));
}

/** Erase all back edges.
 *
 *  Perform a depth first search and erase back edges. */
void
eraseBackEdges(P2::ControlFlowGraph &cfg /*in,out*/, const P2::ControlFlowGraph::ConstVertexIterator &begin) {
    P2::CfgConstEdgeSet backEdges = findBackEdges(cfg, begin);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, backEdges)
        cfg.eraseEdge(edge);
}

// True if path ends with a function call.
bool
pathEndsWithFunctionCall(const P2::Partitioner &partitioner, const P2::CfgPath &path) {
    if (path.isEmpty())
        return false;
    P2::ControlFlowGraph::ConstVertexIterator pathVertex = path.backVertex();
    P2::ControlFlowGraph::ConstVertexIterator cfgVertex = partitioner.findPlaceholder(virtualAddress(pathVertex));
    ASSERT_require(partitioner.cfg().isValidVertex(cfgVertex));
    BOOST_FOREACH (P2::ControlFlowGraph::Edge edge, cfgVertex->outEdges()) {
        if (edge.value().type() == P2::E_FUNCTION_CALL)
            return true;
    }
    return false;
}

/** Determines whether a function call should be summarized instead of inlined. */
bool shouldSummarizeCall(const P2::CfgPath &path, const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().function();
    if (!callee)
        return false;
    if (boost::ends_with(callee->name(), "@plt"))
        return true;                                    // this is probably dynamically linked ELF function
    return false;
}

/** Determines whether a function call should be inlined. */
bool shouldInline(const P2::CfgPath &path, const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().function();
    if (!callee)
        return false;
    if (path.callDepth(callee) >= settings.expansionDepthLimit)
        return false;
    return true;
}

// The GraphViz writer, extended to print something more useful for user-defined vertices (function summaries)
class GraphVizEmitter: public P2::GraphViz::CfgEmitter {
public:
    typedef P2::GraphViz::CfgEmitter Super;
    const P2::ControlFlowGraph &gref;

    GraphVizEmitter(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &cfg)
        : Super(partitioner, cfg), gref(cfg) {}

    virtual std::string vertexLabel(const P2::ControlFlowGraph::ConstVertexIterator &vertex) const ROSE_OVERRIDE {
        if (vertex->value().type() == P2::V_USER_DEFINED) {
            P2::ControlFlowGraph::ConstVertexIterator origVertex = gref.findVertex(vertex->id());
            ASSERT_require(functionSummaries.exists(origVertex));
            const FunctionSummary &summary = functionSummaries[origVertex];
            return P2::GraphViz::escape("summary for " + summary.function->printableName());
        } else {
            return Super::vertexLabel(vertex);
        }
    }
};

static void
printGraphViz(std::ostream &out, const P2::Partitioner &partitioner, const P2::ControlFlowGraph &cfg,
              const P2::ControlFlowGraph::ConstVertexIterator &beginVertex, const P2::CfgConstVertexSet &endVertices,
              const P2::CfgPath &path = P2::CfgPath()) {
    Color::HSV entryColor(0.15, 1.0, 0.6);              // bright yellow
    Color::HSV exitColor(0.088, 1.0, 0.6);              // bright orange
    Color::HSV pathColor(0.84, 1.0, 0.4);               // dark magenta

    GraphVizEmitter gv(partitioner, cfg);
#if 1 // normal output
    gv.srcMapper(srcMapper);
    gv.defaultGraphAttributes().insert("overlap", "scale");
    gv.useFunctionSubgraphs(settings.showFunctionSubgraphs);
    gv.showInstructions(true);
    gv.showInstructionAddresses(true);
    gv.showInstructionStackDeltas(false);
#else // small output
    gv.useFunctionSubgraphs(false);
    gv.showInstructions(false);
    gv.showInstructionAddresses(false);
    gv.showInstructionStackDeltas(false);
#endif
    gv.selectWholeGraph();
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &endVertex, endVertices)
        gv.vertexOrganization(endVertex).attributes().insert("fillcolor", exitColor.toHtml());
    gv.vertexOrganization(beginVertex).attributes().insert("fillcolor", entryColor.toHtml());
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, path.edges()) {
        gv.edgeOrganization(edge).attributes()
            .insert("penwidth", "3")
            .insert("arrowsize", "3")
            .insert("style", "solid")
            .insert("color", pathColor.toHtml());
    }
    gv.emit(out);
}

std::string
printGraphViz(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths, const P2::CfgPath &path, size_t pathIdx) {
    char fileName[256];
    sprintf(fileName, "%s%06zu-%06zu.dot", settings.graphVizPrefix.c_str(), pathIdx, path.nVertices());
    std::ofstream file(fileName);
    P2::CfgConstVertexSet endVertices;
    endVertices.insert(path.backVertex());
    printGraphViz(file, partitioner, paths, path.frontVertex(), endVertices, path);
    return fileName;
}

static SymbolicSemantics::Formatter
symbolicFormat(const std::string &prefix="") {
    SymbolicSemantics::Formatter retval;
    retval.set_line_prefix(prefix);
    retval.expr_formatter.max_depth = settings.maxExprDepth;
    retval.expr_formatter.show_width = settings.showExprWidth;
    return retval;
}

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

// RiscOperators that add some additional tracking information for memory values.
class RiscOperators: public SymbolicSemantics::RiscOperators {
    typedef SymbolicSemantics::RiscOperators Super;
public:
    typedef Sawyer::Container::Map<std::string /*name*/, std::string /*comment*/> VarComments;
    VarComments varComments_;                           // information about certain symbolic variables
    size_t pathInsnIndex_;                              // current location in path, or -1

protected:
    explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
        : Super(protoval, solver), pathInsnIndex_(-1) {
        set_name("FindPath");
    }

    explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
        : Super(state, solver), pathInsnIndex_(-1) {
        set_name("FindPath");
    }

public:
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, SMTSolver *solver=NULL) {
        BaseSemantics::SValuePtr protoval = SymbolicSemantics::SValue::instance();
        BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory;
        if (settings.multiPathSmt) {
            // If we're sending multiple paths at a time to the SMT solver then we need to provide the SMT solver with detailed
            // information about how memory is affected on those different paths.
            memory = BaseSemantics::SymbolicMemory::instance(protoval, protoval);
        } else {
            // We can perform memory-related operations and simplifications inside ROSE, which results in more but smaller
            // expressions being sent to the SMT solver.
            memory = SymbolicSemantics::MemoryState::instance(protoval, protoval);
        }
        BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    static RiscOperatorsPtr instance(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   SMTSolver *solver=NULL) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

public:
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    const VarComments& varComments() const { return varComments_; }

    void varComment(const std::string &name, const std::string &comment) {
        varComments_.insertMaybe(name, comment);
    }

    std::string varComment(const std::string &name) {
        return varComments_.getOptional(name).orElse("");
    }

    size_t pathInsnIndex() const {
        return pathInsnIndex_;
    }

    void pathInsnIndex(size_t n) {
        pathInsnIndex_ = n;
    }

private:
    /** Create a comment to describe a memory address if possible. The nBytes will be non-zero when we're describing
     *  an address as opposed to a value stored across some addresses. */
    std::string commentForVariable(const BaseSemantics::SValuePtr &addr, const std::string &accessMode,
                                   size_t byteNumber=0, size_t nBytes=0) {
        using namespace InsnSemanticsExpr;
        std::string varComment = "first " + accessMode + " at ";
        if (pathInsnIndex_ != (size_t)(-1))
            varComment += "path position #" + StringUtility::numberToString(pathInsnIndex_) + ", ";
        varComment += "instruction " + unparseInstructionWithAddress(get_insn());

        // Sometimes we can save useful information about the address.
        if (nBytes != 1) {
            TreeNodePtr addrExpr = SymbolicSemantics::SValue::promote(addr)->get_expression();
            if (LeafNodePtr addrLeaf = addrExpr->isLeafNode()) {
                if (addrLeaf->is_known()) {
                    varComment += "\n";
                    if (nBytes > 1) {
                        varComment += StringUtility::numberToString(byteNumber) + " of " +
                                      StringUtility::numberToString(nBytes) + " bytes starting ";
                    }
                    varComment += "at address " + addrLeaf->toString();
                }
            } else if (InternalNodePtr addrINode = addrExpr->isInternalNode()) {
                if (addrINode->get_operator() == OP_ADD && addrINode->nchildren() == 2 &&
                    addrINode->child(0)->isLeafNode() && addrINode->child(0)->isLeafNode()->is_variable() &&
                    addrINode->child(1)->isLeafNode() && addrINode->child(1)->isLeafNode()->is_known()) {
                    LeafNodePtr base = addrINode->child(0)->isLeafNode();
                    LeafNodePtr offset = addrINode->child(1)->isLeafNode();
                    varComment += "\n";
                    if (nBytes > 1) {
                        varComment += StringUtility::numberToString(byteNumber) + " of " +
                                      StringUtility::numberToString(nBytes) + " bytes starting ";
                    }
                    varComment += "at address ";
                    if (base->get_comment().empty()) {
                        varComment = base->toString();
                    } else {
                        varComment += base->get_comment();
                    }
                    Sawyer::Container::BitVector tmp = offset->get_bits();
                    if (tmp.get(tmp.size()-1)) {
                        varComment += " - 0x" + tmp.negate().toHex();
                    } else {
                        varComment += " + 0x" + tmp.toHex();
                    }
                }
            }
        }
        return varComment;
    }

public:
    virtual void startInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
        Super::startInstruction(insn);
        if (::mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            ::mlog[DEBUG] <<"  +-------------------------------------------------\n"
                          <<"  | " <<unparseInstructionWithAddress(insn) <<"\n"
                          <<"  +-------------------------------------------------\n"
                          <<"    state before instruction:\n"
                          <<(*get_state() + fmt);
        }
    }

    virtual void finishInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE {
        if (::mlog[DEBUG]) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
            ::mlog[DEBUG] <<"    state after instruction:\n" <<(*get_state()+fmt);
        }
        Super::finishInstruction(insn);
    }
    
    /** Read memory.
     *
     *  If multi-path is enabled, then return a new memory expression that describes the process of reading a value from the
     *  specified address; otherwise, actually read the value and return it.  In any case, record some information about the
     *  address that's being read if we've never seen it before. */
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        BaseSemantics::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);
        if (!get_insn())
            return retval;                              // not called from dispatcher on behalf of an instruction

        // Save a description of the variable
        InsnSemanticsExpr::TreeNodePtr valExpr = SymbolicSemantics::SValue::promote(retval)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->is_variable()) {
            std::string comment = commentForVariable(addr, "read");
            varComment(valExpr->isLeafNode()->toString(), comment);
        }
        
        // Save a description for its addresses
        size_t nBytes = dflt->get_width() / 8;
        for (size_t i=0; i<nBytes; ++i) {
            SymbolicSemantics::SValuePtr va = SymbolicSemantics::SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                std::string comment = commentForVariable(addr, "read", i, nBytes);
                varComment(va->get_expression()->isLeafNode()->toString(), comment);
            }
        }
        return retval;
    }

    /** Write value to memory.
     *
     *  If multi-path is enabled, then return a new memory expression that updates memory with a new address/value pair;
     *  otherwise update the memory directly.  In any case, record some information about the address that was written if we've
     *  never seen it before. */
    virtual void writeMemory(const RegisterDescriptor &segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &value,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        Super::writeMemory(segreg, addr, value, cond);

        // Save a description of the variable
        InsnSemanticsExpr::TreeNodePtr valExpr = SymbolicSemantics::SValue::promote(value)->get_expression();
        if (valExpr->isLeafNode() && valExpr->isLeafNode()->is_variable()) {
            std::string comment = commentForVariable(addr, "write");
            varComment(valExpr->isLeafNode()->toString(), comment);
        }

        // Save a description for its addresses
        size_t nBytes = value->get_width() / 8;
        for (size_t i=0; i<nBytes; ++i) {
            SymbolicSemantics::SValuePtr va = SymbolicSemantics::SValue::promote(add(addr, number_(addr->get_width(), i)));
            if (va->get_expression()->isLeafNode()) {
                std::string comment = commentForVariable(addr, "read", i, nBytes);
                varComment(va->get_expression()->isLeafNode()->toString(), comment);
            }
        }
    }
};

/** Build a new virtual CPU. */
BaseSemantics::DispatcherPtr
buildVirtualCpu(const P2::Partitioner &partitioner) {

    // Augment the register dictionary with information about the execution path constraint.
    static RegisterDictionary *myRegs = NULL;
    if (NULL==myRegs) {
        myRegs = new RegisterDictionary("findPath");
        myRegs->insert(partitioner.instructionProvider().registerDictionary());
        myRegs->insert("path", REG_PATH);

        // Where are return values stored?
        const RegisterDescriptor *r = NULL;
        if ((r = myRegs->lookup("rax")) || (r = myRegs->lookup("eax")) || (r = myRegs->lookup("ax"))) {
            REG_RETURN = *r;
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }
    }
    
    // We could use an SMT solver here also, but it seems to slow things down more than speed them up.
    SMTSolver *solver = NULL;
    RiscOperatorsPtr ops = RiscOperators::instance(myRegs, solver);
    BaseSemantics::DispatcherPtr cpu = partitioner.instructionProvider().dispatcher()->create(ops);

    // Initialize the stack pointer
    if (settings.initialStackPtr) {
        const RegisterDescriptor REG_SP = partitioner.instructionProvider().stackPointerRegister();
        ops->writeRegister(REG_SP, ops->number_(REG_SP.get_nbits(), *settings.initialStackPtr));
    }
    
    return cpu;
}

/** Process instructions for one basic block on the specified virtual CPU. */
void
processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    using namespace InsnSemanticsExpr;

    ASSERT_not_null(bblock);
    
    // Update the path constraint "register"
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    BaseSemantics::SValuePtr ip = ops->readRegister(cpu->instructionPointerRegister());
    BaseSemantics::SValuePtr va = ops->number_(ip->get_width(), bblock->address());
    BaseSemantics::SValuePtr pathConstraint = ops->equal(ip, va);
    ops->writeRegister(REG_PATH, pathConstraint);

    // Process each instruction in the basic block
    try {
        BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
            if (pathInsnIndex != size_t(-1))
                ops->pathInsnIndex(pathInsnIndex++);
            cpu->processInstruction(insn);
        }
    } catch (const BaseSemantics::Exception &e) {
        ::mlog[ERROR] <<"semantics failed: " <<e <<"\n";
        return;
    }
}

/** Process a function summary vertex. */
void
processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &vertex, const BaseSemantics::DispatcherPtr &cpu,
                       size_t pathInsnIndex) {
    ASSERT_require(functionSummaries.exists(vertex));
    const FunctionSummary &summary = functionSummaries[vertex];
    //::mlog[INFO] <<"processFunctionSummary: " <<summary.function->printableName() <<"\n";

    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    if (pathInsnIndex != size_t(-1))
        ops->pathInsnIndex(pathInsnIndex);

    // Make the function return an unknown value
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN.get_nbits()));
    std::string comment = "return value from " + summary.function->printableName() + "\n" +
                          "at path position #" + StringUtility::numberToString(ops->pathInsnIndex());
    ops->varComment(retval->get_expression()->isLeafNode()->toString(), comment);
    ops->writeRegister(REG_RETURN, retval);

    // Cause the function to return by popping the return target address off the top of the stack
    BaseSemantics::SValuePtr stackPointer = ops->readRegister(cpu->stackPointerRegister());
    BaseSemantics::SValuePtr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                            ops->undefined_(stackPointer->get_width()), ops->boolean_(true));
    stackPointer = ops->add(stackPointer, ops->number_(stackPointer->get_width(), returnTarget->get_width()/8));
    ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
    ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);
}

void
showPathEvidence(SMTSolver &solver, const RiscOperatorsPtr &ops) {
    std::cout <<"  Inputs sufficient to cause path to be taken:\n";
    std::vector<std::string> enames = solver.evidence_names();
    if (enames.empty()) {
        std::cout <<"    not available (or none necessary)\n";
    } else {
        BOOST_FOREACH (const std::string &ename, enames) {
            if (ename.substr(0, 2) == "0x") {
                std::cout <<"    memory[" <<ename <<"] == " <<*solver.evidence_for_name(ename) <<"\n";
            } else {
                std::cout <<"    " <<ename <<" == " <<*solver.evidence_for_name(ename) <<"\n";
            }
            std::string varComment;
            if (ops->varComments().getOptional(ename).assignTo(varComment))
                std::cout <<StringUtility::prefixLines(varComment, "      ") <<"\n";
        }
    }
}

P2::ControlFlowGraph
generateTopLevelPaths(const P2::ControlFlowGraph &cfg, const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                      const P2::CfgConstVertexSet &cfgEndVertices, const P2::CfgConstVertexSet &cfgAvoidVertices,
                      const P2::CfgConstEdgeSet &cfgAvoidEdges, P2::CfgVertexMap &vmap /*out*/) {
    vmap.clear();
    P2::ControlFlowGraph paths = findPathsNoCalls(cfg, cfgBeginVertex, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges, vmap);
    if (!vmap.forward().exists(cfgBeginVertex)) {
        ::mlog[WARN] <<"no paths found\n";
    } else {
        ::mlog[INFO] <<"paths graph has " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
    }
    return paths;
}

// Converts vertices from one graph to another based on the vmap
P2::CfgConstVertexSet
cfgToPaths(const P2::CfgConstVertexSet &vertices, const P2::CfgVertexMap &vmap) {
    P2::CfgConstVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, vertices) {
        if (vmap.forward().exists(vertex))
            retval.insert(vmap.forward()[vertex]);
    }
    return retval;
}

void
insertCallSummary(P2::ControlFlowGraph &paths /*in,out*/, const P2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const P2::ControlFlowGraph &cfg, const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge) {
    ASSERT_require(cfg.isValidEdge(cfgCallEdge));
    P2::ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();
    ASSERT_require(cfgCallTarget->value().type() == P2::V_BASIC_BLOCK);
    ASSERT_not_null(cfgCallTarget->value().function());

    P2::ControlFlowGraph::VertexIterator summaryVertex = paths.insertVertex(P2::CfgVertex(P2::V_USER_DEFINED));
    paths.insertEdge(pathsCallSite, summaryVertex, P2::CfgEdge(P2::E_FUNCTION_CALL));
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, P2::findCallReturnEdges(pathsCallSite))
        paths.insertEdge(summaryVertex, callret->target(), P2::CfgEdge(P2::E_FUNCTION_RETURN));

    FunctionSummary summary(cfgCallTarget->value().function());
    functionSummaries.insert(summaryVertex, summary);
}

/** Process one path. Given a path, determine if the path is feasible.  If @p showResults is set, then emit information about
 *  the initial conditions that cause this path to be taken. */
SMTSolver::Satisfiable
singlePathFeasibility(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths, const P2::CfgPath &path,
                      bool emitResults) {
    using namespace rose::BinaryAnalysis::InsnSemanticsExpr;     // TreeNode, InternalNode, LeafNode

    static int npaths = -1;
    ++npaths;
    Stream info(::mlog[INFO]);
    Stream error(::mlog[ERROR]);
    Stream debug(::mlog[DEBUG]);
    info <<"path #" <<npaths <<" with " <<StringUtility::plural(path.nVertices(), "vertices", "vertex") <<"\n";

    std::string graphVizFileName;
    if (ALL_PATHS == settings.graphVizOutput) {
        graphVizFileName = printGraphViz(partitioner, paths, path, npaths);
        info <<"  saved as \"" <<StringUtility::cEscape(graphVizFileName) <<"\"\n";
    }

    YicesSolver solver;
    solver.set_debug(settings.debugSmtSolver ? stderr : NULL);
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    ops->writeRegister(REG_PATH, ops->boolean_(true)); // start of path is always feasible
    ops->writeRegister(cpu->instructionPointerRegister(),
                       ops->number_(cpu->instructionPointerRegister().get_nbits(), virtualAddress(path.frontVertex())));
    std::vector<InsnSemanticsExpr::TreeNodePtr> pathConstraints;

    size_t pathInsnIndex = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, path.edges()) {
        if (pathEdge->source()->value().type() == P2::V_BASIC_BLOCK) {
            processBasicBlock(pathEdge->source()->value().bblock(), cpu, pathInsnIndex);
            pathInsnIndex += pathEdge->source()->value().bblock()->instructions().size();
        } else if (pathEdge->source()->value().type() == P2::V_USER_DEFINED) {
            processFunctionSummary(pathEdge->source(), cpu, pathInsnIndex);
            ++pathInsnIndex;
        } else {
            error <<"cannot compute path feasibility; invalid vertex type at "
                  <<partitioner.vertexName(pathEdge->source()) <<"\n";
            return SMTSolver::SAT_NO;
        }
        BaseSemantics::SValuePtr ip = ops->readRegister(partitioner.instructionProvider().instructionPointerRegister());
        if (ip->is_number()) {
            if (ip->get_number() != virtualAddress(pathEdge->target())) {
                // Executing the path forces us to go a different direction than where the path indicates we should go. We
                // don't need an SMT solver to tell us that when the values are just integers.
                info <<"  not feasible according to ROSE semantics\n";
                return SMTSolver::SAT_NO;
            }
        } else {
            LeafNodePtr targetVa = LeafNode::create_integer(ip->get_width(), virtualAddress(pathEdge->target()));
            TreeNodePtr constraint = InternalNode::create(1, OP_EQ,
                                                          targetVa, SymbolicSemantics::SValue::promote(ip)->get_expression());
            pathConstraints.push_back(constraint);
        }
    }

    // Are the constraints satisfiable.  Empty constraints are tivially satisfiable.
    SMTSolver::Satisfiable isSatisfied = SMTSolver::SAT_UNKNOWN;
    if (settings.multiPathSmt && !pathConstraints.empty()) {
        // If we give the SMT solver an equation built from multiple paths, then we don't care about which path was taken to
        // get from point A to point B, just that some path could be taken.  Therefore, we just need to be sure that the final
        // value for the instruction pointer register is point B.
        isSatisfied = solver.satisfiable(pathConstraints.back());
    } else {
        isSatisfied = solver.satisfiable(pathConstraints);
    }
    if (!emitResults)
        return isSatisfied;
    
    if (isSatisfied == SMTSolver::SAT_YES) {
        info <<"  path is feasible\n";
        std::cout <<"Found feasible path #" <<npaths
                  <<" with " <<StringUtility::plural(path.nVertices(), "vertices", "vertex") <<".\n";

        if (FEASIBLE_PATHS == settings.graphVizOutput)
            graphVizFileName = printGraphViz(partitioner, paths, path, npaths);
        if (!graphVizFileName.empty())
            std::cout <<"  Saved as \"" <<StringUtility::cEscape(graphVizFileName) <<"\"\n";

        std::cout <<"  Path:\n";
        size_t insnIdx=0, pathIdx=0;
        if (path.nEdges() == 0) {
            std::cout <<"    path is trivial (contains only vertex " <<partitioner.vertexName(path.frontVertex()) <<")\n";
        } else {
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &pathVertex, path.vertices()) {
                if (0==pathIdx) {
                    std::cout <<"    at path vertex " <<partitioner.vertexName(pathVertex) <<"\n";
                } else {
                    std::cout <<"    via path edge " <<partitioner.edgeName(path.edges()[pathIdx-1]) <<"\n";
                }
                if (settings.showInstructions) {
                    if (pathVertex->value().type() == P2::V_BASIC_BLOCK) {
                        BOOST_FOREACH (SgAsmInstruction *insn, pathVertex->value().bblock()->instructions()) {
                            std::cout <<"      #" <<std::setw(5) <<std::left <<insnIdx++
                                      <<" " <<unparseInstructionWithAddress(insn) <<"\n";
                        }
                    } else if (pathVertex->value().type() == P2::V_USER_DEFINED) {
                        ASSERT_require(functionSummaries.exists(pathVertex));
                        const FunctionSummary &summary = functionSummaries[pathVertex];
                        std::cout <<"      #" <<std::setw(5) <<std::left <<insnIdx++
                                  <<" summary for " <<summary.function->printableName() <<"\n";
                    }
                }
                ++pathIdx;
            }
        }

        if (settings.showConstraints) {
            std::cout <<"  Constraints:\n";
            if (pathConstraints.empty()) {
                std::cout <<"    none\n";
            } else {
                size_t idx = 0;
                BOOST_FOREACH (const InsnSemanticsExpr::TreeNodePtr &constraint, pathConstraints)
                    std::cout <<"    #" <<std::setw(5) <<std::left <<idx++ <<" " <<*constraint <<"\n";
            }
        }

        showPathEvidence(solver, ops);

        if (settings.showFinalState) {
            SymbolicSemantics::Formatter fmt = symbolicFormat("    ");
            std::cout <<"  Machine state at end of path (prior to entering " <<partitioner.vertexName(path.backVertex()) <<")\n"
                      <<(*ops->get_state() + fmt);
        }
    } else if (isSatisfied == SMTSolver::SAT_NO) {
        info <<"  not feasible according to SMT solver\n";
    } else {
        ASSERT_require(isSatisfied == SMTSolver::SAT_UNKNOWN);
        error <<"SMT solver could not determine satisfiability\n";
    }
    return isSatisfied;
}

/** Find paths and process them one at a time until we've found the desired number of feasible paths. */
void
findAndProcessSinglePaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                          const P2::CfgConstVertexSet &cfgEndVertices, const P2::CfgConstVertexSet &cfgAvoidVertices,
                          const P2::CfgConstEdgeSet &cfgAvoidEdges) {

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    Stream info(::mlog[INFO]);
    P2::CfgVertexMap vmap;                              // relates CFG vertices to path vertices
    P2::ControlFlowGraph paths = generateTopLevelPaths(partitioner.cfg(), cfgBeginVertex, cfgEndVertices, cfgAvoidVertices,
                                                       cfgAvoidEdges, vmap /*out*/);
    P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex = vmap.forward()[cfgBeginVertex];
    P2::CfgConstVertexSet pathsEndVertices = cfgToPaths(cfgEndVertices, vmap);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    P2::CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices.begin(), cfgEndVertices.end());

    // Depth-first traversal of the "paths". When a function call is encountered we do one of two things: either expand the
    // called function into the paths-CFG and replace the call-ret edge with an actual function call and return edges, or do
    // nothing but skip over the function call.  When expanding a function call, we want to insert only those edges and
    // vertices that can participate in a path from the callee's entry point to any of its returning points.
    P2::CfgPath path(pathsBeginVertex);
    while (!path.isEmpty()) {
        if (path.nVisits(path.backVertex()) > settings.vertexVisitLimit) {
            // Path visits the final vertex too many times. Backtrack to find some other path.
            path.backtrack();
        } else if (pathsEndVertices.find(path.backVertex()) != pathsEndVertices.end()) {
            // We've reached the end of the path. Backtrack and follow a different path.
            if (singlePathFeasibility(partitioner, paths, path, true/*emit results*/) == SMTSolver::SAT_YES) {
                if (0 == --settings.maxPaths) {
                    info <<"terminating because the maximum number of feasiable paths has been found\n";
                    exit(0);
                }
            }
            path.backtrack();
        } else if (pathEndsWithFunctionCall(partitioner, path) && !P2::findCallReturnEdges(path.backVertex()).empty()) {
            // Inline callee paths into the paths graph, but continue to avoid any paths that go through user-specified
            // avoidance vertices and edges. We can modify the paths graph during the traversal because we're modifying parts
            // of the graph that aren't part of the current path.  This is where having insert- and erase-stable graph
            // iterators is a huge help!
            P2::ControlFlowGraph::ConstVertexIterator pathsCallSite = path.backVertex();
            P2::ControlFlowGraph::ConstVertexIterator cfgCallSite = partitioner.findPlaceholder(virtualAddress(pathsCallSite));
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, P2::findCallEdges(cfgCallSite)) {
                if (shouldSummarizeCall(path, cfgCallEdge->target())) {
                    insertCallSummary(paths, pathsCallSite, partitioner.cfg(), cfgCallEdge);
                } else if (shouldInline(path, cfgCallEdge->target())) {
                    info <<"inlining function call paths at vertex " <<partitioner.vertexName(pathsCallSite) <<"\n";
                    P2::insertCalleePaths(paths, pathsCallSite,
                                          partitioner.cfg(), cfgCallSite, calleeCfgAvoidVertices, cfgAvoidEdges);
                }
            }

            // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion.
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callRetEdge, P2::findCallReturnEdges(pathsCallSite))
                paths.eraseEdge(callRetEdge);
            P2::eraseUnreachablePaths(paths, pathsBeginVertex, pathsEndVertices, vmap /*in,out*/, path /*in,out*/);
            info <<"paths graph has " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                 <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
            ASSERT_require(!paths.isEmpty() || path.isEmpty());
        } else if (path.backVertex()->nOutEdges() == 0) {
            // We've reached a dead end. This shouldn't normally happen since we're traversing a the paths-CFG and would have
            // caught this case in the previous "if" condition. I.e., the only vertices in the paths-CFG that don't have out
            // edges are those in the endVertices set.
            ASSERT_not_reachable("vertex " + partitioner.vertexName(path.backVertex()) + " has no out edges (output in x.dot)");
        } else {
            // Path is incomplete, so append another edge.  But if the path so far is provably not feasible, then don't bother
            // considering any path that has the current path as a prefix.
            path.pushBack(path.backVertex()->outEdges().begin());
            if (singlePathFeasibility(partitioner, paths, path, false/*quiet*/) == SMTSolver::SAT_NO)
                path.backtrack();
        }
    }
}


/** Merge states for multi-path feasibility analysis. Given two paths, such as when control flow merges after an "if"
 * statement, compute a state that represents both paths.  The new state that's returned will consist largely of ite
 * expressions. */
BaseSemantics::StatePtr
mergeMultipathStates(const BaseSemantics::RiscOperatorsPtr &ops,
                     const BaseSemantics::StatePtr &s1, const BaseSemantics::StatePtr &s2) {
    using namespace InsnSemanticsExpr;

    // The instruction pointer constraint to use values from s1, otherwise from s2.
    SymbolicSemantics::SValuePtr s1Constraint = SymbolicSemantics::SValue::promote(s1->readRegister(REG_PATH, ops.get()));

    Stream debug(::mlog[DEBUG]);
    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"  +-------------------------------------------------\n"
              <<"  | Merging states for next instruction's input\n"
              <<"  +-------------------------------------------------\n"
              <<"    State s1:\n" <<(*s1 + fmt)
              <<"    State s2:\n" <<(*s2 + fmt);
    }

    // Merge register states s1reg and s2reg into mergedReg
    BaseSemantics::RegisterStateGenericPtr s1reg = BaseSemantics::RegisterStateGeneric::promote(s1->get_register_state());
    BaseSemantics::RegisterStateGenericPtr s2reg = BaseSemantics::RegisterStateGeneric::promote(s2->get_register_state());
    BaseSemantics::RegisterStateGenericPtr mergedReg = BaseSemantics::RegisterStateGeneric::promote(s1reg->clone());
    BOOST_FOREACH (const BaseSemantics::RegisterStateGeneric::RegPair &pair, s2reg->get_stored_registers()) {
        if (s1reg->is_partly_stored(pair.desc)) {
            // The register exists (at least partly) in both states, so merge its values.
            BaseSemantics::SValuePtr mergedVal = ops->ite(s1Constraint,
                                                          s1->readRegister(pair.desc, ops.get()),
                                                          s2->readRegister(pair.desc, ops.get()));
            mergedReg->writeRegister(pair.desc, mergedVal, ops.get());
        } else {
            // The register exists only in the s2 state, so copy it.
            mergedReg->writeRegister(pair.desc, pair.value, ops.get());
        }
    }
    mergedReg->erase_register(REG_PATH, ops.get()); // will be updated separately in processBasicBlock

    // Merge memory states s1mem and s2mem into mergedMem
    BaseSemantics::SymbolicMemoryPtr s1mem = BaseSemantics::SymbolicMemory::promote(s1->get_memory_state());
    BaseSemantics::SymbolicMemoryPtr s2mem = BaseSemantics::SymbolicMemory::promote(s2->get_memory_state());
    TreeNodePtr memExpr1 = s1mem->expression();
    TreeNodePtr memExpr2 = s2mem->expression();
    TreeNodePtr mergedExpr = InternalNode::create(memExpr1->get_nbits(), OP_ITE, s1Constraint->get_expression(),
                                                  memExpr1, memExpr2);
    BaseSemantics::SymbolicMemoryPtr mergedMem = BaseSemantics::SymbolicMemory::promote(s1mem->clone());
    mergedMem->expression(mergedExpr);

    return ops->get_state()->create(mergedReg, mergedMem);
}

/** Process all paths at once by sending everything to the SMT solver. */
void
multiPathFeasibility(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths,
                     const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex,
                     const P2::CfgConstVertexSet &pathsEndVertices) {
    using namespace Sawyer::Container::Algorithm;
    using namespace InsnSemanticsExpr;

    Stream debug(::mlog[DEBUG]);
    Stream info(::mlog[INFO]);
    info <<"testing multi-path feasibility for paths graph with "
         <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
         <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";

    if (settings.graphVizOutput==ALL_PATHS) {
        char dotFileName[256];
        sprintf(dotFileName, "%sallpaths.dot", settings.graphVizPrefix.c_str());
        std::ofstream dotFile(dotFileName);
        printGraphViz(dotFile, partitioner, paths, pathsBeginVertex, pathsEndVertices);
        info <<"  saved as \"" <<StringUtility::cEscape(dotFileName) <<"\"\n";
    }

    // Cycles are not allowed for multi-path feasibility analysis. Loops and recursion must have already been unrolled.
    ASSERT_require(paths.isValidVertex(pathsBeginVertex));
    P2::CfgConstEdgeSet backEdges = findBackEdges(paths, pathsBeginVertex);
    if (!backEdges.empty()) {
        ::mlog[ERROR] <<"Cyclic paths are not allowed for multi-path feasibility analysis. The back edges are:\n";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, backEdges)
            ::mlog[ERROR] <<"  " <<partitioner.edgeName(edge) <<"\n";
        ASSERT_not_implemented("cyclic paths not allowed for multi-path feasibility analysis");
    }

    info <<"  building path constraints expression\n";
    size_t pathInsnIndex = -1;
    ASSERT_require(pathsBeginVertex->nInEdges() == 0);
    std::vector<BaseSemantics::StatePtr> outState(paths.nVertices());
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator pathsEndVertex, pathsEndVertices) {
        // Build the semantics framework and initialize the path constraints.
        YicesSolver solver;
        solver.set_debug(settings.debugSmtSolver ? stderr : NULL);
        BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
        RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
        ops->writeRegister(REG_PATH, ops->boolean_(true)); // start of path is always feasible
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().get_nbits(), virtualAddress(pathsBeginVertex)));

        // This loop is a little bit like a data-flow, except we don't have to worry about cycles in the CFG, which simplifies
        // things quite a bit. We can process the vertices by doing a depth-first traversal starting at the end, and building
        // the intermediate states as we back out of the traversal.  This guarantees that the final states for each of a
        // vertex's CFG predecessors are already computed when we're ready to compute this vertex's final state.
        typedef DepthFirstReverseGraphTraversal<const P2::ControlFlowGraph> Traversal;
        for (Traversal t(paths, pathsEndVertex, LEAVE_VERTEX); t; ++t) {
            // Build the initial state by merging all incoming states.
            BaseSemantics::StatePtr state;
            ASSERT_require(outState[t.vertex()->id()]==NULL);
            if (0 == t.vertex()->nInEdges()) {
                ASSERT_require(t.vertex() == pathsBeginVertex);
                state = cpu->get_state();               // the initial state
            } else {
                BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, t.vertex()->inEdges()) {
                    P2::ControlFlowGraph::ConstVertexIterator predecessorVertex = edge.source();
                    ASSERT_not_null(outState[predecessorVertex->id()]);
                    if (edge.id() == t.vertex()->inEdges().begin()->id()) {
                        state = outState[predecessorVertex->id()]->clone();
                    } else {
                        state = mergeMultipathStates(ops, state, outState[predecessorVertex->id()]);
                    }
                }
            }
            
            // Compute and save the final state. The final state for the ending vertex is the same as its initial state because
            // we want to "get to" the final vertex, not necessarily "get to the end of" the final vertex.
            ops->set_state(state);
            if (t.vertex() != pathsEndVertex)
                processBasicBlock(t.vertex()->value().bblock(), cpu, pathInsnIndex);
            outState[t.vertex()->id()] = cpu->get_state()->clone();
        }
        ASSERT_not_null(outState[pathsEndVertex->id()]);
        if (settings.showFinalState) {
            SymbolicSemantics::Formatter fmt = symbolicFormat();
            std::cerr <<"Final state:\n" <<(*cpu->get_state() + fmt);
        }
        
        // Final path expression
        BaseSemantics::SValuePtr path = ops->readRegister(REG_PATH);
        TreeNodePtr constraint = InternalNode::create(1, OP_EQ,
                                                      LeafNode::create_integer(1, 1),
                                                      SymbolicSemantics::SValue::promote(path)->get_expression());
        info <<"  constraints expression has " <<StringUtility::plural(constraint->nnodes(), "terms") <<"\n";
        if (settings.showConstraints) {
            std::cout <<"  Constraints:\n";
            std::cout <<"    " <<*constraint <<"\n";
        }

        // Is the constraint satisfiable?
        info <<"  invoking SMT solver\n";
        SMTSolver::Satisfiable isSatisfied = solver.satisfiable(constraint);
        if (isSatisfied == SMTSolver::SAT_YES) {
            info <<"  constraints are satisfiable\n";
            std::cout <<"Found a feasible path (specified path is not available)\n";
            showPathEvidence(solver, ops);
        } else if (isSatisfied == SMTSolver::SAT_NO) {
            info <<"  constraints are not satisfiable\n";
        } else {
            info <<"  constraint satisfiability could not be determined.\n";
        }
    }
};

/** Find paths and process them all at once. */
void
findAndProcessMultiPaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                         const P2::CfgConstVertexSet &cfgEndVertices, const P2::CfgConstVertexSet &cfgAvoidVertices,
                         const P2::CfgConstEdgeSet &cfgAvoidEdges) {

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    Stream info(::mlog[INFO]);
    P2::CfgVertexMap vmap;                              // relates CFG vertices to path vertices
    P2::ControlFlowGraph paths = generateTopLevelPaths(partitioner.cfg(), cfgBeginVertex, cfgEndVertices, cfgAvoidVertices,
                                                       cfgAvoidEdges, vmap /*out*/);
    P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex = vmap.forward()[cfgBeginVertex];
    P2::CfgConstVertexSet pathsEndVertices = cfgToPaths(cfgEndVertices, vmap);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    P2::CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices.begin(), cfgEndVertices.end());

    // FIXME[Robb P. Matzke 2015-04-14]: we need to break cycles before we call this
    multiPathFeasibility(partitioner, paths, pathsBeginVertex, pathsEndVertices);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    ::mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Parse the command-line
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv).unreachedArgs();
    P2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Disassemble and partition
    Stream info(::mlog[INFO] <<"disassembling");
    engine.postPartitionAnalyses(false);
    engine.load(specimenNames);
#if 0 // [Robb P. Matzke 2015-02-27]
    // Removing write access (if semantics is enabled) makes it so more indirect jumps have known successors even if those
    // successors are only a subset of what's possible at runtime.
    engine.memoryMap().any().changeAccess(0, MemoryMap::WRITABLE);
#endif
    P2::Partitioner partitioner = engine.createTunedPartitioner();
    partitioner.enableSymbolicSemantics(true);
    engine.partition(partitioner);
    info <<"; done\n";

    // We must have instruction semantics in order to calculate path feasibility, so we might was well check that up front
    // before we spend a lot of time looking for paths.
    if (NULL == partitioner.instructionProvider().dispatcher())
        throw std::runtime_error("no instruction semantics for specimen architecture");

    // Find vertex at which all paths begin
    P2::ControlFlowGraph::ConstVertexIterator beginVertex = vertexForInstruction(partitioner, settings.beginVertex);
    if (beginVertex == partitioner.cfg().vertices().end())
        throw std::runtime_error("no --begin vertex at " + settings.beginVertex);

    // Find vertices that terminate paths
    P2::CfgConstVertexSet endVertices;
    BOOST_FOREACH (const std::string &nameOrVa, settings.endVertices) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = vertexForInstruction(partitioner, nameOrVa);
        if (vertex == partitioner.cfg().vertices().end())
            throw std::runtime_error("no --end vertex at " + nameOrVa);
        endVertices.insert(vertex);
    }
    if (endVertices.empty())
        throw std::runtime_error("no --end vertex specified; see --help");

    // Which vertices should be avoided
    P2::CfgConstVertexSet avoidVertices;
    BOOST_FOREACH (const std::string &nameOrVa, settings.avoidVertices) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = vertexForInstruction(partitioner, nameOrVa);
        if (vertex == partitioner.cfg().vertices().end())
            throw std::runtime_error("no vertex at " + nameOrVa);
        avoidVertices.insert(vertex);
    }

    // Which edges should be avoided
    P2::CfgConstEdgeSet avoidEdges;
    ASSERT_require(0 == settings.avoidEdges.size() % 2);
    for (size_t i=0; i<settings.avoidEdges.size(); i+=2) {
        const std::string &source = settings.avoidEdges[i+0];
        const std::string &target = settings.avoidEdges[i+1];
        P2::ControlFlowGraph::ConstEdgeIterator edge = edgeForInstructions(partitioner, source, target);
        if (edge == partitioner.cfg().edges().end()) {
            throw std::runtime_error("no edge from " + source + " to " + target);
        }
        avoidEdges.insert(edge);
    }

    // Show the configuration
    info <<"start at vertex: " <<partitioner.vertexName(beginVertex) <<";\n";
    info <<"end at vertices:";
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, endVertices)
        info <<" " <<partitioner.vertexName(vertex) <<";";
    info <<"\n";
    if (!avoidVertices.empty()) {
        info <<"avoiding the following vertices:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, avoidVertices)
            info <<" " <<partitioner.vertexName(vertex) <<";";
        info <<"\n";
    }
    if (!avoidEdges.empty()) {
        info <<"avoiding the following edges:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, avoidEdges)
            info <<" " <<partitioner.edgeName(edge) <<";";
        info <<"\n";
    }

    // Initialize info about how addresses map to source lines if that info is available
    if (SgProject *project = SageInterface::getProject())
        srcMapper = DwarfLineMapper(project);
    
    // Process individual paths
    if (settings.multiPathSmt) {
        findAndProcessMultiPaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
    } else {
        findAndProcessSinglePaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
    }
}
