#ifndef ROSE_BinaryAnalysis_ControlFlow_H
#define ROSE_BinaryAnalysis_ControlFlow_H

#include "Map.h"
#include "WorkLists.h"
#include "SageBuilderAsm.h"

#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <Sawyer/GraphBoost.h>

class SgNode;
class SgAsmBlock;

namespace Rose {

/** Binary analysis.
 *
 *  ROSE supports both source code analysis and binary analysis. This namespace contains most of the binary analysis
 *  features. The @ref binary_tutorial is a good place to get started, and most of its examples link to the API
 *  documentation. Follow those links to the enclosing class and/or namespace to obtain an overview of how those classes and
 *  namespaces work. */
namespace BinaryAnalysis {

/** Binary control flow analysis.
 *
 *  This class serves mostly to organize the functions that operate on control flow graphs, but also provides a container for
 *  various settings that influence the control flow analyses, such as the vertex and edge filters. The features described here
 *  are one form of control flow graph; see also the control flow graph used by @ref Partitioner2::Partitioner "function
 *  detection", which is easier to work with and has more features.
 *
 *  The AST contains an implied CFG by virtue of storing control flow successor addresses in each basic block (SgAsmBlock).
 *  The successor information is initialized by the Partitioner class when the AST is built (see Partitioner::build_ast())
 *  and is available with SgAsmBlock::get_successors().  The SgAsmBlock::get_successors_complete() returns a Boolean
 *  indicating whether the set of successors is completely known. Successors would be incomplete, for instance, for a block
 *  that ends with a computed branch whose targets could not be statically determined. The AST-stored implied graph can be
 *  turned into an explicit Boost graph with various "build" methods defined in this class.
 *
 *  Explicit CFGs use the Boost Graph Library API and come in two flavors: graphs whose vertices are basic blocks, and
 *  graphs whose vertices are instructions.  Although the AST is optimized to store control flow information over basic
 *  blocks, an instruction-based CFG can always be created from a basic block CFG since flow of control within a basic
 *  block is trivial.  It is often easier to work with instruction-based CFGs even though they can be much larger.  Most of
 *  the following documentation describes these explicit Boost CFG graphs.
 *
 *  The CFGs on which this class operates must follow the Boost Graph Library (BGL) API, although not necessarily a BGL
 *  implementation. The graph type is normally a template parameter for the methods herein.  This class provides two
 *  typedefs for graphs implemented in BGL: BlockGraph is for basic-block CFGs and InsnGraph is for instruction CFGs.
 *  The graph must support the BGL adjacency_list graph API with the following features:
 *
 *  <ul>
 *    <li>the graph vertices are stored as a vector ("vecS" as the second template argument of adjacency_list)</li>
 *    <li>the graph is bidirectional ("bidrectionalS" as the third template argument),</li>
 *    <li>the boost::vertex_name property is a SgAsmBlock pointer.</li>
 *  </ul>
 *
 *  The wikipedia entry for "Control Flow Graph" [1] has many useful definitions. The BGL documentation is located at the
 *  http://www.boost.org web site.
 *
 *  Control flow graphs can be calculated over any subtree of the AST rooted below a SgAsmInterpretation.  It doesn't make
 *  sense to compute inter-interpretation CFGs since the OS only ever loads one interpretation at a time (instructions from
 *  two different interpretations might share the same virtual address). Usually one creates CFGs that span a single
 *  function or a whole interpretation.  The vertex and edge filtes can restrict which vertices and edges are considered by
 *  the various methods of this class.  For instance, to create a global CFG that has no inter-function edges
 *  one could do the following (see also, Rose::BinaryAnalysis::FunctionCall):
 *
 *  @code
 *  // Create a filter that rejects all inter-function edges
 *  struct NoCallEdges: public ControlFlow::EdgeFilter {
 *      bool operator()(ControlFlow *analyzer, SgAsmNode *src, SgAsmNode *dst) {
 *          SgAsmFunction *src_func = SageInterface::getEnclosingNode<SgAsmFunction>(src);
 *          SgAsmFunction *dst_func = SageInterface::getEnclosingNode<SgAsmFunction>(dst);
 *          return dst_func != src_func;
 *      }
 *  } edge_filter;
 *
 *  // Create the control flow analyzer and set its edge filter.
 *  ControlFlow analyzer;
 *  analyzer.set_edge_filter(&edge_filter);
 *
 *  // Generate a block-based CFG over an entire interpretation. It will include
 *  // all basic blocks, but only intra-function edges.
 *  typedef ControlFlow::BlockGraph CFG_B;
 *  SgAsmInterpretation *interp = ...;
 *  CFG_B cfg1 = analyzer.build_block_cfg_from_ast<CFG_B>(interp);
 *
 *  // Generate an instruction-based CFG over an entire interpretation. It will
 *  // include all instructions, but only intra-function edges.
 *  typedef ControlFlow::InsnGraph CFG_I;
 *  CFG_I cfg2 = analyzer.build_insn_cfg_from_ast<CFG_I>(interp);
 *  @endcode
 *
 *  Another way to exclude vertices and/or edges from a graph is to first build a complete graph and then do a filtered
 *  copy to obtain a second graph with only the desired vertices and edges.
 *
 *  @code
 *  // Build a complete CFG graph using a default control flow analyzer.
 *  CFG_I cfg3 = ControlFlow().build_insn_cfg_from_ast<CFG_I>(interp);
 *
 *  // Using the same analyzer as the previous example, one that
 *  // filters out all but the function call edges, create a call
 *  // graph.
 *  CFG_I cfg4 = analyzer.copy(cfg3);
 *  @endcode
 *
 *  See also, Rose::BinaryAnalysis::FunctionCall, which computes a function call graph whose vertices are functions rather
 *  than the basic blocks or instructions used in a CFG.
 *
 *  Since binary control flow graphs follow the BGL API they can be easily printed as GraphViz graphs using
 *  boost::write_graphviz().  If you want something other than vertex descriptors in the graphs, you could use a
 *  PropertyWriter class, like this one, which labels the vertices with the basic block address.  Ideally, one would use a
 *  class template, but we keep this example simple:
 *
 *  @code
 *  // Label the graphviz vertices with basic block addresses.
 *  // Boost requires this to be declared at file scope.
 *  struct GraphvizVertexWriter {
 *      const Rose::BinaryAnalysis::ControlFlow::Graph &cfg;
 *      GraphvizVertexWriter(Rose::BinaryAnalysis::ControlFlow::Graph &cfg): cfg(cfg) {}
 *      typedef boost::graph_traits<Rose::BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;
 *      void operator()(std::ostream &output, const Vertex &v) {
 *          SgAsmBlock *block = get_ast_node(cfg, v);
 *          output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
 *      }
 *  };
 *
 *  // Write the graph
 *  boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter(cfg));
 *  @endcode
 *
 *  We also define a similar write_graphviz method within this class that differs from the Boost implementation in two
 *  ways. First, the GraphViz file that it produces has the vertices organized into clusters based on the function in which
 *  they appear. Second, the vertex and edge property writers are passed as const references to avoid unnecessary copying.
 *
 *  [1] https://secure.wikimedia.org/wikipedia/en/wiki/Control_flow_graph */
class ControlFlow {
public:
    ControlFlow()
        : vertex_filter(NULL), edge_filter(NULL)
        {}


