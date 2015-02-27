#include <rose.h>
#include <rose_strtoull.h>
#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <sawyer/BiMap.h>
#include <sawyer/GraphTraversal.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
using namespace Sawyer::Container::Algorithm;
namespace P2 = Partitioner2;

Diagnostics::Facility mlog;

typedef std::set<P2::ControlFlowGraph::ConstVertexNodeIterator> CfgVertexSet;
typedef std::set<P2::ControlFlowGraph::ConstEdgeNodeIterator> CfgEdgeSet;

/** Map vertices from global-CFG to path-CFG. */
typedef Sawyer::Container::BiMap<P2::ControlFlowGraph::ConstVertexNodeIterator,
                                 P2::ControlFlowGraph::ConstVertexNodeIterator> VMap;


enum FollowCalls { SINGLE_FUNCTION, FOLLOW_CALLS };

// Settings from the command-line
struct Settings {
    std::string isaName;                                // instruction set architecture name
    std::string beginVertex;                            // address or function name where paths should begin
    std::vector<std::string> endVertices;               // addresses or function names where paths should end
    std::vector<std::string> avoidVertices;             // vertices to avoid in any path
    std::vector<std::string> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    size_t expansionDepthLimit;                         // max function call depth when expanding function calls
    size_t vertexVisitLimit;                            // max times a vertex can appear in a path
    bool showInstructions;                              // show instructions in paths
    Settings()
        : beginVertex("_start"), expansionDepthLimit(4), vertexVisitLimit(1), showInstructions(false) {}
};
static Settings settings;

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
               .doc("Max number of times a vertex can appear in a single path for some level of function call.  A value "
                    "of one means that the path will contain no back-edges. A value of two will typically allow one "
                    "iteration for each loop. The default is " +
                    StringUtility::numberToString(settings.vertexVisitLimit) + "."));

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

/** Finds edges that can be part of some path.
 *
 *  Returns a Boolean vector indicating whether an edge is significant.  An edge is significant if it appears on some path that
 *  originates at the @p beginVertex and reaches some vertex in @p endVertices but is not a member of @p avoidEdges and is not
 *  incident to any vertex in @p avoidVertices. */
