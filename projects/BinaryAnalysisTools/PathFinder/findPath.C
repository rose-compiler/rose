#include <rose.h>

#include <PathFinder/PathFinder.h>
#include <PathFinder/semantics.h>

#include <AsmUnparser_compat.h>
#include <BinarySymbolicExprParser.h>
#include <BinaryYicesSolver.h>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <DwarfLineMapper.h>
#include <Partitioner2/CfgPath.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <rose_strtoull.h>
#include <Sawyer/BiMap.h>
#include <Sawyer/GraphTraversal.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>
#include <SymbolicMemory2.h>
#include <SymbolicSemantics2.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace Sawyer::Container::Algorithm;
using namespace Rose::BinaryAnalysis::InstructionSemantics2; // BaseSemantics, SymbolicSemantics
using namespace PathFinder;
namespace P2 = Partitioner2;

DwarfLineMapper srcMapper;

enum FollowCalls { SINGLE_FUNCTION, FOLLOW_CALLS };

// This is a "register" that stores a description of the current path.  The major and minor numbers are arbitrary, but chosen
// so that they hopefully don't conflict with any real registers, which tend to start counting at zero.  Since we're using
// BaseSemantics::RegisterStateGeneric, we can use its flexibility to store extra "registers" without making any other changes
// to the architecture.
static const RegisterDescriptor REG_PATH(15, 1023, 0, 1);

// This is the register where functions will store their return value.
static RegisterDescriptor REG_RETURN;

// Information stored per V_USER_DEFINED vertices.
struct FunctionSummary {
    rose_addr_t address;
    int64_t stackDelta;
    std::string name;
    FunctionSummary(): stackDelta(SgAsmInstruction::INVALID_STACK_DELTA) {}
    FunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &cfgFuncVertex, uint64_t stackDelta)
        : address(cfgFuncVertex->value().address()), stackDelta(stackDelta) {
        if (cfgFuncVertex->value().type() == P2::V_BASIC_BLOCK) {
            if (P2::Function::Ptr function = cfgFuncVertex->value().isEntryBlock()) {
                name = function->printableName();
                return;
            }
        }
        name = P2::Partitioner::vertexName(*cfgFuncVertex);
    }
};

typedef Sawyer::Container::Map<rose_addr_t, FunctionSummary> FunctionSummaries;
static FunctionSummaries functionSummaries;

// Stack of states per vertex
typedef Sawyer::Container::Map<P2::ControlFlowGraph::ConstVertexIterator, std::vector<BaseSemantics::StatePtr> > StateStacks;

static SymbolicExprParser postConditionParser(const BaseSemantics::RiscOperatorsPtr&);

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
    cfg.name("cfg");

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
                    StringUtility::plural(settings.maxPathLength, "instructions") + "."));

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

    cfg.insert(Switch("search")
               .argument("mode", enumParser<SearchMode>(settings.searchMode)
                         ->with("single-dfs", SEARCH_SINGLE_DFS)
                         ->with("single-bfs", SEARCH_SINGLE_BFS)
                         ->with("multi",      SEARCH_MULTI))
               .doc("Determines the search method. The available modes are:"
                    "@named{single-dfs}{" +
                    std::string(settings.searchMode==SEARCH_SINGLE_DFS?"This is the default. ":"") +
                    "This is a depth-first search that solves one path's feasibility at a time with "
                    "short circuiting for infeasible prefixes, unrolling loops up to @s{max-vertex-visits} times. "
                    "This method can take unnecessarily long to execute when loop unrolling is set to a large value "
                    "but the first feasible path executes the loop only a few times.}"

                    "@named{single-bfs}{" +
                    std::string(settings.searchMode==SEARCH_SINGLE_BFS?"This is the default. ":"") +
                    "This is a breadth-first search that solves one path's feasibility at a time with short circuiting "
                    "of infeasible prefixes.  It finds the shortest feasible path. Loops are unrolled as many times "
                    "as necessary until a feasible path is found up to the @s{max-path-length}.  This search method "
                    "can consume more memory than the \"single-dfs\" method because multiple paths are in progress "
                    "at any given time.}"

                    "@named{multi}{" +
                    std::string(settings.searchMode==SEARCH_MULTI?"This is the default. ":"") +
                    "This search method builds an equation for all paths simultaneously and invokes the SMT solver "
                    "only once, but with a larger equation. SMT solvers are designed for this use case and this method "
                    "is often much faster than one of the single-path methods.  The drawback is that the result indicates "
                    "only that a feasible path is found (and the initial conditions to drive that path) and not which "
                    "path is found. Note: as of this writing (2015-05-24) this mode does not work.}"));

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
    SwitchGroup pcond("Post-condition switches");
    pcond.name("post");

    pcond.insert(Switch("condition")
                 .argument("expression", anyParser(settings.postConditionsStr))
                 .whichValue(SAVE_ALL)
                 .doc("Specifies post conditions that must be met at the end of the path. This switch may appear "
                      "multiple times, in which case all post conditions must be met in order for the path to be "
                      "reported.  Post conditions are specified as symbolic expressions and, due to the parentheses and "
                      "white space, likely need to be protected from shell expansion by enclosing them in quotes. " +
                      postConditionParser(BaseSemantics::RiscOperatorsPtr()).docString()));

    //--------------------------- 
    SwitchGroup out("Output switches");
    out.name("out");

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

    out.insert(Switch("show-expr-type")
               .intrinsicValue(true, settings.showExprType)
               .doc("Show data types for symbolic expressions.  This is useful, but can make the expressions overly "
                    "verbose and less readable.  The @s{no-show-expr-type} turns this off. The default is to " +
                    std::string(settings.showExprType ? "" : "not ") + "show expression types."));
    out.insert(Switch("no-show-expr-type")
               .key("show-expr-type")
               .intrinsicValue(false, settings.showExprType)
               .hidden(true));

    ParserResult cmdline = parser.with(cfg).with(pcond).with(out).parse(argc, argv).apply();

    // These switches from the rose library have no Settings struct yet, so we need to query the parser results to get them.
    if (cmdline.have("threads"))
        settings.nThreads = cmdline.parsed("threads")[0].asUnsignedLong();

    return cmdline.unreachedArgs();
}

static bool
hasVirtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    return vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED;
}

static rose_addr_t
virtualAddress(const P2::ControlFlowGraph::ConstVertexIterator &vertex) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK || vertex->value().type() == P2::V_USER_DEFINED)
        return vertex->value().address();
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
edgeForInstructions(const P2::Partitioner &partitioner, const std::string &sourceNameOrVa,
                    const std::string &targetNameOrVa) {
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
    P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
    if (!callee)
        return false;
    if (boost::ends_with(callee->name(), "@plt") || boost::ends_with(callee->name(), ".dll"))
        return true;

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
    P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
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
            const FunctionSummary &summary = functionSummaries[vertex->value().address()];
            return "summary for " + summary.name;
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

    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &endVertex, endVertices.values())
        gv.vertexOrganization(endVertex).attributes().insert("fillcolor", exitColor.toHtml());
    gv.vertexOrganization(beginVertex).attributes().insert("fillcolor", entryColor.toHtml());

    typedef Sawyer::Container::GraphIteratorMap<P2::ControlFlowGraph::ConstEdgeIterator, size_t> EdgeCounts;
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