    /** Default basic block control flow graph type.
     *
     *  A control flow graph is has a Boost graph interface whose vertex descriptors are integers and whose vertices point
     *  to SgAsmBlock nodes in the AST (via the boost::vertex_name property).  The graph edges represent flow of control
     *  from one basic block to another.  Since the control flow graph is a Boost graph, it is endowed with all the
     *  features of a Boost graph and can be the operand of the various Boost graph algorithms.  See
     *  build_block_cfg_from_ast() for specifics about what is included in such a graph.
     *
     *  It is common to need a type for the vertices and edges.  Boost graphs store this information in graph_traits and
     *  users should use that to obtain those types.  Doing so will, in the long run, make your code more extensible since
     *  the only datatype you're depending on is the graph itself--change the graph type and the vertex and edge types will
     *  automatically adjust.  See Boost Graph Library documentation for all the available types.  The most common are:
     *
     *  @code
     *  typedef boost::graph_traits<BlockGraph>::vertex_descriptor Vertex;
     *  typedef boost::graph_traits<BlockGraph>::edge_descriptor Edge;
     *  @endcode
     */
    typedef boost::adjacency_list<boost::setS,                                  /* edges of each vertex in std::list */
                                  boost::vecS,                                  /* vertices in std::vector */
                                  boost::bidirectionalS,
                                  boost::property<boost::vertex_name_t, SgAsmBlock*> > BlockGraph;

    /** Default instruction-based control flow graph.
     *
     *  A control flow graph has a Boost graph interface whose vertex descriptors are integers and whose vertices point to
     *  SgAsmInstruction nodes in the AST (via the boost::vertex_name property).  The graph edges represent flow of control
     *  from one instruction to another.  Since the control flow graph is a Boost graph, it is endowed with all the features
     *  of a Boost graph and can be the operand of the various Boost graph algorithms.  See build_insn_cfg_from_ast() for
     *  specifics about what is included in such a graph.
     *
     *  It is common to need a type for the vertices and edges.  Boost graphs store this information in graph_traits and
     *  users should use that to obtain those types.  Doing so will, in the long run, make your code more extensible since
     *  the only datatype you're depending on is the graph itself--change the graph type and the vertex and edge types will
     *  automatically adjust.  See Boost Graph Library documentation for all the available types.  The most common are:
     *
     *  @code
     *  typedef boost::graph_traits<InsnGraph>::vertex_descriptor Vertex;
     *  typedef boost::graph_traits<InsnGraph>::edge_descriptor Edge;
     *  @endcode
     */
    typedef boost::adjacency_list<boost::setS,
                                  boost::vecS,
                                  boost::bidirectionalS,
                                  boost::property<boost::vertex_name_t, SgAsmInstruction*> > InsnGraph;

    /** Default control flow graph.  The original Rose::BinaryAnalysis::ControlFlow API defined only "Graph", which was a
     *  basic-block control flow graph.  We continue to define that type for backward compatibility. */
    typedef BlockGraph Graph;


    /**********************************************************************************************************************
     *                                      Filters
     **********************************************************************************************************************/
public:

    /** Filter for vertices.
     *
     *  This class can be specialized in order to filter out vertices (SgAsmBlock or SgAsmInstruction depending on the type
     *  of the graph) that satisfy an arbitrary condition.  See set_vertex_filter() for details. */
    class VertexFilter {
    public:
        virtual ~VertexFilter() {}
        virtual bool operator()(ControlFlow*, SgAsmNode*) = 0;
    };

    /** Filter for edges.
     *
     *  This class can be specialized in order to filter out certain edges that would otherwise make it into the control
     *  flow graph.  See set_edge_filter() for details. */
    class EdgeFilter {
    public:
        virtual ~EdgeFilter() {}
        virtual bool operator()(ControlFlow*, SgAsmNode *source, SgAsmNode *target) = 0;
    };

    /** Manipulate the vertex filter.
     *
     *  When building a control flow graph, the vertex filter is invoked on each vertex which is about to be added.  If the
     *  filter returns false then that block is not added to the graph.  A null filter accepts all vertices.
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
    bool is_vertex_filtered(SgAsmNode *bb_or_insn, VertexFilter *filter) { return filter && !(*filter)(this, bb_or_insn); }
    bool is_vertex_filtered(SgAsmNode *bb_or_insn) { return is_vertex_filtered(bb_or_insn, vertex_filter); }
    /** @} */

