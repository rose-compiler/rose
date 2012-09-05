#ifndef ROSE_BinaryAnalysis_ControlFlow_H
#define ROSE_BinaryAnalysis_ControlFlow_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

class SgNode;
class SgAsmBlock;

/** Namespace for lots of binary analysis stuff. */
namespace BinaryAnalysis {

    /** Binary control flow analysis.
     *
     *  This class serves mostly to organize the functions that operate on control flow, but also provides a container for
     *  various settings that influence the control flow analyses, such as the vertex and edge filters.
     *
     *  Control flow is represented in the AST as successor pointers (SgAsmIntegerValueExpression) attached to each basic block
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
     *  struct OnlyCallEdges: public ControlFlow::EdgeFilter {
     *      bool operator()(ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
     *          SgAsmFunction *func = dst->get_enclosing_function();
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
     *  typedef ControlFlow::Graph CFG;
     *  SgAsmInterpretation *interp = ...;
     *  CFG cfg = analyzer.build_cfg_from_ast<CFG>(interp);
     *  @endcode
     *
     *  Note that the build_cfg_from_ast(), as well as most of the other methods in this class, are function templates that
     *  take a graph type as an argument.  For convenience, this class defines a default graph type, Graph.  Any Boost
     *  adjacency list graph type can be used as long as the following are true:
     *
     *  <ul>
     *    <li>the graph vertices are stored as a vector ("vecS" as the second template argument of adjacency_list)</li>
     *    <li>the graph is bidirectional ("bidrectionalS" as the third template argument),</li>
     *    <li>the boost::vertex_name property is a SgAsmBlock pointer.</li>
     *  </ul>
     *
     *  Another way to build a function call graph is to first build a complete control flow graph and then do a filtered copy
     *  to obtain a second graph with only the function call edges:
     *
     *  @code
     *  // Build a complete control flow graph using a default
     *  // control flow analyzer.
     *  typedef BinaryAnalysis::ControlFlow::Graph CFG;
     *  CFG cfg = ControlFlow().build_cfg_from_ast<CFG>(interp);
     *
     *  // Using the same analyzer as the previous example, one that
     *  // filters out all but the function call edges, create a call
     *  // graph.
     *  CFG cg = analyzer.copy(cfg);
     *  @endcode
     *
     *  The BinaryAnalysis::FunctionCall::Graph differs from a filtered ControlFlow::Graph in that the former's vertices point
     *  to functions (SgAsmFunction) in the AST while the latter's points to basic blocks (SgAsmBlock).  However,
     *  building a CFG that has only function call edges is a common enough operation that we provide a method to do just
     *  that.  The benefit of using build_cg_from_ast() is that the user can easily define an additional edge
     *  filter to even further restrict the edges (see that method's source code for an example).
     *
     *  @code
     *  typedef ControlFlow::Graph CFG;
     *  CFG cg = ControlFlow().build_cg_from_ast<CFG>(interp);
     *  @endcode
     *
     *  Since binary control flow graphs are simply Boost graphs, they can be easily printed as GraphViz graphs using
     *  boost::write_graphviz().  If you want something other than vertex descriptors in the graphs, you could use a
     *  PropertyWriter class, like this one, which labels the vertices with the basic block address.  Ideally, one would
     *  use a class template, but we keep this example simple:
     *
     *  @code
     *  // Label the graphviz vertices with basic block addresses.
     *  // Boost requires this to be declared at file scope.
     *  struct GraphvizVertexWriter {
     *      const BinaryAnalysis::ControlFlow::Graph &cfg;
     *      GraphvizVertexWriter(BinaryAnalysis::ControlFlow::Graph &cfg): cfg(cfg) {}
     *      typedef boost::graph<BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;
     *      void operator()(std::ostream &output, const Vertex &v) {
     *          SgAsmBlock *block = get(boost::vertex_name, cfg, v);
     *          output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
     *      }
     *  };
     *
     *  // Write the graph
     *  boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter(cfg));
     *  @endcode
     *
     *  [1] https://secure.wikimedia.org/wikipedia/en/wiki/Control_flow_graph */
    class ControlFlow {
    public:
        ControlFlow()
            : vertex_filter(NULL), edge_filter(NULL)
            {}
        

