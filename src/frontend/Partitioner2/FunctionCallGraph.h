#ifndef ROSE_Partitioner2_FunctionCallGraph_H
#define ROSE_Partitioner2_FunctionCallGraph_H

#include <Partitioner2/Function.h>
#include <sawyer/Graph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

class FunctionCallGraph {
public:
    typedef Sawyer::Container::Graph<Function::Ptr, size_t> Graph;
    typedef Sawyer::Container::Map<rose_addr_t, Graph::VertexNodeIterator> Index;

private:
    Graph graph_;
    Index index_;

public:
    const Graph& graph() const { return graph_; }
    const Index& index() const { return index_; }

    /** Constructs an empty graph. */
    FunctionCallGraph() {}

    /** Copy constructor. */
    FunctionCallGraph(const FunctionCallGraph &other): graph_(other.graph_) {
        for (Graph::VertexNodeIterator iter=graph_.vertices().begin(); iter!=graph_.vertices().end(); ++iter)
            index_.insert(iter->value()->address(), iter);
    }
    
    /** Find function in call graph.
     *
     *  Returns the call graph vertex (as an iterator) for the specified function, or the end vertex iterator if the function
     *  does not exist in the call graph.  The function can be specified by its pointer or entry address.
     *
     * @{ */
    Graph::ConstVertexNodeIterator findFunction(const Function::Ptr &function) const {
        return function ? findFunction(function->address()) : graph_.vertices().end();
    }
    Graph::VertexNodeIterator findFunction(const Function::Ptr &function) {
        return function ? findFunction(function->address()) : graph_.vertices().end();
    }
    Graph::ConstVertexNodeIterator findFunction(rose_addr_t entryVa) const {
        Index::ConstValueIterator found = index_.find(entryVa);
        return found==index_.values().end() ? graph_.vertices().end() : Graph::ConstVertexNodeIterator(*found);
    }
    Graph::VertexNodeIterator findFunction(rose_addr_t entryVa) {
        return index_.getOptional(entryVa).orElse(graph_.vertices().end());
    }
    /** @} */

    /** Determine if a function exists in the call graph.
     *
     *  Returns true if the function is a member of the call graph and false otherwise.  A function can be a member of a call
     *  graph even if it has no incident edges.
     *
     * @{ */
    bool exists(const Function::Ptr &function) const {
        return findFunction(function) != graph_.vertices().end();
    }
    bool exists(rose_addr_t entryVa) const {
        return findFunction(entryVa) != graph_.vertices().end();
    }
    /** @} */

    /** Insert a function vertex.
     *
     *  Inserts the specified function into the call graph if it is not a member of the call graph, otherwise does nothing. In
     *  any case, it returns the vertex for the function. */
    Graph::VertexNodeIterator insert(const Function::Ptr &function);

    /** Insert a call edge.
     *
     *  Inserts an edge representing a call from source (caller) to target (callee).  If such an edge is already present in the
     *  call graph, then the existing edge's counter is incremented; the graph doesn't actually store parallel edges.
     *
     *  Returns the edge that was inserted or incremented.
     *
     * @{ */
    Graph::EdgeNodeIterator insert(const Function::Ptr &source, const Function::Ptr &target) {
        return insert(insert(source), insert(target));
    }
    Graph::EdgeNodeIterator insert(const Graph::VertexNodeIterator &source, const Graph::VertexNodeIterator &target);
    /** @} */
    
    /** List of all functions that call the specified function.
     *
     *  Returns a sorted list of distinct functions that call the specified function.
     *
     * @{ */
    std::vector<Function::Ptr> callers(const Function::Ptr &target) const {
        return callers(findFunction(target));
    }
    std::vector<Function::Ptr> callers(const Graph::ConstVertexNodeIterator &target) const;
    /** @} */

    /** Number of functions that call the specified function.
     *
     *  This returns the same value as <code>cg.callers(f).size()</code> but is slightly faster.
     *
     * @{ */
    size_t nCallers(const Function::Ptr &target) const {
        return nCallers(findFunction(target));
    }
    size_t nCallers(const Graph::ConstVertexNodeIterator &target) const;
    /** @} */

    /** List of all functions called by the specified function.
     *
     *  Returns a sorted list of distinct functions that call the function specified by entry address or call graph vertex.  If
     *  the specified function does not exist in the call graph then an empty list is returned.
     *
     * @{ */
    std::vector<Function::Ptr> callees(const Function::Ptr &source) const {
        return callees(findFunction(source));
    }
    std::vector<Function::Ptr> callees(const Graph::ConstVertexNodeIterator &source) const;
    /** @} */
        
    /** Number of functions that the specified function calls.
     *
     *  This returns the same value as <code>cg.callees(f).size()</code> but is slightly faster.
     *
     * @{ */
    size_t nCallees(const Function::Ptr &source) const {
        return nCallees(findFunction(source));
    }
    size_t nCallees(const Graph::ConstVertexNodeIterator &source) const;
    /** @} */

    /** Total number of calls to a function.
     *
     *  Returns the total number of calls to the specified function, counting each call when a single function calls more than
     *  once.
     *
     * @{ */
    size_t nCallsIn(const Function::Ptr &target) const {
        return nCallsIn(findFunction(target));
    }
    size_t nCallsIn(const Graph::ConstVertexNodeIterator &target) const;
    /** @} */

    /** Total number of calls from a function.
     *
     *  Returns the total number of calls from the specified function, counting each call when a single function is called more
     *  than once.
     *
     * @{ */
    size_t nCallsOut(const Function::Ptr &source) const {
        return nCallsOut(findFunction(source));
    }
    size_t nCallsOut(const Graph::ConstVertexNodeIterator &source) const;
    /** @} */

    /** Number of calls between two specific functions.
     *
     * @{ */
    size_t nCalls(const Function::Ptr &source, const Function::Ptr &target) const {
        return nCalls(findFunction(source), findFunction(target));
    }
    size_t nCalls(const Graph::ConstVertexNodeIterator &source, const Graph::ConstVertexNodeIterator &target) const;
    /** @} */
};
        
} // namespace
} // namespace
} // namespace
#endif
