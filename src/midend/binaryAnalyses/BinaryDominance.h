#ifndef ROSE_BinaryAnalysis_Dominance_H
#define ROSE_BinaryAnalysis_Dominance_H

#include "BinaryControlFlow.h"

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
     */
    class Dominance {
    public:
        Dominance(): debug(NULL) {}

        /** A dominance graph.
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
         */
        typedef boost::adjacency_list<boost::listS,     /* edge storage */
                                      boost::vecS,      /* vertex storage */
                                      boost::bidirectionalS,
                                      boost::property<boost::vertex_name_t, SgAsmBlock*>
                                     > Graph;

        /** Type of graph vertex.
         *
         *  The graph contains vertex descriptors of this type, with an associate internal property map that points to the
         *  SgAsmBlock objects in the AST. */
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

        /** Vector representation of the dominance relation.  The vector is indexed by the CFG vertex descriptor. For each CFG
         *  vertex, idx, the value stored in the vector at index idx is the CFG vertex which is the dominator of vertex idx.
         *  If vertex idx has no dominator then the value at index idx will be the null node.
         *
         *  @code
         *  RelationMap idoms;
         *  for (size_t idx=0; idx<idoms.size(); ++idx) {
         *      if (idoms[idx]!=boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
         *          std::cout <<idoms[idx] <<" is the immediate dominator of " <<idx <<std::endl;
         *      }
         *  }
         *  @endcode
         */
        typedef std::vector<ControlFlow::Vertex> RelationMap;

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
        void apply_to_ast(const Graph &idg);
        void apply_to_ast(const ControlFlow::Graph &cfg, const RelationMap&);
        /** @} */

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
        Graph build_idom_graph(const ControlFlow::Graph &cfg, ControlFlow::Vertex start);
        void build_idom_graph(const ControlFlow::Graph &cfg, ControlFlow::Vertex start, Graph &dg/*out*/);
        /** @} */

        /** Builds a dominance graph from a relation map.
         *
         *  Given a control flow graph (CFG) and a dominance relation map, build a dominance graph where each vertex of the
         *  resulting dominance graph points to a basic block (SgAsmBlock) represented in the CFG, and each edge, (U,V),
         *  represents the fact that U is the immediate dominator of V.
         *
         *  See class documentation for a description of how dominance graph vertices correspond to CFG vertices.
         *
         *  @{ */
        Graph build_idom_graph(const ControlFlow::Graph &cfg, const RelationMap &idoms);
        void build_idom_graph(const ControlFlow::Graph &cfg, const RelationMap &idoms, Graph &dg/*out*/);
        /** @} */

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
        RelationMap build_idom_vector(const ControlFlow::Graph &cfg, ControlFlow::Vertex start);
        void build_idom_vector(const ControlFlow::Graph &cfg, ControlFlow::Vertex start, RelationMap &idom/*out*/);
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


#endif
