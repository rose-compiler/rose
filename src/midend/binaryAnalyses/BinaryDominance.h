#ifndef ROSE_BinaryAnalysis_Dominance_H
#define ROSE_BinaryAnalysis_Dominance_H

#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>

namespace BinaryAnalysis {

    /** Class for calculating dominance on control flow graphs.
     *
     *  Block D "dominates" block I if every possible execution path from the function entry block to block I includes block
     *  D. Note that "dominates" is reflexive; we say D "strictly dominates" I if D dominates I and D!=I.  The "dominates"
     *  relationship is also transitive and antisymmetric.  Furthermore, block D is an "immediate dominator" of block I if D
     *  dominates I and there does not exist a block C (C!=D and C!=I) for which D dominates C and C dominates I.
     *
     *  ROSE tracks dominance via a single block pointer within each block, which can be obtained by
     *  SgAsmBlock::get_immediate_dominator().  Although these pointers can also be explicitly modifed with
     *  SgAsmBlock::set_immediate_dominator(), one normally uses the Dominance class to do this.
     *
     *  Dominance information is represented three ways:
     *  <ol>
     *    <li>Within the AST: The SgAsmBlock::get_dominators() returns a list of all dominators (or all strict dominators) for
     *        a given node by following the immediate dominator pointers stored in the AST.</li>
     *    <li>As a boost graph:  The vertices of the graph point to basic blocks (SgAsmBlock); the edges flow to subordinate
     *        blocks from their immediate dominator, and form a tree.  Note that the direction of edges is opposite that
     *        stored in the AST&mdash;this is intentional since many dominance anlyses use this direction. Methods producing
     *        or operating on this representation usually have "graph" as part of their name.</li>
     *    <li>As a vector:  Since every block has zero or one immediate dominators, the immediate dominance relation can be
     *        represented as a map from suborindate block to immediate dominator block.  The dominance relationship is
     *        calculated from a control flow graph (CFG) whose vertex descriptors are integers, so we use those integers
     *        as indices into the vector. The values stored in the vector are also CFG vertex descriptors.  Methods producing
     *        or operating on this representation usually have "map" as part of their name.</li>
     *  </ol>
     *
     *  The Dominance class provides two forms for most methods: one form in which the result is returned by reference, and one
     *  form in which the result is returned by value.  The return-by-reference functions are slightly faster since they don't
     *  require the result to be copied.
     *
     *  @section BinaryDominance_Ex1 Example
     *
     *  This example shows one way to initialize the SgAsmBlock::get_immediate_dominator() information for an entire AST.  We
     *  use an AST traversal to find the function nodes (SgAsmFunctionDeclaration), and for each function we calculate that
     *  function's control flow graph, and use the control flow graph to calculate the dominance graph.  We then clear all
     *  previous immediate dominator pointers in the function, and re-initialize them with the dominance graph.  The reason we
     *  first clear the AST is because apply_to_ast() only initializes the blocks which have dominators; if we didn't clear the
     *  AST then some of the blocks might have stale data.
     *
     *  Control flow graphs and dominance graphs are created with methods of a control flow and dominance class, respectively.
     *  The reason for this extra class is to hold various properties that might affect how the graphs are created, such as
     *  whether we want a debug trace of the creation process (the commented out line).
     *
     *  Most of the methods of this class are function templates that take any kind of boost graph providing it satisfies
     *  certain interface constraints: the vertices must be stored in a vector, the graph must be bidirectional, and the
     *  boost::vertex_name property must point to a SgAsmBlock.  This is the same interface used for control flow graphs, so
     *  the user can use BinaryAnalysis::ControlFlow::Graph, BinaryAnalysis::Dominance::Graph, or any other Boost graph
     *  satisfying these requirements.
     *
     *  @code
     *  // The AST traversal.
     *  struct CalculateDominance: public AstSimpleProcessing {
     *      BinaryAnalysis::ControlFlow &cfg_analysis;
     *      BinaryAnalysis::Dominance &dom_analysis;
     *      CalculateDominance(BinaryAnalysis::ControlFlow &cfg_analysis,
     *                         BinaryAnalysis::Dominance &dom_analysis)
     *          : cfg_analysis(cfg_analysis), dom_analysis(dom_analysis)
     *          {}
     *      void visit(SgNode *node) {
     *          using namespace BinaryAnalysis;
     *          SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
     *          if (func) {
     *              ControlFlow::Graph cfg = cfg_analysis.build_cfg_from_ast<ControlFlow::Graph>(func);
     *              ControlFlow::Vertex entry = 0; // first vertex is function entry block
     *              assert(get(boost::vertex_name, cfg, entry) == func->get_entry_block());
     *              Dominance::Graph dg = dom_analysis.build_idom_graph_from_cfg<Dominance::Graph>(cfg, entry);
     *              dom_analysis.clear_ast(func);
     *              dom_analysis.apply_to_ast(dg);
     *          }
     *      }
     *  };
     *
     *  // Create the analysis objects
     *  BinaryAnalysis::ControlFlow cfg_analysis;
     *  BinaryAnalysis::Dominance   dom_analysis;
     *  //dom_analysis.set_debug(stderr);
     *
     *  // Perform the analysis; results are stored in the AST.
     *  CalculateDominance(cfg_analysis, dom_analysis).traverse(project, preorder);
     *  @endcode
     */
    class Dominance {
    public:
        Dominance(): debug(NULL) {}