/** Initialize the virtual machine with a new state. */
static void
setInitialState(const BaseSemantics::DispatcherPtr &cpu, const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex) {
    ASSERT_not_null(cpu);

    // Create the new state from an existing state and make the new state current.
    BaseSemantics::StatePtr state = cpu->currentState()->clone();
    state->clear();
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    ops->currentState(state);

    // Start of path is always feasible.
    ops->writeRegister(REG_PATH, ops->boolean_(true));

    // Initialize instruction pointer register
    if (pathsBeginVertex->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().nBits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().nBits(), pathsBeginVertex->value().address()));
    }

    // Initialize stack pointer register
    if (settings.initialStackPtr) {
        const RegisterDescriptor REG_SP = cpu->stackPointerRegister();
        ops->writeRegister(REG_SP, ops->number_(REG_SP.nBits(), *settings.initialStackPtr));
    }

    // Direction flag (DF) is always set
    const RegisterDescriptor REG_DF = cpu->get_register_dictionary()->findOrThrow("df");
    ASSERT_forbid(REG_DF.isEmpty());
    ops->writeRegister(REG_DF, ops->boolean_(true));
}

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
        // FIXME[Robb Matzke 2015-12-01]: We need to support returning multiple values. We should be using the new calling
        // convention analysis to detect these.
        RegisterDescriptor r;
        if ((r = myRegs->find("rax")) || (r = myRegs->find("eax")) || (r = myRegs->find("ax"))) {
            REG_RETURN = r;
        } else if ((r = myRegs->find("d0"))) {
            REG_RETURN = r;                             // m68k also typically has other return registers
        } else {
            ASSERT_not_implemented("function return value register is not implemented for this ISA/ABI");
        }
    }

    SmtSolver::Ptr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
    RiscOperatorsPtr ops = RiscOperators::instance(&partitioner, myRegs, solver);

    return partitioner.instructionProvider().dispatcher()->create(ops);
}

/** Process instructions for one basic block on the specified virtual CPU. */
static void
processBasicBlock(const P2::BasicBlock::Ptr &bblock, const BaseSemantics::DispatcherPtr &cpu, size_t pathInsnIndex) {
    ASSERT_not_null(bblock);
    
    // Update the path constraint "register"
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    RegisterDescriptor IP = cpu->instructionPointerRegister();
    BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.nBits()));
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
        PathFinder::mlog[ERROR] <<"semantics failed: " <<e <<"\n";
        return;
    }
}

/** Process an indeterminate block. This represents flow of control through an unknown address. */
static void
processIndeterminateBlock(const P2::ControlFlowGraph::ConstVertexIterator &vertex, const BaseSemantics::DispatcherPtr &cpu,
                          size_t pathInsnIndex) {
    PathFinder::mlog[WARN] <<"control flow passes through an indeterminate address at path position #" <<pathInsnIndex <<"\n";
}

/** Process a function summary vertex. */
static void
processFunctionSummary(const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex, const BaseSemantics::DispatcherPtr &cpu,
                       size_t pathInsnIndex) {
    ASSERT_require(functionSummaries.exists(pathsVertex->value().address()));
    const FunctionSummary &summary = functionSummaries[pathsVertex->value().address()];

    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    if (pathInsnIndex != size_t(-1))
        ops->pathInsnIndex(pathInsnIndex);

    // Make the function return an unknown value
    SymbolicSemantics::SValuePtr retval = SymbolicSemantics::SValue::promote(ops->undefined_(REG_RETURN.nBits()));
    std::string comment = "return value from " + summary.name + "\n" +
                          "at path position #" + StringUtility::numberToString(ops->pathInsnIndex());
    ops->varComment(retval->get_expression()->isLeafNode()->toString(), comment);
    ops->writeRegister(REG_RETURN, retval);

    // Cause the function to return to the address stored at the top of the stack.
    RegisterDescriptor SP = cpu->stackPointerRegister();
    BaseSemantics::SValuePtr stackPointer = ops->readRegister(SP, ops->undefined_(SP.nBits()));
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

/** Process one path vertex. */
static void
processVertex(const BaseSemantics::DispatcherPtr &cpu, const P2::ControlFlowGraph::ConstVertexIterator &pathsVertex,
              size_t &pathInsnIndex /*in,out*/) {
    switch (pathsVertex->value().type()) {
        case P2::V_BASIC_BLOCK:
            processBasicBlock(pathsVertex->value().bblock(), cpu, pathInsnIndex);
            pathInsnIndex += pathsVertex->value().bblock()->instructions().size();
            break;
        case P2::V_INDETERMINATE:
            processIndeterminateBlock(pathsVertex, cpu, pathInsnIndex);
            ++pathInsnIndex;
            break;
        case P2::V_USER_DEFINED:
            processFunctionSummary(pathsVertex, cpu, pathInsnIndex);
            ++pathInsnIndex;
            break;
        default:
            PathFinder::mlog[ERROR] <<"cannot comput path feasibility; invalid vertex type at "
                          <<P2::Partitioner::vertexName(*pathsVertex) <<"\n";
            cpu->get_operators()->writeRegister(cpu->instructionPointerRegister(),
                                                cpu->get_operators()->number_(cpu->instructionPointerRegister().nBits(),
                                                                              0x911 /*arbitrary, unlikely to be satisfied*/));
            ++pathInsnIndex;
    }
}

static void
showPathEvidence(const SmtSolverPtr &solver, const RiscOperatorsPtr &ops) {
    std::cout <<"  Inputs sufficient to cause path to be taken:\n";
    std::vector<std::string> enames = solver->evidenceNames();
    if (enames.empty()) {
        std::cout <<"    not available (or none necessary)\n";
    } else {
        BOOST_FOREACH (const std::string &ename, enames) {
            if (ename.substr(0, 2) == "0x") {
                std::cout <<"    memory[" <<ename <<"] == " <<*solver->evidenceForName(ename) <<"\n";
            } else {
                std::cout <<"    " <<ename <<" == " <<*solver->evidenceForName(ename) <<"\n";
            }
            std::string varComment = ops->varComment(ename);
            if (!varComment.empty())
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
        PathFinder::mlog[WARN] <<"no paths found\n";
    } else {
        PathFinder::mlog[INFO] <<"paths graph has " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
                     <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";
    }
}

// Converts vertices from one graph to another based on the vmap
static P2::CfgConstVertexSet
cfgToPaths(const P2::CfgConstVertexSet &vertices, const P2::CfgVertexMap &vmap) {
    P2::CfgConstVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, vertices.values()) {
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
    P2::Function::Ptr function = cfgCallTarget->value().isEntryBlock();

    P2::ControlFlowGraph::VertexIterator summaryVertex = paths.insertVertex(P2::CfgVertex(P2::V_USER_DEFINED));
    paths.insertEdge(pathsCallSite, summaryVertex, P2::CfgEdge(P2::E_FUNCTION_CALL));
    P2::CfgConstEdgeSet callReturnEdges = P2::findCallReturnEdges(pathsCallSite);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, callReturnEdges.values())
        paths.insertEdge(summaryVertex, callret->target(), P2::CfgEdge(P2::E_FUNCTION_RETURN));

    int64_t stackDelta = function ? function->stackDeltaConcrete() : SgAsmInstruction::INVALID_STACK_DELTA;

    FunctionSummary summary(cfgCallTarget, stackDelta);
    functionSummaries.insert(summary.address, summary);
    summaryVertex->value().address(summary.address);
}

static void
printResults(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &pathsGraph, const P2::CfgPath &path,
             size_t pathNumber, const std::vector<SymbolicExpr::Ptr> &pathConstraints, const SmtSolverPtr &solver,
             const RiscOperatorsPtr &ops) {
    std::cout <<"Found feasible path #" <<pathNumber
              <<" with " <<StringUtility::plural(path.nVertices(), "vertices", "vertex") <<".\n";

    if (FEASIBLE_PATHS == settings.graphVizOutput) {
        std::string graphVizFileName = printGraphViz(partitioner, pathsGraph, path, pathNumber);
        std::cout <<"  Saved as \"" <<StringUtility::cEscape(graphVizFileName) <<"\"\n";
    }

    std::cout <<"  Path:\n";
    size_t insnIdx=0, pathIdx=0;
    if (path.nEdges() == 0) {
        std::cout <<"    path is trivial (contains only vertex " <<partitioner.vertexName(path.frontVertex()) <<")\n";
    } else {
#if 0 // DEBUGGING [Robb P. Matzke 2015-05-25]
        BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
        setInitialState(cpu, path.frontVertex());
        //setEvidenceState(cpu, solver);
        bool wasDebugEnabled = PathFinder::mlog[DEBUG];
        PathFinder::mlog[DEBUG].enable(true);
        size_t nInsnsProcessed = 0;
#endif
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
                                  <<" " <<partitioner.unparse(insn) <<"\n";
                    }
                } else if (pathVertex->value().type() == P2::V_USER_DEFINED) {
                    ASSERT_require(functionSummaries.exists(pathVertex->value().address()));
                    const FunctionSummary &summary = functionSummaries[pathVertex->value().address()];
                    std::cout <<"      #" <<std::setw(5) <<std::left <<insnIdx++
                              <<" summary for " <<summary.name <<"\n";
                }
#if 0 // DEBUGGING [Robb P. Matzke 2015-05-25]
                processVertex(cpu, pathVertex, nInsnsProcessed);
#endif
            }
            ++pathIdx;
        }
