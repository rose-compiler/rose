// [Robb P Matzke 2017-06-29]: This entire API has been deprecated in favor of using the control flow graphs in
// Rose::BinaryAnalysis::Partitioner2 and the dominance algorithms in Sawyer::Container::Algorithm.

#ifndef ROSE_BinaryAnalysis_Dominance_H
#define ROSE_BinaryAnalysis_Dominance_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>

namespace Rose {
namespace BinaryAnalysis {

class Dominance {
public:
    Dominance(): debug(NULL) {}

    typedef boost::adjacency_list<boost::setS,      /* edge storage */
                                  boost::vecS,      /* vertex storage */
                                  boost::bidirectionalS,
                                  boost::property<boost::vertex_name_t, SgAsmBlock*>
                                 > Graph;

    template<class ControlFlowGraph>
    struct RelationMap: public std::vector<typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor> {
    };

public:
    void clear_ast(SgNode *ast) ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class DominanceGraph>
    void apply_to_ast(const DominanceGraph &idg)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    void apply_to_ast(const ControlFlowGraph &cfg, const RelationMap<ControlFlowGraph> &relation_map)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class DominanceGraph>
    void cache_vertex_descriptors(const DominanceGraph&)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    bool is_consistent(SgNode *ast, std::set<SgAsmBlock*> *bad_blocks=NULL)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

public:
    template<class ControlFlowGraph>
    RelationMap<ControlFlowGraph>
    build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                 typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    void build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                      typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                      RelationMap<ControlFlowGraph> &idom/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    RelationMap<ControlFlowGraph>
    build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    RelationMap<ControlFlowGraph>
    build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                    typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    void build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                         RelationMap<ControlFlowGraph> &idom/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph>
    void build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                         typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop,
                                         RelationMap<ControlFlowGraph> &idom/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

public:
    template<class DominanceGraph, class ControlFlowGraph>
    DominanceGraph build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                             typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph, class DominanceGraph>
    void build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                   typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                   DominanceGraph &dg/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class DominanceGraph, class ControlFlowGraph>
    DominanceGraph build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class DominanceGraph, class ControlFlowGraph>
    DominanceGraph build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                                typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                                typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph, class DominanceGraph>
    void build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                      typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                      DominanceGraph &pdg/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph, class DominanceGraph>
    void build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                      typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                      typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop,
                                      DominanceGraph &pdg/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

public:
    template<class DominanceGraph, class ControlFlowGraph>
    DominanceGraph build_graph_from_relation(const ControlFlowGraph &cfg,
                                             const RelationMap<ControlFlowGraph> &relmap)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    template<class ControlFlowGraph, class DominanceGraph>
    void build_graph_from_relation(const ControlFlowGraph &cfg,
                                   const RelationMap<ControlFlowGraph> &relmap,
                                   DominanceGraph &dg/*out*/)
        ROSE_DEPRECATED("Use Sawyer::Container::Algorithm::graphDominators");

    void set_debug(FILE *debug) { this->debug = debug; }

