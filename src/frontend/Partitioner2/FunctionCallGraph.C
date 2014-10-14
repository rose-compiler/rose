#include "sage3basic.h"

#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/Utility.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

FunctionCallGraph::Graph::VertexNodeIterator
FunctionCallGraph::insert(const Function::Ptr &function) {
    Graph::VertexNodeIterator vertex = graph_.vertices().end();
    if (function && !index_.getOptional(function->address()).assignTo(vertex))
        index_.insert(function->address(), vertex=graph_.insertVertex(function));
    return vertex;
}

FunctionCallGraph::Graph::EdgeNodeIterator
FunctionCallGraph::insert(const Graph::VertexNodeIterator &source, const Graph::VertexNodeIterator &target) {
    ASSERT_forbid(source == graph_.vertices().end());
    ASSERT_forbid(target == graph_.vertices().end());
    for (Graph::EdgeNodeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
        if (edge->target()==target) {
            ++edge->value();
            return edge;
        }
    }
    return graph_.insertEdge(source, target, 1);
}

std::vector<Function::Ptr>
FunctionCallGraph::callers(const Graph::ConstVertexNodeIterator &target) const {
    std::vector<Function::Ptr> callers;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, target->inEdges())
            callers.push_back(edge.source()->value());
        std::sort(callers.begin(), callers.end(), sortFunctionsByAddress);
    }
    return callers;
}

std::vector<Function::Ptr>
FunctionCallGraph::callees(const Graph::ConstVertexNodeIterator &source) const {
    std::vector<Function::Ptr> callees;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges())
            callees.push_back(edge.target()->value());
        std::sort(callees.begin(), callees.end(), sortFunctionsByAddress);
    }
    return callees;
}

size_t
FunctionCallGraph::nCallers(const Graph::ConstVertexNodeIterator &target) const {
    return target == graph_.vertices().end() ? 0 : target->nInEdges();
}

size_t
FunctionCallGraph::nCallees(const Graph::ConstVertexNodeIterator &source) const {
    return source == graph_.vertices().end() ? 0 : source->nOutEdges();
}

size_t
FunctionCallGraph::nCallsIn(const Graph::ConstVertexNodeIterator &target) const {
    size_t n = 0;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, target->inEdges())
            n += edge.value();
    }
    return n;
}

size_t
FunctionCallGraph::nCallsOut(const Graph::ConstVertexNodeIterator &source) const {
    size_t n = 0;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges())
            n += edge.value();
    }
    return n;
}

size_t
FunctionCallGraph::nCalls(const Graph::ConstVertexNodeIterator &source, const Graph::ConstVertexNodeIterator &target) const {
    if (source != graph_.vertices().end() && target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::EdgeNode &edge, source->outEdges()) {
            if (edge.target() == target)
                return edge.value();
        }
    }
    return 0;
}

} // namespace
} // namespace
} // namespace