#if 0 // DEBUGGING [Robb P. Matzke 2015-05-25]
        PathFinder::mlog[DEBUG].enable(wasDebugEnabled);
#endif
    }

    if (settings.showConstraints) {
        std::cout <<"  Constraints:\n";
        if (pathConstraints.empty()) {
            std::cout <<"    none\n";
        } else {
            size_t idx = 0;
            BOOST_FOREACH (const SymbolicExpr::Ptr &constraint, pathConstraints)
                std::cout <<"    #" <<std::setw(5) <<std::left <<idx++ <<" " <<*constraint <<"\n";
        }
    }

    showPathEvidence(solver, ops);

    if (settings.showFinalState) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("    ");
        std::cout <<"  Machine state at end of path (prior to entering " <<partitioner.vertexName(path.backVertex()) <<")\n"
                  <<(*ops->currentState() + fmt);
    }
}

// Parse register names
class RegisterExpansion: public SymbolicExprParser::AtomExpansion {
    BaseSemantics::RiscOperatorsPtr ops_;
protected:
    RegisterExpansion(const BaseSemantics::RiscOperatorsPtr &ops)
        : ops_(ops) {}
public:
    static Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        Ptr functor = Ptr(new RegisterExpansion(ops));
        functor->title("Registers");
        std::string doc = "Register locations are specified by just mentioning the name of the register. Register names "
                          "are usually lower case, such as \"eax\", \"rip\", etc.";
        functor->docString(doc);
        return functor;
    }
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &token) ROSE_OVERRIDE {
        BaseSemantics::RegisterStatePtr regState = ops_->currentState()->registerState();
        const RegisterDescriptor regp = regState->get_register_dictionary()->find(token.lexeme());
        if (!regp)
            return SymbolicExpr::Ptr();
        if (token.exprType().nBits() != 0 && token.exprType().nBits() != regp.nBits()) {
            throw token.syntaxError("invalid register width (specified=" + StringUtility::numberToString(token.exprType().nBits()) +
                                    ", actual=" + StringUtility::numberToString(regp.nBits()) + ")");
        }
        if (token.exprType().typeClass() == SymbolicExpr::Type::MEMORY)
            throw token.syntaxError("register type must be scalar");
        BaseSemantics::SValuePtr regValue = regState->readRegister(regp, ops_->undefined_(regp.nBits()), ops_.get());
        return SymbolicSemantics::SValue::promote(regValue)->get_expression();
    }
};

// Parse memory names. These are expressions of the form (mem ADDRESS)
class MemoryExpansion: public SymbolicExprParser::OperatorExpansion {
    BaseSemantics::RiscOperatorsPtr ops_;
protected:
    MemoryExpansion(const BaseSemantics::RiscOperatorsPtr &ops)
        : SymbolicExprParser::OperatorExpansion(ops->solver()), ops_(ops) {}
public:
    static Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops) {
        Ptr functor = Ptr(new MemoryExpansion(ops));
        functor->title("Memory");
        functor->docString("Memory values are specified with the \"mem\" operator, which takes one operand: a memory address. "
                           "For example, the top byte of the stack for an 32-bit architecture is \"(mem esp)\". Each value "
                           "stored at a memory address is eight bits wide.");
        return functor;
    }
    SymbolicExpr::Ptr immediateExpansion(const SymbolicExprParser::Token &token, const SymbolicExpr::Nodes &operands) {
        if (token.lexeme() != "mem")
            return SymbolicExpr::Ptr();
        if (operands.size() != 1)
            throw token.syntaxError("mem operator expects one argument (address)");
        SymbolicSemantics::SValuePtr addr = SymbolicSemantics::SValue::promote(ops_->undefined_(operands[0]->nBits()));
        addr->set_expression(operands[0]);
        BaseSemantics::MemoryStatePtr memState = ops_->currentState()->memoryState();
        BaseSemantics::SValuePtr memValue = memState->readMemory(addr, ops_->undefined_(8), ops_.get(), ops_.get());
        if (token.exprType().nBits() != 0 && memValue->get_width() != token.exprType().nBits()) {
            throw token.syntaxError("operator size mismatch (specified=" + StringUtility::numberToString(token.exprType().nBits()) +
                                    ", actual=" + StringUtility::numberToString(memValue->get_width()) + ")");
        }
        if (token.exprType().typeClass() == SymbolicExpr::Type::MEMORY)
            throw token.syntaxError("memory operator width must be scalar");
        return SymbolicSemantics::SValue::promote(memValue)->get_expression();
    }
};

static SymbolicExprParser
postConditionParser(const BaseSemantics::RiscOperatorsPtr &ops) {
    SymbolicExprParser parser;
    parser.appendAtomExpansion(RegisterExpansion::instance(ops));
    parser.appendOperatorExpansion(MemoryExpansion::instance(ops));
    return parser;
}

