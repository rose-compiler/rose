#ifndef ROSE_BinaryAnalysis_CG_H
#define ROSE_BinaryAnalysis_CG_H

#include "BinaryCFG.h"

class SgAsmFunctionDeclaration;

namespace RoseBinaryAnalysis {

    /** A function call graph.
     *
     *  A function call graph is simply a Boost graph whose vertex descriptors are integers and whose vertices point to
     *  SgAsmFunctionDeclaration nodes in the AST (via the boost::vertex_name property).  The graph edges represent function
     *  calls from one SgAsmFunctionDeclaration to another.  Since this graph is a Boost graph, it is endowed with all the
     *  features of a Boost graph and can be the operand of the various Boost graph algorithms.  See build_cg() for specifics
     *  about what is included in such a graph.
     *
     *  Another way to represent function calls is to adapt a global control flow graph (RoseBinaryAnalysis::ControlFlowGraph)
     *  to include only the edges (and their incident vertices) that flow from one function to another.  The advantage of using
     *  a control flow graph to represent function call information is that each call site will be included in the function
     *  call graph due to the fact that the control flow graph vertices are blocks (SgAsmBlock) rather than functions
     *  (SgAsmFunctionDeclaration). */
    typedef boost::adjacency_list<boost::listS,                                 /* out-edges of each vertex in std::list */
                                  boost::vecS,                                  /* store vertices in std::vector */
                                  boost::directedS,                             /* call graph is directed */
                                  boost::property<boost::vertex_name_t, SgAsmFunctionDeclaration*> /* each vertex is a function */
                                 > FunctionCallGraph;

    /** Build a function call graph from a control flow graph.
     *
     *  Given a control flow graph (CFG) spanning multiple functions, create a function call graph (CG) by collapsing vertices
     *  in the CFG that belong to a common function.  Any resulting self-loop edges will be removed unless the target of the
     *  corresponding edge in the CFG was the function entry block (i.e., intra-function CFG edges whose target is the
     *  function's entry block are assumed to be recursive calls, while all other intra-function CFG edges are omitted from the
     *  CG). */
    FunctionCallGraph build_cg(ControlFlowGraph&);

    /** Build a function call graph from an AST.
     *
     *  Given an AST, traverse the AST beginning at @p root and build a function call graph (CG).  The function call graph will
     *  contain only SgAsmFunctionDeclaration vertices that are in the specified subtree.   The following two methods of
     *  constructing a CG should result in identical graphs (although vertex and edge order may be different):
     *
     *  @code
     *  SgAsmNode *node = ...;
     *  FunctionCallGraph cg1 = build_cg(node);            // method 1
     *  FunctionCallGraph cg2 = build_cg(build_cfg(node)); // method 2
     *  @endcode
     *
     *  In general, building the function call graph directly from the AST will be faster than first building the control flow
     *  graph. */
    FunctionCallGraph build_cg(SgNode *root);
}

#endif /* !ROSE_BinaryAnalysis_CG_H */