        /** The default dominance graph type.
         *
         *  A dominance graph (DG) is a Boost graph whose vertex descriptors are integers and whose vertices point to
         *  SgAsmBlock nodes in the AST (via the boost::vertex_name property).  Graph edges represent dominance relationships
         *  between the vertices.  The particular relationship varies depending on the method by which the graph was built. For
         *  example, build_idom_graph() results in a graph whose edge (U,V) indicates that U is the immediate dominator of V,
         *  while build_sdom_graph() results in a graph whose edge (U,V) indicates that U is any strict dominator of V.
         *
         *  Note that the edges of the graph are reversed from those stored in the AST.  In the AST each block points to its
         *  immediate dominator, while in the graph each block has edges going to the blocks which it dominates.  We do it this
         *  way because:
         *  <ul>
         *    <li>it's more succinct to store only the immediate dominator pointers in the AST, and</li>
         *    <li>most dominance graph algorithms use the edges in the direction we've made them, and</li>
         *    <li>it's easy to reverse the graph via the boost::reverse_graph if necessary, and</li>
         *    <li>having two directions natively stored gives more flexibility to algorithm writers.</li>
         *  </ul>
         *
         *  When a dominance graph is built from a control flow graph, the vertices of the dominance graph will correspond with
         *  the vertices of the control flow graph.  That is, vertex V in the dominance graph points to the same basic block as
         *  vertex V in the control flow graph.
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
        typedef boost::adjacency_list<boost::listS,     /* edge storage */
                                      boost::vecS,      /* vertex storage */
                                      boost::bidirectionalS,
                                      boost::property<boost::vertex_name_t, SgAsmBlock*>
                                     > Graph;

        /** Vector representation of the dominance relation.  The vector is indexed by the CFG vertex descriptor. For each CFG
         *  vertex, idx, the value stored in the vector at index idx is the CFG vertex which is the dominator of vertex idx.
         *  If vertex idx has no dominator then the value at index idx will be the null node.
         *
         *  @code
         *  RelationMap<ControlFlow::Graph> idoms;
         *  for (size_t idx=0; idx<idoms.size(); ++idx) {
         *      if (idoms[idx]!=boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
         *          std::cout <<idoms[idx] <<" is the immediate dominator of " <<idx <<std::endl;
         *      }
         *  }
         *  @endcode
         */
        template<class ControlFlowGraph>
        struct RelationMap: public std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor> {
        };



        /**********************************************************************************************************************
         *                                      Methods that operate on the AST
         **********************************************************************************************************************/
    public:

        /** Clears immediate dominator pointers in a subtree.
         *
         *  Traverses the specified AST and clears the immediate dominator pointer in each SgAsmNode in the subtree.  The
         *  traversal follows the tree, not the immediate dominator pointers.  Whether the algorithm follows the AST pointers
         *  or the dominance pointers during traversal is irrelevant when dominance has been calculated over a single function
         *  and specified AST to clear is that function since all immediate dominator pointers will point to other blocks
         *  within the same function.  But it can make a difference in other use cases. */
        void clear_ast(SgNode *ast);


        /** Applies dominator information to the AST.
         *
         *  Just as the dominance graph can be built from the immediate dominator pointers in the AST, a immediate dominance
         *  graph can be used to initialize the pointers in the AST.  The AST can be initialized either from a dominance graph
         *  or the dominance relation map.
         *
         *  Blocks that have an immediate dominator will have their dominator pointer updated (see
         *  SgAsmBlock::get_immediate_dominator()), but the pointer will not be set to null if the block has no
         *  dominator. Therefore, it is probably wise to call clear_ast() before apply_to_ast().  The reason this was designed
         *  like this is because it makes it possible to update just the subtree (e.g., SgAsmFunctionDeclaration) over which
         *  dominance was computed even if the control flow graph covers more of the AST.
         *
         *  @{ */
        template<class DominanceGraph>
        void apply_to_ast(const DominanceGraph &idg);