    /** Determines if an edge is filtered out.
     *
     *  Returns true if the edge would be filtered out by being rejected by the current edge filter.  The @p src and @p dst
     *  are the source and destination vertex nodes, either basic blocks or instructions depending on the graph type.
     *
     *  @{ */
    bool is_edge_filtered(SgAsmNode *src, SgAsmNode *dst, EdgeFilter *filter) {
        return filter && !(*filter)(this, src, dst);
    }
    bool is_edge_filtered(SgAsmNode *src, SgAsmNode *dst) {
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
     *  Traverses the specified AST and clears the successor lists for all basic blocks.  The blocks are visited by an AST
     *  traversal, not by following successor pointers.
     *
     *  The current vertex filter determines which edges are filtered. */
    void clear_ast(SgNode *ast);

    /** Applies graph to AST.
     *
     *  Just as a control flow graph can be built from the successor lists stored in the AST (see
     *  build_block_cfg_from_ast() or build_insn_cfg_from_ast()), a graph can be used to initialize the successor
     *  information in an AST.  This function does that.  Only the blocks which are vertices of the graph and which pass
     *  the current vertex filter are affected.  Only edges that pass the current edge filter are added as successors to
     *  the (cleared) block successor list.
     *
     *  Not all instruction-based graphs can be written back to the AST because the implied basic block structure of the
     *  graph might not match the explicit basic block structure in the AST.  Perhaps a future version will allow the AST
     *  to be restructured by this operation.
     *
     *  At this time [2011-05-19] the successor_complete property of each affected block is set to true, but this may
     *  change in the future. */
    template<class ControlFlowGraph>
    void apply_to_ast(const ControlFlowGraph&);

    /** Cache basic block vertex descriptors in AST.
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
    template<class ControlFlowGraph>
    void cache_vertex_descriptors(const ControlFlowGraph&);

    /**********************************************************************************************************************
     *                                      Graph construction methods
     **********************************************************************************************************************/
public:

    /** Builds a control flow graph for part of an AST.
     *
     *  Builds a control flow graph for the part of the abstract syntax tree rooted at @p root by traversing the AST to
     *  find all basic blocks and using the successors of those blocks to define the edges of the control flow graph.
     *  Successors are retrieved via SgAsmBlock::get_successors() and are of type SgAsmIntegerValueExpression.  The
     *  specified @p root for the AST should generally not include multiple interpretations (SgAsmInterpretation) since the
     *  OS never loads more than one interpretation at a time.
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
     * The build_block_cfg_from_ast() builds a CFG whose vertices are basic blocks, while the build_insn_cfg_from_ast()
     * builds a CFG whose vertices are instructions.  Furthermore, the instruction-based CFG makes some adjustments to
     * function call and function return nodes by invoking fixup_fcall_fret().
     *
     *  @{ */
    template<class ControlFlowGraph>
    ControlFlowGraph build_block_cfg_from_ast(SgNode *root);

    template<class ControlFlowGraph>
    void build_block_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/);

    template<class ControlFlowGraph>
    ControlFlowGraph build_insn_cfg_from_ast(SgNode *root);

    template<class ControlFlowGraph>
    void build_insn_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/);
    /** @} */

    /** Create an instruction control flow graph from a basic block control flow graph.
     * @{ */
    template<class BlockCFG, class InsnCFG>
    void explode_blocks(const BlockCFG &cfgb, InsnCFG &cfgi/*out*/);
    /** @} */

    /** Fix up a CFG by changing function call and return edges.  The AST does not store function return edges (which this
     *  method adds), and the AST stores a fall-through edge for function call nodes for callees that may return (which
     *  this method removes). */
    template<class InsnCFG>
    void fixup_fcall_fret(InsnCFG &cfg/*in,out*/, bool preserve_call_fallthrough_edges);

    /** Builds a control flow graph with only function call edges.
     *
     *  This differs from a true FunctionCall::Graph in that the vertices of the returned graph point to basic blocks,
     *  while the vertices of a FunctionCall::Graph point to functions (SgAsmFunction nodes).
     *
     *  The graph is built by applying an edge filter (in addition to the edge filter that might be set by the user) that
     *  only accepts edges whose target is a function entry block.
     *
     *  @{ */
    template<class ControlFlowGraph>
    ControlFlowGraph build_cg_from_ast(SgNode *root);

    template<class ControlFlowGraph>
    void build_cg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/);
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
    template<class ControlFlowGraph>
    ControlFlowGraph copy(const ControlFlowGraph &src);

    template<class ControlFlowGraph>
    void copy(const ControlFlowGraph &src, ControlFlowGraph &dst/*out*/);
    /** @} */

    /***********************************************************************************************************************
     *                                      Graph output
     ***********************************************************************************************************************/

    /** List of vertices and intra-function edges for one function. */
    template<class CFG>
    struct FunctionSubgraphInfo {
        std::vector<typename boost::graph_traits<CFG>::vertex_descriptor> vertices;
        std::vector<typename boost::graph_traits<CFG>::edge_descriptor> edges;
    };

    /** Default vertex property writer is a no-op. */
    template<class CFG>
    struct DefaultVertexPropertyWriter {
        void operator()(std::ostream &o, typename boost::graph_traits<CFG>::vertex_descriptor vertex) const {}
    };

    /** Default edge property writer is a no-op. */
    template<class CFG>
    struct DefaultEdgePropertyWriter {
        void operator()(std::ostream &o, typename boost::graph_traits<CFG>::edge_descriptor vertex) const {}
    };

