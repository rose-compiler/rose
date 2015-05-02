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
findPathEdges(const ControlFlowGraph &graph, const ControlFlowGraph::ConstVertexIterator &beginVertex,
              const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices,
              const CfgConstEdgeSet &avoidEdges) {
    using namespace Sawyer::Container::Algorithm;

    // Mark edges that are reachable with a forward traversal from the starting vertex, avoiding certain vertices and edges.
    std::vector<bool> forwardReachable(graph.nEdges(), false);
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> ForwardTraversal;
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
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &endVertex, endVertices) {
        typedef DepthFirstReverseGraphTraversal<const ControlFlowGraph> ReverseTraversal;
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

CfgConstEdgeSet
findPathReachableEdges(const ControlFlowGraph &graph,
                       const ControlFlowGraph::ConstVertexIterator &beginVertex, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertex, endVertices, avoidVertices, avoidEdges);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}
    
CfgConstEdgeSet
findPathUnreachableEdges(const ControlFlowGraph &graph,
                         const ControlFlowGraph::ConstVertexIterator &beginVertex, const CfgConstVertexSet &endVertices,
                         const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges) {
    CfgConstEdgeSet retval;
    std::vector<bool> goodEdges = findPathEdges(graph, beginVertex, endVertices, avoidVertices, avoidEdges);
    ASSERT_require(goodEdges.size() == graph.nEdges());
    for (size_t i=0; i<graph.nEdges(); ++i) {
        if (!goodEdges[i])
            retval.insert(graph.findEdge(i));
    }
    return retval;
}

size_t
eraseUnreachablePaths(ControlFlowGraph &graph /*in,out*/, const ControlFlowGraph::ConstVertexIterator &beginVertex,
                      const CfgConstVertexSet &endVertices, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/) {
    size_t origPathSize = path.nEdges();
    if (beginVertex == graph.vertices().end()) {
        graph.clear();
        vmap.clear();
        path.clear();
        return origPathSize;
    }
    ASSERT_require(graph.isValidVertex(beginVertex));

    // Erase unreachable edges from the graph and path
    CfgConstVertexSet avoidVertices;
    CfgConstEdgeSet avoidEdges;
    CfgConstEdgeSet badEdges = findPathUnreachableEdges(graph, beginVertex, endVertices, avoidVertices, avoidEdges);
    CfgConstVertexSet incidentVertices = findIncidentVertices(badEdges);
    path.truncate(badEdges);
    eraseEdges(graph, badEdges);

    // This might leave some vertices having no incident edges, so remove them since they can't participate on any path. Don't
    // remove the beginVertex if it can be a singleton path. Avoid scanning the entire graph by considering only vertices that
    // are incident to the edges we just removed.
    if (endVertices.find(beginVertex) != endVertices.end())
        incidentVertices.erase(beginVertex);                 // beginVertex is a valid singleton path
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, incidentVertices) {
        if (vertex->degree() == 0) {
            vmap.eraseTarget(vertex);
            graph.eraseVertex(vertex);
        }
    }

    // If we removed the begin vertex (and thus all vertices) from the graph, then the path should also be emptied.
    if (graph.isEmpty())
        path.clear();

    return origPathSize - path.nEdges();
}

ControlFlowGraph
findPathsNoCalls(const ControlFlowGraph &cfg, CfgVertexMap &vmap /*out*/,
                 const ControlFlowGraph::ConstVertexIterator &beginVertex,
                 const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices,
                 const CfgConstEdgeSet &avoidEdges) {
    ASSERT_require(cfg.isValidVertex(beginVertex));
    vmap.clear();
    ControlFlowGraph paths;
    std::vector<bool> goodEdges = findPathEdges(cfg, beginVertex, endVertices, avoidVertices, avoidEdges);
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, cfg.edges()) {
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
    CfgConstVertexSet cfgReturns = findFunctionReturns(cfg, cfgCallTarget);
    ControlFlowGraph calleePaths = findPathsNoCalls(cfg, vmap1, cfgCallTarget, cfgReturns, cfgAvoidVertices, cfgAvoidEdges);
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

} // namespace
} // namespace
} // namespace