        template<class ControlFlowGraph>
        void apply_to_ast(const ControlFlowGraph &cfg, const RelationMap<ControlFlowGraph> &relation_map);
        /** @} */


        /** Cache vertex descriptors in AST.
         *
         *  The vertices of a dominance graph are of type Vertex, and point at the basic blocks (SgAsmBlock) of the
         *  AST. Although most graph algorithms will only need to map Vertex to SgAsmBlock, the inverse mapping is also
         *  sometimes useful.  That mapping can be stored into an std::map via graph traversal, or stored in the AST itself
         *  attached to each SgAsmBlock.  Using an std::map requires an O(log N) lookup each time we need to get the vertex
         *  descriptor from a block, while storing the vertex descriptor in the AST requires O(1) lookup time.
         *
         *  The vertex descriptors are available via SgAsmBlock::get_cached_vertex().  Other graph types (e.g., control flow
         *  graphs) might also use the same cache line.  The cached vertex is stored as a size_t, which is the same underlying
         *  type for dominance graph vertices. */
        template<class DominanceGraph>
        void cache_vertex_descriptors(const DominanceGraph&);


        /** Checks that dominance relationships are consistent in an AST.
         *
         *  This method traverses the given AST to ensure that dominance relationships are consistent.  It checks the following
         *  conditions:
         *
         *  <ul>
         *    <li>The entry block of a function has no immediate dominator.</li>
         *    <li>All non-entry blocks of a function have an immediate dominator.</li>
         *    <li>If block D is the immediate dominator of block I, then D and I belong to the same function.</li>
         *  </ul>
         *
         *  Specifically, this method does not check for wild immediate dominator pointers, such a pointers to blocks that have
         *  been deleted.
         *
         *  Returns true if no problems were detected, false otherwise.  If the user supplies a pointer to a bad-blocks set,
         *  then blocks where problems were detected will be added to the set.  The set is not cleared by this method. */
        bool is_consistent(SgNode *ast, std::set<SgAsmBlock*> *bad_blocks=NULL);



        /**********************************************************************************************************************
         *                                      Methods that build relationships
         **********************************************************************************************************************/
    public:

        /** Builds a relation map for immediate dominator.
         *
         *  Given a control flow graph (CFG) and a starting vertex within that graph, calculate the immediate dominator of each
         *  vertex.  The relationship is returned through the @p idom argument, which is a vector indexed by CFG vertices
         *  (which are just integers), and where each element is the CFG vertex which is the immediate dominator.  For vertices
         *  that have no immediate dominator (e.g., function entry blocks or blocks that are not connected to the entry block
         *  in the CFG), the stored value is the null vertex.  See RelationMap for details.
         *
         *  This method is intended to be the lowest level implementation for finding dominators; all other methods are built
         *  upon this one.  This method uses an algorithm based on "A Simple, Fast Dominance Algorithm" by Keith
         *  D. Cooper, Timothy J. Harvey, and Ken Kennedy at Rice University, Houston, Texas.  It has been extended in various
         *  ways to simplify it and make it slightly faster.  Although run-time complexity is higher than the Lengauer-Tarjan
         *  algorithm, the algorithm is much simpler, and for CFGs typically encountered in practice is faster than
         *  Lengauer-Tarjan.
         *
         *  @{ */
        template<class ControlFlowGraph>
        RelationMap<ControlFlowGraph>
        build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                     typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);

