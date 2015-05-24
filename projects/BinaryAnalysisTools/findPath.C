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
#include <sawyer/ProgressBar.h>
#include <sawyer/Stopwatch.h>
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
    std::string beginVertex;                            // address or function name where paths should begin
    std::vector<std::string> endVertices;               // addresses or function names where paths should end
    std::vector<std::string> avoidVertices;             // vertices to avoid in any path
    std::vector<std::string> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    std::vector<rose_addr_t> summarizeFunctions;        // functions to summarize
    size_t maxRecursionDepth;                           // max recursion depth when expanding function calls
    size_t maxCallDepth;                                // max function call depth when expanding function calls
    size_t maxPathLength;                               // maximum length of any path considered
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
        : beginVertex("_start"), maxRecursionDepth(4), maxCallDepth(100), maxPathLength(1600), vertexVisitLimit(1),
          showInstructions(true), showConstraints(false), showFinalState(false), showFunctionSubgraphs(true),
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
    P2::ControlFlowGraph::ConstVertexIterator cfgFuncVertex;
    int64_t stackDelta;
    FunctionSummary(): stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}
    FunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &cfgFuncVertex, uint64_t stackDelta)
        : cfgFuncVertex(cfgFuncVertex), stackDelta(stackDelta) {}
    std::string name() const {
        if (cfgFuncVertex->value().type() == P2::V_BASIC_BLOCK) {
            if (P2::Function::Ptr function = cfgFuncVertex->value().function()) {
                if (function->address() == cfgFuncVertex->value().address())
                    return function->printableName();
            }
        }
        return P2::Partitioner::vertexName(*cfgFuncVertex);
    }
};
typedef Sawyer::Container::Map<P2::ControlFlowGraph::ConstVertexIterator, FunctionSummary> FunctionSummaries;
static FunctionSummaries functionSummaries;

// Stack of states per vertex
typedef Sawyer::Container::Map<P2::ControlFlowGraph::ConstVertexIterator, std::vector<BaseSemantics::StatePtr> > StateStacks;

// Describe and parse the command-line
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "finds paths in control flow graph";
    std::string description =
        "Parses, loads, disassembles, and partitions the specimen and then looks for control flow paths. A path "
        "is a sequence of edges in the global control flow graph beginning and ending at user-specified vertices. "
        "A vertex is specified as either the name of a function, or an address contained in a basic block. Addresses "
        "can be decimal, octal, or hexadecimal.";

    Parser parser = engine.commandLineParser(purpose, description);
    
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

    cfg.insert(Switch("max-recursion")
               .argument("n", nonNegativeIntegerParser(settings.maxRecursionDepth))
               .doc("Maximum number of times a particular function can appear on a path when expanding paths through "
                    "function calls.  The default is " +
                    StringUtility::numberToString(settings.maxRecursionDepth) + "."));

    cfg.insert(Switch("max-call-depth")
               .argument("n", nonNegativeIntegerParser(settings.maxCallDepth))
               .doc("Maximum function call depth when expanding paths through function calls. The default is " +
                    StringUtility::numberToString(settings.maxCallDepth) + "."));

    cfg.insert(Switch("max-path-length")
               .argument("n", nonNegativeIntegerParser(settings.maxPathLength))
               .doc("Maximum length of any path considered. The default is " +
                    StringUtility::plural(settings.maxPathLength, "CFG vertices", "CFG vertex") + "."));

    cfg.insert(Switch("max-vertex-visits")
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

    cfg.insert(Switch("summarize-function")
               .argument("va", listParser(nonNegativeIntegerParser(settings.summarizeFunctions)))
               .whichValue(SAVE_ALL)
               .explosiveLists(true)
               .doc("Comma-separated list (or multiple occurrences of this switch) of function entry addresses for "
                    "functions that should be summarized/approximated instead of traversed."));

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

    return parser.with(cfg).with(out).parse(argc, argv).apply().unreachedArgs();
}

static bool
hasVirtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK)
        return true;
    if (vertex->value().type() == P2::V_USER_DEFINED) {
        if (!functionSummaries.exists(vertex))
            return false;
        const FunctionSummary &summary = functionSummaries[vertex];
        return summary.cfgFuncVertex->value().type() == P2::V_BASIC_BLOCK;
    }
    return false;
}

static rose_addr_t
virtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK)
        return vertex->value().address();
    if (vertex->value().type() == P2::V_USER_DEFINED) {
        ASSERT_require(functionSummaries.exists(vertex));
        const FunctionSummary &summary = functionSummaries[vertex];
        ASSERT_require(summary.cfgFuncVertex->value().type() == P2::V_BASIC_BLOCK);
        return summary.cfgFuncVertex->value().address();
    }
    ASSERT_not_reachable("invalid vertex type");
}

static P2::ControlFlowGraph::ConstVertexIterator
pathToCfg(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) {
    if (hasVirtualAddress(pathVertex))
        return partitioner.findPlaceholder(virtualAddress(pathVertex));
    if (pathVertex->value().type() == P2::V_INDETERMINATE)
        return partitioner.indeterminateVertex();
    ASSERT_not_implemented("cannot convert path vertex to CFG vertex");
}

static P2::ControlFlowGraph::ConstVertexIterator
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
static P2::ControlFlowGraph::ConstEdgeIterator
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

static P2::ControlFlowGraph::ConstEdgeIterator
edgeForInstructions(const P2::Partitioner &partitioner, const std::string &sourceNameOrVa, const std::string &targetNameOrVa) {
    return edgeForInstructions(partitioner,
                               vertexForInstruction(partitioner, sourceNameOrVa),
                               vertexForInstruction(partitioner, targetNameOrVa));
}

// True if path vertex is a function call.
static bool
isFunctionCall(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &pathVertex) {
    P2::ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, pathVertex);
    ASSERT_require(partitioner.cfg().isValidVertex(cfgVertex));
    BOOST_FOREACH (P2::ControlFlowGraph::Edge edge, cfgVertex->outEdges()) {
        if (edge.value().type() == P2::E_FUNCTION_CALL)
            return true;
    }
    return false;
}

