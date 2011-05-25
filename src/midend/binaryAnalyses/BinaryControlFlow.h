#ifndef ROSE_BinaryAnalysis_ControlFlow_H
#define ROSE_BinaryAnalysis_ControlFlow_H

#include <boost/graph/adjacency_list.hpp>

class SgNode;
class SgAsmBlock;

/** Namespace for lots of binary analysis stuff. */
namespace BinaryAnalysis {

    /** Binary control flow analysis.
     *
     *  This class serves mostly to organize the functions that operate on control flow, but also provides a container for
     *  various settings that influence the control flow analyses, such as the vertex and edge filters.
     *
     *  Control flow is represented in the AST as successor pointers (SgAsmTarget) attached to each basic block
     *  (SgAsmBlock). The successor information is initialized by the Partitioner class when the AST is built (see
     *  Partitioner::build_ast()) and are available with SgAsmBlock::get_successors().  The
     *  SgAsmBlock::get_successors_complete() returns a Boolean indicating whether the set of successors is completely
     *  known. Successors would be incomplete, for instance, for a block that returns from a function or a block that ends with
     *  a computed branch whose targets could not be statically determined.
     *
     *  The wikipedia entry for "Control Flow Graph" [1] has many useful definitions.
     *
     *  Control flow graphs can be calculated over any subtree of the AST.  Usually one creates CFGs that span a single
     *  function or a whole interpretation.  The vertex and edge filtes can restrict which basic blocks and control flow edges
     *  are considered by the various methods of this class.  For instance, to create a global CFG that has only those edges
     *  that look like function calls, one could do the following (see also, BinaryAnalysis::FunctionCall):
     *
     *  @code
     *  // Create a filter that rejects all edges except those whose target
     *  // is a function entry block.
     *  struct OnlyCallEdges: public EdgeFilter {
     *      bool operator()(ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
     *          SgAsmFunctionDeclaration *func = dst->get_enclosing_function();
     *          return func && dst == func->get_entry_block();
     *      }
     *  } edge_filter;
     *
     *  // Create the control flow analyzer and set its edge filter.
     *  ControlFlow analyzer;
     *  analyzer.set_edge_filter(&edge_filter);
     *
     *  // Generate a control flow graph over an entire interpretation. It will include
     *  // all basic blocks, but only edges which are function calls.
     *  SgAsmInterpretation *interp = ...;
     *  ControlFlow::Graph cfg = analyzer.build_graph(interp);
     *  @endcode
     *
     *  Another way to do the same thing is to first build a complete control flow graph and then do a filtered-copy to obtain
     *  a second graph with only the function call edges:
     *
     *  @code
     *  // Build a complete control flow graph using a default
     *  // control flow analyzer.
     *  ControlFlow::Graph cfg = ControlFlow().build_graph(interp);
     *
     *  // Using the same analyzer as the previous example, one that
     *  // filters out all but the function call edges, create a call
     *  // graph.
     *  ControlFlow::Graph cg = analyzer.copy(cfg);
     *  @endcode
     *
     *  The BinaryAnalysis::FunctionCall::Graph differs from a filtered ControlFlow::Graph in that the former's vertices point
     *  to functions (SgAsmFunctionDeclaration) in the AST while the latter's points to basic blocks (SgAsmBlock).  However,
     *  building a CFG that has only function call edges is a common enough operation that we provide a method to do just
     *  that.  The benefit of using build_call_graph() is that the user can easily define an additional edge
     *  filter to even further restrict the edges (see build_call_graph() source code for an example).
     *
     *  @code
     *  ControlFlow::Graph cg = ControlFlow().build_call_graph(interp);
     *  @endcode
     *
     *  [1] https://secure.wikimedia.org/wikipedia/en/wiki/Control_flow_graph */
    class ControlFlow {
    public:
        ControlFlow()
            : vertex_filter(NULL), edge_filter(NULL)
            {}
        

