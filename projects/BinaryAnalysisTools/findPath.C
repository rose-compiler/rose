#include <rose.h>
#include <rose_strtoull.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <sawyer/GraphTraversal.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace Sawyer::Container::Algorithm;
namespace P2 = Partitioner2;

Diagnostics::Facility mlog;

typedef std::set<P2::ControlFlowGraph::ConstVertexNodeIterator> CfgVertexSet;
typedef std::set<P2::ControlFlowGraph::ConstEdgeNodeIterator> CfgEdgeSet;

// Settings from the command-line
struct Settings {
    std::string isaName;                                // instruction set architecture name
    std::string beginVertex;                            // address or function name where paths should begin
    std::vector<std::string> endVertices;               // addresses or function names where paths should end
    std::vector<std::string> avoidVertices;             // vertices to avoid in any path
    std::vector<std::string> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    bool findAllForwardPaths;                           // do not limit number of paths returned?
    bool showInstructions;                              // show instructions in paths
    Settings(): beginVertex("_start"), findAllForwardPaths(false), showInstructions(false) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
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

    cfg.insert(Switch("all-forward-paths")
               .intrinsicValue(true, settings.findAllForwardPaths)
               .doc("This switch will cause all forward paths to be found which do not contain cycles.  This can produce "
                    "a very large number of paths and consume lots of memory. For instance, there are 1024 paths through "
                    "code with \"if\" statements nested to a depth of just 10. " +
                    std::string(settings.findAllForwardPaths ? "This is the default." :
                                "The default is to prune the number of possible paths by visiting each vertex only once "
                                "(other than the initial and final vertices).") +
                    " The @s{first-forward-paths} is the opposite of this switch."));
    cfg.insert(Switch("first-forward-paths")
               .key("all-forward-paths")
               .intrinsicValue(false, settings.findAllForwardPaths)
               .doc("Returns the set of paths such that no vertex appears in more than one path unless that vertex "
                    "is the first or last vertex of the path (all paths contain the same first and last vertex). This "
                    "switch is the opposite of @s{all-forward-paths}."));


    //--------------------------- 
    SwitchGroup out("Output switches");
    cfg.insert(Switch("show-instructions")
               .intrinsicValue(true, settings.showInstructions)
               .doc("Cause instructions to be listed as part of each path."));

    return parser.with(gen).with(dis).with(cfg).with(out).parse(argc, argv).apply();
}

// Return CFG basic block vertex that contains specified instruction address, or the end vertex if none found.
P2::ControlFlowGraph::ConstVertexNodeIterator
vertexForInstruction(const P2::Partitioner &partitioner, rose_addr_t insnVa) {
    if (P2::BasicBlock::Ptr bblock = partitioner.basicBlockContainingInstruction(insnVa))
        return partitioner.findPlaceholder(bblock->address());
    return partitioner.cfg().vertices().end();
}

P2::ControlFlowGraph::ConstVertexNodeIterator
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
    return vertexForInstruction(partitioner, va);
}

// Convert a pair of addresses to a CFG edge.  The source address can be a basic block or last instruction of the basic
// block.
P2::ControlFlowGraph::ConstEdgeNodeIterator
edgeForInstructions(const P2::Partitioner &partitioner,
                    const P2::ControlFlowGraph::ConstVertexNodeIterator &source,
                    const P2::ControlFlowGraph::ConstVertexNodeIterator &target) {
    if (source == partitioner.cfg().vertices().end() || target == partitioner.cfg().vertices().end())
        return partitioner.cfg().edges().end();         // sourceVa or targetVa is not an instruction starting address
    ASSERT_require(source->value().type() == P2::V_BASIC_BLOCK);
    ASSERT_require(target->value().type() == P2::V_BASIC_BLOCK);
    for (P2::ControlFlowGraph::ConstEdgeNodeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target() == target)
            return edge;
    }
    return partitioner.cfg().edges().end();
}

P2::ControlFlowGraph::ConstEdgeNodeIterator
edgeForInstructions(const P2::Partitioner &partitioner, const std::string &sourceNameOrVa, const std::string &targetNameOrVa) {
    return edgeForInstructions(partitioner,
                               vertexForInstruction(partitioner, sourceNameOrVa),
                               vertexForInstruction(partitioner, targetNameOrVa));
}