// True if path ends with a function call.
static bool
pathEndsWithFunctionCall(const P2::Partitioner &partitioner, const P2::CfgPath &path) {
    if (path.isEmpty())
        return false;
    P2::ControlFlowGraph::ConstVertexIterator pathVertex = path.backVertex();
    return isFunctionCall(partitioner, pathVertex);
}

/** Determines whether a function call should be summarized instead of inlined. */
static bool
shouldSummarizeCall(const P2::ControlFlowGraph::ConstVertexIterator &pathVertex,
                    const P2::ControlFlowGraph &cfg,
                    const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().function();
    if (!callee)
        return false;
    if (boost::ends_with(callee->name(), "@plt") || boost::ends_with(callee->name(), ".dll"))
        return true;                                    // this is probably dynamically linked ELF or PE function

    // Summarize if the user desires it.
    if (std::find(settings.summarizeFunctions.begin(), settings.summarizeFunctions.end(), callee->address()) !=
        settings.summarizeFunctions.end())
        return true;

    // If the called function calls too many more functions then summarize instead of inlining.  This helps avoid problems with
    // the Partitioner2 not being able to find reasonable function boundaries, especially for GNU libc.
    static const size_t maxCallsAllowed = 10;           // arbitrary
    size_t nCalls = 0;
    using namespace Sawyer::Container::Algorithm;
    typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(cfg, cfgCallTarget); t; ++t) {
        if (t.edge()->value().type() == P2::E_FUNCTION_CALL) {
            if (++nCalls > maxCallsAllowed)
                return true;
            t.skipChildren();
        } else if (t.edge()->value().type() == P2::E_FUNCTION_RETURN) {
            t.skipChildren();
        }
    }

    return false;
}

/** Determines whether a function call should be summarized instead of inlined. */
static bool
shouldSummarizeCall(const P2::CfgPath &path,
                    const P2::ControlFlowGraph &cfg,
                    const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    return shouldSummarizeCall(path.backVertex(), cfg, cfgCallTarget);
}

/** Determines whether a function call should be inlined. */
static bool
shouldInline(const P2::CfgPath &path, const P2::ControlFlowGraph::ConstVertexIterator &cfgCallTarget) {
    // We must inline indeterminte functions or else we'll end up removing the call and return edge, which is another way
    // of saying "we know there's no path through here" when in fact there could be.
    if (cfgCallTarget->value().type() == P2::V_INDETERMINATE)
        return true;

    // Don't let the call depth get too deep
    ssize_t callDepth = path.callDepth();
    ASSERT_require(callDepth >= 0);
    if ((size_t)callDepth >= settings.maxCallDepth)
        return false;

    // Don't let recursion get too deep
    if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
        return false;
    P2::Function::Ptr callee = cfgCallTarget->value().function();
    if (!callee)
        return false;
    callDepth = path.callDepth(callee);
    ASSERT_require(callDepth >= 0);
    if ((size_t)callDepth >= settings.maxRecursionDepth)
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
            return "summary for " + summary.name();
        }
        return Super::vertexLabel(vertex);
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
    gv.deselectParallelEdges();

    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &endVertex, endVertices)
        gv.vertexOrganization(endVertex).attributes().insert("fillcolor", exitColor.toHtml());
    gv.vertexOrganization(beginVertex).attributes().insert("fillcolor", entryColor.toHtml());

    typedef Sawyer::Container::Map<P2::ControlFlowGraph::ConstEdgeIterator, size_t> EdgeCounts;
    EdgeCounts edgeCounts;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, path.edges()) {
        gv.edgeOrganization(edge).attributes()
            .insert("penwidth", "3")
            .insert("arrowsize", "3")
            .insert("style", "solid")
            .insert("color", pathColor.toHtml());
        ++edgeCounts.insertMaybe(edge, 0);
    }

    BOOST_FOREACH (const EdgeCounts::Node &edgeCount, edgeCounts.nodes()) {
        if (edgeCount.value() > 1) {
            std::string label = gv.edgeOrganization(edgeCount.key()).label();
            label = P2::GraphViz::concatenate(label, "("+StringUtility::numberToString(edgeCount.value())+"x)", " ");
            gv.edgeOrganization(edgeCount.key()).label(label);
        }
    }

    gv.emit(out);
}

static std::string
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
static BaseSemantics::DispatcherPtr
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
static void
processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    using namespace InsnSemanticsExpr;

    ASSERT_not_null(bblock);
    
    // Update the path constraint "register"
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    BaseSemantics::SValuePtr ip = ops->readRegister(cpu->instructionPointerRegister());
    BaseSemantics::SValuePtr va = ops->number_(ip->get_width(), bblock->address());
    BaseSemantics::SValuePtr pathConstraint = ops->isEqual(ip, va);
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

/** Process an indeterminate block. This represents flow of control through an unknown address. */
static void
processIndeterminateBlock(const P2::ControlFlowGraph::ConstVertexIterator &vertex, const BaseSemantics::DispatcherPtr &cpu,
                          size_t pathInsnIndex) {
    ::mlog[WARN] <<"control flow passes through an indeterminate address at path position #" <<pathInsnIndex <<"\n";
}

/** Process a function summary vertex. */
static void
processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &vertex, const BaseSemantics::DispatcherPtr &cpu,
                       size_t pathInsnIndex) {
    ASSERT_require(functionSummaries.exists(vertex));
    const FunctionSummary &summary = functionSummaries[vertex];

    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    if (pathInsnIndex != size_t(-1))
        ops->pathInsnIndex(pathInsnIndex);

    // Make the function return an unknown value
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN.get_nbits()));
    std::string comment = "return value from " + summary.name() + "\n" +
                          "at path position #" + StringUtility::numberToString(ops->pathInsnIndex());
    ops->varComment(retval->get_expression()->isLeafNode()->toString(), comment);
    ops->writeRegister(REG_RETURN, retval);

    // Cause the function to return to the address stored at the top of the stack.
    BaseSemantics::SValuePtr stackPointer = ops->readRegister(cpu->stackPointerRegister());
    BaseSemantics::SValuePtr returnTarget = ops->readMemory(RegisterDescriptor(), stackPointer,
                                                            ops->undefined_(stackPointer->get_width()), ops->boolean_(true));
    ops->writeRegister(cpu->instructionPointerRegister(), returnTarget);

    // Pop some things from the stack.
    int64_t sd = summary.stackDelta != SgAsmInstruction::INVALID_STACK_DELTA ?
                 summary.stackDelta :
                 returnTarget->get_width() / 8;
    stackPointer = ops->add(stackPointer, ops->number_(stackPointer->get_width(), sd));
    ops->writeRegister(cpu->stackPointerRegister(), stackPointer);
}

