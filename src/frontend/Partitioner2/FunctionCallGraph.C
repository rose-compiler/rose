#include "sage3basic.h"

#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/Utility.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VertexKey
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionCallGraph::VertexKey::VertexKey()
    : address(0) {}

FunctionCallGraph::VertexKey::VertexKey(rose_addr_t va)
    : address(va) {}

FunctionCallGraph::VertexKey::VertexKey(const Function::Ptr &function)
    : address(function ? function->address() : 0) {}

bool
FunctionCallGraph::VertexKey::operator<(VertexKey other) const {
    return address < other.address;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FunctionCallGraph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionCallGraph::FunctionCallGraph() {}
FunctionCallGraph::~FunctionCallGraph() {}

FunctionCallGraph::Graph::ConstVertexIterator
FunctionCallGraph::findFunction(const FunctionPtr &function) const {
    return function ? findFunction(function->address()) : graph_.vertices().end();
}

FunctionCallGraph::Graph::ConstVertexIterator
FunctionCallGraph::findFunction(rose_addr_t entryVa) const {
    return graph_.findVertexKey(VertexKey(entryVa));
}

bool
FunctionCallGraph::exists(const Function::Ptr &function) const {
    return findFunction(function) != graph_.vertices().end();
}

bool
FunctionCallGraph::exists(rose_addr_t entryVa) const {
    return findFunction(entryVa) != graph_.vertices().end();
}

FunctionCallGraph::Graph::VertexIterator
FunctionCallGraph::insertFunction(const Function::Ptr &function) {
    return function ? graph_.insertVertexMaybe(function) : graph_.vertices().end();
}

FunctionCallGraph::Graph::EdgeIterator
FunctionCallGraph::insertCall(const Function::Ptr &source, const Function::Ptr &target, EdgeType type, size_t edgeCount) {
    Graph::VertexIterator srcVertex = insertFunction(source);
    Graph::VertexIterator dstVertex = insertFunction(target);
    return insertCall(srcVertex, dstVertex, type, edgeCount);
}

FunctionCallGraph::Graph::EdgeIterator
FunctionCallGraph::insertCall(const Graph::VertexIterator &source, const Graph::VertexIterator &target,
                          EdgeType type, size_t edgeCount) {
    ASSERT_forbid(source == graph_.vertices().end());
    ASSERT_forbid(target == graph_.vertices().end());
    if (edgeCount) {
        for (Graph::EdgeIterator edge=source->outEdges().begin(); edge!=source->outEdges().end(); ++edge) {
            if (edge->target()==target && edge->value().type()==type) {
                edge->value().count_ += edgeCount;
                return edge;
            }
        }
    }
    return graph_.insertEdge(source, target, Edge(type));
}

std::vector<Function::Ptr>
FunctionCallGraph::callers(const Function::Ptr &target) const {
    return callers(findFunction(target));
}

std::vector<Function::Ptr>
FunctionCallGraph::callers(const Graph::ConstVertexIterator &target) const {
    std::vector<Function::Ptr> callers;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::Edge &edge, target->inEdges())
            insertUnique(callers, edge.source()->value(), sortFunctionsByAddress);
    }
    return callers;
}

std::vector<Function::Ptr>
FunctionCallGraph::callees(const Function::Ptr &source) const {
    return callees(findFunction(source));
}

std::vector<Function::Ptr>
FunctionCallGraph::callees(const Graph::ConstVertexIterator &source) const {
    std::vector<Function::Ptr> callees;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::Edge &edge, source->outEdges())
            insertUnique(callees, edge.target()->value(), sortFunctionsByAddress);
    }
    return callees;
}

size_t
FunctionCallGraph::nCallers(const Function::Ptr &target) const {
    return nCallers(findFunction(target));
}

size_t
FunctionCallGraph::nCallers(const Graph::ConstVertexIterator &target) const {
    return callers(target).size();
}

size_t
FunctionCallGraph::nCallees(const Function::Ptr &source) const {
    return nCallees(findFunction(source));
}

size_t
FunctionCallGraph::nCallees(const Graph::ConstVertexIterator &source) const {
    return callees(source).size();
}

size_t
FunctionCallGraph::nCallsIn(const Function::Ptr &target) const {
    return nCallsIn(findFunction(target));
}

size_t
FunctionCallGraph::nCallsIn(const Graph::ConstVertexIterator &target) const {
    size_t n = 0;
    if (target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::Edge &edge, target->inEdges())
            n += edge.value().count();
    }
    return n;
}

size_t
FunctionCallGraph::nCallsOut(const Function::Ptr &source) const {
    return nCallsOut(findFunction(source));
}

size_t
FunctionCallGraph::nCallsOut(const Graph::ConstVertexIterator &source) const {
    size_t n = 0;
    if (source != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::Edge &edge, source->outEdges())
            n += edge.value().count();
    }
    return n;
}

size_t
FunctionCallGraph::nCalls(const Function::Ptr &source, const Function::Ptr &target) const {
    return nCalls(findFunction(source), findFunction(target));
}

size_t
FunctionCallGraph::nCalls(const Graph::ConstVertexIterator &source, const Graph::ConstVertexIterator &target) const {
    size_t n = 0;
    if (source != graph_.vertices().end() && target != graph_.vertices().end()) {
        BOOST_FOREACH (const Graph::Edge &edge, source->outEdges()) {
            if (edge.target() == target)
                n += edge.value().count();
        }
    }
    return n;
}

} // namespace
} // namespace
} // namespace