// Incorporate post conditions into the path constraints.
static void
incorporatePostConditions(const BaseSemantics::RiscOperatorsPtr &ops, // ops contains final path state
                          std::vector<SymbolicExpr::Ptr> &pathConstraints /*out*/) {
    SymbolicExprParser parser = postConditionParser(ops);
    BOOST_FOREACH (const std::string &postCondStr, settings.postConditionsStr) {
        try {
            SymbolicExpr::Ptr expr = parser.parse(postCondStr, "tool argument");
            pathConstraints.push_back(expr);
        } catch (const SymbolicExprParser::SyntaxError &e) {
            std::cerr <<e <<"\n";
            if (e.lineNumber != 0) {
                std::cerr <<"    argument: " <<postCondStr <<"\n"
                          <<"    here------" <<std::string(e.columnNumber, '-') <<"^\n\n";
            }
            exit(1);
        }
    }
}

// Checks that post-conditions are valid before we spend a long time looking for feasible paths.
static void
checkPostConditionSyntax(const P2::Partitioner &partitioner) {
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    std::vector<SymbolicExpr::Ptr> pathConstraints;
    incorporatePostConditions(ops, pathConstraints);
}

/** Process one path. Given a path, determine if the path is feasible.  If @p showResults is set, then emit information about
 *  the initial conditions that cause this path to be taken. */
