#include "sage3basic.h"
#include "BinaryControlFlow.h"

#include <boost/graph/depth_first_search.hpp>

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::clear_ast(SgNode *root)
{
    struct T1: public AstSimpleProcessing {
        ControlFlow *analyzer;
        T1(ControlFlow *analyzer): analyzer(analyzer) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block && !analyzer->is_vertex_filtered(block)) {
                block->get_successors().clear();
                block->set_successors_complete(false);
            }
        }
    };
    T1(this).traverse(root, preorder);
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::apply_to_ast(const Graph &cfg)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        assert(block!=NULL); /* every vertex must point to a block */
        if (is_vertex_filtered(block))
            continue;

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
            if (!is_edge_filtered(block, target_block)) {
                SgAsmTarget *target = new SgAsmTarget();
                target->set_address(target_block->get_address());
                target->set_block(target_block);
            }
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
        if (!is_vertex_filtered(block))
            block->set_cached_vertex(*vi);
    }
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::build_graph(SgNode *root, Graph &cfg)
{
    typedef std::map<SgAsmBlock*, Vertex> BlockVertexMap;
    BlockVertexMap bv_map;

    cfg.clear();

    /* Define the vertices. */
    struct T1: public AstSimpleProcessing {
        ControlFlow *analyzer;
        Graph &cfg;
        BlockVertexMap &bv_map;
        T1(ControlFlow *analyzer, Graph &cfg, BlockVertexMap &bv_map): analyzer(analyzer), cfg(cfg), bv_map(bv_map) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block && !analyzer->is_vertex_filtered(block)) {
                Vertex vertex = add_vertex(cfg);
                bv_map[block] = vertex;
                put(boost::vertex_name, cfg, vertex, block);
            }
        }
    };
    T1(this, cfg, bv_map).traverse(root, preorder); /* preorder guarantees that the highest SgAsmBlock will be vertex zero */

    /* Add the edges. */
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *source = boost::get(boost::vertex_name, cfg, *vi);
        const SgAsmTargetPtrList &succs = source->get_successors();
        for (SgAsmTargetPtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
            SgAsmBlock *target = (*si)->get_block(); // might be null
            if (target && !is_edge_filtered(source, target)) {
                BlockVertexMap::iterator bvmi=bv_map.find(target);
                if (bvmi!=bv_map.end())
                    add_edge(*vi, bvmi->second, cfg);
            }
        }
    }
}

/* See header file for documentation. */
BinaryAnalysis::ControlFlow::Graph
BinaryAnalysis::ControlFlow::build_graph(SgNode *root)
{
    Graph cfg;
    build_graph(root, cfg);
    return cfg;
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::build_call_graph(SgNode *root, Graph &cfg/*out*/)
{
    struct T1: public EdgeFilter {
        EdgeFilter *parent;
        T1(EdgeFilter *parent): parent(parent) {}
        bool operator()(ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
            assert(dst!=NULL);
            SgAsmFunctionDeclaration *func = dst->get_enclosing_function();
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
        build_graph(root, cfg);
    } catch (...) {
        set_edge_filter(parent);
        throw;
    }
    set_edge_filter(parent);
}

/* See header file for documentation. */
BinaryAnalysis::ControlFlow::Graph
BinaryAnalysis::ControlFlow::build_call_graph(SgNode *root)
{
    Graph cfg;
    build_call_graph(root, cfg);
    return cfg;
}

/* See header file for documentation. */
void
BinaryAnalysis::ControlFlow::copy(const Graph &src, Graph &dst)
{
    Vertex NO_VERTEX = boost::graph_traits<Graph>::null_vertex();

    dst.clear();
    std::vector<Vertex> src_to_dst(num_vertices(src), NO_VERTEX);

    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(src); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, src, *vi);
        if (!is_vertex_filtered(block)) {
            src_to_dst[*vi] = add_vertex(dst);
            put(boost::vertex_name, dst, src_to_dst[*vi], block);
        }
    }

    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(src); ei!=ei_end; ++ei) {
        if (NO_VERTEX!=src_to_dst[source(*ei, src)] && NO_VERTEX!=src_to_dst[target(*ei, src)]) {
            SgAsmBlock *block1 = get(boost::vertex_name, src, source(*ei, src));
            SgAsmBlock *block2 = get(boost::vertex_name, src, target(*ei, src));
            if (!is_edge_filtered(block1, block2))
                add_edge(src_to_dst[source(*ei, src)], src_to_dst[target(*ei, src)], dst);
        }
    }
}

/* See header file for documentation. */
BinaryAnalysis::ControlFlow::Graph
BinaryAnalysis::ControlFlow::copy(const Graph &src)
{
    Graph dst;
    copy(src, dst);
    return dst;
}

/* Used only by BinaryAnalysis::ControlFlow::flow_order(), but needs to be declared at file scope for the sake of
 * boost::depth_first_visit().  Undefined function errors if not. */
struct FlowOrder: public boost::default_dfs_visitor {
    typedef BinaryAnalysis::ControlFlow::Graph Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::vector<Vertex> VertexList;
    typedef std::vector<size_t> ReverseVertexList;

    VertexList *forward_order;
    FlowOrder(VertexList *forward_order): forward_order(forward_order) {}

    void compute(const Graph &g, Vertex v0, ReverseVertexList *reverse_order) {
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
    void finish_vertex(Vertex v, Graph g) {
        forward_order->push_back(v);
    }
};

/* See header file for documentation. */
std::vector<BinaryAnalysis::ControlFlow::Vertex>
BinaryAnalysis::ControlFlow::flow_order(const Graph &cfg, Vertex start, std::vector<size_t> *reverse_order/*=NULL*/)
{
    std::vector<Vertex> forward_order;
    FlowOrder(&forward_order).compute(cfg, start, reverse_order);
    return forward_order;
}

