#include "sage3basic.h"
#include <Partitioner2/ControlFlowGraph.h>
#include <sawyer/GraphTraversal.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

void
insertCfg(ControlFlowGraph &dst, const ControlFlowGraph &src, CfgVertexMap &vmap /*out*/) {
    BOOST_FOREACH (ControlFlowGraph::Vertex vertex, src.vertices())
        vmap.insert(src.findVertex(vertex.id()), dst.insertVertex(vertex.value()));
    BOOST_FOREACH (ControlFlowGraph::Edge edge, src.edges())
        dst.insertEdge(vmap.forward()[edge.source()], vmap.forward()[edge.target()], edge.value());
}

CfgConstEdgeSet
findBackEdges(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &begin) {
    using namespace Sawyer::Container::Algorithm;
    typedef DepthFirstForwardGraphTraversal<const ControlFlowGraph> Traversal;

    CfgConstEdgeSet backEdges;
    std::vector<bool> visitingVertex(cfg.nVertices(), false);
    for (Traversal t(cfg, begin, ENTER_VERTEX|LEAVE_VERTEX|ENTER_EDGE); t; ++t) {
        if (t.event() == ENTER_VERTEX) {
            visitingVertex[t.vertex()->id()] = true;
        } else if (t.event() == LEAVE_VERTEX) {
            visitingVertex[t.vertex()->id()] = false;
        } else if (t.event() == ENTER_EDGE) {
            if (visitingVertex[t.edge()->target()->id()])
                backEdges.insert(t.edge());
        }
    }
    return backEdges;
}

CfgConstEdgeSet
findCallEdges(const ControlFlowGraph::ConstVertexIterator &callSite) {
    CfgConstEdgeSet retval;
    for (ControlFlowGraph::ConstEdgeIterator ei=callSite->outEdges().begin(); ei!=callSite->outEdges().end(); ++ei) {
        if (ei->value().type() == E_FUNCTION_CALL)
            retval.insert(ei);
    }
    return retval;
}

CfgConstVertexSet
findCalledFunctions(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &callSite) {
    ASSERT_require2(cfg.isValidVertex(callSite), "callSite vertex must belong to the CFG");
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::Edge &edge, callSite->outEdges()) {
        if (edge.value().type() == E_FUNCTION_CALL)
            retval.insert(cfg.findVertex(edge.target()->id()));
    }
    return retval;
}

CfgConstEdgeSet
findCallReturnEdges(const ControlFlowGraph::ConstVertexIterator &callSite) {
    CfgConstEdgeSet retval;
    for (ControlFlowGraph::ConstEdgeIterator ei=callSite->outEdges().begin(); ei!=callSite->outEdges().end(); ++ei) {
        if (ei->value().type() == E_CALL_RETURN)
            retval.insert(ei);
    }
    return retval;
}

CfgConstVertexSet
findFunctionReturns(const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &beginVertex) {
    ASSERT_require2(cfg.isValidVertex(beginVertex), "beginVertex must belong to the CFG");
    CfgConstVertexSet endVertices;
    typedef Sawyer::Container::Algorithm::DepthFirstForwardEdgeTraversal<const ControlFlowGraph> Traversal;
    for (Traversal t(cfg, beginVertex); t; ++t) {
        if (t->value().type() == E_FUNCTION_RETURN) {
            endVertices.insert(t->source());
            t.skipChildren();                           // found a function return edge
        } else if (t->value().type() == E_FUNCTION_CALL) { // not E_FUNCTION_XFER
            t.skipChildren();                           // stay in this function
        }
    }
    return endVertices;
}

void
eraseEdges(ControlFlowGraph &graph, const CfgConstEdgeSet &toErase) {
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, toErase) {
        ASSERT_require(graph.isValidEdge(edge));
        graph.eraseEdge(edge);
    }
}

CfgConstVertexSet
findIncidentVertices(const CfgConstEdgeSet &edges) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstEdgeIterator &edge, edges) {
        retval.insert(edge->source());
        retval.insert(edge->target());
    }
    return retval;
}

CfgConstVertexSet
findDetachedVertices(const ControlFlowGraph &graph) {
    CfgConstVertexSet retval;
    for (ControlFlowGraph::ConstVertexIterator vertex=graph.vertices().begin(); vertex!=graph.vertices().end(); ++vertex) {
        if (0 == vertex->degree())
            retval.insert(vertex);
    }
    return retval;
}

CfgConstVertexSet
findDetachedVertices(const CfgConstVertexSet &vertices) {
    CfgConstVertexSet retval;
    BOOST_FOREACH (const ControlFlowGraph::ConstVertexIterator &vertex, vertices) {
        if (0 == vertex->degree())
            retval.insert(vertex);
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
