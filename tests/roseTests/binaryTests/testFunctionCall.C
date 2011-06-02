/* Reads a binary file, disassembles it, and performs various call graph analyses. */
#include "rose.h"
#include "BinaryFunctionCall.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with function entry addresses rather than vertex numbers. */
template<class FunctionCallGraph>
struct GraphvizVertexWriter {
    typedef typename boost::graph_traits<FunctionCallGraph>::vertex_descriptor Vertex;
    const FunctionCallGraph &g;
    GraphvizVertexWriter(FunctionCallGraph &g): g(g) {}
    void operator()(std::ostream &output, const Vertex &v) {
        SgAsmFunctionDeclaration *func = get(boost::vertex_name, g, v);
        output <<"[ label=\"" <<StringUtility::addrToString(func->get_entry_va()) <<"\" ]";
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

    /* Calculate plain old CG over entire interpretation. */
    if (algorithm=="A") {
        typedef BinaryAnalysis::FunctionCall::Graph CG;
        BinaryAnalysis::FunctionCall cg_analyzer;
        CG cg = cg_analyzer.build_cg_from_ast<CG>(interps.back());
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter<CG>(cg));
    }

    /* Calculate the call graph from the control flow graph. */
    if (algorithm=="B") {
        typedef BinaryAnalysis::ControlFlow::Graph CFG;
        typedef BinaryAnalysis::FunctionCall::Graph CG;
        CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(interps.back());
        BinaryAnalysis::FunctionCall cg_analyzer;
        CG cg = cg_analyzer.build_cg_from_cfg<CG>(cfg);
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter<CG>(cg));
    }

    return 0;
}