        template<class ControlFlowGraph>
        void build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                          typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                          RelationMap<ControlFlowGraph> &idom/*out*/);
        /** @} */


        /** Builds a relation map for immediate post dominator.
         *
         *  Given a control flow graph (CFG) and a starting vertex within that graph, calculate the immediate post dominator of
         *  each vertex.  The relationship is returned through the @p pdom argument, which is a vector indexed by CFG vertices
         *  (which are just integers), and where each element is the CFG vertex which is the immediate post dominator.  For
         *  vertices that have no immediate post dominator (e.g., function exit blocks or blocks that eventually exit the
         *  function through multiple exit blocks), the stored value is the null vertex.  See RelationMap for details.
         *
         *  This method finds post dominators by first locating the function exit blocks via ControlFlow::return_blocks().  If
         *  the exit block is not unique, then a temporary exit vertex is added to the CFG and the original exit blocks are
         *  given control flow edges to this unique vertex.  The CFG is then reversed and build_idom_relation_from_cfg() is
         *  called.  Finally, if a temporary unique exit vertex was added, it is removed from the result.
         *
         *  @{ */
        template<class ControlFlowGraph>
        RelationMap<ControlFlowGraph>
        build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);

        template<class ControlFlowGraph>
        void build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                             typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                             RelationMap<ControlFlowGraph> &idom/*out*/);
        /** @} */



        /**********************************************************************************************************************
         *                                      Methods that build graphs
         **********************************************************************************************************************/
    public:

        /** Builds a dominance graph from a control flow graph.
         *
         *  This method traverses a given control flow graph (CFG) to build a graph where each vertex corresponds to a vertex
         *  in the CFG, and each edge, (U,V), represents the fact that U is the immediate dominator of V.  This algorithm does
         *  not use information stored in the SgAsmBlock nodes of the AST (specifically not the immediate dominator pointers)
         *  and does not modify the AST.
         *
         *  See class documentation for a description of how dominance graph vertices correspond to CFG vertices.
         *
         *  @{ */
        template<class DominanceGraph, class ControlFlowGraph>
        DominanceGraph build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                 typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);

        template<class ControlFlowGraph, class DominanceGraph>
        void build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                       typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                       DominanceGraph &dg/*out*/);
        /** @} */


        /** Build a post dominator graph.
         *
         *  The post dominator relation is like the immediate dominator relation except instead of considering the control flow
         *  path from the entry block to the block in question, we consider the path from the block in question to the exit
         *  block.  If every path from the block in question, B, to the exit block passes through vertex D, then D is is a post
         *  dominator of B.  Immediate post dominance and strict post dominance are analogous to the (pre) dominance
         *  definitions presented above.
         *
         *  ROSE does not create a unique basic block to serve as a function exit vertex of the CFG (such a block would be
         *  empty of instructions and would have no virtual address, making it a bit problematic.  Furthermore, the function
         *  may contain basic blocks whose successors are statically unknown (e.g., computed branches).  Therefore, this method
         *  temporarily modifies the CFG (specifically, a copy thereof), if necessary, by adding a unique exit vertex and
         *  adjusting all vertices identified by BinaryAnalysis::ControlFlow::return_blocks() so they point to the unique exit
         *  vertex.  If the exit vertex was added, it will be removed (along with its edges) from the returned post dominator
         *  graph.
         *
         *  @{ */
        template<class DominanceGraph, class ControlFlowGraph>
        DominanceGraph build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start);

        template<class ControlFlowGraph, class DominanceGraph>
        void build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                          typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                          DominanceGraph &pdg/*out*/);
        /** @} */



        /**********************************************************************************************************************
         *                                      Miscellaneous methods
         **********************************************************************************************************************/
    public:

        /** Builds a dominance graph from a relation map.
         *
         *  Given a control flow graph (CFG) and a dominance relation map, build a dominance graph where each vertex of the
         *  resulting dominance graph points to a basic block (SgAsmBlock) represented in the CFG, and each edge, (U,V),
         *  represents the fact that U is the immediate dominator of V.
         *
         *  See class documentation for a description of how dominance graph vertices correspond to CFG vertices.
         *
         *  @{ */
        template<class DominanceGraph, class ControlFlowGraph>
        DominanceGraph build_graph_from_relation(const ControlFlowGraph &cfg,
                                                 const RelationMap<ControlFlowGraph> &relmap);

        template<class ControlFlowGraph, class DominanceGraph>
        void build_graph_from_relation(const ControlFlowGraph &cfg,
                                       const RelationMap<ControlFlowGraph> &relmap,
                                       DominanceGraph &dg/*out*/);
        /** @} */


        /** Control debugging output.
         *
         *  If set to a non-null value, then some of the algorithms will produce debug traces on the specified file. */
        void set_debug(FILE *debug) { this->debug = debug; }

        /** Obtain debugging stream.
         *
         *  Returns the current debug stream that was last set by set_debug().  A null value indicates that debugging is
         *  disabled. */
        FILE *get_debug() const { return debug; }

    protected:
        FILE *debug;                    /**< Debugging stream, or null. */
    };
}



/******************************************************************************************************************************
 *                              Function templates for methods that operate on the AST
 ******************************************************************************************************************************/