static void
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

void
generateTopLevelPaths(const P2::ControlFlowGraph &cfg,
                      const P2::CfgConstVertexSet &cfgBeginVertices, const P2::CfgConstVertexSet &cfgEndVertices,
                      const P2::CfgConstVertexSet &cfgAvoidVertices, const P2::CfgConstEdgeSet &cfgAvoidEdges,
                      P2::ControlFlowGraph &paths /*out*/, P2::CfgVertexMap &vmap /*out*/) {
    vmap.clear();
    findInterFunctionPaths(cfg, paths /*out*/, vmap /*out*/,
                           cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
    if (paths.isEmpty()) {
        ::mlog[WARN] <<"no paths found\n";
    } else {
        ::mlog[INFO] <<"paths graph has " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
    }
}

// Converts vertices from one graph to another based on the vmap
static P2::CfgConstVertexSet
cfgToPaths(const P2::CfgConstVertexSet &vertices, const P2::CfgVertexMap &vmap) {
    P2::CfgConstVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, vertices) {
        if (vmap.forward().exists(vertex))
            retval.insert(vmap.forward()[vertex]);
    }
    return retval;
}

// Insert a function call summary
static void
insertCallSummary(P2::ControlFlowGraph &paths /*in,out*/, const P2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const P2::ControlFlowGraph &cfg, const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge) {
    ASSERT_require(cfg.isValidEdge(cfgCallEdge));
    P2::ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();
    P2::Function::Ptr function;
    if (cfgCallTarget->value().type() == P2::V_BASIC_BLOCK)
        function = cfgCallTarget->value().function();

    P2::ControlFlowGraph::VertexIterator summaryVertex = paths.insertVertex(P2::CfgVertex(P2::V_USER_DEFINED));
    paths.insertEdge(pathsCallSite, summaryVertex, P2::CfgEdge(P2::E_FUNCTION_CALL));
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, P2::findCallReturnEdges(pathsCallSite))
        paths.insertEdge(summaryVertex, callret->target(), P2::CfgEdge(P2::E_FUNCTION_RETURN));

    int64_t stackDelta = SgAsmInstruction::INVALID_STACK_DELTA;
    if (function && function->stackDelta().isCached()) {
        BaseSemantics::SValuePtr sd = function->stackDelta().get();
        if (sd->is_number())
            stackDelta = sd->get_number();
    }

    FunctionSummary summary(cfgCallTarget, stackDelta);
    functionSummaries.insert(summaryVertex, summary);
}

/** Process one path. Given a path, determine if the path is feasible.  If @p showResults is set, then emit information about
 *  the initial conditions that cause this path to be taken. */
static SMTSolver::Satisfiable
singlePathFeasibility(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths, const P2::CfgPath &path,
                      bool emitResults) {
    using namespace rose::BinaryAnalysis::InsnSemanticsExpr;     // TreeNode, InternalNode, LeafNode

    static int npaths = -1;
    ++npaths;
    Stream info(::mlog[INFO]);
    Stream error(::mlog[ERROR]);
    Stream debug(::mlog[DEBUG]);
//    info <<"path #" <<npaths <<" with " <<StringUtility::plural(path.nVertices(), "vertices", "vertex") <<"\n";

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
    if (path.frontVertex()->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().get_nbits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().get_nbits(), virtualAddress(path.frontVertex())));
    }
    std::vector<InsnSemanticsExpr::TreeNodePtr> pathConstraints;

    size_t pathInsnIndex = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, path.edges()) {
        if (pathEdge->source()->value().type() == P2::V_BASIC_BLOCK) {
            processBasicBlock(pathEdge->source()->value().bblock(), cpu, pathInsnIndex);
            pathInsnIndex += pathEdge->source()->value().bblock()->instructions().size();
        } else if (pathEdge->source()->value().type() == P2::V_INDETERMINATE) {
            processIndeterminateBlock(pathEdge->source(), cpu, pathInsnIndex);
            ++pathInsnIndex;
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
            ASSERT_require(hasVirtualAddress(pathEdge->target()));
            if (ip->get_number() != virtualAddress(pathEdge->target())) {
                // Executing the path forces us to go a different direction than where the path indicates we should go. We
                // don't need an SMT solver to tell us that when the values are just integers.
//                info <<"  not feasible according to ROSE semantics\n";
                return SMTSolver::SAT_NO;
            }
        } else if (hasVirtualAddress(pathEdge->target())) {
            LeafNodePtr targetVa = LeafNode::create_integer(ip->get_width(), virtualAddress(pathEdge->target()));
            TreeNodePtr constraint = InternalNode::create(1, OP_EQ,
                                                          targetVa, SymbolicSemantics::SValue::promote(ip)->get_expression());
            pathConstraints.push_back(constraint);
        }
    }

#if 1 // DEBUGGING [Robb P. Matzke 2015-05-24]
    return SMTSolver::SAT_UNKNOWN;
#endif

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
                                  <<" summary for " <<summary.name() <<"\n";
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
//        info <<"  not feasible according to SMT solver\n";
    } else {
        ASSERT_require(isSatisfied == SMTSolver::SAT_UNKNOWN);
        error <<"SMT solver could not determine satisfiability\n";
    }
    return isSatisfied;
}

