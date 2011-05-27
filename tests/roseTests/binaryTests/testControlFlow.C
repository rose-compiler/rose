/* Reads a binary file, disassembles it, and spits out a control flow graph. */
#include "rose.h"
#include "BinaryControlFlow.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with basic block addresses rather than vertex numbers. */
struct GraphvizVertexWriter {
    const BinaryAnalysis::ControlFlow::Graph &cfg;
    GraphvizVertexWriter(BinaryAnalysis::ControlFlow::Graph &cfg): cfg(cfg) {}
    void operator()(std::ostream &output, const BinaryAnalysis::ControlFlow::Vertex &v) {
        SgAsmBlock *block = get(boost::vertex_name, cfg, v);
        output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
    }
};

/* Filter that accepts only function call edges. */
struct OnlyCallEdges: public BinaryAnalysis::ControlFlow::EdgeFilter {
    bool operator()(BinaryAnalysis::ControlFlow *analyzer, SgAsmBlock *src, SgAsmBlock *dst) {
        SgAsmFunctionDeclaration *func = dst->get_enclosing_function();
        return func && dst == func->get_entry_block();
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

    /* Calculate plain old CFG. */
    if (algorithm=="A") {
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_graph(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter(cfg));
    }

    /* Calculate a CFG with only function call edges.  Note that this is not quite the same as
     * BinaryAnalysis::FunctionCall::Graph. */
    if (algorithm=="B") {
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_call_graph(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter(cfg));
    }

    /* Build a pseudo call-graph by first building a CFG and then copying it to filter out non-call edges.  The result
     * should be the same as for algorithm B, assuming our edge filter is semantically equivalent. */
    if (algorithm=="C") {
        BinaryAnalysis::ControlFlow cfg_analyzer;
        BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_graph(interps.back());
        OnlyCallEdges edge_filter;
        cfg_analyzer.set_edge_filter(&edge_filter);
        BinaryAnalysis::ControlFlow::Graph cg = cfg_analyzer.copy(cfg);
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter(cg));
    }

    /* Build a pseudo call-graph by defining an edge filter before building a regular call graph.  The result should be the
     * same as for algorithm C since both use the same filter. */
    if (algorithm=="D") {
        BinaryAnalysis::ControlFlow cfg_analyzer;
        OnlyCallEdges edge_filter;
        cfg_analyzer.set_edge_filter(&edge_filter);
        BinaryAnalysis::ControlFlow::Graph cfg = cfg_analyzer.build_graph(interps.back());
        boost::write_graphviz(std::cout, cfg, GraphvizVertexWriter(cfg));
    }

    return 0;
};

        
        
    
