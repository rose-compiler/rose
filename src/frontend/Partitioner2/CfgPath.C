#include <sage3basic.h>
#include <Diagnostics.h>
#include <Partitioner2/CfgPath.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/GraphTraversal.h>

using namespace rose::Diagnostics;

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

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

void
CfgPath::backtrack() {
    while (!edges_.empty()) {
        ControlFlowGraph::ConstVertexIterator vertex = edges_.back()->source();
        ++edges_.back();
        if (edges_.back() != vertex->outEdges().end())
            return;
        edges_.pop_back();
    }
    clear();
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

void
CfgPath::truncate(const ControlFlowGraph::ConstEdgeIterator &edge) {
    for (Edges::iterator ei=edges_.begin(); ei!=edges_.end(); ++ei) {
        if (*ei == edge) {
            edges_.erase(ei, edges_.end());
            return;
        }
    }
}

size_t
CfgPath::callDepth(const Function::Ptr &function) const {
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
findPathEdges(const ControlFlowGraph &graph, ControlFlowGraph::ConstVertexIterator beginVertex,
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

void
eraseUnreachablePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &beginPathVertex,
                      const CfgConstVertexSet &endPathVertices, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/) {
    if (beginPathVertex == paths.vertices().end()) {
        paths.clear();
        vmap.clear();
        return;
    }
    ASSERT_require(paths.isValidVertex(beginPathVertex));

    // Find edges that are reachable -- i.e., those that are part of a valid path
    CfgConstVertexSet avoidVertices;
    CfgConstEdgeSet avoidEdges;
    std::vector<bool> goodEdges = findPathEdges(paths, beginPathVertex, endPathVertices, avoidVertices, avoidEdges);
    CfgConstEdgeSet badEdges;
    for (size_t i=0; i<goodEdges.size(); ++i) {
        if (!goodEdges[i])
            badEdges.insert(paths.findEdge(i));
    }

    // Erase bad edges from the path and the CFG
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, badEdges) {
        path.truncate(edge);
        paths.eraseEdge(edge);
    }

    // Remove vertices that have no edges, except don't remove the start vertex yet.
    ControlFlowGraph::ConstVertexIterator vertex=paths.vertices().begin();
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

ControlFlowGraph
findPathsNoCalls(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &beginVertex,
                 const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices,
                 const CfgConstEdgeSet &avoidEdges, CfgVertexMap &vmap /*out*/) {
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

void
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstEdgeIterator &pathsCretEdge,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                  const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges) {
    ASSERT_require(paths.isValidEdge(pathsCretEdge));
    ASSERT_require(pathsCretEdge->value().type() == E_CALL_RETURN);

    ControlFlowGraph::ConstVertexIterator pathsCallSite = pathsCretEdge->source();
    ControlFlowGraph::ConstVertexIterator pathsRetTgt = pathsCretEdge->target();
    ASSERT_require2(pathsCallSite->value().type() == V_BASIC_BLOCK, "only basic blocks can call functions");

    // A basic block might call multiple functions if calling through a pointer.
    ASSERT_require(cfg.isValidVertex(cfgCallSite));
    ASSERT_require2(cfgCallSite->value().type() == V_BASIC_BLOCK, "only basic blocks can call functions");
    CfgConstVertexSet callees = findCalledFunctions(cfg, cfgCallSite);
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &callee, callees) {
        if (callee->value().type() == V_INDETERMINATE) {
            // This is a call to some indeterminate location. Just copy another indeterminate vertex into the paths
            // graph. Normally a CFG will have only one indeterminate vertex and it will have no outgoing edges, but the paths
            // graph is different.
            ControlFlowGraph::ConstVertexIterator indet = paths.insertVertex(CfgVertex(V_INDETERMINATE));
            paths.insertEdge(pathsCallSite, indet, CfgEdge(E_FUNCTION_CALL));
            paths.insertEdge(indet, pathsRetTgt, CfgEdge(E_FUNCTION_RETURN));
            mlog[WARN] <<"indeterminate function call from " <<pathsCallSite->value().bblock()->printableName() <<"\n";
        } else {
            // Call to a normal function.
            ASSERT_require2(callee->value().type() == V_BASIC_BLOCK, "non-basic block callees not implemented yet");
            std::string calleeName = callee->value().function() ? callee->value().function()->printableName() :
                                     callee->value().bblock()->printableName();

            // Find all paths through the callee
            CfgVertexMap vmap1;                             // relates CFG to calleePaths
            CfgConstVertexSet returns = findFunctionReturns(cfg, callee);
            ControlFlowGraph calleePaths = findPathsNoCalls(cfg, callee, returns, cfgAvoidVertices, cfgAvoidEdges, vmap1);
            if (calleePaths.isEmpty())
                mlog[WARN] <<calleeName <<" has no paths that return\n";

            // Insert the callee into the paths CFG
            CfgVertexMap vmap2;                             // relates calleePaths to paths
            insertCfg(paths, calleePaths, vmap2);
            CfgVertexMap vmap(vmap1, vmap2);                // composite map from the CFG to paths graph

            // Make an edge from call site to the entry block of the callee in the paths graph
            if (vmap.forward().exists(callee)) {
                ControlFlowGraph::ConstVertexIterator pathStart = vmap.forward()[callee];
                paths.insertEdge(pathsCallSite, pathStart, CfgEdge(E_FUNCTION_CALL));
            }

            // Make edges from the callee's return statements back to the return point in the caller
            BOOST_FOREACH (ControlFlowGraph::ConstVertexIterator ret, returns) {
                if (vmap.forward().exists(ret)) {
                    ControlFlowGraph::ConstVertexIterator pathsRetSrc = vmap.forward()[ret];
                    paths.insertEdge(pathsRetSrc, pathsRetTgt, CfgEdge(E_FUNCTION_RETURN));
                }
            }
        }
    }
}


} // namespace
} // namespace
} // namespace