template<class DominanceGraph>
void
BinaryAnalysis::Dominance::apply_to_ast(const DominanceGraph &idg)
{
    if (debug)
        fprintf(debug, "BinaryAnalysis::Dominance::apply_to_ast:\n");

    typename boost::graph_traits<DominanceGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(idg); ei!=ei_end; ++ei) {
        SgAsmBlock *dom_block = get(boost::vertex_name, idg, source(*ei, idg));
        SgAsmBlock *sub_block = get(boost::vertex_name, idg, target(*ei, idg));
        if (debug) {
            fprintf(debug, "  edge (d,s) = (%zu,%zu) = (0x%08"PRIx64", 0x%08"PRIx64")\n",
                    source(*ei, idg), target(*ei, idg), dom_block->get_address(), sub_block->get_address());
        }
        sub_block->set_immediate_dominator(dom_block);
    }
}

template<class ControlFlowGraph>
void
BinaryAnalysis::Dominance::apply_to_ast(const ControlFlowGraph &cfg,
                                        const RelationMap<ControlFlowGraph> &idom)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;

    assert(idom.size()<=num_vertices(cfg));
    for (size_t subordinate=0; subordinate<idom.size(); subordinate++) {
        SgAsmBlock *sub_block = get(boost::vertex_name, cfg, (CFG_Vertex)subordinate);
        if (sub_block && idom[subordinate]!=boost::graph_traits<ControlFlowGraph>::null_vertex()) {
            CFG_Vertex dominator = idom[subordinate];
            SgAsmBlock *dom_block = get(boost::vertex_name, cfg, dominator);
            sub_block->set_immediate_dominator(dom_block);
        }
    }
}

template<class DominanceGraph>
void
BinaryAnalysis::Dominance::cache_vertex_descriptors(const DominanceGraph &dg)
{
    typename boost::graph_traits<DominanceGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(dg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, dg, *vi);
        if (block)
            block->set_cached_vertex(*vi);
    }
}

/******************************************************************************************************************************
 *                              Function templates for immediate dominators
 ******************************************************************************************************************************/


template<class ControlFlowGraph, class DominanceGraph>
void
BinaryAnalysis::Dominance::build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                     typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                                     DominanceGraph &result)
{
    RelationMap<ControlFlowGraph> idoms;
    build_idom_relation_from_cfg(cfg, start, idoms);
    build_graph_from_relation(cfg, idoms, result);
}

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
BinaryAnalysis::Dominance::build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                     typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    DominanceGraph dg;
    build_idom_graph_from_cfg(cfg, start, dg);
    return dg;
}

template<class ControlFlowGraph>
BinaryAnalysis::Dominance::RelationMap<ControlFlowGraph>
BinaryAnalysis::Dominance::build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    RelationMap<ControlFlowGraph> idom;
    build_idom_relation_from_cfg(cfg, start, idom);
    return idom;
}

/* Loosely based on an algorithm from Rice University known to be O(n^2) where n is the number of vertices in the control flow
 * subgraph connected to the start vertex.  According to the Rice paper, their algorithm outperforms Lengauer-Tarjan on
 * typicall control flow graphs even though asymptotically, Lengauer-Tarjan is better.  The Rice algorithm is also much
 * simpler, as evidenced below.
 *
 * I've added a few minor optimizations:
 *   (1) reverse post-order dfs is calculated once rather than each time through the loop.  Rice's analysis indicates that
 *       they also made this optimization, although their listed algorithm does not show it.
 *   (2) the first processed predecessor of the vertex under consideration is determined in the same loop that processes
 *       the other predecessors, while in the listed algorithm this was a separate operation.
 *   (3) self loops in the control flow graph are not processed, since they don't contribute to the dominance relation.
 *   (4) undefined state for idom(x) is represented by idom(x)==x.
 *   (5) nodes are labeled in reverse order from Rice, but traversed in the same order.  This simplifies the code a bit
 *       because the vertices are traversed according to the "flowlist" vector, and the index into the "flowlist" vector
 *       can serve as the node label.
 *
 * The set of dominators of vertex v, namely dom(v), is represented as a linked list stored as an array indexed by vertex
 * number. That is
 *      dom(v) = { v, idom(v), idom(idom(v)), ..., start }
 *
 * is stored in the idom array as:
 *
 *      dom(v) = { v, idom[v], idom[idom[v]], ..., start }
 *
 * This representation, combined with the fact that:
 *
 *      a ELEMENT_OF dom(v) implies dom(a) SUBSET_OF dom(v)
 *
 * allows us to perform intersection by simply walking the two sorted lists until we find an element in common, and including
 * that element an all subsequent elements in the intersection result.  The idom array uses the flow-list vertex numbering
 * produced by a post-order visitor of a depth-first search, and the nodes are processed from highest to lowest.
 */