#if 0 // [Robb P. Matzke 2015-02-06]
// Returns a Boolean vector indicating whether an edge is significant.  An edge is significant if it appears on some
// path that originates at the beginVertex and reaches the endEdge.
std::vector<bool>
significantEdges(const P2::Partitioner &partitioner,
                 P2::ControlFlowGraph::ConstVertexNodeIterator beginVertex,
                 P2::ControlFlowGraph::ConstEdgeNodeIterator endEdge) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from the starting vertex
    std::vector<bool> forwardReachable(partitioner.cfg().nEdges(), false);
    typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> ForwardTraversal;
    for (ForwardTraversal t(partitioner.cfg(), beginVertex); t; ++t)
        forwardReachable[t->id()] = true;

    // Mark edges that are reachable with a backward traversal from the ending edge
    std::vector<bool> significant(partitioner.cfg().nEdges(), false);
    typedef DepthFirstReverseEdgeTraversal<const P2::ControlFlowGraph> ReverseTraversal;
    for (ReverseTraversal t(partitioner.cfg(), endEdge); t; ++t)
        significant[t->id()] = forwardReachable[t->id()];

#if 1 // DEBUGGING [Robb P. Matzke 2014-10-24]
    for (size_t i=0; i<significant.size(); ++i) {
        const P2::ControlFlowGraph::EdgeNode &edge = *partitioner.cfg().findEdge(i);
        if (significant[i] &&
            edge.source()->value().type()==P2::V_BASIC_BLOCK &&
            edge.target()->value().type()==P2::V_BASIC_BLOCK) {
            std::cerr <<"edge " <<StringUtility::addrToString(edge.source()->value().address())
                      <<":" <<StringUtility::addrToString(edge.target()->value().address())
                      <<"<" <<i <<"> is significant\n";
        }
    }
#endif

    return significant;
}
#endif

class CfgPath {
public:
    typedef std::list<P2::ControlFlowGraph::ConstEdgeNodeIterator> Edges;
private:
    Edges edges_;
    Sawyer::Optional<P2::ControlFlowGraph::ConstVertexNodeIterator> vertex_; // for paths of length zero
public:
    CfgPath() {}
    explicit CfgPath(const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex): vertex_(vertex) {}
    explicit CfgPath(const Edges &edges): edges_(edges) {}
    
    bool isEmpty() const {
        return edges_.empty() && !vertex_;
    }

    P2::ControlFlowGraph::ConstVertexNodeIterator firstVertex() const {
        return edges_.empty() ? *vertex_ : edges_.front()->source();
    }

    const Edges& edges() const {
        return edges_;
    }

    void append(const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge) {
        edges_.push_back(edge);
    }

    void append(const CfgPath &other) {
        if (!other.isEmpty())
            edges_.insert(edges_.end(), other.edges_.begin(), other.edges_.end());
    }
};

// Print a CFG path
void
showVertex(std::ostream &out, const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex, const Settings &settings) {
    if (vertex->value().type() == P2::V_BASIC_BLOCK) {
        out <<"  " <<StringUtility::addrToString(vertex->value().address());
        if (P2::Function::Ptr function = vertex->value().function())
            out <<" in " <<function->printableName();
        out <<"\n";
        if (settings.showInstructions) {
            if (P2::BasicBlock::Ptr bblock = vertex->value().bblock()) {
                BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
                    out <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
            }
        }
    }
}

void
showPath(std::ostream &out, const P2::Partitioner &partitioner, const CfgPath &path, const Settings &settings) {
    out <<"Path (" <<StringUtility::plural(path.edges().size(), "edges") <<"):\n";
    if (path.isEmpty()) {
        out <<" empty\n";
    } else {
        showVertex(out, path.firstVertex(), settings);
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge, path.edges()) {
            out <<"    edge " <<partitioner.edgeName(edge) <<"\n";
            showVertex(out, edge->target(), settings);
        }
    }
}

/** Find non-cyclic paths.
 *
 *  Finds all non-cyclic paths in the CFG starting at the @p beginVertex and ending at any of the @p endVertices (the first
 *  reached in each case), but not passing through any of the @p avoidVertices or @p avoidEdges.  Function calls are skipped
 *  over if the call does not reach any @p endVertex. */