        /** A control flow graph.
         *
         *  A control flow graph is simply a Boost graph whose vertex descriptors are integers and whose vertices point to
         *  SgAsmBlock nodes in the AST (via the boost::vertex_name property).  The graph edges represent flow of control from one
         *  SgAsmBlock to another.  Since the control flow graph is a Boost graph, it is endowed with all the features of a Boost
         *  graph and can be the operand of the various Boost graph algorithms.  See build_cfg() for specifics about what is
         *  included in such a graph. */
        typedef boost::adjacency_list<boost::listS,                                 /* edges of each vertex in std::list */
                                      boost::vecS,                                  /* vertices in std::vector */
                                      boost::bidirectionalS,
                                      boost::property<boost::vertex_name_t, SgAsmBlock*> > Graph;

        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;   /**< Graph vertex type. */
        typedef boost::graph_traits<Graph>::edge_descriptor   Edge;     /**< Graph edge type. */

        /**********************************************************************************************************************
         *                                      Filters
         **********************************************************************************************************************/
    public:

        /** Filter for vertices.
         *
         *  This class can be specialized in order to filter out basic blocks (SgAsmBlock) that satisfy an arbitrary
         *  condition.  See set_vertex_filter() for details. */
        class VertexFilter {
        public:
            virtual ~VertexFilter() {}
            virtual bool operator()(ControlFlow*, SgAsmBlock*) = 0;
        };

        /** Filter for edges.
         *
         *  This class can be specialized in order to filter out certain edges that would otherwise make it into the control
         *  flow graph.  See set_edge_filter() for details. */
        class EdgeFilter {
        public:
            virtual ~EdgeFilter() {}
            virtual bool operator()(ControlFlow*, SgAsmBlock *source, SgAsmBlock *target) = 0;
        };

        /** Manipulate the vertex filter.
         *
         *  When building a control flow graph, the vertex filter is invoked on each basic block which is about to be added as
         *  a vertex.  If the filter returns false then that block is not added to the graph.  A null filter accepts all
         *  vertices.
         *
         *  @{ */
        void set_vertex_filter(VertexFilter *filter) { vertex_filter = filter; }
        VertexFilter *get_vertex_filter() const { return vertex_filter; }
        /** @}*/

        /** Manipulate the edge filter.
         *
         *  When building a control flow graph, the edge filter is invoked for each edge which is about to be added to the
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
        bool is_vertex_filtered(SgAsmBlock *block, VertexFilter *filter) { return filter && !(*filter)(this, block); }
        bool is_vertex_filtered(SgAsmBlock *block) { return is_vertex_filtered(block, vertex_filter); }
        /** @} */

