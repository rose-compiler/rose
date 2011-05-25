#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "sage3basic.h"
#include "BinaryDominance.h"

#include <boost/graph/depth_first_search.hpp>


void
BinaryAnalysis::Dominance::clear_ast(SgNode *ast)
{
    struct T1: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block)
                block->set_immediate_dominator(NULL);
        }
    };
    T1().traverse(ast, preorder);
}

bool
BinaryAnalysis::Dominance::is_consistent(SgNode *ast, std::set<SgAsmBlock*> *bad_blocks/*=NULL*/)
{
    struct T1: public AstSimpleProcessing {
        bool failed;
        std::set<SgAsmBlock*> *bad_blocks;
        T1(std::set<SgAsmBlock*> *bad_blocks): failed(false), bad_blocks(bad_blocks) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            SgAsmFunctionDeclaration *func = block ? block->get_enclosing_function() : NULL;
            if (block && func) {
                if (block==func->get_entry_block()) {
                    if (block->get_immediate_dominator()) {
                        if (bad_blocks)
                            bad_blocks->insert(block);
                        failed = true;
                    }
                } else {
                    SgAsmBlock *idom = block->get_immediate_dominator();
                    if (!idom || idom->get_enclosing_function()!=func) {
                        if (bad_blocks)
                            bad_blocks->insert(block);
                        failed = true;
                    }
                }
            }
        }
    } t1(bad_blocks);
    t1.traverse(ast, preorder);
    return t1.failed;
}



void
BinaryAnalysis::Dominance::apply_to_ast(const Graph &idg)
{
    if (debug)
        fprintf(debug, "BinaryAnalysis::Dominance::apply_to_ast:\n");

    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(idg); ei!=ei_end; ++ei) {
        SgAsmBlock *dom_block = get(boost::vertex_name, idg, source(*ei, idg));
        SgAsmBlock *sub_block = get(boost::vertex_name, idg, target(*ei, idg));
        if (debug) {
            fprintf(debug, "  edge (d,s) = (%zu,%zu) = (0x%08"PRIx64", 0x%08"PRIx64")\n",
                    source(*ei, idg), target(*ei, idg), dom_block->get_address(), sub_block->get_address());
        }
        sub_block->set_immediate_dominator(dom_block);
    }
#if 0
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(idg); vi!=vi_end; ++vi) {
        SgAsmBlock *idom_block = get(boost::vertex_name, idg, *vi);
        assert(idom_block!=NULL); /* all vertices point to a block */
        boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = in_edges(*vi, idg); ei!=ei_end; ++ei) {
            assert(source(*ei, idg)!=target(*ei, idg));
            SgAsmBlock *sub_block = get(boost::vertex_name, idg, target(*ei, idg));
            assert(sub_block!=NULL); /* all vertices point to a block */
            sub_block->set_immediate_dominator(idom_block);
        }
    }
#endif
}

void
BinaryAnalysis::Dominance::apply_to_ast(const ControlFlow::Graph &cfg, const RelationMap &idom)
{
    assert(idom.size()<=num_vertices(cfg));
    for (size_t subordinate=0; subordinate<idom.size(); subordinate++) {
        SgAsmBlock *sub_block = get(boost::vertex_name, cfg, (ControlFlow::Vertex)subordinate);
        assert(sub_block!=NULL);
        if (idom[subordinate] != boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
            ControlFlow::Vertex dominator = idom[subordinate];
            SgAsmBlock *dom_block = get(boost::vertex_name, cfg, dominator);
            assert(dom_block!=NULL);
            sub_block->set_immediate_dominator(dom_block);
        }
    }
}

