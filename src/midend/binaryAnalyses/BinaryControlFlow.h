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
     *  various settings that influence the control flow analyses (currently none).
     *
     *  Control flow is represented in the AST as successor pointers (SgAsmTarget) attached to each basic block
     *  (SgAsmBlock). The successor information is initialized by the Partitioner class when the AST is built (see
     *  Partitioner::build_ast()) and are available with SgAsmBlock::get_successors().  The
     *  SgAsmBlock::get_successors_complete() returns a Boolean indicating whether the set of successors is completely
     *  known. Successors would be incomplete, for instance, for block that returns from a function or a block that ends with a
     *  computed branch whose targets could not be statically determined.
     *
     *  The wikipedia entry for "Control Flow Graph" [1] has many useful definitions.
     * 
     *  [1] https://secure.wikimedia.org/wikipedia/en/wiki/Control_flow_graph */
    class ControlFlow {
    public:

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

        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

        /** Clears successor information from the AST.
         *
         *  Traverses the specified AST and clears the successor lists for all blocks.  The blocks are visited by an AST
         *  traversal, not by following successor pointers. */
        void clear_ast(SgNode *ast);

        /** Applies graph to AST.
         *
         *  Just as a control flow graph can be built from the successor lists stored in the AST (see build_graph()), a graph
         *  can be used to initialize the successor information in an AST.  This function does that.  Only the blocks which are
         *  vertices of the graph are affected.  At this time [2011-05-19] the successor_complete property of each affected
         *  block is set to true, but this may change in the future. */
        void apply_to_ast(const Graph&);

        /** Builds a control flow graph for part of an AST.
         *
         *  Builds a control flow graph for the part of the abstract syntax tree rooted at @p root by traversing the AST to
         *  find all basic blocks and using the successors of those blocks to define the edges of the control flow graph.
         *  Successors are retrieved via SgAsmBlock::get_successors() and are of type SgAsmTarget.
         *
         *  The following types of successors are not added as edges in the returned control flow graph:
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
         */
        static std::vector<Vertex> flow_order(const Graph&, Vertex start, std::vector<Vertex> *reverse_order=NULL);
        
    };
}

#endif