    /** Write a CFG to a graphviz file, creating a cluster subgraph for each function.
     * @{ */
    template<typename CFG, class VertexPropertyWriter, class EdgePropertyWriter>
    void write_graphviz(std::ostream&, const CFG&, const VertexPropertyWriter&, const EdgePropertyWriter&);

    template<typename CFG>
    void write_graphviz(std::ostream &out, const CFG &cfg) {
        write_graphviz(out, cfg, DefaultVertexPropertyWriter<CFG>(), DefaultEdgePropertyWriter<CFG>());
    }

    template<typename CFG, class VertexPropertyWriter>
    void write_graphviz(std::ostream &out, const CFG &cfg, const VertexPropertyWriter &vpw) {
        write_graphviz(out, cfg, vpw, DefaultEdgePropertyWriter<CFG>());
    }
    /** @} */

    /**********************************************************************************************************************
     *                                      Miscellaneous members
     **********************************************************************************************************************/

private:
    /* Visitor used by flow_order().  Declaring this in function scope results in boost errors (boost-1.42, 2011-05). */
    template<class ControlFlowGraph>
    struct FlowOrder: public boost::default_dfs_visitor {
        typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
        typedef std::vector<Vertex> VertexList;
        typedef std::vector<size_t> ReverseVertexList;
        VertexList *forward_order;
        FlowOrder(VertexList *forward_order): forward_order(forward_order) {}
        void compute(const ControlFlowGraph &g, Vertex v0, ReverseVertexList *reverse_order);
        void finish_vertex(Vertex v, ControlFlowGraph g);
    };

    /* Helper class for build_block_cfg_from_ast().  Adds vertices to its 'cfg' member. Vertices are any SgAsmBlock that
     * contains at least one SgAsmInstruction. */
    template<class ControlFlowGraph>
    class VertexInserter: public AstSimpleProcessing {
    public:
        ControlFlow *analyzer;
        ControlFlowGraph &cfg;
        typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
        typedef Map<SgAsmBlock*, Vertex> BlockVertexMap;
        BlockVertexMap &bv_map;
        VertexInserter(ControlFlow *analyzer, ControlFlowGraph &cfg, BlockVertexMap &bv_map)
            : analyzer(analyzer), cfg(cfg), bv_map(bv_map)
            {}
        // Add basic block to graph if it hasn't been added already.
        void conditionally_add_vertex(SgAsmBlock *block);

        void visit(SgNode *node) {
            if (isSgAsmFunction(node)) {
                // Add the function entry block before the other blocks of the function.  This ensures that the entry block
                // of a function has a lower vertex number than the other blocks of the function (the traversal is not
                // guaranteed to visit the function basic blocks in that order).
                conditionally_add_vertex(isSgAsmFunction(node)->get_entry_block());
            } else {
                conditionally_add_vertex(isSgAsmBlock(node));
            }
        }
    };

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
    template<class ControlFlowGraph>
    std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
    flow_order(const ControlFlowGraph&,
               typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
               std::vector<size_t> *reverse_order=NULL);

private:
    /* Visitor used by return_blocks(). Declaring this in function scope results in boost errors (boost-1.42, 2011-05). */
    template<class ControlFlowGraph>
    struct ReturnBlocks: public boost::default_dfs_visitor {
        typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
        typedef std::vector<Vertex> Vector;
        Vector &blocks;
        ReturnBlocks(Vector &blocks): blocks(blocks) {}
        void finish_vertex(Vertex v, ControlFlowGraph g);
    };

public:
    /** Returns list of function return blocks.
     *
     *  More specifically, this method traverses the control flow graph (CFG) beginning at the specified node and returns a
     *  list (in depth first search order) of all vertices which are in the connected subgraph and which do not have any
     *  known successors, but at least one unknown successor.
     *
     *  Currently works only for basic block CFGs. */
    template<class ControlFlowGraph>
    std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
    return_blocks(const ControlFlowGraph &cfg,
                  typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);
};


/*******************************************************************************************************************************
 *                                      Functions
 *******************************************************************************************************************************/

/** Return the AST node associated with a vertex. The return value is either a basic block (SgAsmBlock) or instruction
  * (SgAsmInstruction) depending on the graph type. */
template<class V, class E>
typename Sawyer::Container::Graph<V, E>::VertexValue
get_ast_node(const Sawyer::Container::Graph<V, E> &cfg, size_t vertexId) {
    typedef typename Sawyer::Container::Graph<V, E> CFG;
    typename CFG::ConstVertexValueIterator iter = cfg.findVertex(vertexId);
    ASSERT_forbid2(iter==cfg.vertices().end(), "invalid vertex ID " + StringUtility::numberToString(vertexId));
    return *iter;
}

/** Set the AST node associated with a vertex. The value is either a basic block (SgAsmBlock) or instruction
 * (SgAsmInstruction) depending on the graph type. */
template<class V, class E, class AstNode>
void
put_ast_node(Sawyer::Container::Graph<V, E> &cfg, size_t vertexId, AstNode *astNode) {
    typedef typename Sawyer::Container::Graph<V, E> CFG;
    typename CFG::VertexValueIterator iter = cfg.findVertex(vertexId);
    ASSERT_forbid2(iter==cfg.vertices().end(), "invalid vertex ID " + StringUtility::numberToString(vertexId));
    *iter = astNode;
}

// Sorry about this mess!  The goal is to match only boost::adjacency_list graphs.
template<class A, class B, class C, class D, class E, class F, class G>
typename boost::property_traits<typename boost::property_map<boost::adjacency_list<A, B, C, D, E, F, G>,
                                                             boost::vertex_name_t>::type>::value_type
get_ast_node(const boost::adjacency_list<A, B, C, D, E, F, G> &cfg,
             typename boost::graph_traits<boost::adjacency_list<A, B, C, D, E, F, G> >::vertex_descriptor vertex) {
    return boost::get(boost::vertex_name, cfg, vertex);
}