/** Find paths and process them one at a time until we've found the desired number of feasible paths. */
static void 
findAndProcessSinglePaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                          const P2::CfgConstVertexSet &cfgEndVertices, const P2::CfgConstVertexSet &cfgAvoidVertices,
                          const P2::CfgConstEdgeSet &cfgAvoidEdges) {

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    Stream info(::mlog[INFO]);
    P2::CfgVertexMap vmap;                              // relates CFG vertices to path vertices
    P2::CfgConstVertexSet cfgBeginVertices; cfgBeginVertices.insert(cfgBeginVertex);
    P2::ControlFlowGraph paths;
    generateTopLevelPaths(partitioner.cfg(), cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges,
                          paths /*out*/, vmap /*out*/);
    if (paths.isEmpty())
        return;
    P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex = vmap.forward()[cfgBeginVertex];
    P2::CfgConstVertexSet pathsBeginVertices; pathsBeginVertices.insert(pathsBeginVertex);
    P2::CfgConstVertexSet pathsEndVertices = cfgToPaths(cfgEndVertices, vmap);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    P2::CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices.begin(), cfgEndVertices.end());

    // Depth-first traversal of the "paths". When a function call is encountered we do one of two things: either expand the
    // called function into the paths graph and replace the call-ret edge with an actual function call and return edges, or do
    // nothing but skip over the function call.  When expanding a function call, we want to insert only those edges and
    // vertices that can participate in a path from the callee's entry point to any of its returning points.
    P2::CfgPath path(pathsBeginVertex);
    while (!path.isEmpty()) {
#if 1 // DEBUGGING [Robb P. Matzke 2015-05-15]
        std::cerr <<"ROBB: path = " <<path <<"\n";
#endif
        P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
        P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(partitioner, backVertex);
        ASSERT_require(partitioner.cfg().isValidVertex(cfgBackVertex));
        bool doBacktrack = false;
        bool atEndOfPath = pathsEndVertices.find(backVertex) != pathsEndVertices.end();

        // Test path feasibility
        SMTSolver::Satisfiable isFeasible = singlePathFeasibility(partitioner, paths, path, atEndOfPath);
        if (atEndOfPath && isFeasible == SMTSolver::SAT_YES) {
            if (0 == --settings.maxPaths) {
                info <<"terminating because the maximum number of feasiable paths has been found\n";
                exit(0);
            }
            doBacktrack = true;
        } else if (atEndOfPath || isFeasible == SMTSolver::SAT_NO) {
            doBacktrack = true;
        }
        
        // If we've visited a vertex too many times (e.g., because of a loop or recursion), then don't go any further.
        if (path.nVisits(backVertex) > settings.vertexVisitLimit)
            doBacktrack = true;

        // Limit path length
        if (path.nEdges() > settings.maxPathLength) {
            ::mlog[WARN] <<"maximum path length exceeded (" <<settings.maxPathLength <<" vertices)\n";
            doBacktrack = true;
        }
        
        // If we're visiting a function call site, then inline callee paths into the paths graph, but continue to avoid any
        // paths that go through user-specified avoidance vertices and edges. We can modify the paths graph during the
        // traversal because we're modifying parts of the graph that aren't part of the current path.  This is where having
        // insert- and erase-stable graph iterators is a huge help!
        if (!doBacktrack && pathEndsWithFunctionCall(partitioner, path) && !P2::findCallReturnEdges(backVertex).empty()) {
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, P2::findCallEdges(cfgBackVertex)) {
                if (shouldSummarizeCall(path, partitioner.cfg(), cfgCallEdge->target())) {
                    insertCallSummary(paths, backVertex, partitioner.cfg(), cfgCallEdge);
                } else if (shouldInline(path, cfgCallEdge->target())) {
                    info <<"inlining function call paths at vertex " <<partitioner.vertexName(backVertex) <<"\n";
                    P2::insertCalleePaths(paths, backVertex,
                                          partitioner.cfg(), cfgBackVertex, calleeCfgAvoidVertices, cfgAvoidEdges);
                } else {
                    insertCallSummary(paths, backVertex, partitioner.cfg(), cfgCallEdge);
                }
            }
#if 1 // DEBUGGING [Robb P. Matzke 2015-05-15]
            {
                std::ofstream out("x-robb-1.dot");
                P2::CfgConstVertexSet end;
                end.insert(backVertex);
                printGraphViz(out, partitioner, paths, path.frontVertex(), end, path);
            }
#endif

            // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion. No need
            // to worry about adjusting the path because these edges aren't on the current path.
            P2::eraseEdges(paths, P2::findCallReturnEdges(backVertex));
#if 1 // DEBUGGING [Robb P. Matzke 2015-05-15]
            {
                std::ofstream out("x-robb-2.dot");
                P2::CfgConstVertexSet end;
                end.insert(backVertex);
                printGraphViz(out, partitioner, paths, path.frontVertex(), end, path);
            }
#endif


            // If the inlined function had no return sites but the call site had a call-return edge, then part of the paths
            // graph might now be unreachable. In fact, there might now be no paths from the begin vertex to any end vertex.
            // Erase those parts of the paths graph that are now unreachable.
            P2::eraseUnreachablePaths(paths, pathsBeginVertices, pathsEndVertices, vmap /*in,out*/, path /*in,out*/);
            ASSERT_require2(!paths.isEmpty() || path.isEmpty(), "path is empty only if paths graph is empty");
            if (path.isEmpty())
                break;
            backVertex = path.backVertex();
            cfgBackVertex = pathToCfg(partitioner, backVertex);

            info <<"paths graph has " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                 <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
        }
        
        // We've reached a dead end that isn't a final vertex.  This shouldn't ever happen.
        ASSERT_require2(doBacktrack || backVertex->nOutEdges()!=0,
                        "non-final vertex " + partitioner.vertexName(backVertex) + " has no out edges");

        // Advance to next path.
        if (doBacktrack) {
            // Backtrack and follow a different path.  The backtrack not only pops edges off the path, but then also appends
            // the next edge.  We must adjust visit counts for the vertices we backtracked.
            path.backtrack();
        } else {
            // Push next edge onto path.
            ASSERT_require(paths.isValidEdge(backVertex->outEdges().begin()));
            path.pushBack(backVertex->outEdges().begin());
        }
    }
};