std::vector<CfgPath>
findForwardPaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex,
                 const CfgVertexSet &endVertices, const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {
    ASSERT_forbid(beginVertex == partitioner.cfg().vertices().end());
    std::vector<CfgPath> paths;
    if (endVertices.empty())
        return paths;                                   // no end point specified
    if (avoidVertices.find(beginVertex) != avoidVertices.end())
        return paths;                                   // all paths would start with a vertex-to-be-avoided

    if (endVertices.find(beginVertex) != endVertices.end()) {
        paths.push_back(CfgPath(beginVertex));
        return paths;
    }
    if (beginVertex->nOutEdges() == 0)
        return paths;

    // Edges in the current path
    std::list<P2::ControlFlowGraph::ConstEdgeNodeIterator> pathEdges;
    pathEdges.push_back(beginVertex->outEdges().begin());

    // Vertices visited by the current path. pathEdges.back()->target() is not ever counted in these totals.
    static const size_t vertexVisitLimit = 2;           // max times a vertex can appear on a single path
    std::vector<size_t> visitingVertex(partitioner.cfg().nVertices(), 0);
    visitingVertex[beginVertex->id()] = 1;

    // The original implementation used a graph traversal, but it's hard to have fine control over how often a traversal is
    // allowed to visit each vertex. Therefore, we do it by hand.
    while (!pathEdges.empty()) {
        P2::ControlFlowGraph::ConstVertexNodeIterator nextVertex = pathEdges.back()->target();
        if (visitingVertex[nextVertex->id()] >= vertexVisitLimit) {
            // don't visit the vertex again
        } else if (avoidVertices.find(nextVertex) != avoidVertices.end()) {
            // don't visit this vertex even once!
        } else if (avoidEdges.find(pathEdges.back()) != avoidEdges.end()) {
            // don't follow this path
        } else if (endVertices.find(nextVertex) != endVertices.end()) {
            // found a complete path; don't go farther
            paths.push_back(CfgPath(pathEdges));
        } else if (nextVertex->nOutEdges() > 0) {
            // enter next vertex and append its first outgoing edge
            ++visitingVertex[nextVertex->id()];
            pathEdges.push_back(nextVertex->outEdges().begin());
            continue;
        }

        // Replace the last edge in the path with the next outgoing edge from the same vertex, but if we've reached the last
        // outgoing edge then pop vertices from the stack until we reach one that has another outgoing edge.
        while (!pathEdges.empty()) {
            P2::ControlFlowGraph::ConstVertexNodeIterator source = pathEdges.back()->source();
            ASSERT_require(source != partitioner.cfg().vertices().end());
            if (++pathEdges.back() != partitioner.cfg().edges().end())
                break;
            pathEdges.pop_back();
            ASSERT_require(visitingVertex[source->id()] > 0);
            --visitingVertex[source->id()];
        }
    }
    return paths;
}

/** Find functions called by this vertex.
 *
 *  Returns a set of CFG function call edges for functions the @p caller calls. */
CfgEdgeSet
findFunctionCalls(const P2::ControlFlowGraph::ConstVertexNodeIterator &caller) {
    CfgEdgeSet calls;
    for (P2::ControlFlowGraph::ConstEdgeNodeIterator edge=caller->outEdges().begin(); edge!=caller->outEdges().end(); ++edge) {
        if (edge->value().type() == P2::E_FUNCTION_CALL)
            calls.insert(edge);
    }
    return calls;
}

/** Find function return vertices.
 *
 *  Returns the list of vertices with outgoing E_FUNCTION_RETURN edges. */
CfgVertexSet
findFunctionReturns(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex) {
    CfgVertexSet endVertices;
    typedef DepthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(partitioner.cfg(), beginVertex); t; ++t) {
        if (t->value().type() == P2::E_FUNCTION_RETURN) {
            endVertices.insert(t->source());
            t.skipChildren();                           // found a function return edge
        } else if (t->value().type() == P2::E_FUNCTION_CALL) { // not E_FUNCTION_XFER
            t.skipChildren();                           // stay in this function
        }
    }
    return endVertices;
}

/** Expands function calls in a path.
 *
 *  Any function call in a path that was skipped over because the call to the function does not directly reach an endVertex is
 *  expanded by replacing that vertex of the path with all possible non-cyclic paths through the called function. */