// Sorry about this mess!  The goal is to match only boost::adjacency_list graphs.
template<class A, class B, class C, class D, class E, class F, class G>
void
put_ast_node(boost::adjacency_list<A, B, C, D, E, F, G> &cfg,
             typename boost::graph_traits<boost::adjacency_list<A, B, C, D, E, F, G> >::vertex_descriptor vertex,
             typename boost::property_traits<
                 typename boost::property_map<boost::adjacency_list<A, B, C, D, E, F, G>, boost::vertex_name_t>::type
                 >::value_type ast_node) {
    boost::put(boost::vertex_name, cfg, vertex, ast_node);
}

/******************************************************************************************************************************
 *                                      Function template definitions
 ******************************************************************************************************************************/

template<class ControlFlowGraph>
void
ControlFlow::apply_to_ast(const ControlFlowGraph &cfg)
{
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get_ast_node(cfg, *vi); // FIXME: Instruction CFGs not supported yet
        if (!block || is_vertex_filtered(block))
            continue;

        /* Delete old targets */
        const SgAsmIntegerValuePtrList &targets = block->get_successors();
        for (SgAsmIntegerValuePtrList::const_iterator ti=targets.begin(); ti!=targets.end(); ++ti)
            delete *ti;

        /* Add new targets */
        block->set_successors_complete(true);
        block->get_successors().clear();
        typename boost::graph_traits<ControlFlowGraph>::out_edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::out_edges(*vi, cfg); ei!=ei_end; ++ei) {
            SgAsmBlock *target_block = get_ast_node(cfg, boost::target(*ei, cfg));
            if (target_block && !is_edge_filtered(block, target_block)) {
                SgAsmIntegerValueExpression *target = SageBuilderAsm::buildValueU64(target_block->get_address());
                target->makeRelativeTo(target_block);
                target->set_parent(block);
                block->get_successors().push_back(target);
            }
        }
    }
}

template<class ControlFlowGraph>
void
ControlFlow::cache_vertex_descriptors(const ControlFlowGraph &cfg)
{
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get_ast_node(cfg, *vi); // FIXME: Instruction CFGs not supported yet
        if (block && !is_vertex_filtered(block))
            block->set_cached_vertex(*vi);
    }
}

template<class ControlFlowGraph>
void
ControlFlow::VertexInserter<ControlFlowGraph>::conditionally_add_vertex(SgAsmBlock *block)
{
    if (block && block->has_instructions() && !analyzer->is_vertex_filtered(block) && !bv_map.exists(block)) {
        Vertex vertex = boost::add_vertex(cfg);
        bv_map[block] = vertex;
        put_ast_node(cfg, vertex, block);
    }
}

template<class ControlFlowGraph>
void
ControlFlow::build_block_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
    Vertex NO_VERTEX = boost::graph_traits<ControlFlowGraph>::null_vertex();
    typedef Map<SgAsmBlock*, Vertex> BlockVertexMap;
    BlockVertexMap bv_map;

    // Add the vertices
    cfg.clear();
    VertexInserter<ControlFlowGraph>(this, cfg, bv_map).traverse(root, preorder);

    // Mapping from block entry address to CFG vertex
    Map<rose_addr_t, Vertex> addrToVertex;
    for (typename BlockVertexMap::iterator bvi=bv_map.begin(); bvi!=bv_map.end(); ++bvi)
        addrToVertex[bvi->first->get_address()] = bvi->second;

    // Add the edges
    BOOST_FOREACH (Vertex sourceVertex, boost::vertices(cfg)) {
        SgAsmBlock *sourceBlock = get_ast_node(cfg, sourceVertex);
        BOOST_FOREACH (SgAsmIntegerValueExpression *integerValue, sourceBlock->get_successors()) {
            Vertex targetVertex = addrToVertex.get_value_or(integerValue->get_absoluteValue(), NO_VERTEX);
            if (targetVertex!=NO_VERTEX) {
                SgAsmBlock *targetBlock = get_ast_node(cfg, targetVertex);
                assert(targetBlock!=NULL); // since we have a vertex, there must be an SgAsmBlock!
                if (!is_edge_filtered(sourceBlock, targetBlock))
                    boost::add_edge(sourceVertex, targetVertex, cfg);
            }
        }
    }
}

template<class ControlFlowGraph>
void
ControlFlow::build_insn_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg)
{
    BlockGraph cfgb;
    build_block_cfg_from_ast(root, cfgb);
    explode_blocks(cfgb, cfg);
    bool preserve_call_fallthrough_edges = false;
    fixup_fcall_fret(cfg, preserve_call_fallthrough_edges);
}

template<class ControlFlowGraph>
void
ControlFlow::build_cg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/)
{
    struct T1: public EdgeFilter {
        EdgeFilter *parent;
        T1(EdgeFilter *parent): parent(parent) {}
        bool operator()(ControlFlow *analyzer, SgAsmNode *src, SgAsmNode *dst) {
            SgAsmFunction *src_func = SageInterface::getEnclosingNode<SgAsmFunction>(src, true);
            SgAsmBlock *dst_block = SageInterface::getEnclosingNode<SgAsmBlock>(dst, true);
            SgAsmFunction *dst_func = SageInterface::getEnclosingNode<SgAsmFunction>(dst_block);
            if (!src_func || !dst_func || dst_block!=dst_func->get_entry_block()) {
                return false;
            } else if (src_func!=dst_func) {
                // inter-function call, not a return edge
            } else {
                // FIXME: this might not actually be a recursive call [Robb P. Matzke 2013-09-05]
            }
            return parent ? (*parent)(analyzer, src, dst) : true;
        }
    };

    EdgeFilter *parent = get_edge_filter();
    T1 edge_filter(parent);
    try {
        set_edge_filter(&edge_filter);
        build_block_cfg_from_ast(root, cfg);
    } catch (...) {
        set_edge_filter(parent);
        throw;
    }
    set_edge_filter(parent);
}