template<class ControlFlowGraph>
void
BinaryAnalysis::Dominance::build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                                        RelationMap<ControlFlowGraph> &result)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;

    struct debug_dom_set {
        debug_dom_set(FILE *debug, size_t vertex_i, size_t idom_i,
                      const std::vector<size_t> &domsets, const std::vector<CFG_Vertex> &flowlist) {
            if (debug) {
                fprintf(debug, "{ #%zu(%zu)", vertex_i, flowlist[vertex_i]);
                for (size_t d=idom_i; d!=vertex_i; vertex_i=d, d=domsets[d])
                    fprintf(debug, " #%zu(%zu)", d, flowlist[d]);
                fprintf(debug, " }");
            }
        }
    };

    if (debug) {
        fprintf(debug, "BinaryAnalysis::Dominance::build_idom_relation_from_cfg: starting at vertex %zu\n", start);
        SgAsmBlock *block = get(boost::vertex_name, cfg, start);
        SgAsmFunctionDeclaration *func = block ? block->get_enclosing_function() : NULL;
        if (func) {
            fprintf(debug, "  Vertex %zu is %s block of", start, func->get_entry_block()==block?"the entry":"a");
            if (func->get_name().empty()) {
                fprintf(debug, " an unnamed function");
            } else {
                fprintf(debug, " function <%s>", func->get_name().c_str());
            }
            fprintf(debug, " at 0x%08"PRIx64"\n", func->get_entry_va());
        }
    }

    /* Initialize */
    std::vector<size_t> rflowlist; /* reverse mapping; flowlist[i]==v implies rflowlist[v]==i */
    std::vector<CFG_Vertex> flowlist = ControlFlow().flow_order(cfg, start, &rflowlist);
    std::vector<size_t> idom(flowlist.size());
    for (size_t i=0; i<flowlist.size(); i++)
        idom[i] = i; /* idom[i]==i implies idom[i] is unknown */

    if (debug) {
        fprintf(debug, "  CFG:\n");
        typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
            SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
            fprintf(debug, "    %zu 0x%08"PRIx64" --> {", (size_t)(*vi), block?block->get_address():0);
            typename boost::graph_traits<ControlFlowGraph>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(*vi, cfg); ei!=ei_end; ++ei) {
                fprintf(debug, " %zu", (size_t)target(*ei, cfg));
            }
            fprintf(debug, " }\n");
        }

        fprintf(debug, "  Note: notation #M(N) means CFG vertex N at position M in the flow list.\n");
        fprintf(debug, "  Flowlist: {");
        for (size_t i=0; i<flowlist.size(); i++) {
            fprintf(debug, " #%zu(%zu)", i, (size_t)flowlist[i]);
            assert((size_t)flowlist[i]<rflowlist.size());
            assert(rflowlist[flowlist[i]]==i);
        }
        fprintf(debug, " }\n");
    }

    /* Iterative data flow */
    bool changed;
    do {
        changed = false;
        if (debug)
            fprintf(debug, "  Next pass through vertices...\n");
        for (size_t vertex_i=0; vertex_i<flowlist.size(); vertex_i++) {
            CFG_Vertex vertex = flowlist[vertex_i];
            if (debug) {
                fprintf(debug, "    vertex #%zu(%zu)", (size_t)vertex_i, (size_t)vertex);
                if (vertex==start) {
                    fprintf(debug, " [skipping start vertex]\n");
                } else {
                    fprintf(debug, " dominators are ");
                    debug_dom_set(debug, vertex_i, idom[vertex_i], idom, flowlist);
                    fprintf(debug, "\n");
                }
            }

            if (vertex!=start) {
                typename boost::graph_traits<ControlFlowGraph>::in_edge_iterator pi, pi_end; /*predecessors*/
                size_t new_idom = vertex_i; /*undefined for now*/
                for (boost::tie(pi, pi_end)=in_edges(vertex, cfg); pi!=pi_end; ++pi) {
                    CFG_Vertex predecessor = source(*pi, cfg);
                    assert(predecessor>=0 && predecessor<rflowlist.size());
                    size_t predecessor_i = rflowlist[predecessor];
                    if (debug)
                        fprintf(debug, "      pred #%zd(%zu)", (size_t)predecessor_i, (size_t)predecessor);

                    /* It's possible that the predecessor lies outside the part of the CFG connected to the entry node. We
                     * should not consider those predecessors. */
                    if (predecessor!=vertex && predecessor_i!=boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                        if (predecessor==start) {
                            new_idom = predecessor_i;
                            if (debug) {
                                fprintf(debug, "; new doms of #%zu(%zu) are ", vertex_i, vertex);
                                debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                            }
                        } else if (idom[predecessor_i]!=predecessor_i) {
                            if (new_idom==vertex_i) {
                                new_idom = predecessor_i;
                                if (debug) {
                                    fprintf(debug, "; new doms of #%zu(%zu) are ", vertex_i, vertex);
                                    debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                                }
                            } else {
                                if (debug) {
                                    fprintf(debug, "; new doms of #%zu(%zu) are intersect(", vertex_i, vertex);
                                    debug_dom_set(debug, vertex_i, new_idom, idom, flowlist);
                                    fprintf(debug, ", ");
                                    debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                                }
                                size_t f1=new_idom, f2=predecessor_i;
                                while (f1!=f2) {
                                    while (f1 > f2)
                                        f1 = idom[f1];
                                    while (f2 > f1)
                                        f2 = idom[f2];
                                }
                                new_idom = f1;
                                if (debug) {
                                    fprintf(debug, ") = ");
                                    debug_dom_set(debug, vertex_i, new_idom, idom, flowlist);
                                }
                            }
                        }
                    }
                    if (debug)
                        fprintf(debug, "\n");
                }
                if (idom[vertex_i]!=new_idom) {
                    idom[vertex_i] = new_idom;
                    changed = true;
                }
            }
        }
    } while (changed);

    /* Build result relation */
    result.clear();
    result.resize(num_vertices(cfg), boost::graph_traits<ControlFlowGraph>::null_vertex());
    for (size_t i=0; i<flowlist.size(); i++) {
        if (idom[i]!=i)
            result[flowlist[i]] = flowlist[idom[i]];
    }

    if (debug) {
        fprintf(debug, "  Final dom sets:\n");
        for (size_t vertex_i=0; vertex_i<flowlist.size(); vertex_i++) {
            CFG_Vertex vertex = flowlist[vertex_i];
            fprintf(debug, "    #%zu(%zu) has dominators ", (size_t)vertex_i, (size_t)vertex);
            debug_dom_set(debug, vertex_i, idom[vertex_i], idom, flowlist);
            fprintf(debug, "\n");
        }
        fprintf(debug, "  Final result:\n");
        for (size_t i=0; i<result.size(); i++) {
            if (result[i]==boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %zu has no immediate dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %zu has immediate dominator %zu\n", i, result[i]);
            }
        }
    }
}