// Inliner for single-path shortest-first mode.  Use normal inlining except in some special situations use user-defined
// inlining instead (i.e., insert a V_USER_DEFINED vertex instead of the function body.
class ShouldInline: public P2::Inliner::ShouldInline {
public:
    typedef Sawyer::SharedPointer<ShouldInline> Ptr;
    static Ptr instance() { return Ptr(new ShouldInline); }
    virtual P2::Inliner::HowInline operator()(const P2::Partitioner &partitioner,
                                              const P2::ControlFlowGraph::ConstEdgeIterator cfgCallEdge,
                                              const P2::ControlFlowGraph &paths,
                                              const P2::ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                              size_t callDepth) {
        P2::ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();

        if (callDepth > settings.maxCallDepth)
            return P2::Inliner::INLINE_USER;            // max call depth exceeded

        // We must inline indeterminte functions or else we'll end up removing the call and return edge, which is another
        // way of saying "we know there's no path through here" when in fact there could be.
        if (cfgCallTarget->value().type() == P2::V_INDETERMINATE)
            return P2::Inliner::INLINE_NORMAL;
        
        if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
            return P2::Inliner::INLINE_USER;            // callee is not a basic block
        
        P2::Function::Ptr callee = cfgCallTarget->value().function();
        if (!callee)
            return P2::Inliner::INLINE_USER;            // missing CFG information at this location?
        if (boost::ends_with(callee->name(), "@plt") || boost::ends_with(callee->name(), ".dll"))
            return P2::Inliner::INLINE_USER;            // dynamically linked function
        if (std::find(settings.summarizeFunctions.begin(), settings.summarizeFunctions.end(), callee->address()) !=
            settings.summarizeFunctions.end())
            return P2::Inliner::INLINE_USER;            // user requested summary from command-line

        // If the called function calls too many more functions then summarize instead of inlining.  This helps avoid
        // problems with the Partitioner2 not being able to find reasonable function boundaries, especially for GNU libc.
        static const size_t maxCallsAllowed = 10;   // arbitrary
        size_t nCalls = 0;
        using namespace Sawyer::Container::Algorithm;
        typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
        for (Traversal t(partitioner.cfg(), cfgCallTarget); t; ++t) {
            if (t.edge()->value().type() == P2::E_FUNCTION_CALL) {
                if (++nCalls > maxCallsAllowed)
                    return P2::Inliner::INLINE_USER;
                t.skipChildren();
            } else if (t.edge()->value().type() == P2::E_FUNCTION_RETURN) {
                t.skipChildren();
            }
        }

        return P2::Inliner::INLINE_NORMAL;
    }
};

// Label the progress report and also show some other statistics.  It is okay for this to be slightly expensive since its only
// called when a progress report is actually emitted.
class SinglePathProgress {
    size_t nPaths_, pathLength_, forestSize_, frontierSize_;
    Sawyer::Stopwatch timer_;
public:
    SinglePathProgress(): nPaths_(0), pathLength_(0), forestSize_(0), frontierSize_(0) {}
    void print(std::ostream &out) const {
        out <<" length=" <<pathLength_
            <<" forest=" <<forestSize_
            <<" frontier=" <<frontierSize_
            <<" rate=" <<nPaths_/timer_.report();
    }
    void nPaths(size_t n) { nPaths_ = n; }
    void pathLength(size_t n) { pathLength_ = n; }
    void forestSize(size_t n) { forestSize_ = n; }
    void frontierSize(size_t n) { frontierSize_ = n; }
};
std::ostream& operator<<(std::ostream &out, const SinglePathProgress &x) {
    x.print(out);
    return out;
}