static SmtSolver::Satisfiable
singlePathFeasibility(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &paths, const P2::CfgPath &path,
                      bool atEndOfPath) {
    ASSERT_require(settings.searchMode == SEARCH_SINGLE_DFS);

    static int npaths = -1;
    ++npaths;
    Stream info(PathFinder::mlog[INFO]);
    Stream error(PathFinder::mlog[ERROR]);
    Stream debug(PathFinder::mlog[DEBUG]);
    info <<"path #" <<npaths <<" with " <<StringUtility::plural(path.nVertices(), "vertices", "vertex") <<"\n";

    std::string graphVizFileName;
    if (ALL_PATHS == settings.graphVizOutput) {
        graphVizFileName = printGraphViz(partitioner, paths, path, npaths);
        info <<"  saved as \"" <<StringUtility::cEscape(graphVizFileName) <<"\"\n";
    }

    SmtSolverPtr solver = SmtSolver::instance(CommandLine::genericSwitchArgs.smtSolver);
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    setInitialState(cpu, path.frontVertex());
    std::vector<SymbolicExpr::Ptr> pathConstraints;

    size_t pathInsnIndex = 0;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &pathEdge, path.edges()) {
        processVertex(cpu, pathEdge->source(), pathInsnIndex /*in,out*/);
        RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();
        BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.nBits()));
        if (ip->is_number()) {
            ASSERT_require(hasVirtualAddress(pathEdge->target()));
            if (ip->get_number() != virtualAddress(pathEdge->target())) {
                // Executing the path forces us to go a different direction than where the path indicates we should go. We
                // don't need an SMT solver to tell us that when the values are just integers.
                info <<"  not feasible according to ROSE semantics\n";
                return SmtSolver::SAT_NO;
            }
        } else if (hasVirtualAddress(pathEdge->target())) {
            SymbolicExpr::Ptr targetVa = SymbolicExpr::makeIntegerConstant(ip->get_width(), virtualAddress(pathEdge->target()));
            SymbolicExpr::Ptr constraint = SymbolicExpr::makeEq(targetVa,
                                                                SymbolicSemantics::SValue::promote(ip)->get_expression(),
                                                                solver);
            pathConstraints.push_back(constraint);
        }
    }
    if (atEndOfPath)
        incorporatePostConditions(ops, pathConstraints /*out*/);

    // Are the constraints satisfiable.  Empty constraints are tivially satisfiable.
    SmtSolver::Satisfiable isSatisfied = SmtSolver::SAT_UNKNOWN;
    isSatisfied = solver->satisfiable(pathConstraints);

    if (!atEndOfPath)
        return isSatisfied;

    if (isSatisfied == SmtSolver::SAT_YES) {
        printResults(partitioner, paths, path, npaths, pathConstraints, solver, ops);
    } else if (isSatisfied == SmtSolver::SAT_NO) {
        info <<"  not feasible according to SMT solver\n";
    } else {
        ASSERT_require(isSatisfied == SmtSolver::SAT_UNKNOWN);
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
    Stream info(PathFinder::mlog[INFO]);
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
    calleeCfgAvoidVertices.insert(cfgEndVertices);

    // Depth-first traversal of the "paths". When a function call is encountered we do one of two things: either expand the
    // called function into the paths graph and replace the call-ret edge with an actual function call and return edges, or do
    // nothing but skip over the function call.  When expanding a function call, we want to insert only those edges and
    // vertices that can participate in a path from the callee's entry point to any of its returning points.
    P2::CfgPath path(pathsBeginVertex);
    while (!path.isEmpty()) {
        // If backVertex is a function summary, then there is no corresponding cfgBackVertex.
        P2::ControlFlowGraph::ConstVertexIterator backVertex = path.backVertex();
        P2::ControlFlowGraph::ConstVertexIterator cfgBackVertex = pathToCfg(partitioner, backVertex);

        bool doBacktrack = false;
        bool atEndOfPath = pathsEndVertices.exists(backVertex);

        // Test path feasibility
        SmtSolver::Satisfiable isFeasible = singlePathFeasibility(partitioner, paths, path, atEndOfPath);
        if (atEndOfPath && isFeasible == SmtSolver::SAT_YES) {
            if (0 == --settings.maxPaths) {
                info <<"terminating because the maximum number of feasiable paths has been found\n";
                exit(0);
            }
            doBacktrack = true;
        } else if (atEndOfPath || isFeasible == SmtSolver::SAT_NO) {
            doBacktrack = true;
        }
        
        // If we've visited a vertex too many times (e.g., because of a loop or recursion), then don't go any further.
        if (path.nVisits(backVertex) > settings.vertexVisitLimit)
            doBacktrack = true;

        // Limit path length (in terms of number of instructions)
        size_t pathNInsns = 0;
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, path.vertices()) {
            switch (vertex->value().type()) {
                case P2::V_BASIC_BLOCK:
                    pathNInsns += vertex->value().bblock()->instructions().size();
                    break;
                case P2::V_INDETERMINATE:
                case P2::V_USER_DEFINED:
                    ++pathNInsns;
                    break;
                default:
                    ASSERT_not_reachable("invalid path vertex type");
            }
        }
        if (pathNInsns > settings.maxPathLength) {
            PathFinder::mlog[WARN] <<"maximum path length exceeded (" <<settings.maxPathLength <<" instructions)\n";
            doBacktrack = true;
        }
        
        // If we're visiting a function call site, then inline callee paths into the paths graph, but continue to avoid any
        // paths that go through user-specified avoidance vertices and edges. We can modify the paths graph during the
        // traversal because we're modifying parts of the graph that aren't part of the current path.  This is where having
        // insert- and erase-stable graph iterators is a huge help!
        if (!doBacktrack && pathEndsWithFunctionCall(partitioner, path) && !P2::findCallReturnEdges(backVertex).empty()) {
            ASSERT_require(partitioner.cfg().isValidVertex(cfgBackVertex));
            P2::CfgConstEdgeSet callEdges = P2::findCallEdges(cfgBackVertex);
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, callEdges.values()) {
                if (shouldSummarizeCall(path, partitioner.cfg(), cfgCallEdge->target())) {
                    insertCallSummary(paths, backVertex, partitioner.cfg(), cfgCallEdge);
                } else if (shouldInline(path, cfgCallEdge->target())) {
                    info <<"inlining function call paths at vertex " <<partitioner.vertexName(backVertex) <<"\n";
                    P2::inlineMultipleCallees(paths, backVertex, partitioner.cfg(), cfgBackVertex,
                                              calleeCfgAvoidVertices, cfgAvoidEdges);
                } else {
                    insertCallSummary(paths, backVertex, partitioner.cfg(), cfgCallEdge);
                }
            }

            // Remove all call-return edges. This is necessary so we don't re-enter this case with infinite recursion. No need
            // to worry about adjusting the path because these edges aren't on the current path.
            P2::eraseEdges(paths, P2::findCallReturnEdges(backVertex));

            // If the inlined function had no return sites but the call site had a call-return edge, then part of the paths
            // graph might now be unreachable. In fact, there might now be no paths from the begin vertex to any end vertex.
            // Erase those parts of the paths graph that are now unreachable.
            P2::eraseUnreachablePaths(paths, pathsBeginVertices, pathsEndVertices, vmap, path); // all args modified
            ASSERT_require2(!paths.isEmpty() || path.isEmpty(), "path is empty only if paths graph is empty");
            if (path.isEmpty())
                break;
            ASSERT_require(!pathsBeginVertices.empty());
            ASSERT_require(!pathsEndVertices.empty());
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
        P2::Inliner::HowInline how = P2::Inliner::INLINE_USER;

        do {
            if (callDepth > settings.maxCallDepth)
                break;                                  // max call depth exceeded
            
            // We must inline indeterminte functions or else we'll end up removing the call and return edge, which is another
            // way of saying "we know there's no path through here" when in fact there could be.
            if (cfgCallTarget->value().type() == P2::V_INDETERMINATE) {
                how = P2::Inliner::INLINE_NORMAL;
                break;
            }

            if (cfgCallTarget->value().type() != P2::V_BASIC_BLOCK)
                break;                                  // callee is not a basic block
        
            P2::Function::Ptr callee = cfgCallTarget->value().isEntryBlock();
            if (!callee)
                break;                                  // missing CFG information at this location?
            if (boost::ends_with(callee->name(), "@plt") || boost::ends_with(callee->name(), ".dll"))
                break;                                  // dynamically linked function
            if (std::find(settings.summarizeFunctions.begin(), settings.summarizeFunctions.end(), callee->address()) !=
                settings.summarizeFunctions.end())
                break;                                  // user requested summary from command-line

            // If the called function calls too many more functions then summarize instead of inlining.  This helps avoid
            // problems with the Partitioner2 not being able to find reasonable function boundaries, especially for GNU libc.
            static const size_t maxCallsAllowed = 10;   // arbitrary
            size_t nCalls = 0;
            using namespace Sawyer::Container::Algorithm;
            typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
            for (Traversal t(partitioner.cfg(), cfgCallTarget); t; ++t) {
                if (t.edge()->value().type() == P2::E_FUNCTION_CALL) {
                    if (++nCalls > maxCallsAllowed)
                        break;
                    t.skipChildren();
                } else if (t.edge()->value().type() == P2::E_FUNCTION_RETURN) {
                    t.skipChildren();
                }
            }
            if (nCalls > maxCallsAllowed)
                break;                                  // too many calls made by callee

            how = P2::Inliner::INLINE_NORMAL;
        } while (0);

        // Make sure there's a summary record for this function if we're using user-defined inlining
        if (P2::Inliner::INLINE_USER == how && !functionSummaries.exists(cfgCallTarget->value().address())) {
            P2::Function::Ptr function = cfgCallTarget->value().isEntryBlock();
            int64_t stackDelta = function ? function->stackDeltaConcrete() : SgAsmInstruction::INVALID_STACK_DELTA;
            
            FunctionSummary summary(cfgCallTarget, stackDelta);
            functionSummaries.insert(summary.address, summary);
        }

        return how;
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

struct BfsForestVertex {
    P2::ControlFlowGraph::ConstEdgeIterator pathsEdge;  // last CFG edge in path
    BaseSemantics::StatePtr state;                      // state at entry to this edge
    SymbolicExpr::Ptr constraint;                       // optional path constraint
    size_t nInsns;                                      // number of instructions to start of this edge
    size_t id;                                          // id number for debugging worklist
    BfsForestVertex(const P2::ControlFlowGraph::ConstEdgeIterator &pathsEdge, const BaseSemantics::StatePtr &state,
                    size_t nInsns)
        : pathsEdge(pathsEdge), state(state), nInsns(nInsns) {
        static size_t nextId = 0;
        id = nextId++;
    }
};

// We'll be performing a breadth-first traversal of the paths graph so we can process paths in order of their length. Some
// paths may visit a vertex or edge of the paths graph more than once (e.g., loops). Since this is a breadth-first search,
// we need a work list, but we don't want excessive copying of the edges that make up each path in the work list. We can
// use the fact that many paths share the same prefix, thus the set of all paths forms a tree. Those paths that need to be
// processed yet end at the leaves of the tree and form the frontier of the breadth-first search.  Actually, we need a
// forest because our tree is storing edges and even though the paths all start at the same vertex in the paths-graph, they
// don't all begin with the same edge. We don't have a specific tree container, but we can use Sawyer::Container::Graph as
// the tree.  The frontier is stored as an std::list whose members point to the tree leaves. We're also using the fact that
// Sawyer::Container::Graph iterators are stable over insert and erase operations.
typedef Sawyer::Container::Graph<BfsForestVertex> BfsForest;
typedef std::list<BfsForest::VertexIterator> BfsFrontier;

// Shared by all worker threads.
struct BfsContext {
    const P2::Partitioner &partitioner;
    const P2::ControlFlowGraph &pathsGraph;
    const P2::ControlFlowGraph::ConstVertexIterator pathsBeginVertex;
    const P2::CfgConstVertexSet pathsEndVertices;

    boost::mutex bfsForestMutex;                        // protects "bfs*" data members.
    BfsForest bfsForest;                                // tree keeping track of all outstanding path prefixes
    BfsFrontier bfsFrontier;                            // points to leaf nodes of the bfsForest sorted by path length
    boost::condition_variable bfsFrontierChanged;       // signaled when the frontier changes
    int nWorking;                                       // number of threads working, protected by bfsForestMutex
    static const int EXIT_NOW = -1;                     // special value for nWorking

    boost::mutex outputMutex;                           // protects output so things don't get mixed up between threads
    Sawyer::ProgressBar<size_t, SinglePathProgress> progress;// progress reports protected by outputMutex

    explicit BfsContext(const P2::Partitioner &partitioner, const P2::ControlFlowGraph &pathsGraph,
                        const P2::ControlFlowGraph::ConstVertexIterator &pathsBeginVertex,
                        const P2::CfgConstVertexSet &pathsEndVertices)
        : partitioner(partitioner), pathsGraph(pathsGraph), pathsBeginVertex(pathsBeginVertex),
          pathsEndVertices(pathsEndVertices), nWorking(0), progress(PathFinder::mlog[MARCH], "paths") {}
};


// Runs in a separate thread.
static void
singleThreadBfsWorker(BfsContext *ctx) {
    SmtSolverPtr solver = SmtSolver::instance(CommandLine::genericSwitchArgs.smtSolver);
    size_t lastTestedPathLength = 0;
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(ctx->partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    Sawyer::Message::Stream debug(PathFinder::mlog[DEBUG]);

    while (1) {
        // Get the next item of work. While we are working on it, no other worker will process a path for which our path is a
        // prefix, and no other worker will delete any part of our path from the bfsForest.
        BfsForest::VertexIterator bfsVertex;
        {
            boost::unique_lock<boost::mutex> lock(ctx->bfsForestMutex);
            while (ctx->nWorking != BfsContext::EXIT_NOW &&
                   ctx->bfsFrontier.empty() && ctx->nWorking != 0)
                ctx->bfsFrontierChanged.wait(lock);
            if ((ctx->bfsFrontier.empty() && 0 == ctx->nWorking) || ctx->nWorking == -1)
                return;                                 // done since no worker is creating more work
            bfsVertex = ctx->bfsFrontier.front();
            ctx->bfsFrontier.pop_front();
            ASSERT_require(ctx->bfsForest.isValidVertex(bfsVertex));
            ++ctx->nWorking;
        }

        // Get applicable vertices and edges
        bool abandonPrefix = false;                     // abandon searching of paths with this prefix?
        P2::ControlFlowGraph::ConstEdgeIterator pathsEdge = bfsVertex->value().pathsEdge;
        ASSERT_require(ctx->pathsGraph.isValidEdge(pathsEdge)); // forest vertices point to paths-graph edges
        SAWYER_MESG(debug) <<"processing path #" <<bfsVertex->value().id
                           <<" ending with edge " <<ctx->partitioner.edgeName(pathsEdge) <<"\n";

        // Initialize the cpu to the incoming state for this path edge. Incoming states for edges that originate at the same
        // paths-graph vertex share their state objects, so clone the state.  That way any semantic operations we perform in
        // this loop will be accessing the correct state.
        ASSERT_not_null(bfsVertex->value().state);
        ops->currentState(bfsVertex->value().state->clone());

        // If this edge's incoming instruction pointer is concrete and is not equal to this edge's address then we already know
        // that this path isn't feasible.
        RegisterDescriptor IP = cpu->instructionPointerRegister();
        BaseSemantics::SValuePtr ip = ops->readRegister(IP, ops->undefined_(IP.nBits()));
        if (!abandonPrefix && ip->is_number() &&
            pathsEdge->target()->value().type() != P2::V_INDETERMINATE && // has no address
            ip->get_number() != pathsEdge->target()->value().address()) {
            abandonPrefix = true;
            SAWYER_MESG(debug) <<"  path is infeasible according to ROSE\n";
        }

        // If this edge is not a number and we know the EIP at the end of this path edge, then we have a path constraint that
        // needs to be solved.
        if (!abandonPrefix && !ip->is_number() && pathsEdge->target()->value().type() != P2::V_INDETERMINATE) {
            SymbolicExpr::Ptr targetVa = SymbolicExpr::makeIntegerConstant(ip->get_width(), pathsEdge->target()->value().address());
            SymbolicExpr::Ptr constraint = SymbolicExpr::makeEq(targetVa,
                                                                SymbolicSemantics::SValue::promote(ip)->get_expression(),
                                                                solver);
            bfsVertex->value().constraint = constraint;
            SAWYER_MESG(debug) <<"  path edge has constraint expression\n";
        }

        // Accumulate all constraints along this path and invoke the SMT solver.
        bool atEndOfPath = ctx->pathsEndVertices.exists(pathsEdge->target());
        SmtSolver::Satisfiable isFeasible = SmtSolver::SAT_UNKNOWN;
        std::vector<SymbolicExpr::Ptr> pathConstraints;
        if (!abandonPrefix) {
            BfsForest::VertexIterator vertex=bfsVertex;
            lastTestedPathLength = 0;
            while (1) {
                ++lastTestedPathLength;
                if (vertex->value().constraint)
                    pathConstraints.push_back(vertex->value().constraint);
                if (vertex->nInEdges() != 1)
                    break;                              // vertex is the root of the tree
                vertex = vertex->inEdges().begin()->source();
            }
            if (atEndOfPath)
                incorporatePostConditions(ops, pathConstraints /*out*/);
            SAWYER_MESG(debug) <<"  solving " <<StringUtility::plural(pathConstraints.size(), "path constraints") <<"\n";
            isFeasible = solver->satisfiable(pathConstraints);
            if (SmtSolver::SAT_NO == isFeasible)
                abandonPrefix = true;
            SAWYER_MESG(debug) <<"  solver returned "
                               <<(SmtSolver::SAT_YES==isFeasible?"yes":(SmtSolver::SAT_NO==isFeasible?"no":"unknown")) <<"\n";
        }

        // Print results
        bool shouldExit = false;
        if (isFeasible == SmtSolver::SAT_YES && atEndOfPath) {
            SAWYER_MESG(debug) <<"  path is feasible and complete (printing results)\n";
            // Build the path. We do so backward from the end toward the beginning since that's most convenient.
            P2::CfgPath path(pathsEdge->target()); // end of the path, a vertex in the paths-graph

            {
                boost::lock_guard<boost::mutex>(ctx->bfsForestMutex);
                for (BfsForest::VertexIterator vi=bfsVertex;
                     vi!=ctx->bfsForest.vertices().end();
                     vi=vi->inEdges().begin()->source()) {
                    path.pushFront(vi->value().pathsEdge);
                    if (0 == vi->nInEdges())
                        break;                          // made it to root of bfsForest
                    ASSERT_require2(vi->nInEdges()==1, "must form a tree");
                }
            }
            
            {
                boost::lock_guard<boost::mutex> lock(ctx->outputMutex);
                printResults(ctx->partitioner, ctx->pathsGraph, path, bfsVertex->value().id, pathConstraints, solver, ops);
                if (0 == --settings.maxPaths) {
                    PathFinder::mlog[INFO] <<"terminating because the maximum number of feasible paths has been found\n";
                    shouldExit = true;
                }
            }
            abandonPrefix = true;                       // no need to keep going since we found a feasible path
        }

        // If this path contains too many instructions then abandon it.
        if (!abandonPrefix && bfsVertex->value().nInsns > settings.maxPathLength) {
            SAWYER_MESG(debug) <<"  path is too long (" <<bfsVertex->value().nInsns <<" instructions)\n";
            abandonPrefix = true;
        }

        // Progress report
        static size_t nReports = 0;
        if (++nReports % 16 == 0) {
            size_t bfsForestSize = 0;
            size_t bfsFrontierSize = 0;
            {
                boost::lock_guard<boost::mutex> lock(ctx->bfsForestMutex);
                bfsForestSize = ctx->bfsForest.nVertices();
                bfsFrontierSize = ctx->bfsFrontier.size();
            }

            {
                boost::lock_guard<boost::mutex> lock(ctx->outputMutex);
                ctx->progress.suffix().nPaths(bfsVertex->value().id+1);
                ctx->progress.suffix().pathLength(lastTestedPathLength);
                ctx->progress.suffix().forestSize(bfsForestSize);
                ctx->progress.suffix().frontierSize(bfsFrontierSize);
                ++(ctx->progress);
            }
        }

        // Either abandon this path and all paths that have this path as a prefix, or create some new path(s) by adding another
        // edge to this path.  When abandoning a prefix, erase the part of the bfsForest that isn't needed anymore (be sure not
        // to erase parts of this path that are prefixes of a path in the bfsFrontier).
        ASSERT_require(atEndOfPath || pathsEdge->target()->nOutEdges() > 0);
        ASSERT_require(abandonPrefix || !atEndOfPath);
        if (shouldExit) {
            boost::lock_guard<boost::mutex> lock(ctx->bfsForestMutex);
            ctx->nWorking = BfsContext::EXIT_NOW;
            ctx->bfsFrontierChanged.notify_all();
        } else if (abandonPrefix) {
            boost::lock_guard<boost::mutex> lock(ctx->bfsForestMutex);
            SAWYER_MESG(debug) <<"  abandoning path\n";
            while (1) {
                ASSERT_require2(bfsVertex->nInEdges() <= 1, "tree nodes have 0 or 1 parent");
                if (bfsVertex->nOutEdges() > 0)
                    break;                              // prefix is being used still
                if (bfsVertex->nInEdges() == 1) {       // has parent?
                    BfsForest::VertexIterator parent = bfsVertex->inEdges().begin()->source();
                    ctx->bfsForest.eraseVertex(bfsVertex);   // and incident edges
                    bfsVertex = parent;
                } else {
                    ctx->bfsForest.eraseVertex(bfsVertex);
                    break;
                }
            }
            if (ctx->nWorking != BfsContext::EXIT_NOW) {
                ASSERT_require(ctx->nWorking > 0);
                --ctx->nWorking;
            }
            ctx->bfsFrontierChanged.notify_all();
        } else {
            // We've already given the cpu a private copy of our incoming state. Compute the outgoing state using that copy,
            // and then cause all new paths (the ones created from this path by extending it one edge) to share that same
            // incoming state.  Also, once the new paths have been created there's no reason to continue holding on to our own
            // incoming state, so free it by assigning a null pointer (states are reference counted).
            SAWYER_MESG(debug) <<"  extending path\n";
            size_t nInsns = bfsVertex->value().nInsns;
            processVertex(cpu, pathsEdge->target(), nInsns /*in,out*/);
            boost::lock_guard<boost::mutex> lock(ctx->bfsForestMutex);
            BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, pathsEdge->target()->outEdges()) {
                BfsForest::VertexIterator v = ctx->bfsForest.insertVertex(BfsForestVertex(ctx->pathsGraph.findEdge(edge.id()),
                                                                                          cpu->currentState(), nInsns));
                ctx->bfsForest.insertEdge(bfsVertex, v);
                ctx->bfsFrontier.push_back(v);
                SAWYER_MESG(debug) <<"    path #" <<v->value().id <<" for edge " <<ctx->partitioner.edgeName(edge) <<"\n";
            }
            bfsVertex->value().state = BaseSemantics::StatePtr();
            if (ctx->nWorking != BfsContext::EXIT_NOW) {
                ASSERT_require(ctx->nWorking > 0);
                --ctx->nWorking;
            }
            ctx->bfsFrontierChanged.notify_all();
        }
    }
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
    PathFinder::mlog[INFO] <<"  final paths graph: " <<StringUtility::plural(inliner.paths().nVertices(), "vertices", "vertex")
                 <<", " <<StringUtility::plural(inliner.paths().nEdges(), "edges") <<"\n";
    if (inliner.paths().nVertices() <= 1)
        return;                                         // no path, or trivially feasible singleton path
    ASSERT_require(inliner.pathsBeginVertices().size()==1);
    ASSERT_require(!inliner.pathsEndVertices().empty());
    BfsContext ctx(partitioner, inliner.paths(), *inliner.pathsBeginVertices().values().begin(), inliner.pathsEndVertices());

    // Initialize the forest with unit length paths emanating from the beginning vertex. We've already taken care of the
    // singleton path case above, so we know there's at least one edge in every path.  Each vertex in the forest points to
    // a path edge in the paths-graph, and the virtual machine state at the source of that edge (i.e., as the edge is
    // entered).  Thus we must initialize the states of the first edge of all paths to be the state comuted by executing the
    // begining vertex. Incoming states for an edge are shared.
    Sawyer::Stopwatch searchTime;
    Sawyer::Message::Stream searching(PathFinder::mlog[INFO] <<"searching for feasible paths");
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    setInitialState(cpu, ctx.pathsBeginVertex);
    size_t nInsns = 0;
    processVertex(cpu, ctx.pathsBeginVertex, nInsns /*in,out*/);
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, ctx.pathsBeginVertex->outEdges()) {
        ctx.bfsFrontier.push_back(ctx.bfsForest.insertVertex(BfsForestVertex(ctx.pathsGraph.findEdge(edge.id()),
                                                                             cpu->currentState(), nInsns)));
    }

    // Start worker threads.
    if (settings.nThreads != 1)
        PathFinder::mlog[INFO] <<"starting " <<settings.nThreads <<" worker threads\n";
    boost::thread *threads = new boost::thread[settings.nThreads];
    for (size_t i=0; i<settings.nThreads; ++i)
        threads[i] = boost::thread(singleThreadBfsWorker, &ctx);
    for (size_t i=0; i<settings.nThreads; ++i)
        threads[i].join();
    delete[] threads;
    ASSERT_require(ctx.nWorking == BfsContext::EXIT_NOW || ctx.bfsFrontier.empty());
    ASSERT_require(ctx.nWorking == BfsContext::EXIT_NOW || ctx.bfsForest.isEmpty());
    searching <<"; took " <<searchTime <<" seconds\n";
}