        /** Default control flow graph type.
         *
         *  A control flow graph is simply a Boost graph whose vertex descriptors are integers and whose vertices point to
         *  SgAsmBlock nodes in the AST (via the boost::vertex_name property).  The graph edges represent flow of control from
         *  one SgAsmBlock to another.  Since the control flow graph is a Boost graph, it is endowed with all the features of a
         *  Boost graph and can be the operand of the various Boost graph algorithms.  See build_cfg_from_ast() for specifics
         *  about what is included in such a graph.
         *
         *  It is common to need a type for the vertices and edges.  Boost graphs store this information in graph_traits and
         *  users should use that to obtain those types.  Doing so will, in the long run, make your code more extensible since
         *  the only datatype you're depending on is the graph itself--change the graph type and the vertex and edge types will
         *  automatically adjust.  See Boost Graph Library documentation for all the available types.  The most common are:
         *
         *  @code
         *  typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
         *  typedef boost::graph_traits<Graph>::edge_descriptor Edge;
         *  @endcode
         */
        typedef boost::adjacency_list<boost::setS,                                  /* edges of each vertex in std::list */
                                      boost::vecS,                                  /* vertices in std::vector */
                                      boost::bidirectionalS,
                                      boost::property<boost::vertex_name_t, SgAsmBlock*> > Graph;

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
         *  Just as a control flow graph can be built from the successor lists stored in the AST (see build_cfg_from_ast()), a
         *  graph can be used to initialize the successor information in an AST.  This function does that.  Only the blocks
         *  which are vertices of the graph and which pass the current vertex filter are affected.  Only edges that pass the
         *  current edge filter are added as successors to the (cleared) block successor list.
         *
         *  At this time [2011-05-19] the successor_complete property of each affected block is set to true, but this may
         *  change in the future. */
        template<class ControlFlowGraph>
        void apply_to_ast(const ControlFlowGraph&);

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
         *  Successors are retrieved via SgAsmBlock::get_successors() and are of type SgAsmIntegerValueExpression.
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
        template<class ControlFlowGraph>
        ControlFlowGraph build_cfg_from_ast(SgNode *root);

        template<class ControlFlowGraph>
        void build_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/);
        /** @} */

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
            
        /* Helper class for build_cfg_from_ast().  Adds vertices to its 'cfg' member. Vertices are any SgAsmBlock that contains
         * at least one SgAsmInstruction. */
        template<class ControlFlowGraph>
        class VertexInserter: public AstSimpleProcessing {
        public:
            ControlFlow *analyzer;
            ControlFlowGraph &cfg;
            typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
            typedef std::map<SgAsmBlock*, Vertex> BlockVertexMap;
            BlockVertexMap &bv_map;
            VertexInserter(ControlFlow *analyzer, ControlFlowGraph &cfg, BlockVertexMap &bv_map)
                : analyzer(analyzer), cfg(cfg), bv_map(bv_map)
                {}
            // Add basic block to graph if it hasn't been added already.
            void conditionally_add_vertex(SgAsmBlock *block) {
                if (block && block->has_instructions() && !analyzer->is_vertex_filtered(block) &&
                    bv_map.find(block)==bv_map.end()) {
                    Vertex vertex = add_vertex(cfg);
                    bv_map[block] = vertex;
                    put(boost::vertex_name, cfg, vertex, block);
                }
            }
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
         *  known successors, but at least one unknown successor. */
        template<class ControlFlowGraph>
        std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
        return_blocks(const ControlFlowGraph &cfg,
                      typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);
    };
}





