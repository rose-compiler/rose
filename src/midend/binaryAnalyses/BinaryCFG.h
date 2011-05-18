#ifndef ROSE_BinaryAnalysis_CFG_H
#define ROSE_BinaryAnalysis_CFG_H

#include <boost/graph/adjacency_list.hpp>

class SgNode;
class SgAsmBlock;

namespace RoseBinaryAnalysis {

    /** A control flow graph.
     *
     *  A control flow graph is simply a Boost graph whose vertex descriptors are integers and whose vertices point to
     *  SgAsmBlock nodes in the AST (via the boost::vertex_name property).  The graph edges represent flow of control from one
     *  SgAsmBlock to another.  Since the control flow graph is a Boost graph, it is endowed with all the features of a Boost
     *  graph and can be the operand of the various Boost graph algorithms.  See build_cfg() for specifics about what is
     *  included in such a graph. */
    typedef boost::adjacency_list<boost::listS,                                 /* out-edges of each vertex in std::list */
                                  boost::vecS,                                  /* store vertices in std::vector */
                                  boost::directedS,                             /* control flow is directed */
                                  boost::property<boost::vertex_name_t, SgAsmBlock*> /* each vertex points to a SgAsmBlock */
                                 > ControlFlowGraph;


    /** Build a control flow graph for part of an AST.
     *
     *  Builds a control flow graph for the part of the abstract syntax tree rooted at @p root by traversing the AST to find
     *  all basic blocks and using the successors of those blocks to define the edges of the control flow graph.  Successors
     *  are retrieved via SgAsmBlock::get_successors() and are of type SgAsmTarget.
     *
     *  The following types of successors are not added as edges in the returned control flow graph:
     *  <ul>
     *    <li>Successors that have no block pointer, but only an address.  This can happen when we didn't disassemble any
     *        instruction at the successor address, and thus don't have a block at which to point.</li>
     *    <li>Successors that point to a block outside the specified AST subtree.  For instance, when considering the control flow
     *        graph of an individual function, successors for function calls will point outside the calling function (unless the
     *        call is recursive).<li>
     *    <li>Successors that are not known.  Some basic blocks, such as function return blocks or blocks ending with computed
     *        branches, usually only have unknown successors.  Such edges are not added to the graph.</li>
     *  </ul>
     */
    ControlFlowGraph build_cfg(SgNode *root);
}

#endif /* !ROSE_BinaryAnalysis_CFG_H */
