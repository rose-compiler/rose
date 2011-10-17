/* Reads a binary file, disassembles it, and spits out a control flow graph. */
#include "rose.h"
#include "BinaryControlFlow.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with basic block addresses rather than vertex numbers. */
template<class ControlFlowGraph>
struct GraphvizVertexWriter {
    const ControlFlowGraph &cfg;
    GraphvizVertexWriter(ControlFlowGraph &cfg): cfg(cfg) {}
    void operator()(std::ostream &output,
                    const typename boost::graph_traits<ControlFlowGraph>::vertex_descriptor &v) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, v);
        output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
    }
};

/* Filter that accepts only function call edges. */
struct OnlyCallEdges: public BinaryAnalysis::ControlFlow::EdgeFilter {
    bool operator()(BinaryAnalysis::ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
        SgAsmFunction *func = dst->get_enclosing_function();
        return func && dst == func->get_entry_block();
    }
};

/* Filter that rejects basic block that are uncategorized.  I.e., those blocks that were disassemble but not ultimately
 * linked into the list of known functions.  We excluded these because their control flow information is often nonsensical. */
struct ExcludeLeftovers: public BinaryAnalysis::ControlFlow::VertexFilter {
    bool operator()(BinaryAnalysis::ControlFlow *analyzer, SgAsmBlock *block) {
        SgAsmFunction *func = block->get_enclosing_function();
        return func && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS);
    }
};

int
main(int argc, char *argv[])
{
    /* Algorithm is first argument. */
    assert(argc>1);
    std::string algorithm = argv[1];
    memmove(argv+1, argv+2, argc-1); /* also copy null ptr */
    --argc;

    /* Parse the binary file */
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        fprintf(stderr, "no binary interpretations found\n");
        exit(1);
    }

    ExcludeLeftovers exclude_leftovers;

    /* Calculate plain old CFG. */
    if (algorithm=="A") {
        typedef BinaryAnalysis::ControlFlow::Graph CFG;
        BinaryAnalysis::ControlFlow cfg_analyzer;
        cfg_analyzer.set_vertex_filter(&exclude_leftovers);
        CFG cfg = cfg_analyzer.build_cfg_from_ast<CFG>(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter<CFG>(cfg));
    }

    /* Calculate a CFG with only function call edges.  Note that this is not quite the same as
     * BinaryAnalysis::FunctionCall::Graph. */
    if (algorithm=="B") {
        typedef BinaryAnalysis::ControlFlow::Graph CFG;
        BinaryAnalysis::ControlFlow cfg_analyzer;
        cfg_analyzer.set_vertex_filter(&exclude_leftovers);
        CFG cfg = cfg_analyzer.build_cg_from_ast<CFG>(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter<CFG>(cfg));
    }

    /* Build a pseudo call-graph by first building a CFG and then copying it to filter out non-call edges.  The result
     * should be the same as for algorithm B, assuming our edge filter is semantically equivalent. */
    if (algorithm=="C") {
        typedef BinaryAnalysis::ControlFlow::Graph CFG;
        BinaryAnalysis::ControlFlow cfg_analyzer;
        cfg_analyzer.set_vertex_filter(&exclude_leftovers);
        CFG cfg = cfg_analyzer.build_cfg_from_ast<CFG>(interps.back());
        OnlyCallEdges edge_filter;
        cfg_analyzer.set_edge_filter(&edge_filter);
        BinaryAnalysis::ControlFlow::Graph cg = cfg_analyzer.copy(cfg);
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter<CFG>(cg));
    }

    /* Build a pseudo call-graph by defining an edge filter before building a regular call graph.  The result should be the
     * same as for algorithm C since both use the same filter. */
    if (algorithm=="D") {
        typedef BinaryAnalysis::ControlFlow::Graph CFG;
        BinaryAnalysis::ControlFlow cfg_analyzer;
        cfg_analyzer.set_vertex_filter(&exclude_leftovers);
        OnlyCallEdges edge_filter;
        cfg_analyzer.set_edge_filter(&edge_filter);
        CFG cfg = cfg_analyzer.build_cfg_from_ast<CFG>(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter<CFG>(cfg));
    }

    return 0;
};

        
        
    