std::vector<CfgPath>
expandPath(const P2::Partitioner &partitioner, const CfgPath &toExpand,
           const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {
    std::vector<CfgPath> retval;
    if (toExpand.isEmpty())
        return retval;
    if (toExpand.edges().empty()) {
        retval.push_back(CfgPath(toExpand.firstVertex()));
        return retval;
    }

    retval.push_back(CfgPath());
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator edge, toExpand.edges()) {
        if (edge->value().type() == P2::E_CALL_RETURN) {
            // This vertex represents a function that was skipped over.  Recursively expand all paths through that function and
            // then join them with the return value. If the return value has N paths and the callee has M paths then the new
            // return value will have N*M paths.  Also, we must handle the case of multiple callees (i.e., indirect call).
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &call, findFunctionCalls(edge->source())) {
                CfgVertexSet returns = findFunctionReturns(partitioner, call->target());
                std::vector<CfgPath> calleePaths = findForwardPaths(partitioner, call->target(), returns,
                                                                    avoidVertices, avoidEdges);
                std::vector<CfgPath> newRetval;
                BOOST_FOREACH (const CfgPath &oldPath, retval) {
                    BOOST_FOREACH (const CfgPath &calleePath, calleePaths) {
                        std::vector<CfgPath> expandedCalleePaths = expandPath(partitioner, calleePath, avoidVertices, avoidEdges);
                        BOOST_FOREACH (const CfgPath &expandedCalleePath, expandedCalleePaths) {
                            CfgPath path = oldPath;
                            path.append(call);
                            path.append(expandedCalleePath);
                            path.append(edge);
                            newRetval.push_back(path);
                        }
                    }
                }
                retval = newRetval;
            }
        } else {
            // This is not a call, so add it to each of the retval paths
            BOOST_FOREACH (CfgPath &path, retval)
                path.append(edge);
        }
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse the command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    P2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Disassemble and partition
    P2::Partitioner partitioner = engine.partition(specimenNames);

    // Find vertex at which all paths begin
    P2::ControlFlowGraph::ConstVertexNodeIterator beginVertex = vertexForInstruction(partitioner, settings.beginVertex);
    if (beginVertex == partitioner.cfg().vertices().end())
        throw std::runtime_error("no --begin vertex at " + settings.beginVertex);

    // Find vertices that terminate paths
    CfgVertexSet endVertices;
    BOOST_FOREACH (const std::string &nameOrVa, settings.endVertices) {
        P2::ControlFlowGraph::ConstVertexNodeIterator vertex = vertexForInstruction(partitioner, nameOrVa);
        if (vertex == partitioner.cfg().vertices().end())
            throw std::runtime_error("no --end vertex at " + nameOrVa);
        endVertices.insert(vertex);
    }
    if (endVertices.empty())
        throw std::runtime_error("no --end vertex specified; see --help");

    // Which vertices should be avoided
    CfgVertexSet avoidVertices;
    BOOST_FOREACH (const std::string &nameOrVa, settings.avoidVertices) {
        P2::ControlFlowGraph::ConstVertexNodeIterator vertex = vertexForInstruction(partitioner, nameOrVa);
        if (vertex == partitioner.cfg().vertices().end())
            throw std::runtime_error("no vertex at " + nameOrVa);
        avoidVertices.insert(vertex);
    }

    // Which edges should be avoided
    CfgEdgeSet avoidEdges;
    ASSERT_require(0 == settings.avoidEdges.size() % 2);
    for (size_t i=0; i<settings.avoidEdges.size(); i+=2) {
        const std::string &source = settings.avoidEdges[i+0];
        const std::string &target = settings.avoidEdges[i+1];
        P2::ControlFlowGraph::ConstEdgeNodeIterator edge = edgeForInstructions(partitioner, source, target);
        if (edge == partitioner.cfg().edges().end()) {
            throw std::runtime_error("no edge from " + source + " to " + target);
        }
        avoidEdges.insert(edge);
    }

    // Show the configuration
    mlog[INFO] <<"start at vertex: " <<partitioner.vertexName(beginVertex) <<";\n";
    mlog[INFO] <<"end at vertices:";
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex, endVertices)
        mlog[INFO] <<" " <<partitioner.vertexName(vertex) <<";";
    mlog[INFO] <<"\n";
    if (!avoidVertices.empty()) {
        mlog[INFO] <<"avoiding the following vertices:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex, avoidVertices)
            mlog[INFO] <<" " <<partitioner.vertexName(vertex) <<";";
        mlog[INFO] <<"\n";
    }
    if (!avoidEdges.empty()) {
        mlog[INFO] <<"avoiding the following edges:";
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge, avoidEdges)
            mlog[INFO] <<" " <<partitioner.edgeName(edge) <<";";
        mlog[INFO] <<"\n";
    }

    std::vector<CfgPath> paths = findForwardPaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
    mlog[INFO] <<"found " <<StringUtility::plural(paths.size(), "basic paths") <<"\n";
    BOOST_FOREACH (const CfgPath &path, paths) {
#if 1 // DEBUGGING [Robb P. Matzke 2015-02-11]
        std::cout <<"Unexpanded ";
        showPath(std::cout, partitioner, path, settings);
#endif
        std::vector<CfgPath> expandedPaths = expandPath(partitioner, path, avoidVertices, avoidEdges);
        std::cout <<"Expands to " <<StringUtility::plural(expandedPaths.size(), "paths") <<"\n";
        BOOST_FOREACH (const CfgPath &expandedPath, expandedPaths) {
            showPath(std::cout, partitioner, expandedPath, settings);
        }
    }
}