std::vector<bool>
findSignificantEdges(const P2::ControlFlowGraph &graph,
                     P2::ControlFlowGraph::ConstVertexNodeIterator beginVertex, const CfgVertexSet &endVertices,
                     const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from the starting vertex, avoiding certain vertices and edges.
    std::vector<bool> forwardReachable(graph.nEdges(), false);
    typedef DepthFirstForwardGraphTraversal<const P2::ControlFlowGraph> ForwardTraversal;
    for (ForwardTraversal t(graph, beginVertex, ENTER_EVENTS); t; ++t) {
        switch (t.event()) {
            case ENTER_VERTEX:
                if (avoidVertices.find(t.vertex()) != avoidVertices.end())
                    t.skipChildren();
                break;
            case ENTER_EDGE:
                if (avoidEdges.find(t.edge()) != avoidEdges.end()) {
                    t.skipChildren();
                } else {
                    forwardReachable[t.edge()->id()] = true;
                }
                break;
            default:
                break;
        }
    }

    // Mark edges that are reachable with a backward traversal from any ending vertex, avoiding certain vertices and edges.
    std::vector<bool> significant(graph.nEdges(), false);
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &endVertex, endVertices) {
        typedef DepthFirstReverseGraphTraversal<const P2::ControlFlowGraph> ReverseTraversal;
        for (ReverseTraversal t(graph, endVertex, ENTER_EVENTS); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (avoidVertices.find(t.vertex()) != avoidVertices.end())
                        t.skipChildren();
                    break;
                case ENTER_EDGE:
                    if (avoidEdges.find(t.edge()) != avoidEdges.end()) {
                        t.skipChildren();
                    } else if (forwardReachable[t.edge()->id()]) {
                        significant[t.edge()->id()] = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return significant;
}

/** Insert one graph into another.
 *
 *  The @p vmap is updated with the mapping of vertices from source to destination. Upon return,
 *  <code>vmap[srcVertex->id()]</code> will point to the same vertex in the destination graph. */
void
insert(P2::ControlFlowGraph &dst, const P2::ControlFlowGraph &src, VMap &vmap /*out*/) {
    BOOST_FOREACH (P2::ControlFlowGraph::VertexNode vertex, src.vertices())
        vmap.insert(src.findVertex(vertex.id()), dst.insertVertex(vertex.value()));
    BOOST_FOREACH (P2::ControlFlowGraph::EdgeNode edge, src.edges())
        dst.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
}

/** Remove edges and vertices that cannot be on the paths.
 *
 *  Removes those edges that aren't reachable in both forward and reverse directions from the begin and end vertices,
 *  respectively. Vertices must belong to the paths-CFG. Erased vertices are also removed from @p vmap. */
void
eraseUnreachable(P2::ControlFlowGraph &paths /*in,out*/, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginPathVertex,
                 const CfgVertexSet &endPathVertices, VMap &vmap /*in,out*/) {
    if (beginPathVertex == paths.vertices().end()) {
        paths.clear();
        vmap.clear();
        return;
    }
    ASSERT_require(paths.isValidVertex(beginPathVertex));

    // Find edges that are reachable -- i.e., those that are part of a valid path
    CfgVertexSet avoidVertices;
    CfgEdgeSet avoidEdges;
    std::vector<bool> goodEdges = findSignificantEdges(paths, beginPathVertex, endPathVertices, avoidVertices, avoidEdges);

    // Remove edges that are not on any path. We do this in two steps since edge ID numbers are not erase-stable.
    CfgEdgeSet badEdges;
    for (size_t i=0; i<goodEdges.size(); ++i) {
        if (!goodEdges[i])
            badEdges.insert(paths.findEdge(i));
    }
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge, badEdges)
        paths.eraseEdge(edge);

    // Remove vertices that have no edges, except don't remove the start vertex yet.
    P2::ControlFlowGraph::ConstVertexNodeIterator vertex=paths.vertices().begin();
    while (vertex!=paths.vertices().end()) {
        if (vertex->degree()==0 && vertex!=beginPathVertex) {
            vmap.eraseTarget(vertex);
            vertex = paths.eraseVertex(vertex);
        } else {
            ++vertex;
        }
    }

    // If all that's left is the start vertex and the start vertex by itself is not a valid path, then remove it.
    if (paths.nVertices()==1 && endPathVertices.find(beginPathVertex)==endPathVertices.end()) {
        paths.clear();
        vmap.clear();
    }
}

/** Remove edges and vertices that cannot be on the paths.
 *
 *  Removes those edges that aren't reachable in both forward and reverse directions from the begin and end vertices,
 *  respectively. Vertices must belong to the global-CFG. Erased vertices are also removed from @p vmap. */
void
eraseUnreachable(const P2::Partitioner &partitioner, P2::ControlFlowGraph &paths /*in,out*/,
                 const P2::ControlFlowGraph::ConstVertexNodeIterator &beginGlobalVertex,
                 const CfgVertexSet &endGlobalVertices, VMap &vmap /*in,out*/) {
    ASSERT_require(partitioner.cfg().isValidVertex(beginGlobalVertex));
    if (!vmap.forward().exists(beginGlobalVertex)) {
        paths.clear();
        vmap.clear();
        return;
    }
    P2::ControlFlowGraph::ConstVertexNodeIterator beginVertex = vmap.forward()[beginGlobalVertex];
    ASSERT_require(paths.isValidVertex(beginVertex));

    CfgVertexSet endVertices;
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &globalVertex, endGlobalVertices) {
        ASSERT_require(partitioner.cfg().isValidVertex(globalVertex));
        if (vmap.forward().exists(globalVertex)) {
            P2::ControlFlowGraph::ConstVertexNodeIterator vertex = vmap.forward()[globalVertex];
            ASSERT_require(paths.isValidVertex(vertex));
            endVertices.insert(vertex);
        }
    }
    eraseUnreachable(paths, beginVertex, endVertices, vmap);
}
            