        /** Determines if an edge is filtered out.
         *
         *  Returns true if the edge would be filtered out by being rejected by the current edge filter.
         *
         *  @{ */
        bool is_edge_filtered(SgAsmBlock *src, SgAsmBlock *dst, EdgeFilter *filter) {
            return filter && !(*filter)(this, src, dst);
        }
        bool is_edge_filtered(SgAsmBlock *src, SgAsmBlock *dst) {
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

        /** Clears successor information from the AST.
         *
         *  Traverses the specified AST and clears the successor lists for all blocks.  The blocks are visited by an AST
         *  traversal, not by following successor pointers.
         *
         *  The current vertex filter determines which edges are filtered. */
        void clear_ast(SgNode *ast);

        /** Applies graph to AST.
         *
         *  Just as a control flow graph can be built from the successor lists stored in the AST (see build_graph()), a graph
         *  can be used to initialize the successor information in an AST.  This function does that.  Only the blocks which are
         *  vertices of the graph and which pass the current vertex filter are affected.  Only edges that pass the current edge
         *  filter are added as successors to the (cleared) block successor list.
         *
         *  At this time [2011-05-19] the successor_complete property of each affected block is set to true, but this may
         *  change in the future. */
        void apply_to_ast(const Graph&);

        /** Cache vertex descriptors in AST.
         *
         *  The vertices of a control flow graph are of type Vertex, and point at the basic blocks (SgAsmBlock) of the
         *  AST. Although most graph algorithms will only need to map Vertex to SgAsmBlock, the inverse mapping is also
         *  sometimes useful.  That mapping can be stored into an std::map via graph traversal, or stored in the AST itself
         *  attached to each SgAsmBlock.  Using an std::map requires an O(log N) lookup each time we need to get the vertex
         *  descriptor from a block, while storing the vertex descriptor in the AST requires O(1) lookup time.
         *
         *  The vertex descriptors are available via SgAsmBlock::get_cached_vertex().  Other graph types (e.g., dominance
         *  graphs) might also use the same cache line.  The cached vertex is stored as a size_t, which is the same underlying
         *  type for CFG vertices.
         *
         *  The current vertex filter determines which blocks are modified. */
        void cache_vertex_descriptors(const Graph&);

        /**********************************************************************************************************************
         *                                      Graph construction methods
         **********************************************************************************************************************/
    public:

        /** Builds a control flow graph for part of an AST.
         *
         *  Builds a control flow graph for the part of the abstract syntax tree rooted at @p root by traversing the AST to
         *  find all basic blocks and using the successors of those blocks to define the edges of the control flow graph.
         *  Successors are retrieved via SgAsmBlock::get_successors() and are of type SgAsmTarget.
         *
         *  The current vertex and edge filters are used to determine which blocks and flow edges are added to the
         *  graph. However, the following types of successors are never added (and don't trigger a call to the filter):
         *  <ul>
         *    <li>Successors that have no block pointer, but only an address.  This can happen when we didn't disassemble any
         *        instruction at the successor address, and thus don't have a block at which to point.</li>
         *    <li>Successors that point to a block outside the specified AST subtree.  For instance, when considering the
         *        control flow graph of an individual function, successors for function calls will point outside the calling
         *        function (unless the call is recursive).<li>
         *    <li>Successors that are not known.  Some basic blocks, such as function return blocks or blocks ending with computed
         *        branches, usually only have unknown successors.  Such edges are not added to the graph.</li>
         *  </ul>
         *
         *  @{ */
        Graph build_graph(SgNode *root);
        void build_graph(SgNode *root, Graph &cfg/*out*/);
        /** @} */

        /** Builds a control flow graph with only function call edges.
         *
         *  This differs from a true FunctionCall::Graph in that the vertices of the returned graph point to basic blocks,
         *  while the vertices of a FunctionCall::Graph point to functions (SgAsmFunctionDeclaration nodes).
         *
         *  The graph is built by applying an edge filter (in addition to the edge filter that might be set by the user) that
         *  only accepts edges whose target is a function entry block.
         *
         *  @{ */
        Graph build_call_graph(SgNode *root);
        void build_call_graph(SgNode *root, Graph &cfg/*out*/);
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

        /**********************************************************************************************************************
         *                                      Miscellaneous members
         **********************************************************************************************************************/
    public:

        /** Orders nodes by depth first search reverse post order.
         *
         *  Reversed, depth-first-search, post-order is a common node order needed for solving flow equations, and this method
         *  returns a vector whose elements are graph vertices.  The algorithm is to allocate the vector to be the same length
         *  as the total number of vertices in the graph, and then do a depth-first-search starting with the specified
         *  node.  Each node is visited after all its children (post-order).  Each node visit adds the node to the vector
         *  starting at the end of the vector and working forward.
         *
         *  If reverse_order is non-null then it will be the inverse mapping from the returned vector.  In other words, if the
         *  return value is named forward_order, then
         *
         *    forward_order[i] == v  implies reverse_order[v] == i
         *
         *  Here's an example of how this would typically be used.
         *
         *  @code
         *  std::vector<size_t> rflowlist; // reverse mapping
         *  std::vector<ControlFlow::Vector> flowlist;
         *  flowlist = ControlFlow().flow_order(cfg, start, &rflowlist);
         *
         *  bool changed;
         *  do {
         *      changed = false;
         *      for (size_t i=0; i<flowlist.size(); ++i) {
         *          ControlFlow::Vertex vertex = flowlist[i];
         *          // solve flow equation for vertex...
         *          if (result_at_vertex_changed)
         *              changed = true;
         *      }
         *  } while (changed);
         *  @endcode
         */
        std::vector<Vertex> flow_order(const Graph&, Vertex start, std::vector<size_t> *reverse_order=NULL);
    };
}

#endif
