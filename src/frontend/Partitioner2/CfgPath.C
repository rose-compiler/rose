#include <sage3basic.h>
#include <Diagnostics.h>
#include <Partitioner2/CfgPath.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/GraphTraversal.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

bool
CfgPath::isConnected() const {
    if (!isEmpty()) {
        ControlFlowGraph::ConstVertexIterator vertex = frontVertex();
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
            if (edge->source() != vertex)
                return false;
            vertex = edge->target();
        }
    }
    return true;
}

CfgPath::Vertices
CfgPath::vertices() const {
    Vertices retval;
    if (!isEmpty()) {
        retval.push_back(frontVertex());
        BOOST_FOREACH (const Edges::value_type &edge, edges_)
            retval.push_back(edge->target());
    }
    return retval;
}

void
CfgPath::pushBack(const ControlFlowGraph::ConstEdgeIterator &edge) {
    ASSERT_require(isEmpty() || edge->source()==backVertex());
    if (isEmpty())
        frontVertex_ = edge->source();
    edges_.push_back(edge);
}

void
CfgPath::popBack() {
    ASSERT_forbid(isEmpty());
    ASSERT_forbid(isEmpty());
    if (edges_.empty()) {
        // Erasing the starting vertex; then the path will be empty
        frontVertex_ = Sawyer::Nothing();
    } else {
        edges_.pop_back();
    }
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::backtrack() {
    std::vector<ControlFlowGraph::ConstEdgeIterator> removedEdges;
    while (!edges_.empty()) {
        ControlFlowGraph::ConstEdgeIterator edgeToRemove = edges_.back();
        ControlFlowGraph::ConstVertexIterator vertex = edgeToRemove->source();
        removedEdges.push_back(edgeToRemove);
        ++edges_.back();
        if (edges_.back() != vertex->outEdges().end())
            return removedEdges;
        edges_.pop_back();
    }
    clear();
    return removedEdges;
}

size_t
CfgPath::nVisits(const ControlFlowGraph::ConstVertexIterator &vertex) const {
    size_t retval = 0;
    if (!isEmpty()) {
        if (frontVertex() == vertex)
            ++retval;
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
            if (edge->target() == vertex)
                ++retval;
        }
    }
    return retval;
}

size_t
CfgPath::nVisits(const ControlFlowGraph::ConstEdgeIterator &edge) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &e, edges_) {
        if (e == edge)
            ++retval;
    }
    return retval;
}

size_t
CfgPath::nCalls(const Function::Ptr &function) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++retval;
            } else if (edge->target()->value().type() == V_BASIC_BLOCK &&
                       edge->target()->value().function() == function)
                ++retval;
        }
    }
    return retval;
}

size_t
CfgPath::nReturns(const Function::Ptr &function) const {
    size_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                ++retval;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().function() == function)
                ++retval;
        }
    }
    return retval;
}

ssize_t
CfgPath::callDepth(const Function::Ptr &function) const {
    ssize_t depth = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().function() == function)
                ++depth;
        } else if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                --depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().function() == function)
                --depth;
        }
    }
    return depth;
}

size_t
CfgPath::maxCallDepth(const Function::Ptr &function) const {
    ssize_t depth = 0;
    ssize_t retval = 0;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_) {
        if (edge->value().type() == E_FUNCTION_CALL) {
            if (!function) {
                ++depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().function() == function)
                ++depth;
        } else if (edge->value().type() == E_FUNCTION_RETURN) {
            if (!function) {
                --depth;
            } else if (edge->source()->value().type() == V_BASIC_BLOCK &&
                       edge->source()->value().function() == function)
                --depth;
        }
        retval = std::max(retval, depth);
    }
    return retval;
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::truncate(const CfgConstEdgeSet &toRemove) {
    for (Edges::iterator ei=edges_.begin(); ei!=edges_.end(); ++ei) {
        if (toRemove.find(*ei) != toRemove.end()) {
            // Remove path edges from here to the end
            std::vector<ControlFlowGraph::ConstEdgeIterator> removedEdges(ei, edges_.end());
            std::reverse(removedEdges.begin(), removedEdges.end());
            edges_.erase(ei, edges_.end());
            return removedEdges;
        }
    }
    return std::vector<ControlFlowGraph::ConstEdgeIterator>();
}

