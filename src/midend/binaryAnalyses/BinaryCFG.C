#include "sage3basic.h"
#include "BinaryCFG.h"

/* See header file for documentation */
RoseBinaryAnalysis::ControlFlowGraph
RoseBinaryAnalysis::build_cfg(SgNode *root)
{
    typedef boost::graph_traits<ControlFlowGraph>::vertex_descriptor Vertex;
    typedef std::map<SgAsmBlock*, Vertex> BlockVertexMap;
    BlockVertexMap bv_map;
    ControlFlowGraph cfg;

    /* Define the vertices. */
    struct T1: public SgSimpleProcessing {
        ControlFlowGraph &cfg;
        BlockVertexMap &bv_map;
        T1(ControlFlowGraph &cfg, BlockVertexMap &bv_map): cfg(cfg), bv_map(bv_map) {}
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block) {
                Vertex vertex = add_vertex(cfg);
                bv_map[block] = vertex;
                put(boost::vertex_name, cfg, vertex, block);
            }
        }
    };
    T1(cfg, bv_map).traverse(root, preorder);

    /* Add the edges. */
    boost::graph_traits<ControlFlowGraph>::vertex_iterator vi, vi_end;
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

    return cfg;
}