/******************************************************************************************************************************
 *                                      Function template definitions
 ******************************************************************************************************************************/

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::apply_to_ast(const ControlFlowGraph &cfg)
{
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
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
        for (boost::tie(ei, ei_end)=out_edges(*vi, cfg); ei!=ei_end; ++ei) {
            SgAsmBlock *target_block = get(boost::vertex_name, cfg, target(*ei, cfg));
            if (target_block && !is_edge_filtered(block, target_block)) {
                SgAsmIntegerValueExpression *target = new SgAsmIntegerValueExpression(target_block->get_address());
                target->make_relative_to(target_block);
                target->set_parent(block);
                block->get_successors().push_back(target);
            }
        }
    }
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::cache_vertex_descriptors(const ControlFlowGraph &cfg)
{
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        if (block && !is_vertex_filtered(block))
            block->set_cached_vertex(*vi);
    }
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::build_cfg_from_ast(SgNode *root, ControlFlowGraph &cfg)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
    typedef std::map<SgAsmBlock*, Vertex> BlockVertexMap;
    BlockVertexMap bv_map;

    cfg.clear();
    VertexInserter<ControlFlowGraph>(this, cfg, bv_map).traverse(root, preorder);

    /* Add the edges. */
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *source = boost::get(boost::vertex_name, cfg, *vi);
        const SgAsmIntegerValuePtrList &succs = source->get_successors();
        for (SgAsmIntegerValuePtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
            SgAsmBlock *target = isSgAsmBlock((*si)->get_base_node()); // might be null
            if (target && !is_edge_filtered(source, target)) {
                typename BlockVertexMap::iterator bvmi=bv_map.find(target);
                if (bvmi!=bv_map.end())
                    add_edge(*vi, bvmi->second, cfg);
            }
        }
    }
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::build_cg_from_ast(SgNode *root, ControlFlowGraph &cfg/*out*/)
{
    struct T1: public EdgeFilter {
        EdgeFilter *parent;
        T1(EdgeFilter *parent): parent(parent) {}
        bool operator()(ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
            SgAsmFunction *func = dst ? dst->get_enclosing_function() : NULL;
            if (!func || dst!=func->get_entry_block())
                return false;
            if (parent)
                return (*parent)(analyzer, src, dst);
            return true;
        }
    };

    EdgeFilter *parent = get_edge_filter();
    T1 edge_filter(parent);
    try {
        set_edge_filter(&edge_filter);
        build_cfg_from_ast(root, cfg);
    } catch (...) {
        set_edge_filter(parent);
        throw;
    }
    set_edge_filter(parent);
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::copy(const ControlFlowGraph &src, ControlFlowGraph &dst/*out*/)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
    Vertex NO_VERTEX = boost::graph_traits<ControlFlowGraph>::null_vertex();

    dst.clear();
    std::vector<Vertex> src_to_dst(num_vertices(src), NO_VERTEX);

    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(src); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, src, *vi);
        if (!is_vertex_filtered(block)) {
            src_to_dst[*vi] = add_vertex(dst);
            put(boost::vertex_name, dst, src_to_dst[*vi], block);
        }
    }

    typename boost::graph_traits<ControlFlowGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(src); ei!=ei_end; ++ei) {
        if (NO_VERTEX!=src_to_dst[source(*ei, src)] && NO_VERTEX!=src_to_dst[target(*ei, src)]) {
            SgAsmBlock *block1 = get(boost::vertex_name, src, source(*ei, src));
            SgAsmBlock *block2 = get(boost::vertex_name, src, target(*ei, src));
            if (!is_edge_filtered(block1, block2))
                add_edge(src_to_dst[source(*ei, src)], src_to_dst[target(*ei, src)], dst);
        }
    }
}

template<class ControlFlowGraph>
ControlFlowGraph
BinaryAnalysis::ControlFlow::copy(const ControlFlowGraph &src)
{
    ControlFlowGraph dst;
    copy(src, dst);
    return dst;
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::FlowOrder<ControlFlowGraph>::compute(const ControlFlowGraph &g, Vertex v0,
                                                                  ReverseVertexList *reverse_order) {
    forward_order->clear();
    std::vector<boost::default_color_type> colors(num_vertices(g), boost::white_color);
    depth_first_visit(g, v0, *this, &(colors[0]));
    assert(!forward_order->empty()); /* it should at least contain v0 */
    std::reverse(forward_order->begin(), forward_order->end());
    if (reverse_order) {
        reverse_order->clear();
        reverse_order->resize(num_vertices(g), (size_t)(-1));
        for (size_t i=0; i<forward_order->size(); i++)
            (*reverse_order)[(*forward_order)[i]] = i;
    }
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::FlowOrder<ControlFlowGraph>::finish_vertex(Vertex v, ControlFlowGraph g) {
    forward_order->push_back(v);
}

template<class ControlFlowGraph>
std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
BinaryAnalysis::ControlFlow::flow_order(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                        std::vector<size_t> *reverse_order/*=NULL*/)
{
    std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor> forward_order;
    FlowOrder<ControlFlowGraph>(&forward_order).compute(cfg, start, reverse_order);
    return forward_order;
}

template<class ControlFlowGraph>
void
BinaryAnalysis::ControlFlow::ReturnBlocks<ControlFlowGraph>::finish_vertex(Vertex v, ControlFlowGraph g)
{
    typename boost::graph_traits<ControlFlowGraph>::out_edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = out_edges(v, g);
    if (ei==ei_end)
        blocks.push_back(v);
}
    
template<class ControlFlowGraph>
std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor>
BinaryAnalysis::ControlFlow::return_blocks(const ControlFlowGraph &cfg,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    typename ReturnBlocks<ControlFlowGraph>::Vector result;
    ReturnBlocks<ControlFlowGraph> visitor(result);
    std::vector<boost::default_color_type> colors(num_vertices(cfg), boost::white_color);
    depth_first_visit(cfg, start, visitor, &(colors[0]));
    return result;
}

template<class ControlFlowGraph>
ControlFlowGraph
BinaryAnalysis::ControlFlow::build_cfg_from_ast(SgNode *root)
{
    ControlFlowGraph cfg;
    build_cfg_from_ast(root, cfg);
    return cfg;
}

template<class ControlFlowGraph>
ControlFlowGraph
BinaryAnalysis::ControlFlow::build_cg_from_ast(SgNode *root)
{
    ControlFlowGraph cfg;
    build_cg_from_ast(root, cfg);
    return cfg;
}

#endif