std::vector<ControlFlowGraph::ConstEdgeIterator>
CfgPath::truncate(const ControlFlowGraph::ConstEdgeIterator &edge) {
    CfgConstEdgeSet toRemove;
    toRemove.insert(edge);
    return truncate(toRemove);
}

void
CfgPath::print(std::ostream &out) const {
    if (isEmpty()) {
        out <<"empty";
    } else if (edges_.empty()) {
        out <<"vertex " <<Partitioner::vertexName(*frontVertex());
    } else {
        out <<StringUtility::plural(edges_.size(), "edges") <<":";
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges_)
            out <<" " <<Partitioner::edgeName(*edge);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Utility functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, const CfgPath &path) {
    path.print(out);
    return out;
}

std::vector<bool>
findPathEdges(const ControlFlowGraph &graph, const CfgConstVertexSet &beginVertices,
              const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices,
              const CfgConstEdgeSet &avoidEdges, bool avoidCallsAndReturns) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from any starting vertex, avoiding certain vertices and edges.
    std::vector<bool> forwardReachable(graph.nEdges(), false);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &beginVertex, beginVertices) {
        ASSERT_require(graph.isValidVertex(beginVertex));
        typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> ForwardTraversal;
        for (ForwardTraversal t(graph, beginVertex, ENTER_EVENTS); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (avoidVertices.find(t.vertex()) != avoidVertices.end())
                        t.skipChildren();
                    break;
                case ENTER_EDGE:
                    if (avoidCallsAndReturns &&
                        (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_RETURN)) {
                        t.skipChildren();
                    } else if (avoidEdges.find(t.edge()) != avoidEdges.end()) {
                        t.skipChildren();
                    } else {
                        forwardReachable[t.edge()->id()] = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Mark edges that are reachable with a backward traversal from any ending vertex, avoiding certain vertices and edges.
    std::vector<bool> significant(graph.nEdges(), false);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &endVertex, endVertices) {
        ASSERT_require(graph.isValidVertex(endVertex));
        typedef DepthFirstReverseGraphTraversal<const ControlFlowGraph> ReverseTraversal;
        for (ReverseTraversal t(graph, endVertex, ENTER_EVENTS); t; ++t) {
            switch (t.event()) {
                case ENTER_VERTEX:
                    if (avoidVertices.find(t.vertex()) != avoidVertices.end())
                        t.skipChildren();
                    break;
                case ENTER_EDGE:
                    if (avoidCallsAndReturns &&
                        (t.edge()->value().type() == E_FUNCTION_CALL || t.edge()->value().type() == E_FUNCTION_RETURN)) {
                        t.skipChildren();
                    } else if (avoidEdges.find(t.edge()) != avoidEdges.end()) {
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

CfgConstEdgeSet
findPathReachableEdges(const ControlFlowGraph &graph,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
                       bool avoidCallsAndReturns) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges,
                                                avoidCallsAndReturns);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}
    
CfgConstEdgeSet
findPathUnreachableEdges(const ControlFlowGraph &graph,
                         const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                         const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
                         bool avoidCallsAndReturns) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges,
                                                avoidCallsAndReturns);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (!goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}

size_t
eraseUnreachablePaths(ControlFlowGraph &graph /*in,out*/, const CfgConstVertexSet &beginVertices,
                      const CfgConstVertexSet &endVertices, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/) {
    size_t origPathSize = path.nEdges();
    if (beginVertices.empty() || endVertices.empty()) {
        graph.clear();
        vmap.clear();
        path.clear();
        return origPathSize;
    }

    // Erase unreachable edges from the graph and path
    CfgConstVertexSet avoidVertices;
    CfgConstEdgeSet avoidEdges;
    CfgConstEdgeSet badEdges = findPathUnreachableEdges(graph, beginVertices, endVertices, avoidVertices, avoidEdges, false);
    CfgConstVertexSet incidentVertices = findIncidentVertices(badEdges);
    path.truncate(badEdges);
    eraseEdges(graph, badEdges);

    // This might leave some vertices having no incident edges, so remove them since they can't participate on any path. Avoid
    // scanning the entire graph by considering only vertices that are incident to the edges we just removed.
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, incidentVertices) {
        if (vertex->degree() == 0) {
            if (beginVertices.find(vertex)!=beginVertices.end() && endVertices.find(vertex)!=endVertices.end()) {
                // Don't remove this vertex if it can be a singleton path.
            } else {
                vmap.eraseTarget(vertex);
                graph.eraseVertex(vertex);
            }
        }
    }
    if (graph.isEmpty())
        path.clear();

    return origPathSize - path.nEdges();
}

void
findPaths(const ControlFlowGraph &cfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
          const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
          const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
          bool avoidCallsAndReturns) {
    paths.clear();
    vmap.clear();

    // Find all edges and insert them along with incident vertices.
    std::vector<bool> goodEdges = findPathEdges(cfg, beginVertices, endVertices, avoidVertices, avoidEdges, avoidCallsAndReturns);
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfg.edges()) {
        if (goodEdges[edge.id()]) {
            if (!vmap.forward().exists(edge.source()))
                vmap.insert(edge.source(), paths.insertVertex(edge.source()->value()));
            if (!vmap.forward().exists(edge.target()))
                vmap.insert(edge.target(), paths.insertVertex(edge.target()->value()));
            paths.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
        }
    }

    // Make sure begin vertices are present. They were probably inserted above except if the vertex has no incident edges, in
    // which case the begin vertex should be inserted only if it can be a singleton path.
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &beginVertex, beginVertices) {
        if (!vmap.forward().exists(beginVertex) &&                      // not inserted above
            endVertices.find(beginVertex)!=endVertices.end() &&         // can be a singleton path
            avoidVertices.find(beginVertex)==avoidVertices.end()) {     // not an avoided vertex
            vmap.insert(beginVertex, paths.insertVertex(beginVertex->value()));
        }
    }
}

