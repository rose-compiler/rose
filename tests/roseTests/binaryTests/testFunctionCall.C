/* Reads a binary file, disassembles it, and performs various call graph analyses. */
#include "rose.h"
#include "BinaryFunctionCall.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with function entry addresses rather than vertex numbers. */
struct GraphvizVertexWriter {
    const BinaryAnalysis::FunctionCall::Graph &g;
    GraphvizVertexWriter(BinaryAnalysis::FunctionCall::Graph &g): g(g) {}
    void operator()(std::ostream &output, const BinaryAnalysis::FunctionCall::Vertex &v) {
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
        BinaryAnalysis::FunctionCall cg_analyzer;
        BinaryAnalysis::FunctionCall::Graph cg = cg_analyzer.build_graph(interps.back());
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter(cg));
    }

    /* Calculate the call graph from the control flow graph. */
    if (algorithm=="B") {
        BinaryAnalysis::ControlFlow::Graph cfg = BinaryAnalysis::ControlFlow().build_graph(interps.back());
        BinaryAnalysis::FunctionCall cg_analyzer;
        BinaryAnalysis::FunctionCall::Graph cg = cg_analyzer.build_graph(cfg);
        boost::write_graphviz(std::cout, cg, GraphvizVertexWriter(cg));
    }

    return 0;
}
