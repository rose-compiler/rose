#ifndef ROSE_BinaryAnalysis_FunctionCall_H
#define ROSE_BinaryAnalysis_FunctionCall_H

#include "BinaryControlFlow.h"

class SgAsmFunctionDeclaration;

namespace BinaryAnalysis {

    /** Binary function call analysis.
     *
     *  This class serves mostly to organize the functions that operate on function calls, but also provides a container for
     *  various settings that influence the function call analyses, such as vertex and edge filters.
     *
     *  Function call graphs can be computed over any subtree of the AST, although one usually does so over an entire binary
     *  interpretation (SgAsmInterpretation).  The vertex and edge filters can restrict which functions and call edges are
     *  considered by the various methods of this class. */
    class FunctionCall {
    public:

        FunctionCall()
            : vertex_filter(NULL), edge_filter(NULL)
            {}

        /** A function call graph.
         *
         *  A function call graph is simply a Boost graph whose vertex descriptors are integers and whose vertices point to
         *  SgAsmFunctionDeclaration nodes in the AST (via the boost::vertex_name property).  The graph edges represent
         *  function calls from one SgAsmFunctionDeclaration to another.  Since this graph is a Boost graph, it is endowed with
         *  all the features of a Boost graph and can be the operand of the various Boost graph algorithms.  See build_cg() for
         *  specifics about what is included in such a graph.
         *
         *  Another way to represent function calls is to adapt a global control flow graph
         *  (BinaryAnalysis::ControlFlowGraph) to include only the edges (and their incident vertices) that flow from one
         *  function to another.  The advantage of using a control flow graph to represent function call information is that
         *  each call site will be included in the function call graph due to the fact that the control flow graph vertices are
         *  blocks (SgAsmBlock) rather than functions (SgAsmFunctionDeclaration). */
        typedef boost::adjacency_list<boost::listS,                                 /* out-edges of each vertex in std::list */
                                      boost::vecS,                                  /* store vertices in std::vector */
                                      boost::directedS,                             /* call graph is directed */
                                      boost::property<boost::vertex_name_t, SgAsmFunctionDeclaration*>
                                     > Graph;

        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
        typedef boost::graph_traits<Graph>::edge_descriptor   Edge;     /**< Graph edge type. */
        


        /**********************************************************************************************************************
         *                                      Filters
         **********************************************************************************************************************/
    public:

        /** Filter for vertices.
         *
         *  This class can be specialized in order to filter out functions (SgAsmFunctionDeclaration) that satisfy an arbitrary
         *  condition.  See set_vertex_filter() for details. */
        class VertexFilter {
        public:
            virtual ~VertexFilter() {}
            virtual bool operator()(FunctionCall*, SgAsmFunctionDeclaration*) = 0;
        };

        /** Filter for edges.
         *
         *  This class can be specialized in order to filter out certain edges that would otherwise make it into the function
         *  call graph.  See set_edge_filter() for details. */
        class EdgeFilter {
        public:
            virtual ~EdgeFilter() {}
            virtual bool operator()(FunctionCall*, SgAsmFunctionDeclaration *source, SgAsmFunctionDeclaration *target) = 0;
        };

        /** Manipulate the vertex filter.
         *
         *  When building a function call graph, the vertex filter is invoked on each function which is about to be added as
         *  a vertex.  If the filter returns false then that function is not added to the graph.  A null filter accepts all
         *  vertices.
         *
         *  @{ */
        void set_vertex_filter(VertexFilter *filter) { vertex_filter = filter; }
        VertexFilter *get_vertex_filter() const { return vertex_filter; }
        /** @}*/

        /** Manipulate the edge filter.
         *
         *  When building a function call graph, the edge filter is invoked for each edge which is about to be added to the
         *  graph. If the filter returns false then that edge is not added to the graph.  A null filter accepts all edges.
         *
         *  @{ */
        void set_edge_filter(EdgeFilter *filter) { edge_filter = filter; }
        EdgeFilter *get_edge_filter() const { return edge_filter; }
        /** @}*/

        /** Determines if a vertex is filtered out.
         *
         *  Returns true if the vertex would be filtered out by being rejected by the current vertex filter.
         *
         *  @{ */
        bool is_vertex_filtered(SgAsmFunctionDeclaration *func, VertexFilter *filter) {
            return filter && !(*filter)(this, func);
        }
        bool is_vertex_filtered(SgAsmFunctionDeclaration *func) {
            return is_vertex_filtered(func, vertex_filter);
        }
        /** @} */