// This later version of findAndProcessSinglePaths uses the ROSE inliner in a separate initial pass, then uses a breadth-first
// traversal so process paths in length order.
static void
findAndProcessSinglePathsShortestFirst(const P2::Partitioner &partitioner,
                                       const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                                       const P2::CfgConstVertexSet &cfgEndVertices,
                                       const P2::CfgConstVertexSet &cfgAvoidVertices,
                                       const P2::CfgConstEdgeSet &cfgAvoidEdges) {

    // Build the paths graph with inlined functions.
    P2::Inliner inliner;
    inliner.shouldInline(ShouldInline::instance());
    P2::CfgConstVertexSet cfgBeginVertices; cfgBeginVertices.insert(cfgBeginVertex);
    inliner.inlinePaths(partitioner, cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
    ::mlog[INFO] <<"  final paths graph: " <<StringUtility::plural(inliner.paths().nVertices(), "vertices", "vertex")
                 <<", " <<StringUtility::plural(inliner.paths().nEdges(), "edges") <<"\n";
    const P2::ControlFlowGraph &pathsGraph = inliner.paths();
    if (pathsGraph.nVertices() <= 1)
        return;                                         // no path, or trivially feasible singleton path
    ASSERT_require(inliner.pathsBeginVertices().size()==1);
    const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex = *inliner.pathsBeginVertices().begin();
    const P2::CfgConstVertexSet &pathsEndVertices = inliner.pathsEndVertices();
    ASSERT_require(!pathsEndVertices.empty());

    // We'll be performing a breadth-first traversal of the paths graph so we can process paths in order of their length. Some
    // paths may visit a vertex or edge of the paths graph more than once (e.g., loops). Since this is a breadth-first search,
    // we need a work list, but we don't want excessive copying of the edges that make up each path in the work list. We can
    // use the fact that many paths share the same prefix, thus the set of all paths forms a tree. Those paths that need to be
    // processed yet end at the leaves of the tree and form the frontier of the breadth-first search.  Actually, we need a
    // forest because our tree is storing edges and even though the paths all start at the same vertex in the paths-graph, they
    // don't all begin with the same edge. We don't have a specific tree container, but we can use Sawyer::Container::Graph as
    // the tree.  The frontier is stored as an std::list whose members point to the tree leaves. We're also using the fact that
    // Sawyer::Container::Graph iterators are stable over insert and erase operations.
    typedef Sawyer::Container::Graph<P2::ControlFlowGraph::ConstEdgeIterator> BfsForest;
    typedef std::list<BfsForest::VertexIterator> BfsFrontier;
    BfsForest bfsForest;
    BfsFrontier bfsFrontier;

    // Initialize the forest with unit length paths emanating from the beginning vertex. We've already taken care of the
    // singleton path case above, so we know there's at least one edge in every path.
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, pathsBeginVertex->outEdges())
        bfsFrontier.push_back(bfsForest.insertVertex(pathsGraph.findEdge(edge.id())));

    // Process each tree vertex that's in the traversal frontier. If the vertex represents a feasible path prefix then create
    // new paths in the frontier by extending this path one more edge in every possible direction.
    Sawyer::ProgressBar<size_t, SinglePathProgress> progress(::mlog[MARCH], "paths");
    while (!bfsFrontier.empty()) {
        BfsForest::VertexIterator bfsVertex = bfsFrontier.front();
        bfsFrontier.pop_front();
        ASSERT_require(bfsForest.isValidVertex(bfsVertex));

        // Build the path. We do so backward from the end toward the beginning since that's most convenient.
        P2::ControlFlowGraph::ConstEdgeIterator pathsEdge = bfsVertex->value();
        ASSERT_require(pathsGraph.isValidEdge(pathsEdge)); // forest vertices point to paths-graph edges
        P2::CfgPath path(bfsVertex->value()->target()); // end of the path, a vertex in the paths-graph
        for (BfsForest::VertexIterator vi=bfsVertex; vi!=bfsForest.vertices().end(); vi=vi->inEdges().begin()->source()) {
            ASSERT_require(bfsForest.isValidVertex(vi));
            path.pushFront(vi->value());
            if (0 == vi->nInEdges())
                break;                                  // made it to root of bfsForest
            ASSERT_require2(vi->nInEdges()==1, "must form a tree");
        }
        progress.suffix().nPaths(progress.value()+1);
        progress.suffix().pathLength(path.nEdges());
        progress.suffix().forestSize(bfsForest.nVertices());
        progress.suffix().frontierSize(bfsFrontier.size());
        ++progress;

        // Test whether the path is feasible. If not feasible, then any path with this one as a prefix is also not feasible.
        bool abandonPrefix = false;
        bool atEndOfPath = pathsEndVertices.find(path.backVertex()) != pathsEndVertices.end();
        SMTSolver::Satisfiable isFeasible = singlePathFeasibility(partitioner, pathsGraph, path, atEndOfPath);
        if (atEndOfPath && isFeasible == SMTSolver::SAT_YES) {
            if (0 == --settings.maxPaths) {
                ::mlog[INFO] <<"terminating because the maximum number of feasiable paths has been found\n";
                exit(0);
            }
            abandonPrefix = true;
        } else if (atEndOfPath || isFeasible == SMTSolver::SAT_NO) {
            abandonPrefix = true;
        }

        // Extend the path, or abandon this prefix. When abandoning a prefix, erase the part of the bfsForest that isn't needed
        // any more, but be sure not to erase any prefix of this path that might be a prefix of some other path.
        ASSERT_require(atEndOfPath || path.backVertex()->nOutEdges() > 0);
        if (abandonPrefix) {
            while (1) {
                ASSERT_require2(bfsVertex->nInEdges() <= 1, "tree nodes have 0 or 1 parent");
                if (bfsVertex->nOutEdges() > 0)
                    break;                              // prefix is being used still
                if (bfsVertex->nInEdges() == 1) {       // has parent?
                    BfsForest::VertexIterator parent = bfsVertex->inEdges().begin()->source();
                    bfsForest.eraseVertex(bfsVertex);   // and incident edges
                    bfsVertex = parent;
                } else {
                    bfsForest.eraseVertex(bfsVertex);
                    break;
                }
            }
        } else {                                        // extend this prefix
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, path.backVertex()->outEdges()) {
                BfsForest::VertexIterator v = bfsForest.insertVertex(pathsGraph.findEdge(edge.id()));
                bfsForest.insertEdge(bfsVertex, v);
                bfsFrontier.push_back(v);
            }
        }
    }
}
    

/** Merge states for multi-path feasibility analysis. Given two paths, such as when control flow merges after an "if"
 * statement, compute a state that represents both paths.  The new state that's returned will consist largely of ite
 * expressions. */
static BaseSemantics::StatePtr
mergeMultipathStates(const BaseSemantics::RiscOperatorsPtr &ops,
                     const BaseSemantics::StatePtr &s1, const BaseSemantics::StatePtr &s2) {
    using namespace InsnSemanticsExpr;

    ASSERT_not_null(ops);
    ASSERT_not_null(s2);
    if (s1 == NULL)
        return s2->clone();

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

// Merge all the predecessor outgoing states to create a new incoming state for the specified vertex.
static BaseSemantics::StatePtr
mergePredecessorStates(const BaseSemantics::RiscOperatorsPtr &ops, const P2::ControlFlowGraph::ConstVertexIterator vertex,
                       const StateStacks &outStates) {
    // If this is the initial vertex, then use the state that the caller has initialized already.
    if (0 == vertex->nInEdges())
        return ops->get_state();

    // Create the incoming state for this vertex by merging the outgoing states of all predecessors.
    BaseSemantics::StatePtr state;
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->inEdges()) {
        P2::ControlFlowGraph::ConstVertexIterator predecessorVertex = edge.source();
        ASSERT_require(outStates.exists(predecessorVertex));
        ASSERT_forbid(outStates[predecessorVertex].empty());
        BaseSemantics::StatePtr predecessorOutState = outStates[predecessorVertex].back();
        state = mergeMultipathStates(ops, state, predecessorOutState);
    }
    ASSERT_not_null(state);
    return state;
}

class ReverseMultiVisitDfsTraversal {
    struct Node {
        P2::ControlFlowGraph::ConstVertexIterator vertex;
        P2::ControlFlowGraph::ConstEdgeIterator inEdge; // next edge to follow for this vertex
        bool skipChildren;
        explicit Node(const P2::ControlFlowGraph::ConstVertexIterator &vertex)
            : vertex(vertex), inEdge(vertex->inEdges().begin()), skipChildren(false) {}
    };
    typedef std::vector<Node> Stack;
    Stack stack_;
    Sawyer::Container::Algorithm::TraversalEvent event_;
    P2::ControlFlowGraph::ConstVertexIterator eventVertex_;
public:
    ReverseMultiVisitDfsTraversal(const P2::ControlFlowGraph::ConstVertexIterator vertex)
        : stack_(1, Node(vertex)), event_(Sawyer::Container::Algorithm::ENTER_VERTEX), eventVertex_(vertex) {}