/** Compute all paths.
 *
 *  Computes all paths from @p beginVertex to any @p endVertices that does not go through any @p avoidVertices or @p
 *  avoidEdges. The paths are returned as a CFG so that cycles can be represented. A CFG can represent an exponential number of
 *  paths. The paths-CFG is formed by taking the global CFG and removing all @p avoidVertices and @p avoidEdges, any edge that
 *  cannot appear on a path from the @p beginVertex to any @p endVertices, and any vertex that has degree zero provided it is
 *  not the beginVertex.
 *
 *  If the returned graph is empty then no paths were found.  If the returned graph has a vertex but no edges then the vertex
 *  serves as both the begin and end of the path (i.e., a single path of unit length).  The @p vmap is updated to indicate the
 *  mapping from global-CFG vertices in the returned graph. */
P2::ControlFlowGraph
findPathsNoCalls(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex,
                 const CfgVertexSet &endVertices, const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges,
                 VMap &vmap /*out*/) {
    ASSERT_require(beginVertex != partitioner.cfg().vertices().end());
    ASSERT_require2(beginVertex == partitioner.cfg().findVertex(beginVertex->id()), "beginVertex does not belong to global CFG");

    vmap.clear();
    P2::ControlFlowGraph paths;
    std::vector<bool> goodEdges = findSignificantEdges(partitioner.cfg(), beginVertex, endVertices, avoidVertices, avoidEdges);
    BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNode &edge, partitioner.cfg().edges()) {
        if (goodEdges[edge.id()]) {
            if (!vmap.forward().exists(edge.source()))
                vmap.insert(edge.source(), paths.insertVertex(edge.source()->value()));
            if (!vmap.forward().exists(edge.target()))
                vmap.insert(edge.target(), paths.insertVertex(edge.target()->value()));
            paths.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
        }
    }
    if (!vmap.forward().exists(beginVertex) &&
        endVertices.find(beginVertex)!=endVertices.end() &&
        avoidVertices.find(beginVertex)==avoidVertices.end()) {
        vmap.insert(beginVertex, paths.insertVertex(beginVertex->value()));
    }
    return paths;
}

/** Find called functions.
 *
 *  Given some vertex in the global CFG, return the vertices representing the functions that are called. */