        /** Determines if an edge is filtered out.
         *
         *  Returns true if the edge would be filtered out by being rejected by the current edge filter.
         *
         *  @{ */
        bool is_edge_filtered(SgAsmFunctionDeclaration *src, SgAsmFunctionDeclaration *dst, EdgeFilter *filter) {
            return filter && !(*filter)(this, src, dst);
        }
        bool is_edge_filtered(SgAsmFunctionDeclaration *src, SgAsmFunctionDeclaration *dst) {
            return is_edge_filtered(src, dst, edge_filter);
        }
        /** @} */

    protected:
        VertexFilter *vertex_filter;
        EdgeFilter *edge_filter;

        /**********************************************************************************************************************
         *                                      Methods that modify the AST
         **********************************************************************************************************************/
    public:

        /** Cache vertex descriptors in AST.
         *
         *  The vertices of a function call graph are of type Vertex, and point at the functions (SgAsmFunctionDeclaration) of
         *  the AST. Although most graph algorithms will only need to map Vertex to SgAsmFunctionDeclaration, the inverse
         *  mapping is also sometimes useful.  That mapping can be stored into an std::map via graph traversal, or stored in
         *  the AST itself attached to each SgAsmFunctionDeclaration.  Using an std::map requires an O(log N) lookup each time
         *  we need to get the vertex descriptor for a function, while storing the vertex descriptor in the AST requires O(1)
         *  lookup time.
         *
         *  The vertex descriptors are available via SgAsmFunctionDeclaration::get_cached_vertex().  Other graph types (e.g.,
         *  dominance graphs) might also use the same cache line.  The cached vertex is stored as a size_t, which is the same
         *  underlying type for function call graph vertices.
         *
         *  The current vertex filter determines which function nodes are modified. */
        void cache_vertex_descriptors(const Graph&);

        /**********************************************************************************************************************
         *                                      Graph construction methods
         **********************************************************************************************************************/
    public:

        /** Build a function call graph from a control flow graph.
         *
         *  Given a control flow graph (CFG) spanning multiple functions, create a function call graph (CG) by collapsing
         *  vertices in the CFG that belong to a common function.  Any resulting self-loop edges will be removed unless the
         *  target of the corresponding edge in the CFG was the function entry block (i.e., intra-function CFG edges whose
         *  target is the function's entry block are assumed to be recursive calls, while all other intra-function CFG edges
         *  are omitted from the CG).
         *
         *  The current vertex and edge filters are used to restrict which functions and calls make it into the graph.
         *
         *  @{ */
        Graph build_graph(const ControlFlow::Graph&);
        void build_graph(const ControlFlow::Graph &cfg, Graph &cg/*out*/);
        /** @} */

        /** Build a function call graph from an AST.
         *
         *  Given an AST, traverse the AST beginning at @p root and build a function call graph (CG).  The function call graph
         *  will contain only SgAsmFunctionDeclaration vertices that are in the specified subtree and which are not filtered
         *  out by the current vertex filter.  Edges also must pass the edge filter to be included in the graph.
         *
         *  The following two methods of constructing a CG should result in identical graphs (although vertex and edge order
         *  may be different):
         *
         *  @code
         *  using namespace BinaryAnalysis;
         *  SgAsmNode *node = ...;
         *  FunctionCall::Graph cg1 = FunctionCall().build_graph(node);                            // method 1
         *  FunctionCall::Graph cg2 = FunctionCall().build_graph(FlowControl().build_graph(node)); // method 2
         *  @endcode
         *
         *  In general, building the function call graph directly from the AST will be faster than first building the control flow
         *  graph.
         *
         *  @{ */
        Graph build_graph(SgNode *root);
        void build_graph(SgNode *root, Graph &cg/*out*/);
        /** @} */

        /** Copies a graph while filtering.
         *
         *  Copies a graph while applying the current source and destination vertex and edge filters.  If all vertices are
         *  selected by the vertex filter, then the desintation graph's vertex descriptors will correspond to the same
         *  vertices in the source graph (i.e., vertex V in the source will be the same basic block as vertex V in the
         *  destination).
         *
         *  If an edge is unfiltered but one of its vertices is filtered, then the edge will not be included in the result.
         *
         *  @{ */
        Graph copy(const Graph&src);
        void copy(const Graph &src, Graph &dst);
        /** @} */

    };
}

#endif