    ReverseMultiVisitDfsTraversal& operator++() {
        if (stack_.empty()) {
            event_ = Sawyer::Container::Algorithm::NO_EVENT;
        } else if (!stack_.back().skipChildren && stack_.back().inEdge != stack_.back().vertex->inEdges().end()) {
            Node nextNode(stack_.back().inEdge->source());
            ++stack_.back().inEdge;
            stack_.push_back(nextNode);
            event_ = ENTER_VERTEX;
            eventVertex_ = nextNode.vertex;
        } else {
            event_ = LEAVE_VERTEX;
            eventVertex_ = stack_.back().vertex;
            stack_.pop_back();
        }
        return *this;
    }

    operator bool() const {
        return event_ != Sawyer::Container::Algorithm::NO_EVENT;
    }

    Sawyer::Container::Algorithm::TraversalEvent event() const {
        return event_;
    }

    P2::ControlFlowGraph::ConstVertexIterator vertex() const {
        ASSERT_require(event_ != Sawyer::Container::Algorithm::NO_EVENT);
        return eventVertex_;
    }

    void skipChildren() {
        ASSERT_forbid(stack_.empty());
        stack_.back().skipChildren = true;
    }

    size_t depth() const {
        return stack_.size() + (event_ == LEAVE_VERTEX ? 1 : 0);
    }
};


/** Process all paths at once by sending everything to the SMT solver. */
static void
multiPathFeasibility(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths,
                     const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex,
                     const P2::CfgConstVertexSet &pathsEndVertices) {
    using namespace Sawyer::Container::Algorithm;
    using namespace InsnSemanticsExpr;
#if 1 // [Robb P. Matzke 2015-05-09]
    TODO("[Robb P. Matzke 2015-05-09]");
#else

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

    // Build the semantics framework and initialize the path constraints.
    YicesSolver solver;
    solver.set_debug(settings.debugSmtSolver ? stderr : NULL);
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    ops->writeRegister(REG_PATH, ops->boolean_(true)); // start of path is always feasible
    if (pathsBeginVertex->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().get_nbits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().get_nbits(), virtualAddress(pathsBeginVertex)));
    }

    // Reverse depth-first visit starting at the end vertex and processing each vertex as we back out.  This ensures that the
    // final state of the predecessors is computed before the successor.
    size_t pathInsnIndex(-1);
    info <<"  building path constraints expression\n";
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &pathsEndVertex, pathsEndVertices) {
        std::vector<std::vector<BaseSemantics::StatePtr> > outgoingStates(paths.nVertices());
        std::vector<size_t> nVisits(paths.nVertices(), 0); // number of times this vertex appears in the current traversal path
        for (ReverseMultiVisitDfsTraversal t(pathsEndVertex); t; ++t) {
            size_t vId = t.vertex()->id();
            if (t.event() == ENTER_VERTEX) {
                if (++nVisits[vId] > settings.vertexVisitLimit)
                    t.skipChildren();                   // break cycles in the CFG
            } else if (t.depth() == 1) {
                // We're leaving the pathsEndVertex for the last time. There's no need to process this vertex since are goal is
                // to only reach it by some path.
                ASSERT_require(t.event() == LEAVE_VERTEX);
                ASSERT_require(t.vertex() == pathsEndVertex);
                TODO("end of traversal");
            } else {
                // As we leave a vertex we know that all its CFG predecessors have been processed.  Compute an incoming state
                // for this vertex based on the outgoing states of its predecessors.
                

                BaseSemantics::StatePtr state = cpu->state()->clone();
                state->reset();
                ops->set_state(state); // modifing incoming state in place, changing it to an outgoing state
                if (t.vertex()->value().type() == P2::V_BASIC_BLOCK) {
                    processBasicBlock(t.vertex()->value().bblock(), cpu, pathInsnIndex);
                } else if (t.vertex()->value().type() == P2::V_INDETERMINATE) {
                    processIndeterminateBlock(t.vertex(), cpu, pathInsnIndex);
                } else if (t.vertex()->value().type() == P2::V_USER_DEFINED) {
                    processFunctionSummary(t.vertex(), cpu, pathInsnIndex);
                } else {
                    ASSERT_not_reachable("invalid vertex type at " + partitioner.vertexName(t.vertex()));
                }
                


                outgoingStates[vId] = cpu->state()->clone();
                outgoingStates[vId]->reset();

                ASSERT_require(t.event() == LEAVE_VERTEX);
                ASSERT_require(!outgoingStates[vId].empty());
                BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, t.vertex()->inEdges()) {
                    P2::ControlFlowGraph::ConstVertexIterator predecessor = edge.target();
                    ASSERT_require(!outgoingStates[predecessor->id()].empty());
                    outgoingStates[


                BaseSemantics::StatePtr outgoingState = ops->get_state();
                incomingStates[vId].pop_back();

                // Now that we have this vertex's outgoing state, merge the outgoing state into all this vertex's CFG
                // successors' incoming states.  Because the depth-first traversal is going backward along edges we know that
                // the successors have not been processed yet, although the traversal has encountered (entered) them and
                // allocated their incoming states.
                BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, t.vertex()->outEdges()) {
                    P2::ControlFlowGraph::ConstVertexIterator successor = edge.target();
                    ASSERT_require(!incomingStates[successor->id()].empty());
                    incomingStates[successor->id()].back() = mergeMultipathStates(ops,
                                                                                  incomingStates[successor->id()].back(),
                                                                                  outgoingState);
                }
            }
        }
    }
    
                    
#if 0 // [Robb P. Matzke 2015-05-07]: old stuff
    ASSERT_require(pathsBeginVertex->nInEdges() == 0);
    std::vector<BaseSemantics::StatePtr> outState(paths.nVertices());
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator pathsEndVertex, pathsEndVertices) {

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
#endif
#endif
}

struct CallSite {
    P2::ControlFlowGraph::ConstVertexIterator vertex;
    size_t callDepth;
    CallSite(const P2::ControlFlowGraph::ConstVertexIterator &vertex, size_t callDepth): vertex(vertex), callDepth(callDepth) {}
};

    

/** Construct a control flow graph with inlined functions.
 *
 *  Computes paths from @p cfgBeginVertex to @p cfgEndVertices that do not pass through @p cfgAvoidVertices or @p
 *  cfgAvoidEdges. Any function calls along these paths are then inlined, but only those paths through the function that don't
 *  pass through the @p cfgAvoidVertices or @p cfgAvoidEdges.  If a call satisfies the @ref shouldSummarizeCall predicate or if
 *  the call depth becomes too deep then instead of inlining, the a special summary vertex is inserted.
 *
 *  Returns the resulting control flow graph, a.k.a., the paths graph. */
