#ifndef ROSE_Partitioner2_FunctionCallGraph_H
#define ROSE_Partitioner2_FunctionCallGraph_H

#include <Partitioner2/BasicTypes.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Map.h>

namespace Rose {
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

    class VertexKey {
    public:
        rose_addr_t address;
        VertexKey();
        explicit VertexKey(rose_addr_t);
        explicit VertexKey(const FunctionPtr&);
        bool operator<(VertexKey) const;
    };
        
    /** Function call graph. */
    typedef Sawyer::Container::Graph<FunctionPtr, Edge, VertexKey> Graph;

private:
    Graph graph_;

public:
    /** Underlying function call graph.
     *
     *  This returns the Sawyer::Container::Graph representing inter-function edges. It is read-only since modifying the graph
     *  must be done in conjunction with updating the function-to-vertex index. */
    const Graph& graph() const { return graph_; }

    /** Constructs an empty function call graph. */
    FunctionCallGraph();

    ~FunctionCallGraph();

    /** Return all functions in the call graph. */
    boost::iterator_range<Graph::ConstVertexValueIterator> functions() {
        return graph_.vertexValues();
    }
    
    /** Find function in call graph.
     *
     *  Returns the call graph vertex (as an iterator) for the specified function, or the end vertex iterator if the function
     *  does not exist in the call graph.  The function can be specified by its pointer or entry address.
     *
     * @{ */
    Graph::ConstVertexIterator findFunction(const FunctionPtr &function) const;
    Graph::ConstVertexIterator findFunction(rose_addr_t entryVa) const;
    /** @} */

    /** Determine if a function exists in the call graph.
     *
     *  Returns true if the function is a member of the call graph and false otherwise.  A function can be a member of a call
     *  graph even if it has no incident edges.
     *
     * @{ */
    bool exists(const FunctionPtr &function) const;
    bool exists(rose_addr_t entryVa) const;
    /** @} */

    /** Insert a function vertex.
     *
     *  Inserts the specified function into the call graph if it is not a member of the call graph, otherwise does nothing. In
     *  any case, it returns the vertex for the function. */
    Graph::VertexIterator insertFunction(const FunctionPtr &function);

    /** Insert a call edge.
     *
     *  Inserts an edge representing a call from source (caller) to target (callee). The @p type can be @ref E_FUNCTION_CALL or
     *  @ref E_FUNCTION_XFER.
     *
     *  If @p edgeCount is non-zero and an edge of the correct type already exists between the @p source and @p target, then the
     *  count on that edge is incremented instead. Otherwise, when @p edgeCount is zero, a new edge with unit count is inserted
     *  even if it means creating an edge parallel to an existing edge.
     *
     *  Returns the edge that was inserted or incremented.
     *
     * @{ */
    Graph::EdgeIterator insertCall(const FunctionPtr &source, const FunctionPtr &target,
                                   EdgeType type = E_FUNCTION_CALL, size_t edgeCount = 0);
    Graph::EdgeIterator insertCall(const Graph::VertexIterator &source, const Graph::VertexIterator &target,
                                   EdgeType type = E_FUNCTION_CALL, size_t edgeCount = 0);
    /** @} */
    
    /** List of all functions that call the specified function.
     *
     *  Returns a sorted list of distinct functions that call the specified function.
     *
     * @{ */
    std::vector<FunctionPtr> callers(const FunctionPtr &target) const;
    std::vector<FunctionPtr> callers(const Graph::ConstVertexIterator &target) const;
    /** @} */

    /** Number of functions that call the specified function.
     *
     *  This is the number of distinct functions that call the specified @p target function.
     *
     * @{ */
    size_t nCallers(const FunctionPtr &target) const;
    size_t nCallers(const Graph::ConstVertexIterator &target) const;
    /** @} */

    /** List of all functions called by the specified function.
     *
     *  Returns a sorted list of distinct functions that call the function specified by entry address or call graph vertex.  If
     *  the specified function does not exist in the call graph then an empty list is returned.
     *
     * @{ */
    std::vector<FunctionPtr> callees(const FunctionPtr &source) const;
    std::vector<FunctionPtr> callees(const Graph::ConstVertexIterator &source) const;
    /** @} */
        
    /** Number of functions that the specified function calls.
     *
     *  This is the number of distinct functions called from the specified @p source function.
     *
     * @{ */
    size_t nCallees(const FunctionPtr &source) const;
    size_t nCallees(const Graph::ConstVertexIterator &source) const;
    /** @} */

    /** Total number of calls to a function.
     *
     *  Returns the total number of calls to the specified function, counting each call when a single function calls more than
     *  once.  I.e., it is the sum of the @c count fields of all the incoming edges for @p target.
     *
     * @{ */
    size_t nCallsIn(const FunctionPtr &target) const;
    size_t nCallsIn(const Graph::ConstVertexIterator &target) const;
    /** @} */

    /** Total number of calls from a function.
     *
     *  Returns the total number of calls from the specified function, counting each call when a single function is called more
     *  than once.  I.e., it is the sum of the @c count fields of all the outgoing edges for @p source.
     *
     * @{ */
    size_t nCallsOut(const FunctionPtr &source) const;
    size_t nCallsOut(const Graph::ConstVertexIterator &source) const;
    /** @} */

    /** Number of calls between two specific functions.
     *
     *  This is the sum of the @c count fields for all edges between @p source and @p target.
     *
     * @{ */
    size_t nCalls(const FunctionPtr &source, const FunctionPtr &target) const;
    size_t nCalls(const Graph::ConstVertexIterator &source, const Graph::ConstVertexIterator &target) const;
    /** @} */
};
        
} // namespace
} // namespace
} // namespace
#endif