#if 0 // [Robb Matzke 2018-04-10]
/** Merge states for multi-path feasibility analysis. Given two paths, such as when control flow merges after an "if"
 * statement, compute a state that represents both paths.  The new state that's returned will consist largely of ite
 * expressions. */
static BaseSemantics::StatePtr
mergeMultipathStates(const BaseSemantics::RiscOperatorsPtr &ops,
                     const BaseSemantics::StatePtr &s1, const BaseSemantics::StatePtr &s2) {
    ASSERT_not_null(ops);
    ASSERT_not_null(s2);
    if (s1 == NULL)
        return s2->clone();

    // The instruction pointer constraint to use values from s1, otherwise from s2.
    SymbolicSemantics::SValuePtr s1Constraint =
        SymbolicSemantics::SValue::promote(s1->readRegister(REG_PATH, ops->undefined_(REG_PATH.nBits()), ops.get()));

    Stream debug(PathFinder::mlog[DEBUG]);
    if (debug) {
        SymbolicSemantics::Formatter fmt = symbolicFormat("      ");
        debug <<"  +-------------------------------------------------\n"
              <<"  | Merging states for next instruction's input\n"
              <<"  +-------------------------------------------------\n"
              <<"    State s1:\n" <<(*s1 + fmt)
              <<"    State s2:\n" <<(*s2 + fmt);
    }

    // Merge register states s1reg and s2reg into mergedReg
    BaseSemantics::RegisterStateGenericPtr s1reg = BaseSemantics::RegisterStateGeneric::promote(s1->registerState());
    BaseSemantics::RegisterStateGenericPtr s2reg = BaseSemantics::RegisterStateGeneric::promote(s2->registerState());
    BaseSemantics::RegisterStateGenericPtr mergedReg = BaseSemantics::RegisterStateGeneric::promote(s1reg->clone());
    BOOST_FOREACH (const BaseSemantics::RegisterStateGeneric::RegPair &pair, s2reg->get_stored_registers()) {
        if (s1reg->is_partly_stored(pair.desc)) {
            // The register exists (at least partly) in both states, so merge its values.
            BaseSemantics::SValuePtr mergedVal =
                ops->ite(s1Constraint,
                         s1->readRegister(pair.desc, ops->undefined_(pair.desc.nBits()), ops.get()),
                         s2->readRegister(pair.desc, ops->undefined_(pair.desc.nBits()), ops.get()));
            mergedReg->writeRegister(pair.desc, mergedVal, ops.get());
        } else {
            // The register exists only in the s2 state, so copy it.
            mergedReg->writeRegister(pair.desc, pair.value, ops.get());
        }
    }
    mergedReg->erase_register(REG_PATH, ops.get()); // will be updated separately in processBasicBlock

    // Merge memory states s1mem and s2mem into mergedMem
    BaseSemantics::SymbolicMemoryPtr s1mem = BaseSemantics::SymbolicMemory::promote(s1->memoryState());
    BaseSemantics::SymbolicMemoryPtr s2mem = BaseSemantics::SymbolicMemory::promote(s2->memoryState());
    SymbolicExpr::Ptr memExpr1 = s1mem->expression();
    SymbolicExpr::Ptr memExpr2 = s2mem->expression();
    SymbolicExpr::Ptr mergedExpr = SymbolicExpr::makeIte(s1Constraint->get_expression(), memExpr1, memExpr2,
                                                         ops->solver());
    BaseSemantics::SymbolicMemoryPtr mergedMem = BaseSemantics::SymbolicMemory::promote(s1mem->clone());
    mergedMem->expression(mergedExpr);

    return ops->currentState()->create(mergedReg, mergedMem);
}
#endif