void
findFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                  const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                  const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, endVertices, avoidVertices, avoidEdges, true);
}

void
findInterFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    return findPaths(srcCfg, paths, vmap, beginVertices, endVertices, avoidVertices, avoidEdges, false);
}
    
bool
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                  const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges,
                  std::vector<ControlFlowGraph::ConstVertexIterator> *newVertices /*=NULL*/) {
    bool somethingInserted = false;
    CfgConstEdgeSet cfgCallEdges = findCallEdges(cfgCallSite);
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, cfgCallEdges) {
        if (insertCalleePaths(paths /*in,out*/, pathsCallSite, cfg, cfgCallEdge, cfgAvoidVertices, cfgAvoidEdges, newVertices))
            somethingInserted = true;
    }
    return somethingInserted;
}

bool
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge,
                  const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges,
                  std::vector<ControlFlowGraph::ConstVertexIterator> *newVertices /*=NULL*/) {
    ASSERT_require(paths.isValidVertex(pathsCallSite));
    ASSERT_require(cfg.isValidEdge(cfgCallEdge));
    ASSERT_require2(pathsCallSite->value().type() == V_BASIC_BLOCK, "only basic blocks can call functions");

    // Most functions either don't return or have a single return target.  Functions like longjmp might have multiple return
    // targets. For need to know the return targets in the paths graph because we will create new edges from the inlined
    // function's return sites to each return target.
    CfgConstVertexSet pathsReturnTargets;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, pathsCallSite->outEdges()) {
        if (edge.value().type() == E_CALL_RETURN)
            pathsReturnTargets.insert(edge.target());
    }

    // If this is a call to some indeterminate function, just copy another indeterminate vertex into the paths graph. Normally
    // a CFG will have only one indeterminate vertex and it will have no outgoing edges, but the paths graph is different. We
    // need separate indeterminate vertices so that each has its own function-return edge(s).
    ControlFlowGraph::ConstVertexIterator cfgCallTarget = cfgCallEdge->target();
    if (cfgCallTarget->value().type() == V_INDETERMINATE) {
        ControlFlowGraph::ConstVertexIterator indet = paths.insertVertex(CfgVertex(V_INDETERMINATE));
        if (newVertices)
            newVertices->push_back(indet);
        paths.insertEdge(pathsCallSite, indet, CfgEdge(E_FUNCTION_CALL));
        BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &returnTarget, pathsReturnTargets)
            paths.insertEdge(indet, returnTarget, CfgEdge(E_FUNCTION_RETURN));
        SAWYER_MESG(mlog[DEBUG]) <<"insertCalleePaths: indeterminate function call from "
                                 <<pathsCallSite->value().bblock()->printableName() <<"\n";
        return true;
    }

    // Call to a normal function.
    ASSERT_require2(cfgCallTarget->value().type() == V_BASIC_BLOCK, "non-basic block callees not implemented yet");
    std::string calleeName = cfgCallTarget->value().function() ? cfgCallTarget->value().function()->printableName() :
                             cfgCallTarget->value().bblock()->printableName();

    // Find all paths through the callee that return and avoid certain vertices and edges.
    CfgVertexMap vmap1;                                 // relates CFG to calleePaths
    CfgConstVertexSet cfgCallTargets; cfgCallTargets.insert(cfgCallTarget);
    CfgConstVertexSet cfgReturns = findFunctionReturns(cfg, cfgCallTarget);
    ControlFlowGraph calleePaths;
    findFunctionPaths(cfg, calleePaths, vmap1, cfgCallTargets, cfgReturns, cfgAvoidVertices, cfgAvoidEdges);
    if (calleePaths.isEmpty()) {
        SAWYER_MESG(mlog[DEBUG]) <<"insertCalleePaths: " <<calleeName <<" has no paths to insert\n";
        return false;
    }

    // Insert the callee into the paths CFG
    CfgVertexMap vmap2;                                 // relates calleePaths to paths
    insertCfg(paths, calleePaths, vmap2);
    if (newVertices) {
        BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vmap2.forward().values()) {
            ASSERT_require(paths.isValidVertex(vertex));
            newVertices->push_back(vertex);
        }
    }
    CfgVertexMap vmap(vmap1, vmap2);                    // composite map from the CFG to paths graph

    // Make an edge from call site to the entry block of the callee in the paths graph
    if (vmap.forward().exists(cfgCallTarget)) {
        ControlFlowGraph::ConstVertexIterator pathStart = vmap.forward()[cfgCallTarget];
        paths.insertEdge(pathsCallSite, pathStart, CfgEdge(E_FUNCTION_CALL));
    }

    // Make edges from the callee's return statements back to the return targets in the caller
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &cfgReturnSite, cfgReturns) {
        if (vmap.forward().exists(cfgReturnSite)) {
            ControlFlowGraph::ConstVertexIterator pathsReturnSite = vmap.forward()[cfgReturnSite];
            BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathsReturnTarget, pathsReturnTargets)
                paths.insertEdge(pathsReturnSite, pathsReturnTarget, CfgEdge(E_FUNCTION_RETURN));
        }
    }
    return true;
}