    FILE *get_debug() const { return debug; }

protected:
    FILE *debug;                    /**< Debugging stream, or null. */
};

/******************************************************************************************************************************
 *                              Function templates for methods that operate on the AST
 ******************************************************************************************************************************/

template<class DominanceGraph>
void
Dominance::apply_to_ast(const DominanceGraph &idg)
{
    if (debug)
        fprintf(debug, "Rose::BinaryAnalysis::Dominance::apply_to_ast:\n");

    typename boost::graph_traits<DominanceGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(idg); ei!=ei_end; ++ei) {
        SgAsmBlock *dom_block = get(boost::vertex_name, idg, source(*ei, idg));
        SgAsmBlock *sub_block = get(boost::vertex_name, idg, target(*ei, idg));
        if (debug) {
            fprintf(debug, "  edge (d,s) = (%" PRIuPTR ",%" PRIuPTR ") = (0x%08" PRIx64 ", 0x%08" PRIx64 ")\n",
                    source(*ei, idg), target(*ei, idg), dom_block->get_address(), sub_block->get_address());
        }
        sub_block->set_immediate_dominator(dom_block);
    }
}

template<class ControlFlowGraph>
void
Dominance::apply_to_ast(const ControlFlowGraph &cfg,
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
Dominance::cache_vertex_descriptors(const DominanceGraph &dg)
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
Dominance::build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                     typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                     DominanceGraph &result)
{
    RelationMap<ControlFlowGraph> idoms;
    build_idom_relation_from_cfg(cfg, start, idoms);
    build_graph_from_relation(cfg, idoms, result);
}

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
Dominance::build_idom_graph_from_cfg(const ControlFlowGraph &cfg,
                                     typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    DominanceGraph dg;
    build_idom_graph_from_cfg(cfg, start, dg);
    return dg;
}

template<class ControlFlowGraph>
Dominance::RelationMap<ControlFlowGraph>
Dominance::build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
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
Dominance::build_idom_relation_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                        RelationMap<ControlFlowGraph> &result)
{
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;

    struct debug_dom_set {
        debug_dom_set(FILE *debug, size_t vertex_i, size_t idom_i,
                      const std::vector<size_t> &domsets, const std::vector<CFG_Vertex> &flowlist) {
            if (debug) {
                fprintf(debug, "{ #%" PRIuPTR "(%" PRIuPTR ")", vertex_i, flowlist[vertex_i]);
                for (size_t d=idom_i; d!=vertex_i; vertex_i=d, d=domsets[d])
                    fprintf(debug, " #%" PRIuPTR "(%" PRIuPTR ")", d, flowlist[d]);
                fprintf(debug, " }");
            }
        }
    };

    if (debug) {
        fprintf(debug, "Rose::BinaryAnalysis::Dominance::build_idom_relation_from_cfg: starting at vertex %" PRIuPTR "\n", start);
        SgAsmBlock *block = get(boost::vertex_name, cfg, start);
        SgAsmFunction *func = block ? block->get_enclosing_function() : NULL;
        if (func) {
            fprintf(debug, "  Vertex %" PRIuPTR " is %s block of", start, func->get_entry_block()==block?"the entry":"a");
            if (func->get_name().empty()) {
                fprintf(debug, " an unnamed function");
            } else {
                fprintf(debug, " function <%s>", func->get_name().c_str());
            }
            fprintf(debug, " at 0x%08" PRIx64 "\n", func->get_entry_va());
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
            fprintf(debug, "    %" PRIuPTR " 0x%08" PRIx64 " --> {", (size_t)(*vi), block?block->get_address():0);
            typename boost::graph_traits<ControlFlowGraph>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(*vi, cfg); ei!=ei_end; ++ei) {
                fprintf(debug, " %" PRIuPTR "", (size_t)target(*ei, cfg));
            }
            fprintf(debug, " }\n");
        }

        fprintf(debug, "  Note: notation #M(N) means CFG vertex N at position M in the flow list.\n");
        fprintf(debug, "  Flowlist: {");
        for (size_t i=0; i<flowlist.size(); i++) {
            fprintf(debug, " #%" PRIuPTR "(%" PRIuPTR ")", i, (size_t)flowlist[i]);
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
                fprintf(debug, "    vertex #%" PRIuPTR "(%" PRIuPTR ")", (size_t)vertex_i, (size_t)vertex);
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
                        fprintf(debug, "      pred #%zd(%" PRIuPTR ")", (size_t)predecessor_i, (size_t)predecessor);

                    /* It's possible that the predecessor lies outside the part of the CFG connected to the entry node. We
                     * should not consider those predecessors. */
                    if (predecessor!=vertex && predecessor_i!=boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                        if (predecessor==start) {
                            new_idom = predecessor_i;
                            if (debug) {
                                fprintf(debug, "; new doms of #%" PRIuPTR "(%" PRIuPTR ") are ", vertex_i, vertex);
                                debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                            }
                        } else if (idom[predecessor_i]!=predecessor_i) {
                            if (new_idom==vertex_i) {
                                new_idom = predecessor_i;
                                if (debug) {
                                    fprintf(debug, "; new doms of #%" PRIuPTR "(%" PRIuPTR ") are ", vertex_i, vertex);
                                    debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                                }
                            } else {
                                if (debug) {
                                    fprintf(debug, "; new doms of #%" PRIuPTR "(%" PRIuPTR ") are intersect(", vertex_i, vertex);
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
            fprintf(debug, "    #%" PRIuPTR "(%" PRIuPTR ") has dominators ", (size_t)vertex_i, (size_t)vertex);
            debug_dom_set(debug, vertex_i, idom[vertex_i], idom, flowlist);
            fprintf(debug, "\n");
        }
        fprintf(debug, "  Final result:\n");
        for (size_t i=0; i<result.size(); i++) {
            if (result[i]==boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has no immediate dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has immediate dominator %" PRIuPTR "\n", i, result[i]);
            }
        }
    }
}





/******************************************************************************************************************************
 *                              Function templates for post dominators
 ******************************************************************************************************************************/

template<class ControlFlowGraph, class DominanceGraph>
void
Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                        DominanceGraph &result)
{
    RelationMap<ControlFlowGraph> pdoms;
    build_postdom_relation_from_cfg(cfg, start, pdoms);
    build_graph_from_relation(cfg, pdoms, result);
}

template<class ControlFlowGraph, class DominanceGraph>
void
Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop,
                                        DominanceGraph &result)
{
    RelationMap<ControlFlowGraph> pdoms;
    build_postdom_relation_from_cfg(cfg, start, stop, pdoms);
    build_graph_from_relation(cfg, pdoms, result);
}

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    DominanceGraph dg;
    build_postdom_graph_from_cfg(cfg, start, dg);
    return dg;
}

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
Dominance::build_postdom_graph_from_cfg(const ControlFlowGraph &cfg,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                        typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop)
{
    DominanceGraph dg;
    build_postdom_graph_from_cfg(cfg, start, stop, dg);
    return dg;
}