template<class ControlFlowGraph>
void
ControlFlow::copy(const ControlFlowGraph &src, ControlFlowGraph &dst/*out*/)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
    Vertex NO_VERTEX = boost::graph_traits<ControlFlowGraph>::null_vertex();

    dst.clear();
    std::vector<Vertex> src_to_dst(boost::num_vertices(src), NO_VERTEX);

    typename boost::graph_traits<const ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(src); vi!=vi_end; ++vi) {
        SgAsmNode *node = get_ast_node(src, *vi);
        if (!is_vertex_filtered(node)) {
            src_to_dst[*vi] = boost::add_vertex(dst);
            put_ast_node(dst, src_to_dst[*vi], get_ast_node(src, *vi));
        }
    }

    typename boost::graph_traits<const ControlFlowGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::edges(src); ei!=ei_end; ++ei) {
        if (NO_VERTEX!=src_to_dst[boost::source(*ei, src)] && NO_VERTEX!=src_to_dst[boost::target(*ei, src)]) {
            SgAsmNode *node1 = get_ast_node(src, boost::source(*ei, src));
            SgAsmNode *node2 = get_ast_node(src, boost::target(*ei, src));
            if (!is_edge_filtered(node1, node2))
                boost::add_edge(src_to_dst[boost::source(*ei, src)], src_to_dst[boost::target(*ei, src)], dst);
        }
    }
}

template<class ControlFlowGraph>
ControlFlowGraph
ControlFlow::copy(const ControlFlowGraph &src)
{
    ControlFlowGraph dst;
    copy(src, dst);
    return dst;
}

template<class BlockCFG, class InsnCFG>
void
ControlFlow::explode_blocks(const BlockCFG &cfgb, InsnCFG &cfgi/*out*/)
{
    // BlockCFG is the basic-block binary control flow graph
    typedef typename boost::graph_traits<const BlockCFG>::vertex_descriptor BlockCFG_Vertex;
    typedef typename boost::graph_traits<const BlockCFG>::vertex_iterator BlockCFG_VertexIterator;
    typedef typename boost::graph_traits<const BlockCFG>::edge_iterator BlockCFG_EdgeIterator;

    // InsnCFG is the instruction binary control flow graph--it points to instructions rather than basic blocks, and changes
    // some edges regarding function calls.
    typedef typename boost::graph_traits<InsnCFG>::vertex_descriptor InsnCFG_Vertex;
    typedef std::pair<InsnCFG_Vertex, InsnCFG_Vertex> InsnCFG_VertexPair;

    // Expand the cfgb basic blocks to create a cfgi that has instructions instead of blocks, and add the intra-block edges
    cfgi.clear();
    Map<BlockCFG_Vertex, InsnCFG_VertexPair> vertex_translation; // enter and leave instructions for each of the blocks in cfgb
    {
        BlockCFG_VertexIterator vi, vi_end;
        for (boost::tie(vi, vi_end)=boost::vertices(cfgb); vi!=vi_end; ++vi) {
            SgAsmBlock *blk = get_ast_node(cfgb, *vi);
            const SgAsmStatementPtrList &insns = blk->get_statementList();
            assert(!insns.empty());
            InsnCFG_Vertex enter_vertex = boost::graph_traits<InsnCFG>::null_vertex();
            InsnCFG_Vertex prev_vertex = boost::graph_traits<InsnCFG>::null_vertex();
            for (SgAsmStatementPtrList::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
                SgAsmInstruction *insn = isSgAsmInstruction(*ii);
                assert(insn!=NULL); // basic blocks contain only instructions, no other type of asm statement
                InsnCFG_Vertex vertex = boost::add_vertex(cfgi);
                put_ast_node(cfgi, vertex, insn);
                if (ii==insns.begin()) {
                    enter_vertex = vertex;
                } else {
                    boost::add_edge(prev_vertex, vertex, cfgi);
                }
                prev_vertex = vertex;
            }
            assert(prev_vertex!=boost::graph_traits<InsnCFG>::null_vertex()); // basic block had no instructions but was in CFG!
            vertex_translation[*vi] = InsnCFG_VertexPair(enter_vertex, prev_vertex);
        }
    }
    
    // Insert the edges from cfgb.  The corresponding edge in cfgi must emanate from the final instruction of the source basic
    // block and enter at the first instruction of the target basic block.
    {
        BlockCFG_EdgeIterator ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::edges(cfgb); ei!=ei_end; ++ei) {
            InsnCFG_Vertex src_leave_vertex = vertex_translation.get_one(boost::source(*ei, cfgb)).second;
            InsnCFG_Vertex dst_enter_vertex = vertex_translation.get_one(boost::target(*ei, cfgb)).first;
            assert(src_leave_vertex!=boost::graph_traits<InsnCFG>::null_vertex());
            assert(dst_enter_vertex!=boost::graph_traits<InsnCFG>::null_vertex());
            boost::add_edge(src_leave_vertex, dst_enter_vertex, cfgi);
        }
    }
}

