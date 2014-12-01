#ifndef ROSE_Partitioner2_FunctionCallGraph_H
#define ROSE_Partitioner2_FunctionCallGraph_H

#include <Partitioner2/Function.h>
#include <sawyer/Graph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Function call information.
 *
 *  This class provides methods that operate on a function call graph, such as constructing a function call graph from a
 *  control flow graph. The graph vertices are function pointers (Function::Ptr) and the edges contain information about the
 *  type of inter-function edge (function call, function transfer, etc) and the number of such edges.  Function call graphs can
 *  be built so that inter-function control transfer is represented by its own edge, or so that multiple transfers share an
 *  edge. */
class FunctionCallGraph {
public:
    /** Information about each edge in the call graph. */
    class Edge {
        friend class FunctionCallGraph;
        EdgeType type_;
        size_t count_;
    public:
        explicit Edge(EdgeType type): type_(type), count_(1) {}

        /** Type of edge. Edge types @ref E_FUNCTION_CALL and @ref E_FUNCTION_XFER are supported. */
        EdgeType type() const { return type_; }

        /** Number of inter-function control flow edges represented by this edge. */
        size_t count() const { return count_; }
    };
    
    /** Function call graph. */
    typedef Sawyer::Container::Graph<Function::Ptr, Edge> Graph;

    /** Maps function address to function call graph vertex. */
    typedef Sawyer::Container::Map<rose_addr_t, Graph::VertexNodeIterator> Index;

private:
    Graph graph_;
    Index index_;

public:
    /** Underlying function call graph.
     *
     *  This returns the Sawyer::Container::Graph representing inter-function edges. It is read-only since modifying the graph
     *  must be done in conjunction with updating the function-to-vertex index. */
    const Graph& graph() const { return graph_; }

    /** Function-to-vertex index.
     *
     *  Returns the index mapping function addresses to function call graph vertices. The index is read-only since updating the
     *  index must be done in conjunction with updating the graph. */
    const Index& index() const { return index_; }

    /** Constructs an empty function call graph. */
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
    Graph::ConstVertexNodeIterator findFunction(rose_addr_t entryVa) const {
        Index::ConstValueIterator found = index_.find(entryVa);
        return found==index_.values().end() ? graph_.vertices().end() : Graph::ConstVertexNodeIterator(*found);
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
    Graph::VertexNodeIterator insertFunction(const Function::Ptr &function);

    /** Insert a call edge.
     *
     *  Inserts an edge representing a call from source (caller) to target (callee). The @p type can be @ref E_FUNCTION_CALL or
     *  @ref E_FUNCTION_XFER. If @p allowParallelEdges is false and such an edge is already present in the graph, then the
     *  existing edge's counter is incremented rather than adding a new parallel edge.
     *
     *  Returns the edge that was inserted or incremented.
     *
     * @{ */
    Graph::EdgeNodeIterator insertCall(const Function::Ptr &source, const Function::Ptr &target,
                                       EdgeType type = E_FUNCTION_CALL, bool allowParallelEdges = true) {
        return insertCall(insertFunction(source), insertFunction(target), type, allowParallelEdges);
    }
    Graph::EdgeNodeIterator insertCall(const Graph::VertexNodeIterator &source, const Graph::VertexNodeIterator &target,
                                       EdgeType type = E_FUNCTION_CALL, bool allowParallelEdges = true);
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
     *  This is the number of distinct functions that call the specified @p target function.
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
     *  This is the number of distinct functions called from the specified @p source function.
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
     *  once.  I.e., it is the sum of the @c count fields of all the incoming edges for @p target.
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
     *  than once.  I.e., it is the sum of the @c count fields of all the outgoing edges for @p source.
     *
     * @{ */
    size_t nCallsOut(const Function::Ptr &source) const {
        return nCallsOut(findFunction(source));
    }
    size_t nCallsOut(const Graph::ConstVertexNodeIterator &source) const;
    /** @} */

    /** Number of calls between two specific functions.
     *
     *  This is the sum of the @c count fields for all edges between @p source and @p target.
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
