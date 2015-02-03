#include "sage3basic.h"

#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

FunctionCallGraph::Graph::VertexNodeIterator
FunctionCallGraph::insertFunction(const Function::Ptr &function) {
    Graph::VertexNodeIterator vertex = graph_.vertices().end();
    if (function && !index_.getOptional(function->address()).assignTo(vertex))
        index_.insert(function->address(), vertex=graph_.insertVertex(function));
    return vertex;
}

FunctionCallGraph::Graph::EdgeNodeIterator
FunctionCallGraph::insertCall(const Graph::VertexNodeIterator &source, const Graph::VertexNodeIterator &target,
                          EdgeType type, bool allowParallelEdges) {
    ASSERT_forbid(source == graph_.vertices().end());
    ASSERT_forbid(target == graph_.vertices().end());
    if (!allowParallelEdges) {
        for (Graph::EdgeNodeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
            if (edge->target()==target && edge->value().type()==type) {
                ++edge->value().count_;
                return edge;
            }
        }
    }
    return graph_.insertEdge(source, target, Edge(type));
}

std::vector<Function::Ptr>
FunctionCallGraph::callers(const Graph::ConstVertexNodeIterator &target) const {
    std::vector<Function::Ptr> callers;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, target->inEdges())
            insertUnique(callers, edge.source()->value(), sortFunctionsByAddress);
    }
    return callers;
}

std::vector<Function::Ptr>
FunctionCallGraph::callees(const Graph::ConstVertexNodeIterator &source) const {
    std::vector<Function::Ptr> callees;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges())
            insertUnique(callees, edge.target()->value(), sortFunctionsByAddress);
    }
    return callees;
}

size_t
FunctionCallGraph::nCallers(const Graph::ConstVertexNodeIterator &target) const {
    return callers(target).size();
}

size_t
FunctionCallGraph::nCallees(const Graph::ConstVertexNodeIterator &source) const {
    return callees(source).size();
}

size_t
FunctionCallGraph::nCallsIn(const Graph::ConstVertexNodeIterator &target) const {
    size_t n = 0;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, target->inEdges())
            n += edge.value().count();
    }
    return n;
}

size_t
FunctionCallGraph::nCallsOut(const Graph::ConstVertexNodeIterator &source) const {
    size_t n = 0;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges())
            n += edge.value().count();
    }
    return n;
}

size_t
FunctionCallGraph::nCalls(const Graph::ConstVertexNodeIterator &source, const Graph::ConstVertexNodeIterator &target) const {
    size_t n = 0;
    if (source != graph_.vertices().end() && target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges()) {
            if (edge.target() == target)
                n += edge.value().count();
        }
    }
    return n;
}

} // namespace
} // namespace
} // namespace