/* See header file for documentation. */
void
BinaryAnalysis::Dominance::cache_vertex_descriptors(const Graph &cfg)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        assert(block!=NULL); /* every vertex must point to a block */
        block->set_cached_vertex(*vi);
    }
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
void
BinaryAnalysis::Dominance::build_idom_vector(const ControlFlow::Graph &cfg, ControlFlow::Vertex start, RelationMap &result)
{
    struct debug_dom_set {
        debug_dom_set(FILE *debug, size_t vertex_i, size_t idom_i,
                      const std::vector<size_t> &domsets, const std::vector<ControlFlow::Vertex> &flowlist) {
            if (debug) {
                fprintf(debug, "{ #%zu(%zu)", vertex_i, flowlist[vertex_i]);
                for (size_t d=idom_i; d!=vertex_i; vertex_i=d, d=domsets[d])
                    fprintf(debug, " #%zu(%zu)", d, flowlist[d]);
                fprintf(debug, " }");
            }
        }
    };

    if (debug) {
        fprintf(debug, "BinaryAnalysis::Dominance::build_idom_vector: starting at vertex %zu\n", start);
        SgAsmBlock *block = get(boost::vertex_name, cfg, start);
        assert(block!=NULL);
        SgAsmFunctionDeclaration *func = block->get_enclosing_function();
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
    std::vector<ControlFlow::Vertex> rflowlist; /* reverse mapping; flowlist[i]==v implies rflowlist[v]==i */
    std::vector<ControlFlow::Vertex> flowlist = ControlFlow::flow_order(cfg, start, &rflowlist);
    std::vector<size_t> idom(flowlist.size());
    for (size_t i=0; i<flowlist.size(); i++)
        idom[i] = i; /* idom[i]==i implies idom[i] is unknown */

    if (debug) {
        fprintf(debug, "  CFG:\n");
        boost::graph_traits<ControlFlow::Graph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
            SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
            fprintf(debug, "    %zu 0x%08"PRIx64" --> {", (size_t)(*vi), block->get_address());
            boost::graph_traits<ControlFlow::Graph>::out_edge_iterator ei, ei_end;
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
        for (ControlFlow::Vertex vertex_i=0; vertex_i<flowlist.size(); vertex_i++) {
            ControlFlow::Vertex vertex = flowlist[vertex_i];
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
                boost::graph_traits<ControlFlow::Graph>::in_edge_iterator pi, pi_end; /*predecessors*/
                ControlFlow::Vertex new_idom = vertex_i; /*undefined for now*/
                for (boost::tie(pi, pi_end)=in_edges(vertex, cfg); pi!=pi_end; ++pi) {
                    ControlFlow::Vertex predecessor = source(*pi, cfg);
                    assert(predecessor>=0 && predecessor<rflowlist.size());
                    ControlFlow::Vertex predecessor_i = rflowlist[predecessor];
                    if (debug)
                        fprintf(debug, "      pred #%zd(%zu)", (size_t)predecessor_i, (size_t)predecessor);

                    /* It's possible that the predecessor lies outside the part of the CFG connected to the entry node. We
                     * should not consider those predecessors. */
                    if (predecessor!=vertex && predecessor_i!=boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
                        if (new_idom==vertex_i) {
                            new_idom = predecessor_i;
                            if (debug) {
                                fprintf(debug, "; new doms of #%zu(%zu) are ", vertex_i, vertex);
                                debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                            }
                        } else if (idom[predecessor_i]!=predecessor_i) {
                            if (debug) {
                                fprintf(debug, "; new doms of #%zu(%zu) are intersect(", vertex_i, vertex);
                                debug_dom_set(debug, vertex_i, new_idom, idom, flowlist);
                                fprintf(debug, ", ");
                                debug_dom_set(debug, vertex_i, predecessor_i, idom, flowlist);
                            }
                            ControlFlow::Vertex f1=new_idom, f2=predecessor_i;
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
    result.resize(num_vertices(cfg), boost::graph_traits<ControlFlow::Graph>::null_vertex());
    for (size_t i=0; i<flowlist.size(); i++) {
        if (idom[i]!=i)
            result[flowlist[i]] = flowlist[idom[i]];
    }

    if (debug) {
        fprintf(debug, "  Final dom sets:\n");
        for (ControlFlow::Vertex vertex_i=0; vertex_i<flowlist.size(); vertex_i++) {
            ControlFlow::Vertex vertex = flowlist[vertex_i];
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

BinaryAnalysis::Dominance::RelationMap
BinaryAnalysis::Dominance::build_idom_vector(const ControlFlow::Graph &cfg, ControlFlow::Vertex start)
{
    RelationMap idom;
    build_idom_vector(cfg, start, idom);
    return idom;
}

void
BinaryAnalysis::Dominance::build_idom_graph(const ControlFlow::Graph &cfg, ControlFlow::Vertex start, Graph &result)
{
    RelationMap idoms;
    build_idom_vector(cfg, start, idoms);
    build_idom_graph(cfg, idoms, result);
}

void
BinaryAnalysis::Dominance::build_idom_graph(const ControlFlow::Graph &cfg, const RelationMap &idoms, Graph &dg/*out*/)
{
    if (debug) {
        fprintf(debug, "BinaryAnalysis::Dominance::build_idom_graph:\n");
        fprintf(debug, "  building from this relation:\n");
        for (size_t i=0; i<idoms.size(); i++) {
            if (idoms[i]==boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
                fprintf(debug, "    CFG vertex %zu has no immeidate dominator\n", i);
            } else {
                fprintf(debug, "    CFG vertex %zu has immediate dominator %zu\n", i, idoms[i]);
            }
        }
    }

    dg.clear();
    boost::graph_traits<ControlFlow::Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; vi++) {
        Dominance::Vertex v = add_vertex(dg);
        assert(v==*vi); /* because idoms[] refers to CFG vertices; otherwise we need to map them */
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        assert(block!=NULL);
        put(boost::vertex_name, dg, v, block);
    }
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; vi++) {
        ControlFlow::Vertex subordinate = *vi;
        ControlFlow::Vertex dominator = idoms[subordinate];
        if (dominator!=boost::graph_traits<ControlFlow::Graph>::null_vertex()) {
            if (debug)
                fprintf(debug, "   adding edge (d,s) = (%zu,%zu)\n", dominator, subordinate);
            add_edge(dominator, subordinate, dg);
        }
    }
}

BinaryAnalysis::Dominance::Graph
BinaryAnalysis::Dominance::build_idom_graph(const ControlFlow::Graph &cfg, const RelationMap &idoms)
{
    Graph dg;
    build_idom_graph(cfg, idoms);
    return dg;
}

BinaryAnalysis::Dominance::Graph
BinaryAnalysis::Dominance::build_idom_graph(const ControlFlow::Graph &cfg, ControlFlow::Vertex start)
{
    Graph dg;
    build_idom_graph(cfg, start, dg);
    return dg;
}