template<class InsnCFG>
void
ControlFlow::fixup_fcall_fret(InsnCFG &cfg, bool preserve_call_fallthrough_edges)
{
    typedef typename boost::graph_traits<InsnCFG>::vertex_descriptor CFG_Vertex;
    typedef typename boost::graph_traits<InsnCFG>::vertex_iterator CFG_VertexIterator;
    typedef typename boost::graph_traits<InsnCFG>::in_edge_iterator CFG_InEdgeIterator;
    typedef std::pair<CFG_Vertex, CFG_Vertex> CFG_VertexPair;
    typedef Map<SgAsmInstruction*, CFG_Vertex> InsnToVertex;
    CFG_Vertex NO_VERTEX = boost::graph_traits<InsnCFG>::null_vertex();

    // Build mappings needed later and find the function return points.  We just look for the x86
    // RET instruction for now and assume that each one we find is a return if it has no control flow successors.  They have no
    // successors at this point because CFG1 didn't have any.
    InstructionMap insns;
    InsnToVertex insn_to_vertex;
    std::vector<bool> isret(boost::num_vertices(cfg), false);
    {
        CFG_VertexIterator vi, vi_end;
        for (boost::tie(vi, vi_end)=boost::vertices(cfg); vi!=vi_end; ++vi) {
            SgAsmInstruction *insn = get_ast_node(cfg, *vi);
            insns[insn->get_address()] = insn;
            insn_to_vertex[insn] = *vi;

            if (0==boost::out_degree(*vi, cfg)) {
                // FIXME: Architecture-specific code here
                if (SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn)) {
                    isret[*vi] = x86_ret==insn_x86->get_kind();
                }
            }
        }
    }

    // Return the entry vertex for a function that owns the indicated instruction
    struct FunctionEntryVertex {
        const InsnToVertex &insn_to_vertex;
        const InstructionMap &imap;
        FunctionEntryVertex(const InsnToVertex &insn_to_vertex, const InstructionMap &imap)
            : insn_to_vertex(insn_to_vertex), imap(imap) {}
        CFG_Vertex operator()(SgAsmInstruction *insn) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn, true);
            SgAsmInstruction *entry_insn = imap.get_one(func->get_entry_va());
            CFG_Vertex entry_vertex = insn_to_vertex.get_one(entry_insn);
            return entry_vertex;
        }
    } function_entry_vertex(insn_to_vertex, insns);
    
    // Process each return site in order to add edges from the return site to the vertex representing the return address
    std::vector<CFG_VertexPair> edges_to_insert, edges_to_erase;
    {
        CFG_VertexIterator vi, vi_end;
        for (boost::tie(vi, vi_end)=boost::vertices(cfg); vi!=vi_end; ++vi) {
            CFG_Vertex returner_vertex = *vi;
            if (!isret[returner_vertex])
                continue;
            SgAsmInstruction *returner_insn = get_ast_node(cfg, returner_vertex);

            // Find all of the true call sites for the function that owns the returner instruction (e.g., RET) by recursively
            // following inter-function CFG edges until we find the true calls (those edges that follow CALL semantics).
            // Inter-function CFG edges can represent true calls or simply inter-function branches such as thunks.  We have to
            // gather up the information without adding it to the CFG yet (can't add while we're iterating)
            std::vector<bool> seen(boost::num_vertices(cfg), false);
            WorkList<CFG_Vertex> worklist; // targets of inter-function CFG edges; function callees
            worklist.push(function_entry_vertex(returner_insn));
            while (!worklist.empty()) {
                CFG_Vertex callee_vertex = worklist.shift();
                CFG_InEdgeIterator ei, ei_end;
                for (boost::tie(ei, ei_end)=boost::in_edges(callee_vertex, cfg); ei!=ei_end; ++ei) {
                    CFG_Vertex caller_vertex = boost::source(*ei, cfg); // caller is a inter-function call or branch site
                    if (!seen[caller_vertex]) {
                        seen[caller_vertex] = true;
                        SgAsmInstruction *caller_insn = get_ast_node(cfg, caller_vertex);
                        SgAsmBlock *caller_block = SageInterface::getEnclosingNode<SgAsmBlock>(caller_insn);
                        assert(caller_block!=NULL);
                        rose_addr_t target_va, returnee_va; // returnee_va is usually the call's fall-through address
                        if (caller_block->is_function_call(target_va/*out*/, returnee_va/*out*/)) {
                            // This is a true call, so we need to add a return edge from the return instruction (the
                            // "returner") to what is probably the fall-through address of the call site (the returnee).
                            SgAsmInstruction *returnee_insn = insns.get_value_or(returnee_va, NULL);
                            CFG_Vertex returnee_vertex = insn_to_vertex.get_value_or(returnee_insn, NO_VERTEX);
                            if (returnee_vertex!=NO_VERTEX) {
                                edges_to_insert.push_back(CFG_VertexPair(returner_vertex, returnee_vertex));
                                edges_to_erase.push_back(CFG_VertexPair(caller_vertex, returnee_vertex));
                            }
                        } else {
                            // This is a non-call inter-function edge; probably a thunk. We need to find its call sites and add
                            // the returnee addresses (call fall throughs) to the returnee addresses of the RET we're
                            // processing.
                            worklist.push(function_entry_vertex(caller_insn));
                        }
                    }
                }
            }
        }
    }
    
    // Erase and insert edges now that we're done iterating.
    if (!preserve_call_fallthrough_edges) {
        for (size_t i=0; i<edges_to_erase.size(); ++i)
            boost::remove_edge(edges_to_erase[i].first, edges_to_erase[i].second, cfg);
    }
    for (size_t i=0; i<edges_to_insert.size(); ++i)
        boost::add_edge(edges_to_insert[i].first, edges_to_insert[i].second, cfg);
}

template<class ControlFlowGraph>
void
ControlFlow::FlowOrder<ControlFlowGraph>::compute(const ControlFlowGraph &g, Vertex v0,
                                                  ReverseVertexList *reverse_order) {
    forward_order->clear();
    std::vector<boost::default_color_type> colors(boost::num_vertices(g), boost::white_color);
    boost::depth_first_visit(g, v0, *this, &(colors[0]));
    assert(!forward_order->empty()); /* it should at least contain v0 */
    std::reverse(forward_order->begin(), forward_order->end());
    if (reverse_order) {
        reverse_order->clear();
        reverse_order->resize(boost::num_vertices(g), (size_t)(-1));
        for (size_t i=0; i<forward_order->size(); i++)
            (*reverse_order)[(*forward_order)[i]] = i;
    }
}