bool
Inliner::ShouldInline::operator()(const Partitioner &partitioner, const ControlFlowGraph::ConstEdgeIterator cfgCallEdge,
                                  const ControlFlowGraph &paths, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                  size_t callDepth) {
    ASSERT_require(partitioner.cfg().isValidEdge(cfgCallEdge));
    ASSERT_require(paths.isValidVertex(pathsCallSite));
    return callDepth <= maxCallDepth_;
}

// class method
ControlFlowGraph::ConstVertexIterator
Inliner::pathToCfg(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexIterator &pathVertex) {
    switch (pathVertex->value().type()) {
        case V_BASIC_BLOCK:
            return partitioner.findPlaceholder(pathVertex->value().address());
        case V_INDETERMINATE:
            return partitioner.indeterminateVertex();
        case V_NONEXISTING:
            return partitioner.nonexistingVertex();
        case V_UNDISCOVERED:
            return partitioner.undiscoveredVertex();
        case V_USER_DEFINED:
            return partitioner.cfg().vertices().end();
    }
    ASSERT_not_reachable("invalid vertex type");
}

// class method
bool
Inliner::isFunctionCall(const Partitioner &partitioner, const ControlFlowGraph::ConstVertexIterator &pathVertex) {
    ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, pathVertex);
    ASSERT_require(partitioner.cfg().isValidVertex(cfgVertex));
    BOOST_FOREACH (ControlFlowGraph::Edge edge, cfgVertex->outEdges()) {
        if (edge.value().type() == E_FUNCTION_CALL)
            return true;
    }
    return false;
}

