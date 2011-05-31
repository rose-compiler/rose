#include "sage3basic.h"
#include "BinaryFunctionCall.h"

/* See header file for documentation. */
void
BinaryAnalysis::FunctionCall::cache_vertex_descriptors(const Graph &cg)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cg); vi!=vi_end; ++vi) {
        SgAsmFunctionDeclaration *func = get(boost::vertex_name, cg, *vi);
        assert(func!=NULL); /* every vertex must point to a function */
        if (!is_vertex_filtered(func))
            func->set_cached_vertex(*vi);
    }
}

/* See header file for documentation. */
void
BinaryAnalysis::FunctionCall::build_graph(const ControlFlow::Graph &cfg, Graph &cg/*out*/)
{
    typedef boost::graph_traits<Graph>::vertex_descriptor CG_Vertex;
    typedef boost::graph_traits<ControlFlow::Graph>::vertex_descriptor CFG_Vertex;
    typedef std::map<SgAsmFunctionDeclaration*, CG_Vertex> FunctionVertexMap;

    cg.clear();

    /* Add CG vertices by collapsing CFG nodes that belong to a common function. */
    FunctionVertexMap fv_map;
    boost::graph_traits<ControlFlow::Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, *vi);
        SgAsmFunctionDeclaration *func = block->get_enclosing_function();
        if (!is_vertex_filtered(func)) {
            FunctionVertexMap::iterator fi=fv_map.find(func);
            if (func && fi==fv_map.end()) {
                CG_Vertex v = add_vertex(cg);
                put(boost::vertex_name, cg, v, func);
                fv_map[func] = v;
            }
        }
    }

    /* Add edges whose target is a function entry block. */
    boost::graph_traits<ControlFlow::Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(cfg); ei!=ei_end; ++ei) {
        CFG_Vertex cfg_a = source(*ei, cfg);
        CFG_Vertex cfg_b = target(*ei, cfg);
        SgAsmBlock *block_a = get(boost::vertex_name, cfg, cfg_a);
        SgAsmBlock *block_b = get(boost::vertex_name, cfg, cfg_b);
        SgAsmFunctionDeclaration *func_a = block_a->get_enclosing_function();
        SgAsmFunctionDeclaration *func_b = block_b->get_enclosing_function();
        if (func_a && func_b && block_b==func_b->get_entry_block() && !is_edge_filtered(func_a, func_b)) {
            FunctionVertexMap::iterator fi_a = fv_map.find(func_a);
            if (fi_a!=fv_map.end()) {
                FunctionVertexMap::iterator fi_b = fv_map.find(func_b);
                if (fi_b!=fv_map.end())
                    add_edge(fi_a->second, fi_b->second, cg);
            }
        }
    }
}

/* See header file for documentation. */
BinaryAnalysis::FunctionCall::Graph
BinaryAnalysis::FunctionCall::build_graph(const ControlFlow::Graph &cfg)
{
    Graph cg;
    build_graph(cfg, cg);
    return cg;
}

/* See header file for documentation. */
void
BinaryAnalysis::FunctionCall::build_graph(SgNode *root, Graph &cg/*out*/)
{
    typedef std::map<SgAsmFunctionDeclaration*, Vertex> FunctionVertexMap;
    FunctionVertexMap fv_map;

    cg.clear();

    /* Visiter that adds a vertex for each unique function. */
    struct VertexAdder: public AstSimpleProcessing {
        FunctionCall *analyzer;
        Graph &cg;
        FunctionVertexMap &fv_map;
        VertexAdder(FunctionCall *analyzer, Graph &cg, FunctionVertexMap &fv_map)
            : analyzer(analyzer), cg(cg), fv_map(fv_map)
            {}
        void visit(SgNode *node) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
            if (func && !analyzer->is_vertex_filtered(func)) {
                Vertex vertex = add_vertex(cg);
                fv_map[func] = vertex;
                put(boost::vertex_name, cg, vertex, func);
            }
        }
    };

    /* Visitor that adds edges for each vertex.  Traversal should be over one function at a time. */
    struct EdgeAdder: public AstSimpleProcessing {
        FunctionCall *analyzer;
        Graph &cg;
        FunctionVertexMap &fv_map;
        Vertex source_vertex;
        EdgeAdder(FunctionCall *analyzer, Graph &cg, FunctionVertexMap &fv_map, Vertex source_vertex)
            : analyzer(analyzer), cg(cg), fv_map(fv_map), source_vertex(source_vertex)
            {}
        SgAsmFunctionDeclaration *function_of(SgAsmBlock *block) {
            return block ? block->get_enclosing_function() : NULL;
        }
        void visit(SgNode *node) {
            SgAsmBlock *block_a = isSgAsmBlock(node); /* the calling block */
            SgAsmFunctionDeclaration *func_a = function_of(block_a); /* the calling function */
            if (!func_a)
                return;
            const SgAsmTargetPtrList &succs = block_a->get_successors();
            for (SgAsmTargetPtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
                SgAsmBlock *block_b = (*si)->get_block(); /* the called block */
                SgAsmFunctionDeclaration *func_b = function_of(block_b); /* the called function */
                if (func_b && block_b==func_b->get_entry_block() && !analyzer->is_edge_filtered(func_a, func_b)) {
                    FunctionVertexMap::iterator fi_b = fv_map.find(func_b); /* find vertex for called function */
                    if (fi_b!=fv_map.end())
                        add_edge(source_vertex, fi_b->second, cg);
                }
            }
        }
    };

    VertexAdder(this, cg, fv_map).traverse(root, preorder);
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cg); vi!=vi_end; ++vi) {
        SgAsmFunctionDeclaration *source_func = get(boost::vertex_name, cg, *vi);
        EdgeAdder(this, cg, fv_map, *vi).traverse(source_func, preorder);
    }
}

/* See header file for documentation. */
BinaryAnalysis::FunctionCall::Graph
BinaryAnalysis::FunctionCall::build_graph(SgNode *root)
{
    Graph cg;
    build_graph(root, cg);
    return cg;
}

/* See header file for documentation. */
void
BinaryAnalysis::FunctionCall::copy(const Graph &src, Graph &dst)
{
    Vertex NO_VERTEX = boost::graph_traits<Graph>::null_vertex();

    dst.clear();
    std::vector<Vertex> src_to_dst(num_vertices(src), NO_VERTEX);

    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(src); vi!=vi_end; ++vi) {
        SgAsmFunctionDeclaration *func = get(boost::vertex_name, src, *vi);
        if (!is_vertex_filtered(func)) {
            src_to_dst[*vi] = add_vertex(dst);
            put(boost::vertex_name, dst, src_to_dst[*vi], func);
        }
    }

    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end)=edges(src); ei!=ei_end; ++ei) {
        if (NO_VERTEX!=src_to_dst[source(*ei, src)] && NO_VERTEX!=src_to_dst[target(*ei, src)]) {
            SgAsmFunctionDeclaration *func1 = get(boost::vertex_name, src, source(*ei, src));
            SgAsmFunctionDeclaration *func2 = get(boost::vertex_name, src, target(*ei, src));
            if (!is_edge_filtered(func1, func2))
                add_edge(src_to_dst[source(*ei, src)], src_to_dst[target(*ei, src)], dst);
        }
    }
}

/* See header file for documentation. */
BinaryAnalysis::FunctionCall::Graph
BinaryAnalysis::FunctionCall::copy(const Graph &src)
{
    Graph dst;
    copy(src, dst);
    return dst;
}