template<class ControlFlowGraph>
Dominance::RelationMap<ControlFlowGraph>
Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start)
{
    RelationMap<ControlFlowGraph> pdom;
    build_postdom_relation_from_cfg(cfg, start, pdom);
    return pdom;
}

template<class ControlFlowGraph>
Dominance::RelationMap<ControlFlowGraph>
Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop)
{
    RelationMap<ControlFlowGraph> pdom;
    build_postdom_relation_from_cfg(cfg, start, stop, pdom);
    return pdom;
}

template<class ControlFlowGraph>
void
Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor stop,
                                           RelationMap<ControlFlowGraph> &result)
{
    /* Post dominance is the same as doing dominance, but on a reversed CFG, using the stop vertex as the starting point. */
    if (debug)
        fprintf(debug, "  Calling build_idom_relation_from_cfg() on reversed CFG...\n");
    typedef typename boost::reverse_graph<ControlFlowGraph> ReversedControlFlowGraph;
    ReversedControlFlowGraph rcfg(cfg);
    RelationMap<ReversedControlFlowGraph> rrelation;
    build_idom_relation_from_cfg(rcfg, stop, rrelation);
    result.assign(rrelation.begin(), rrelation.end());
}

template<class ControlFlowGraph>
void
Dominance::build_postdom_relation_from_cfg(const ControlFlowGraph &cfg,
                                           typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor start,
                                           RelationMap<ControlFlowGraph> &result)
{
    if (debug) {
        fprintf(debug, "Rose::BinaryAnalysis::Dominance::build_postdom_relation_from_cfg: starting at vertex %" PRIuPTR "\n", start);
        SgAsmBlock *block = get(boost::vertex_name, cfg, start);
        SgAsmFunction *func = block ? block->get_enclosing_function() : NULL;
        if (func) {
            fprintf(debug, "  Vertex %" PRIuPTR " is %s block of", start, func->get_entry_block()==block?"the entry":"a");
            if (func->get_name().empty()) {
                fprintf(debug, " an unnamed function");
            } else {
                fprintf(debug, " function <%s>", func->get_name().c_str());
            }
            fprintf(debug, " at 0x%08" PRIx64 "\n", func->get_entry_va());
        }
    }

    /* Does the graph have more than one return block?  By "return", we mean any block whose control flow successor is possibly
     * outside the start node's function.  See ControlFlow::return_blocks(). */
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;
    std::vector<CFG_Vertex> retblocks = ControlFlow().return_blocks(cfg, start);
    if (1==retblocks.size()) {
        if (debug)
            fprintf(debug, "  CFG has unique exit vertex %" PRIuPTR ", block 0x%08" PRIx64 "\n",
                    retblocks[0],
                    get(boost::vertex_name, cfg, retblocks[0])->get_address());
        build_postdom_relation_from_cfg(cfg, start, retblocks[0], result);
    } else if (0==retblocks.size()) {
        // This can happen for a function like "void f() { while(1); }", or the assembly code
        //    f:  jmp f
        // It can also happen for functions with more than one basic block as long as every basic block branches to
        // another basic block in the same function.  Post-dominance is defined in terms of a stop node (i.e., function return,
        // HLT instruction, etc), but such a function has no stop node.
        if (debug)
            fprintf(debug, "  CFG has no exit or terminal vertex; post-dominance cannot be calculated\n");
        result.insert(result.begin(), num_vertices(cfg), boost::graph_traits<ControlFlowGraph>::null_vertex());
    } else {
        // Create a temporary unique exit/halt vertex and make all the others point to it.
        ControlFlowGraph cfg_copy = cfg; /* we need our own copy to modify */
        assert(!retblocks.empty());
        CFG_Vertex unique_exit = add_vertex(cfg_copy);
        put(boost::vertex_name, cfg_copy, unique_exit, (SgAsmBlock*)0); /* vertex has no basic block */
        for (size_t i=0; i<retblocks.size(); i++)
            add_edge(retblocks[i], unique_exit, cfg_copy);
        if (debug)
            fprintf(debug, "  CFG has %" PRIuPTR " exit blocks. Added unique exit vertex %" PRIuPTR "\n", retblocks.size(), unique_exit);

        // Perform the post dominance on this new CFG using the unique exit vertex.
        build_postdom_relation_from_cfg(cfg_copy, start, unique_exit, result);

        // Remove the unique exit vertex from the returned relation
        assert(unique_exit+1==num_vertices(cfg_copy));
        result.pop_back();
        for (size_t i=0; i<result.size(); ++i) {
            if (result[i]>=result.size())
                result[i] = boost::graph_traits<ControlFlowGraph>::null_vertex();
        }
    }

    if (debug) {
        fprintf(debug, "  Final result:\n");
        for (size_t i=0; i<result.size(); i++) {
            if (result[i]==boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has no immediate post dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has immediate post dominator %" PRIuPTR "\n", i, result[i]);
            }
        }
    }
}