/******************************************************************************************************************************
 *                              Function templates for post dominators
 ******************************************************************************************************************************/

template<class ControlFlowGraph, class DominanceGraph>
void
BinaryAnalysis::Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                                        DominanceGraph &result)
{
    RelationMap<ControlFlowGraph> pdoms;
    build_postdom_relation_from_cfg(cfg, start, pdoms);
    build_graph_from_relation(cfg, pdoms, result);
}

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
BinaryAnalysis::Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    DominanceGraph dg;
    build_postdom_graph_from_cfg(cfg, start, dg);
    return dg;
}

template<class ControlFlowGraph>
BinaryAnalysis::Dominance::RelationMap<ControlFlowGraph>
BinaryAnalysis::Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    RelationMap<ControlFlowGraph> pdom;
    build_postdom_relation_from_cfg(cfg, start, pdom);
    return pdom;
}

template<class ControlFlowGraph>
void
BinaryAnalysis::Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &_cfg,
                                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                                           RelationMap<ControlFlowGraph> &result)
{
    ControlFlowGraph cfg = _cfg;        /* we need our own copy since we might modify it */

    if (debug) {
        fprintf(debug, "BinaryAnalysis::Dominance::build_postdom_relation_from_cfg: starting at vertex %zu\n", start);
        SgAsmBlock *block = get(boost::vertex_name, cfg, start);
        SgAsmFunctionDeclaration *func = block ? block->get_enclosing_function() : NULL;
        if (func) {
            fprintf(debug, "  Vertex %zu is %s block of", start, func->get_entry_block()==block?"the entry":"a");
            if (func->get_name().empty()) {
                fprintf(debug, " an unnamed function");
            } else {
                fprintf(debug, " function <%s>", func->get_name().c_str());
            }
            fprintf(debug, " at 0x%08"PRIx64"\n", func->get_entry_va());
        }
    }

    /* Does the graph have more than one return block?  By "return", we mean any block whose control flow successor is possibly
     * outside the start node's function.  See ControlFlow::return_blocks(). */
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;
    CFG_Vertex unique_exit;
    bool unique_exit_created = false;
    std::vector<CFG_Vertex> retblocks = ControlFlow().return_blocks(cfg, start);
    if (1==retblocks.size()) {
        if (debug)
            fprintf(debug, "  CFG has unique exit vertex %zu, block 0x%08"PRIx64"\n",
                    retblocks[0],
                    get(boost::vertex_name, cfg, retblocks[0])->get_address());
        unique_exit = retblocks[0];
    } else {
        assert(!retblocks.empty());
        unique_exit = add_vertex(cfg);
        unique_exit_created = true;
        put(boost::vertex_name, cfg, unique_exit, (SgAsmBlock*)0); /* vertex has no basic block */
        for (size_t i=0; i<retblocks.size(); i++)
            add_edge(retblocks[i], unique_exit, cfg);
        if (debug)
            fprintf(debug, "  CFG has %zu exit blocks. Added unique exit vertex %zu\n", retblocks.size(), unique_exit);
    }

    /* Post dominance is the same as doing dominance, but on a reversed CFG, using the unique return vertex as the starting
     * point. */
    if (debug)
        fprintf(debug, "  Calling build_idom_relation_from_cfg() on reversed CFG...\n");
    typedef typename boost::reverse_graph<ControlFlowGraph> ReversedControlFlowGraph;
    ReversedControlFlowGraph rcfg(cfg);
    RelationMap<ReversedControlFlowGraph> rrelation;
    build_idom_relation_from_cfg(rcfg, unique_exit, rrelation);
    if (debug)
        fprintf(debug, "BinaryAnalysis::Dominance::build_postdom_relation_from_cfg() resuming...\n");

    /* Remove the unique exit vertex if appropriate. */
    if (unique_exit_created) {
        assert(unique_exit+1==num_vertices(cfg));
        rrelation.pop_back();
        for (size_t i=0; i<rrelation.size(); ++i) {
            if (rrelation[i]>=rrelation.size())
                rrelation[i] = boost::graph_traits<ControlFlowGraph>::null_vertex();
        }
    }

    /* Intiailize the result vector. */
    result.assign(rrelation.begin(), rrelation.end());
    if (debug) {
        fprintf(debug, "  Final result:\n");
        for (size_t i=0; i<result.size(); i++) {
            if (result[i]==boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %zu has no immediate post dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %zu has immediate post dominator %zu\n", i, result[i]);
            }
        }
    }
}