#if 0 // [Robb Matzke 2018-04-10]
// Merge all the predecessor outgoing states to create a new incoming state for the specified vertex.
static BaseSemantics::StatePtr
mergePredecessorStates(const BaseSemantics::RiscOperatorsPtr &ops, const P2::ControlFlowGraph::ConstVertexIterator vertex,
                       const StateStacks &outStates) {
    // If this is the initial vertex, then use the state that the caller has initialized already.
    if (0 == vertex->nInEdges())
        return ops->currentState();

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
#endif

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
#if 1 // [Robb P. Matzke 2015-05-09]
    TODO("[Robb P. Matzke 2015-05-09]");
#else

    Stream debug(PathFinder::mlog[DEBUG]);
    Stream info(PathFinder::mlog[INFO]);
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
    BaseSemantics::DispatcherPtr cpu = buildVirtualCpu(partitioner);
    RiscOperatorsPtr ops = RiscOperators::promote(cpu->get_operators());
    ops->writeRegister(REG_PATH, ops->boolean_(true)); // start of path is always feasible
    if (pathsBeginVertex->value().type() == P2::V_INDETERMINATE) {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->undefined_(cpu->instructionPointerRegister().nBits()));
    } else {
        ops->writeRegister(cpu->instructionPointerRegister(),
                           ops->number_(cpu->instructionPointerRegister().nBits(), virtualAddress(pathsBeginVertex)));
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
                ops->currentState(state); // modifing incoming state in place, changing it to an outgoing state
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


                BaseSemantics::StatePtr outgoingState = ops->currentState();
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
            std::cerr <<"Final state:\n" <<(*cpu->currentState() + fmt);
        }
        
        // Final path expression
        BaseSemantics::SValuePtr path = ops->readRegister(REG_PATH);
        TreeNodePtr constraint = InternalNode::create(1, OP_EQ,
                                                      SymbolicExpr::makeInteger(1, 1),
                                                      SymbolicSemantics::SValue::promote(path)->get_expression());
        info <<"  constraints expression has " <<StringUtility::plural(constraint->nnodes(), "terms") <<"\n";
        if (settings.showConstraints) {
            std::cout <<"  Constraints:\n";
            std::cout <<"    " <<*constraint <<"\n";
        }

        // Is the constraint satisfiable?
        info <<"  invoking SMT solver\n";
        SmtSolver::Satisfiable isSatisfied = solver.satisfiable(constraint);
        if (isSatisfied == SmtSolver::SAT_YES) {
            info <<"  constraints are satisfiable\n";
            std::cout <<"Found a feasible path (specified path is not available)\n";
            showPathEvidence(solver, ops);
        } else if (isSatisfied == SmtSolver::SAT_NO) {
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
    Stream info(PathFinder::mlog[INFO]);
    Stream progress(PathFinder::mlog[MARCH]);
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
    calleeCfgAvoidVertices.insert(cfgEndVertices);

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
        P2::CfgConstEdgeSet callEdges = P2::findCallEdges(cfgVertex);
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, callEdges.values()) {
            ++nVertsProcessed;
            if (work.callDepth < settings.maxCallDepth) {
                if (shouldSummarizeCall(work.vertex, partitioner.cfg(), cfgCallEdge->target())) {
                    ++nFuncsSummarized;
                    insertCallSummary(paths, work.vertex, partitioner.cfg(), cfgCallEdge);
                } else { // inline the function
                    ++nFuncsInlined;
                    std::vector<P2::ControlFlowGraph::ConstVertexIterator> insertedVertices;
                    P2::inlineOneCallee(paths, work.vertex, partitioner.cfg(), cfgCallEdge->target(),
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
    if (pathsEndVertices.exists(pathsBeginVertex))
        pathsIncidentVertices.erase(pathsBeginVertex); // allow singleton paths if appropriate
    P2::eraseEdges(paths, pathsUnreachableEdges);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &pathVertex, pathsIncidentVertices.values()) {
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
    // Initialization
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&PathFinder::mlog, "tool");
    Sawyer::Message::Stream info(PathFinder::mlog[INFO]);

    P2::Engine engine;
    engine.doingPostAnalysis(false);
    engine.usingSemantics(true);

    // Parse the command-line
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
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, endVertices.values())
        info <<" " <<partitioner.vertexName(vertex) <<";";
    info <<"\n";
    if (!avoidVertices.empty()) {
        info <<"avoiding the following vertices:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexIterator &vertex, avoidVertices.values())
            info <<" " <<partitioner.vertexName(vertex) <<";";
        info <<"\n";
    }
    if (!avoidEdges.empty()) {
        info <<"avoiding the following edges:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &edge, avoidEdges.values())
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

    checkPostConditionSyntax(partitioner);

    // Process individual paths (this may take a LONG TIME!)
    switch (settings.searchMode) {
        case SEARCH_MULTI:
            findAndProcessMultiPaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
            break;
        case SEARCH_SINGLE_DFS:
            findAndProcessSinglePaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
            break;
        case SEARCH_SINGLE_BFS:
            findAndProcessSinglePathsShortestFirst(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
            break;
    }
}