CfgVertexSet
findCalledFunctions(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &callSite) {
    ASSERT_require(callSite != partitioner.cfg().vertices().end());
    ASSERT_require2(callSite == partitioner.cfg().findVertex(callSite->id()), "callSite vertex must belong to global CFG");
    CfgVertexSet retval;
    BOOST_FOREACH (const P2::ControlFlowGraph::EdgeNode &edge, callSite->outEdges()) {
        if (edge.value().type() == P2::E_FUNCTION_CALL)
            retval.insert(partitioner.cfg().findVertex(edge.target()->id()));
    }
    return retval;
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

/** Replace a call-return edge with a function call.
 *
 *  The @p cretEdge must be of type E_CALL_RETURN, which signifies that a function call from the source vertex can return to
 *  the target vertex.  The called function is inserted into the path-CFG and edges are created to represent the call to the
 *  function and the return from the function. */
void
insertCallee(P2::ControlFlowGraph &paths, const P2::ControlFlowGraph::ConstEdgeNodeIterator &cretEdge,
             const P2::Partitioner &partitioner, const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {
    ASSERT_require(paths.isValidEdge(cretEdge));
    ASSERT_require(cretEdge->value().type() == P2::E_CALL_RETURN);

    P2::ControlFlowGraph::ConstVertexNodeIterator callSite = cretEdge->source();
    P2::ControlFlowGraph::ConstVertexNodeIterator pathRetTgt = cretEdge->target();
    ASSERT_require2(callSite->value().type() == P2::V_BASIC_BLOCK, "only basic blocks can call functions");

    // A basic block might call multiple functions if calling through a pointer.
    CfgVertexSet callees = findCalledFunctions(partitioner, partitioner.findPlaceholder(callSite->value().address()));
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &callee, callees) {
        if (callee->value().type() == P2::V_INDETERMINATE) {
            // This is a call to some indeterminate location. Just copy another indeterminate vertex into the
            // paths-CFG. Normally a CFG will have only one indeterminate vertex and it will have no outgoing edges, but the
            // paths-CFG is different.
            P2::ControlFlowGraph::ConstVertexNodeIterator indet = paths.insertVertex(P2::CfgVertex(P2::V_INDETERMINATE));
            paths.insertEdge(callSite, indet, P2::CfgEdge(P2::E_FUNCTION_CALL));
            paths.insertEdge(indet, pathRetTgt, P2::CfgEdge(P2::E_FUNCTION_RETURN));
            mlog[WARN] <<"indeterminate function call from " <<callSite->value().bblock()->printableName() <<"\n";
        } else {
            // Call to a normal function.
            ASSERT_require2(callee->value().type() == P2::V_BASIC_BLOCK, "non-basic block callees not implemented yet");
            std::string calleeName = callee->value().function() ? callee->value().function()->printableName() :
                                     callee->value().bblock()->printableName();

            // Find all paths through the callee
            VMap vmap1;                                     // relates global CFG to calleePaths
            CfgVertexSet returns = findFunctionReturns(partitioner, callee);
            P2::ControlFlowGraph calleePaths = findPathsNoCalls(partitioner, callee, returns, avoidVertices, avoidEdges, vmap1);
            if (calleePaths.isEmpty())
                mlog[WARN] <<calleeName <<" has no paths that return\n";

            // Insert the callee into the paths CFG
            VMap vmap2;                                     // relates calleePaths to paths
            insert(paths, calleePaths, vmap2);
            VMap vmap(vmap1, vmap2);                        // composite map from global-CFG to paths-CFG

            // Make an edge from call site to the entry block of the callee in the paths CFG
            if (vmap.forward().exists(callee)) {
                P2::ControlFlowGraph::ConstVertexNodeIterator pathStart = vmap.forward()[callee];
                paths.insertEdge(callSite, pathStart, P2::CfgEdge(P2::E_FUNCTION_CALL));
            }

            // Make edges from the callee's return statements back to the return point in the caller
            BOOST_FOREACH (P2::ControlFlowGraph::ConstVertexNodeIterator ret, returns) {
                if (vmap.forward().exists(ret)) {
                    P2::ControlFlowGraph::ConstVertexNodeIterator pathRetSrc = vmap.forward()[ret];
                    paths.insertEdge(pathRetSrc, pathRetTgt, P2::CfgEdge(P2::E_FUNCTION_RETURN));
                }
            }
        }
    }
}

P2::ControlFlowGraph::ConstEdgeNodeIterator
findFirstCallReturnEdge(const P2::ControlFlowGraph &graph, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex) {
    ASSERT_require(graph.isValidVertex(beginVertex));
    typedef BreadthFirstForwardEdgeTraversal<const P2::ControlFlowGraph> Traversal;
    for (Traversal t(graph, beginVertex); t; ++t) {
        if (t->value().type() == P2::E_CALL_RETURN)
            return t.edge();
    }
    return graph.edges().end();
}

void
robb(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex,
     const CfgVertexSet &endVertices, const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {

    //------------------------------------------------------------------------------------------------------------------------
    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    Stream info(mlog[INFO] <<"finding top-level paths");
    VMap vmap;                                          // relates global CFG vertices to path vertices
    P2::ControlFlowGraph paths = findPathsNoCalls(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges, vmap);
    if (!vmap.forward().exists(beginVertex)) {
        mlog[WARN] <<"no paths found\n";
        return;
    }
    P2::ControlFlowGraph::ConstVertexNodeIterator beginPath = vmap.forward()[beginVertex];
    info <<"; " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
         <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";

    //------------------------------------------------------------------------------------------------------------------------
    // Inline calls to functions. We must do this because in order to calculate the feasibility of a path we must know the
    // effect of calling the function.
    info <<"inlining function call paths";
    CfgVertexSet calleeAvoidVertices = avoidVertices;
    calleeAvoidVertices.insert(endVertices.begin(), endVertices.end());
    for (size_t nSubst=0; nSubst<settings.expansionDepthLimit; ++nSubst) {
        P2::ControlFlowGraph::ConstEdgeNodeIterator callRetEdge = findFirstCallReturnEdge(paths, beginPath);
        if (callRetEdge == paths.edges().end())
            break;
        P2::ControlFlowGraph::ConstVertexNodeIterator callReturnTarget = callRetEdge->target();
        insertCallee(paths, callRetEdge, partitioner, calleeAvoidVertices, avoidEdges);
        paths.eraseEdge(callRetEdge); callRetEdge = paths.edges().end();

        // If there are no edges coming into the return point and the return point is not the begin vertex, then the
        // return point is now unreachable. We need to prune away all parts of the paths-CFG that are not part of a
        // valid path.
        if (callReturnTarget->nInEdges()==0)
            eraseUnreachable(partitioner, paths, beginVertex, endVertices, vmap);
    }
    if (!vmap.forward().exists(beginVertex)) {
        mlog[WARN] <<"no paths found\n";
        return;
    }
    info <<"; " <<StringUtility::plural(paths.nVertices(), "vertices", "vertex")
         <<" and " <<StringUtility::plural(paths.nEdges(), "edges") <<"\n";


    //------------------------------------------------------------------------------------------------------------------------
    // Produce some output.
    info <<"producing GraphViz output";
    Color::HSV entryColor(0.15, 1.0, 0.6);              // bright yellow
    Color::HSV exitColor(0.088, 1.0, 0.6);              // bright orange

    // Show the results as a GraphViz file
    P2::GraphViz::CfgEmitter gv(partitioner, paths);
#if 1 // [Robb P. Matzke 2015-02-26]: makes dot slow
    gv.defaultGraphAttributes().insert("overlap", "scale");
    gv.useFunctionSubgraphs(true);
    gv.showInstructions(true);
    gv.showInstructionAddresses(true);
    gv.showInstructionStackDeltas(false);
#else
    gv.useFunctionSubgraphs(false);
    gv.showInstructions(false);
#endif
    gv.selectWholeGraph();
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstVertexNodeIterator &endVertex, endVertices) {
        if (vmap.forward().exists(endVertex))
            gv.vertexOrganization(vmap.forward()[endVertex]).attributes().insert("fillcolor", exitColor.toHtml());
    }
    if (vmap.forward().exists(beginVertex))
        gv.vertexOrganization(vmap.forward()[beginVertex]).attributes().insert("fillcolor", entryColor.toHtml());
    gv.emit(std::cout);
    info <<"; done\n";
}



    





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
showVertex(std::ostream &out, const P2::ControlFlowGraph::ConstVertexNodeIterator &vertex) {
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
showPath(std::ostream &out, const P2::Partitioner &partitioner, const CfgPath &path) {
    out <<"Path (" <<StringUtility::plural(path.edges().size(), "edges") <<"):\n";
    if (path.isEmpty()) {
        out <<" empty\n";
    } else {
        showVertex(out, path.firstVertex());
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge, path.edges()) {
            out <<"    edge " <<partitioner.edgeName(edge) <<"\n";
            showVertex(out, edge->target());
        }
    }
}

/** Find non-cyclic paths.
 *
 *  Finds all non-cyclic paths in the CFG starting at the @p beginVertex and ending at any of the @p endVertices (the first
 *  reached in each case), but not passing through any of the @p avoidVertices or @p avoidEdges.  Function calls are skipped
 *  over if the call does not reach any @p endVertex. */
std::vector<CfgPath>
findPaths(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstVertexNodeIterator &beginVertex,
          const CfgVertexSet &endVertices, const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges,
          FollowCalls followCalls, const std::vector<bool> &significantEdges = std::vector<bool>()) {
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

    if (mlog[DEBUG]) {
        std::string beginName;
        if (beginVertex->value().type() == P2::V_BASIC_BLOCK) {
            if (P2::Function::Ptr function = beginVertex->value().function())
                beginName = function->printableName();
        }
        if (beginName.empty())
            beginName = "vertex " + partitioner.vertexName(beginVertex);
        mlog[DEBUG] <<"findPaths: begin=" <<beginName <<"\n";
    }

    // Edges in the current path
    std::list<P2::ControlFlowGraph::ConstEdgeNodeIterator> pathEdges;
    pathEdges.push_back(beginVertex->outEdges().begin());

    // Vertices visited by the current path. pathEdges.back()->target() is not ever counted in these totals.
    std::vector<size_t> visitingVertex(partitioner.cfg().nVertices(), 0);
    visitingVertex[beginVertex->id()] = 1;

    // The original implementation used a graph traversal, but it's hard to have fine control over how often a traversal is
    // allowed to visit each vertex. Therefore, we do it by hand.
    while (!pathEdges.empty()) {
        P2::ControlFlowGraph::ConstVertexNodeIterator nextVertex = pathEdges.back()->target();
        if (!significantEdges.empty() && !significantEdges[pathEdges.back()->id()]) {
            // this edge cannot be on a valid path -- we proved that earlier
        } else if (followCalls==SINGLE_FUNCTION && pathEdges.back()->value().type() == P2::E_FUNCTION_CALL) {
            // don't follow function calls
        } else if (visitingVertex[nextVertex->id()] >= settings.vertexVisitLimit) {
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

std::string
edgeName(const P2::Partitioner &partitioner, const P2::ControlFlowGraph::ConstEdgeNodeIterator &edge) {
    std::string s;
    if (edge == partitioner.cfg().edges().end())
        return "end";
    if (edge->value().type() == P2::E_FUNCTION_CALL || edge->value().type() == P2::E_FUNCTION_XFER) {
        s = edge->value().type() == P2::E_FUNCTION_CALL ? "call" : "xfer";
        if (edge->source()->value().type() == P2::V_BASIC_BLOCK && edge->source()->value().function())
            s += " from " + edge->source()->value().function()->printableName();
        s += " at " + partitioner.vertexName(edge->source());
        if (edge->target()->value().type() == P2::V_BASIC_BLOCK && edge->target()->value().function()) {
            s += " to " + edge->target()->value().function()->printableName();
        } else {
            s += " to " + partitioner.vertexName(edge->target());
        }
    } else {
        s = partitioner.edgeName(edge);
    }
    return s;
}

void
showCallStack(std::ostream &out, const P2::Partitioner &partitioner,
              const std::vector<P2::ControlFlowGraph::ConstVertexNodeIterator> &callStack) {
    if (callStack.empty()) {
        out <<"call stack: empty\n";
    } else {
        out <<"call stack:\n";
        for (size_t i=0; i<callStack.size(); ++i) {
            out <<"  #" <<i <<": ";
            out <<partitioner.vertexName(callStack[i]);
            if (callStack[i] != partitioner.cfg().vertices().end() &&
                callStack[i]->value().type() == P2::V_BASIC_BLOCK &&
                callStack[i]->value().function())
                out <<" in " <<callStack[i]->value().function()->printableName();
            out <<"\n";
        }
    }
}

/** Expands function calls in a path.
 *
 *  Any function call in a path that was skipped over because the call to the function does not directly reach an endVertex is
 *  expanded by replacing that vertex of the path with all possible non-cyclic paths through the called function. */
std::vector<CfgPath>
expandPath(const P2::Partitioner &partitioner, const CfgPath &toExpand,
           const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges,
           std::vector<P2::ControlFlowGraph::ConstVertexNodeIterator> &callStack) {
    std::vector<CfgPath> retval;
    if (toExpand.isEmpty())
        return retval;
    if (toExpand.edges().empty()) {
        retval.push_back(CfgPath(toExpand.firstVertex()));
        return retval;
    }

    retval.push_back(CfgPath());
    BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator edge, toExpand.edges()) {
        if (edge->value().type() == P2::E_CALL_RETURN && callStack.size() < settings.expansionDepthLimit) {
            // This vertex represents a function that was skipped over.  Recursively expand all paths through that function and
            // then join them with the return value. If the return value has N paths and the callee has M paths then the new
            // return value will have N*M paths.  Also, we must handle the case of multiple callees (i.e., indirect call).
            BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeNodeIterator &call, findFunctionCalls(edge->source())) {
                if (std::find(callStack.begin(), callStack.end(), call->target()) != callStack.end()) {
                    // This is a recursive call to the function. We need to break the recursion to prevent this from running
                    // forever. We do so by not expanding the function call -- we treat it like a regular path node instead.
                    if (mlog[WARN]) {
                        mlog[WARN] <<"expandPath: recursive edge " <<edgeName(partitioner, edge) <<" avoided\n";
                        showCallStack(mlog[WARN], partitioner, callStack);
                    }
                    BOOST_FOREACH (CfgPath &path, retval)
                        path.append(edge);

                } else {
                    // Find all paths through the called function
                    CfgVertexSet returns = findFunctionReturns(partitioner, call->target());
                    std::vector<CfgPath> calleePaths = findPaths(partitioner, call->target(), returns,
                                                                 avoidVertices, avoidEdges, SINGLE_FUNCTION);
                    if (calleePaths.empty()) {
                        if (mlog[WARN]) {
                            mlog[WARN] <<"expandPath: no paths through " <<edgeName(partitioner, call) <<"\n";
                            showCallStack(mlog[WARN], partitioner, callStack);
                        }
                        BOOST_FOREACH (CfgPath &path, retval)
                            path.append(edge);
                    } else {
                        std::vector<CfgPath> newRetval;
                        BOOST_FOREACH (const CfgPath &oldPath, retval) {
                            BOOST_FOREACH (const CfgPath &calleePath, calleePaths) {
                                callStack.push_back(call->target());
                                std::vector<CfgPath> expandedCalleePaths = expandPath(partitioner, calleePath,
                                                                                      avoidVertices, avoidEdges, callStack);
                                ASSERT_require(!callStack.empty());
                                ASSERT_require(callStack.back() == call->target());
                                callStack.pop_back();
                                BOOST_FOREACH (const CfgPath &expandedCalleePath, expandedCalleePaths) {
                                    CfgPath path = oldPath;
                                    path.append(call);
                                    path.append(expandedCalleePath);
                                    path.append(edge);
                                    newRetval.push_back(path);
                                }
                            }
                        }
                        std::swap(retval, newRetval);
                        SAWYER_MESG(mlog[DEBUG]) <<"expandPath: depth=" <<callStack.size() <<", npaths=" <<retval.size() <<"\n";
                    }
                }
            }
        } else {
            // This is not a call, so add it to each of the retval paths
            BOOST_FOREACH (CfgPath &path, retval)
                path.append(edge);
        }
    }
    return retval;
}

std::vector<CfgPath>
expandPath(const P2::Partitioner &partitioner, const CfgPath &toExpand,
           const CfgVertexSet &avoidVertices, const CfgEdgeSet &avoidEdges) {
    std::vector<P2::ControlFlowGraph::ConstVertexNodeIterator> callStack;
    return expandPath(partitioner, toExpand, avoidVertices, avoidEdges, callStack);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Diagnostics::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse the command-line
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv).unreachedArgs();
    P2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Disassemble and partition
    Stream info(mlog[INFO] <<"disassembling");
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
            mlog[INFO] <<" " <<edgeName(partitioner, edge) <<";";
        mlog[INFO] <<"\n";
    }

#if 1 // [Robb P. Matzke 2015-02-25]
    robb(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges);
#else
    // Finding paths in the global CFG can be slow because the depth-first search can spend a huge amount of time exploring
    // parts of the CFG that are irrelevant. Therefore, we first compute an edge-reachability vector with a faster algorithm
    // that doesn't find paths, and use the reachability vector to restrict the first phase of path searching.
    std::vector<bool> significantEdges = findSignificantEdges(partitioner.cfg(), beginVertex, endVertices, avoidVertices,
                                                              avoidEdges);
    std::vector<CfgPath> paths = findPaths(partitioner, beginVertex, endVertices, avoidVertices, avoidEdges,
                                           FOLLOW_CALLS, significantEdges);
    mlog[INFO] <<"found " <<StringUtility::plural(paths.size(), "basic paths") <<"\n";

    BOOST_FOREACH (const CfgPath &path, paths) {
#if 1 // DEBUGGING [Robb P. Matzke 2015-02-11]
        std::cout <<"Unexpanded ";
        showPath(std::cout, partitioner, path);
#endif
        std::vector<CfgPath> expandedPaths = expandPath(partitioner, path, avoidVertices, avoidEdges);
        std::cout <<"Expands to " <<StringUtility::plural(expandedPaths.size(), "paths") <<"\n";
        BOOST_FOREACH (const CfgPath &expandedPath, expandedPaths) {
            showPath(std::cout, partitioner, expandedPath);
        }
    }
#endif
}