/******************************************************************************************************************************
 *                              Function templates for miscellaneous methods
 ******************************************************************************************************************************/

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
BinaryAnalysis::Dominance::build_graph_from_relation(const ControlFlowGraph &cfg,
                                                     const RelationMap<ControlFlowGraph> &relmap)
{
    DominanceGraph g;
    build_graph_from_relation(cfg, relmap, g);
    return g;
}

template<class ControlFlowGraph, class DominanceGraph>
void
BinaryAnalysis::Dominance::build_graph_from_relation(const ControlFlowGraph &cfg,
                                                     const RelationMap<ControlFlowGraph> &relmap,
                                                     DominanceGraph &dg/*out*/)
{
    typedef typename boost::graph_traits<DominanceGraph>::vertex_descriptor D_Vertex;
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;

    if (debug) {
        fprintf(debug, "BinaryAnalysis::Dominance::build_graph_from_relation:\n");
        fprintf(debug, "  building from this relation:\n");
        for (size_t i=0; i<relmap.size(); i++) {
            if (relmap[i]==boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %zu has no immediate dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %zu has immediate dominator %zu\n", i, relmap[i]);
            }
        }
    }

    dg.clear();
    typename boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; vi++) {
        D_Vertex v = add_vertex(dg);
        assert(v==*vi); /* because relmap[] refers to CFG vertices; otherwise we need to map them */
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        put(boost::vertex_name, dg, v, block);
    }
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; vi++) {
        CFG_Vertex subordinate = *vi;
        CFG_Vertex dominator = relmap[subordinate];
        if (dominator!=boost::graph_traits<ControlFlowGraph>::null_vertex()) {
            if (debug)
                fprintf(debug, "  adding edge (d,s) = (%zu,%zu)\n", dominator, subordinate);
            add_edge(dominator, subordinate, dg);
        }
    }
}

#endif
