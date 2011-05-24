#include "sage3basic.h"
#include "BinaryFunctionCall.h"

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
        FunctionVertexMap::iterator fi=fv_map.find(func);
        if (func && fi==fv_map.end()) {
            CG_Vertex v = add_vertex(cg);
            put(boost::vertex_name, cg, v, func);
            fv_map[func] = v;
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
        CG_Vertex cg_a = fv_map[func_a];
        CG_Vertex cg_b = fv_map[func_b];
        if (block_b==func_b->get_entry_block())
            add_edge(cg_a, cg_b, cg);
    }
}

BinaryAnalysis::FunctionCall::Graph
BinaryAnalysis::FunctionCall::build_graph(const ControlFlow::Graph &cfg)
{
    Graph cg;
    build_graph(cfg, cg);
    return cg;
}

void
BinaryAnalysis::FunctionCall::build_graph(SgNode *root, Graph &cg/*out*/)
{
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::map<SgAsmFunctionDeclaration*, Vertex> FunctionVertexMap;
    FunctionVertexMap fv_map;

    cg.clear();

    /* Visiter that adds a vertex for each unique function. */
    struct VertexAdder: public AstSimpleProcessing {
        Graph &cg;
        FunctionVertexMap &fv_map;
        VertexAdder(Graph &cg, FunctionVertexMap &fv_map): cg(cg), fv_map(fv_map) {}
        void visit(SgNode *node) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
            if (func) {
                Vertex vertex = add_vertex(cg);
                fv_map[func] = vertex;
                put(boost::vertex_name, cg, vertex, func);
            }
        }
    };

    /* Visitor that adds edges for each vertex.  Traversal should be over one function at a time. */
    struct EdgeAdder: public AstSimpleProcessing {
        Graph &cg;
        FunctionVertexMap &fv_map;
        EdgeAdder(Graph &cg, FunctionVertexMap &fv_map): cg(cg), fv_map(fv_map) {}
        SgAsmFunctionDeclaration *function_of(SgAsmBlock *block) {
            return block ? block->get_enclosing_function() : NULL;
        }
        void visit(SgNode *node) {
            SgAsmBlock *block_a = isSgAsmBlock(node);
            SgAsmFunctionDeclaration *func_a = function_of(block_a);
            if (func_a) {
                const SgAsmTargetPtrList &succs = block_a->get_successors();
                for (SgAsmTargetPtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si) {
                    SgAsmBlock *block_b = (*si)->get_block();
                    SgAsmFunctionDeclaration *func_b = function_of(block_b);
                    FunctionVertexMap::iterator fvi = fv_map.find(func_b);
                    if (func_b && block_b==func_b->get_entry_block() && fvi!=fv_map.end()) {
                        Vertex a=fv_map[func_a], b=fvi->second;
                        add_edge(a, b, cg);
                    }
                }
            }
        }
    };

    VertexAdder(cg, fv_map).traverse(root, preorder);
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(cg); vi!=vi_end; ++vi) {
        SgAsmFunctionDeclaration *source_func = get(boost::vertex_name, cg, *vi);
        EdgeAdder(cg, fv_map).traverse(source_func, preorder);
    }
}

BinaryAnalysis::FunctionCall::Graph
BinaryAnalysis::FunctionCall::build_graph(SgNode *root)
{
    Graph cg;
    build_graph(root, cg);
    return cg;
}
