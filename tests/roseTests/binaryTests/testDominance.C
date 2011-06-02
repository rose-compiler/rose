/* Reads a binary file, disassembles it, and performs various dominance analyses. */
#include "rose.h"
#include "BinaryDominance.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with basic block addresses rather than vertex numbers. */
template<class DominanceGraph>
struct GraphvizVertexWriter {
    const DominanceGraph &g;
    GraphvizVertexWriter(DominanceGraph &g): g(g) {}
    void operator()(std::ostream &output,
                    const typename boost::graph_traits<DominanceGraph>::vertex_descriptor &v) {
        SgAsmBlock *block = get(boost::vertex_name, g, v);
        output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
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

    static const std::string nameprefix = "testDominance-" + algorithm;

    /* Parse the binary file */
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    if (interps.empty()) {
        fprintf(stderr, "no binary interpretations found\n");
        exit(1);
    }
    SgAsmInterpretation *interp = interps.back();

    /* Calculate the control flow graph over the entire interpretation. */
    typedef BinaryAnalysis::ControlFlow::Graph CFG;
    typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
    CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(interp);

    /* Calculate immediate dominator graph from the control flow graph. Do this for each function. */
    if (algorithm=="A") {
        typedef BinaryAnalysis::Dominance::Graph DG;

        struct TA: public AstSimpleProcessing {
            int counter;
            TA(): counter(0) {}
            void visit(SgNode *node) {
                SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
                if (func) {
                    std::cout <<"Function <" <<func->get_name() <<">"
                              <<" at " <<StringUtility::addrToString(func->get_entry_va()) <<"\n";
                    CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
                    CFG_Vertex start = 0;
                    assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
                    BinaryAnalysis::Dominance analyzer;
                    DG dg = analyzer.build_idom_graph_from_cfg<DG>(cfg, start);
                    std::string fname = nameprefix + StringUtility::numberToString(++counter) + ".dot";
                    std::ofstream out(fname.c_str());
                    boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));
                }
            }
        };
        TA().traverse(interp, preorder);
    }

    /* Calculate immediate dominator graph from dominator relation map. Do this for each function. */
    if (algorithm=="B") {
        typedef BinaryAnalysis::Dominance::Graph DG;
        typedef BinaryAnalysis::Dominance::RelationMap<CFG> RelMap;

        struct TB: public AstSimpleProcessing {
            int counter;
            TB(): counter(0) {}
            void visit(SgNode *node) {
                SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
                if (func) {
                    std::cout <<"Function <" <<func->get_name() <<">"
                              <<" at " <<StringUtility::addrToString(func->get_entry_va()) <<"\n";
                    CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
                    CFG_Vertex start = 0;
                    assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
                    BinaryAnalysis::Dominance analyzer;
                    RelMap rmap = analyzer.build_idom_relation_from_cfg(cfg, start);
                    DG dg = analyzer.build_idom_graph_from_relation<DG>(cfg, rmap);
                    std::string fname = nameprefix + StringUtility::numberToString(++counter) + ".dot";
                    std::ofstream out(fname.c_str());
                    boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));
                }
            }
        };
        TB().traverse(interp, preorder);
    }

    return 0;
}