static void
findAndProcessMultiPaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexIterator &cfgBeginVertex,
                         const P2::CfgConstVertexSet &cfgEndVertices, const P2::CfgConstVertexSet &cfgAvoidVertices,
                         const P2::CfgConstEdgeSet &cfgAvoidEdges) {
    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    Stream info(::mlog[INFO]);
    Stream progress(::mlog[MARCH]);
    P2::CfgVertexMap vmap;                              // relates CFG vertices to path vertices
    P2::CfgConstVertexSet cfgBeginVertices; cfgBeginVertices.insert(cfgBeginVertex);
    P2::ControlFlowGraph paths;
    generateTopLevelPaths(partitioner.cfg(), cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges,
                          paths /*out*/, vmap /*out*/);
    P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex = vmap.forward()[cfgBeginVertex];
    P2::CfgConstVertexSet pathsBeginVertices; pathsBeginVertices.insert(pathsBeginVertex);
    P2::CfgConstVertexSet pathsEndVertices = cfgToPaths(cfgEndVertices, vmap);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    P2::CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices.begin(), cfgEndVertices.end());

    // Set up the initial worklist
    size_t nVertsProcessed = 0, nFuncsInlined = 0, nFuncsSummarized = 0;
    std::list<CallSite> workList;
    for (P2::ControlFlowGraph::ConstVertexIterator vert=paths.vertices().begin(); vert!=paths.vertices().end(); ++vert) {
        ++nVertsProcessed;
        if (isFunctionCall(partitioner, vert) && !P2::findCallReturnEdges(vert).empty())
            workList.push_back(CallSite(vert, 0));
    }

    // Process the worklist, inlining functions until there's nothing more to do.
    Sawyer::Stopwatch timer;                            // time betwen reports
    while (!workList.empty()) {
        CallSite work = workList.back();
        workList.pop_back();
        P2::ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, work.vertex);
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, P2::findCallEdges(cfgVertex)) {
            ++nVertsProcessed;
            if (work.callDepth < settings.maxCallDepth) {
                if (shouldSummarizeCall(work.vertex, partitioner.cfg(), cfgCallEdge->target())) {
                    ++nFuncsSummarized;
                    insertCallSummary(paths, work.vertex, partitioner.cfg(), cfgCallEdge);
                } else { // inline the function
                    ++nFuncsInlined;
                    std::vector<P2::ControlFlowGraph::ConstVertexIterator> insertedVertices;
                    P2::insertCalleePaths(paths, work.vertex, partitioner.cfg(), cfgCallEdge,
                                          calleeCfgAvoidVertices, cfgAvoidEdges, &insertedVertices);
                    P2::eraseEdges(paths, P2::findCallReturnEdges(work.vertex));
                    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, insertedVertices) {
                        ++nVertsProcessed;
                        if (isFunctionCall(partitioner, vertex) && !P2::findCallReturnEdges(vertex).empty())
                            workList.push_back(CallSite(vertex, work.callDepth+1));
                    }
                }
            } else {
                ++nFuncsSummarized;
                insertCallSummary(paths, work.vertex, partitioner.cfg(), cfgCallEdge);
            }
            P2::eraseEdges(paths, P2::findCallReturnEdges(work.vertex));
        }
        // Report some statistics every so often
        if (progress && timer.report() >= 60.0) {
            double secondsSinceLastReport = timer.report();
            progress <<"  status report:\n";
            progress <<"    paths graph: " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                     <<", " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
            progress <<"    path depth: " <<StringUtility::plural(work.callDepth, "calls") <<"\n";
            progress <<"    worklist length: " <<StringUtility::plural(workList.size(), "calls") <<"\n";
            progress <<"    processed: " <<(nVertsProcessed/secondsSinceLastReport) <<" vert/sec"
                     <<"; " <<(nFuncsInlined/secondsSinceLastReport) <<" func/sec inlined"
                     <<"; " <<(nFuncsSummarized/secondsSinceLastReport) <<" func/sec summarized\n";
            nVertsProcessed = nFuncsInlined = 0;
            timer.restart();
        }
    }

    // Remove edges and vertices that cannot be part of any path
    P2::CfgConstEdgeSet pathsUnreachableEdges = findPathUnreachableEdges(paths, pathsBeginVertices, pathsEndVertices);
    P2::CfgConstVertexSet pathsIncidentVertices = P2::findIncidentVertices(pathsUnreachableEdges);
    if (pathsEndVertices.find(pathsBeginVertex) != pathsEndVertices.end())
        pathsIncidentVertices.erase(pathsBeginVertex); // allow singleton paths if appropriate
    P2::eraseEdges(paths, pathsUnreachableEdges);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &pathVertex, pathsIncidentVertices) {
        if (pathVertex->degree() == 0) {
            vmap.eraseTarget(pathVertex);
            paths.eraseVertex(pathVertex);
        }
    }

    info <<"  final paths graph: " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
         <<", " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
    multiPathFeasibility(partitioner, paths, pathsBeginVertex, pathsEndVertices);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    ::mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);
    Sawyer::Message::Stream info(::mlog[INFO]);

    // Parse the command-line
    P2::Engine engine;
    engine.doingPostAnalysis(false);
    engine.usingSemantics(true);
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine);
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Disassemble and partition
    P2::Partitioner partitioner = engine.partition(specimenNames);

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


    // Calculate average number of function calls per function
    if (1) {
        size_t nFunctionCalls = 0;
        BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, partitioner.cfg().edges()) {
            if (edge.value().type() == P2::E_FUNCTION_CALL)
                ++nFunctionCalls;
        }
        info <<"average number of function calls per function: "
             <<((double)nFunctionCalls / partitioner.nFunctions()) <<"\n";
    }
    
    // Process individual paths
    if (settings.multiPathSmt) {
        findAndProcessMultiPaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
    } else {
#if 0 // [Robb P. Matzke 2015-05-22]
        findAndProcessSinglePaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
#else
        findAndProcessSinglePathsShortestFirst(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
#endif
    }
}