/******************************************************************************************************************************
 *                              Function templates for miscellaneous methods
 ******************************************************************************************************************************/

template<class DominanceGraph, class ControlFlowGraph>
DominanceGraph
Dominance::build_graph_from_relation(const ControlFlowGraph &cfg,
                                     const RelationMap<ControlFlowGraph> &relmap)
{
    DominanceGraph g;
    build_graph_from_relation(cfg, relmap, g);
    return g;
}

template<class ControlFlowGraph, class DominanceGraph>
void
Dominance::build_graph_from_relation(const ControlFlowGraph &cfg,
                                     const RelationMap<ControlFlowGraph> &relmap,
                                     DominanceGraph &dg/*out*/)
{
    typedef typename boost::graph_traits<DominanceGraph>::vertex_descriptor D_Vertex;
    typedef typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor CFG_Vertex;

    if (debug) {
        fprintf(debug, "Rose::BinaryAnalysis::Dominance::build_graph_from_relation:\n");
        fprintf(debug, "  building from this relation:\n");
        for (size_t i=0; i<relmap.size(); i++) {
            if (relmap[i]==boost::graph_traits<ControlFlowGraph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has no immediate dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %" PRIuPTR " has immediate dominator %" PRIuPTR "\n", i, relmap[i]);
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
                fprintf(debug, "  adding edge (d,s) = (%" PRIuPTR ",%" PRIuPTR ")\n", dominator, subordinate);
            add_edge(dominator, subordinate, dg);
        }
    }
}

} // namespace
} // namespace

#endif
#endif