template<class ControlFlowGraph>
void
ControlFlow::FlowOrder<ControlFlowGraph>::finish_vertex(Vertex v, ControlFlowGraph g) {
    forward_order->push_back(v);
}

template<class ControlFlowGraph>
std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
ControlFlow::flow_order(const ControlFlowGraph &cfg,
                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                        std::vector<size_t> *reverse_order/*=NULL*/)
{
    std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor> forward_order;
    FlowOrder<ControlFlowGraph>(&forward_order).compute(cfg, start, reverse_order);
    return forward_order;
}

template<class ControlFlowGraph>
void
ControlFlow::ReturnBlocks<ControlFlowGraph>::finish_vertex(Vertex v, ControlFlowGraph g)
{
    typename boost::graph_traits<ControlFlowGraph>::out_edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = boost::out_edges(v, g);
    if (ei==ei_end)
        blocks.push_back(v);
}

template<class ControlFlowGraph>
std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
ControlFlow::return_blocks(const ControlFlowGraph &cfg,
                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    typename ReturnBlocks<ControlFlowGraph>::Vector result;
    ReturnBlocks<ControlFlowGraph> visitor(result);
    std::vector<boost::default_color_type> colors(boost::num_vertices(cfg), boost::white_color);
    boost::depth_first_visit(cfg, start, visitor, &(colors[0]));
    return result;
}

template<class ControlFlowGraph>
ControlFlowGraph
ControlFlow::build_block_cfg_from_ast(SgNode *root)
{
    ControlFlowGraph cfg;
    build_block_cfg_from_ast(root, cfg);
    return cfg;
}

template<class ControlFlowGraph>
ControlFlowGraph
ControlFlow::build_insn_cfg_from_ast(SgNode *root)
{
    ControlFlowGraph cfg;
    build_insn_cfg_from_ast(root, cfg);
    return cfg;
}

template<class ControlFlowGraph>
ControlFlowGraph
ControlFlow::build_cg_from_ast(SgNode *root)
{
    ControlFlowGraph cfg;
    build_cg_from_ast(root, cfg);
    return cfg;
}

/** Write a control flow graph to a graphviz file, creating a cluster subgraph for each function. */
template<typename CFG, class VertexPropertyWriter, class EdgePropertyWriter>
void
ControlFlow::write_graphviz(std::ostream &out, const CFG &cfg,
                            const VertexPropertyWriter &vpw, const EdgePropertyWriter &epw)
{
 // typedef typename boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
    typedef typename boost::graph_traits<CFG>::edge_descriptor CFG_Edge;
    typedef typename boost::graph_traits<CFG>::vertex_iterator CFG_VertexIterator;
    typedef typename boost::graph_traits<CFG>::out_edge_iterator CFG_OutEdgeIterator;

    // Partition the graph into functions and inter-function edges
    typedef Map<SgAsmFunction*, FunctionSubgraphInfo<CFG> > Functions;
    Functions funcs;
    std::vector<CFG_Edge> interfunc_edges;
    CFG_VertexIterator vi, vi_end;
    for (boost::tie(vi, vi_end)=boost::vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(get_ast_node(cfg, *vi), true);
        FunctionSubgraphInfo<CFG> &f = funcs[func];
        f.vertices.push_back(*vi);
        CFG_OutEdgeIterator ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::out_edges(*vi, cfg); ei!=ei_end; ++ei) {
            SgNode *tgt_node = get_ast_node(cfg, boost::target(*ei, cfg));
            SgAsmFunction *tgt_func = SageInterface::getEnclosingNode<SgAsmFunction>(tgt_node, true);
            if (tgt_func==func) {
                f.edges.push_back(*ei);
            } else {
                interfunc_edges.push_back(*ei);
            }
        }
    }

    // Output subgraph info, each function in its own cluster
    out <<"digraph G {\n";
    for (typename Functions::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi) {
        FunctionSubgraphInfo<CFG> &f = fi->second;
        if (!f.vertices.empty() || !f.edges.empty()) {
            SgNode *node = get_ast_node(cfg, f.vertices.front());
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(node, true);
            char cluster_name[64];
            sprintf(cluster_name, "cluster_F%" PRIx64, func->get_entry_va());
            out <<"  subgraph " <<cluster_name <<" {\n"
                <<"    style=filled;\n"
                <<"    color=lightgrey;\n"
                <<"    label=\"Function " <<StringUtility::addrToString(func->get_entry_va())
                <<(func->get_name().empty()?std::string(""):(" <"+func->get_name()+">")) <<"\";\n";
            for (size_t i=0; i<f.vertices.size(); ++i) {
                out <<"    " <<f.vertices[i];
                vpw(out, f.vertices[i]);
                out <<";\n";
            }
            for (size_t i=0; i<f.edges.size(); ++i) {
                out <<"    " <<boost::source(f.edges[i], cfg) <<"->" <<boost::target(f.edges[i], cfg);
                epw(out, f.edges[i]);
                out <<";\n";
            }
            out <<"  }\n"; // subgraph
        }
    }

    // Inter-function edges
    for (size_t i=0; i<interfunc_edges.size(); ++i) {
        out <<"  " <<boost::source(interfunc_edges[i], cfg) <<"->" <<boost::target(interfunc_edges[i], cfg);
        epw(out, interfunc_edges[i]);
        out <<";\n";
    }
    out <<"}\n"; // digraph
}

} // namespace
} // namespace

#endif
