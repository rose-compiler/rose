#include "sage3basic.h"
#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::clear_ast(SgNode *root)
{
    struct T1: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block) {
                block->get_successors().clear();
                block->set_successors_complete(false);
            }
        }
    };
    T1().traverse(root, preorder);
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::apply_to_ast(const Graph &cfg)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        assert(block!=NULL); /* every vertex must point to a block */

        /* Delete old targets */
        const SgAsmTargetPtrList &targets = block->get_successors();
        for (SgAsmTargetPtrList::const_iterator ti=targets.begin(); ti!=targets.end(); ++ti)
            delete *ti;

        /* Add new targets */
        block->set_successors_complete(true);
        block->get_successors().clear();
        boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=out_edges(*vi, cfg); ei!=ei_end; ++ei) {
            SgAsmBlock *target_block = get(boost::vertex_name, cfg, target(*ei, cfg));
            assert(target_block!=NULL); /* every vertex must point to a block */
            SgAsmTarget *target = new SgAsmTarget();
            target->set_address(target_block->get_address());
            target->set_block(target_block);
        }
    }
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::cache_vertex_descriptors(const Graph &cfg)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        assert(block!=NULL); /* every vertex must point to a block */
        block->set_cached_vertex(*vi);
    }
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::build_graph(SgNode *root, Graph &cfg)
{
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::map<SgAsmBlock*, Vertex> BlockVertexMap;
    BlockVertexMap bv_map;

    cfg.clear();

    /* Define the vertices. */
    struct T1: public AstSimpleProcessing {
        Graph &cfg;
        BlockVertexMap &bv_map;
        T1(Graph &cfg, BlockVertexMap &bv_map): cfg(cfg), bv_map(bv_map) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block) {
                Vertex vertex = add_vertex(cfg);
                bv_map[block] = vertex;
                put(boost::vertex_name, cfg, vertex, block);
            }
        }
    };
    T1(cfg, bv_map).traverse(root, preorder); /* preorder guarantees that the highest SgAsmBlock will be vertex zero */

    /* Add the edges. */
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *source = boost::get(boost::vertex_name, cfg, *vi);
        const SgAsmTargetPtrList &succs = source->get_successors();
        for (SgAsmTargetPtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
            SgAsmBlock *target = (*si)->get_block(); // might be null
            BlockVertexMap::iterator bvmi=bv_map.find(target);
            if (bvmi!=bv_map.end())
                add_edge(*vi, bvmi->second, cfg);
        }
    }
}

BinaryAnalysis::ControlFlow::Graph
BinaryAnalysis::ControlFlow::build_graph(SgNode *root)
{
    Graph cfg;
    build_graph(root, cfg);
    return cfg;
}

/* Used only by BinaryAnalysis::ControlFlow::flow_order(), but needs to be declared at file scope for the sake of
 * boost::depth_first_visit().  Undefined function errors if not. */
struct FlowOrder: public boost::default_dfs_visitor {
    typedef BinaryAnalysis::ControlFlow::Graph Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::vector<Vertex> VertexList;

    VertexList *forward_order;
    FlowOrder(VertexList *forward_order): forward_order(forward_order) {}

    void compute(const Graph &g, Vertex v0, VertexList *reverse_order) {
        forward_order->clear();
        std::vector<boost::default_color_type> colors(num_vertices(g), boost::white_color);
        depth_first_visit(g, v0, *this, &(colors[0]));
        assert(!forward_order->empty()); /* it should at least contain v0 */
        std::reverse(forward_order->begin(), forward_order->end());
        if (reverse_order) {
            reverse_order->clear();
            reverse_order->resize(num_vertices(g), boost::graph_traits<Graph>::null_vertex());
            for (size_t i=0; i<forward_order->size(); i++)
                (*reverse_order)[(*forward_order)[i]] = i;
        }
    }
    void finish_vertex(Vertex v, Graph g) {
        forward_order->push_back(v);
    }
};

/* See header file for documentation. */
std::vector<BinaryAnalysis::ControlFlow::Vertex>
BinaryAnalysis::ControlFlow::flow_order(const Graph &cfg, Vertex start, std::vector<Vertex> *reverse_order/*=NULL*/)
{
    std::vector<Vertex> forward_order;
    FlowOrder(&forward_order).compute(cfg, start, reverse_order);
    return forward_order;
}