void
Inliner::reset(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
              const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
              const CfgConstEdgeSet &cfgAvoidEdges) {

    paths_.clear();
    vmap_.clear();
    workList_.clear();
    if (!shouldInline_)
        shouldInline_ = ShouldInline::instance();

    // Find top-level paths. These paths don't traverse into function calls unless they must do so in order to reach an ending
    // vertex.
    findInterFunctionPaths(partitioner.cfg(), paths_/*out*/, vmap_/*out*/,
                           cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);
    if (paths_.isEmpty()) {
        pathsBeginVertices_.clear();
        pathsEndVertices_.clear();
        return;
    }
    pathsBeginVertices_ = forwardMapped(cfgBeginVertices, vmap_);
    pathsEndVertices_ = forwardMapped(cfgEndVertices, vmap_);

    // Set up the initial worklist
    for (ControlFlowGraph::ConstVertexIterator vert=paths_.vertices().begin(); vert!=paths_.vertices().end(); ++vert) {
        if (isFunctionCall(partitioner, vert) && !findCallReturnEdges(vert).empty())
            workList_.push_back(CallSite(vert, 0));
    }
}

void
Inliner::inlinePaths(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
                     const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
                     const CfgConstEdgeSet &cfgAvoidEdges) {

    reset(partitioner, cfgBeginVertices, cfgEndVertices, cfgAvoidVertices, cfgAvoidEdges);

    // When finding paths through a called function, avoid the usual vertices and edges, but also avoid those vertices that
    // mark the end of paths. We want paths that go all the way from the entry block of the called function to its returning
    // blocks.
    CfgConstVertexSet calleeCfgAvoidVertices = cfgAvoidVertices;
    calleeCfgAvoidVertices.insert(cfgEndVertices.begin(), cfgEndVertices.end());

    while (!workList_.empty()) {
        CallSite work = workList_.back();
        workList_.pop_back();
        ControlFlowGraph::ConstVertexIterator cfgVertex = pathToCfg(partitioner, work.pathsVertex);
        BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge, findCallEdges(cfgVertex)) {
            if ((*shouldInline_)(partitioner, cfgCallEdge, paths_, work.pathsVertex, work.callDepth)) {
                std::vector<ControlFlowGraph::ConstVertexIterator> insertedVertices;
                insertCalleePaths(paths_, work.pathsVertex, partitioner.cfg(), cfgCallEdge, calleeCfgAvoidVertices, cfgAvoidEdges,
                                  &insertedVertices);
                eraseEdges(paths_, findCallReturnEdges(work.pathsVertex));
                BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, insertedVertices) {
                    if (isFunctionCall(partitioner, vertex) && !findCallReturnEdges(vertex).empty())
                        workList_.push_back(CallSite(vertex, work.callDepth+1));
                }
            } else {
                // FIXME[Robb P. Matzke 2015-05-22]: how to process summaries?
            }
        }
        eraseEdges(paths_, findCallReturnEdges(work.pathsVertex));
    }

    // Remove edges and vertices that cannot be part of any path
    CfgConstEdgeSet pathsUnreachableEdges = findPathUnreachableEdges(paths_, pathsBeginVertices_, pathsEndVertices_);
    CfgConstVertexSet pathsIncidentVertices = findIncidentVertices(pathsUnreachableEdges);
    eraseEdges(paths_, pathsUnreachableEdges);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &pathVertex, pathsIncidentVertices) {
        if (pathVertex->degree() == 0) {
            if (pathsBeginVertices_.find(pathVertex)!=pathsBeginVertices_.end() &&
                pathsEndVertices_.find(pathVertex)!=pathsEndVertices_.end()) {
                // dont erase vertex for a singleton path
            } else {
                vmap_.eraseTarget(pathVertex);
                paths_.eraseVertex(pathVertex);
            }
        }
    }
}

} // namespace
} // namespace
} // namespace
