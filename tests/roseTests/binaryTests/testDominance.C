/* Reads a binary file, disassembles it, and performs various dominance analyses. */
#include "rose.h"
#include "BinaryDominance.h"
#include "AsmFunctionIndex.h"

#include <boost/graph/graphviz.hpp>

/* Label the graphviz vertices with basic block addresses rather than vertex numbers. */
template<class DominanceGraph>
struct GraphvizVertexWriter {
    const DominanceGraph &g;
    GraphvizVertexWriter(DominanceGraph &g): g(g) {}
    void operator()(std::ostream &output,
                    const typename boost::graph_traits<DominanceGraph>::vertex_descriptor &v) {
        SgAsmBlock *block = get(boost::vertex_name, g, v);
        if (block)
            output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
    }
};

int
main(int argc, char *argv[])
{
    /* Algorithm is first argument.  It is a letter and can optionally be followed by a colon and either an address or
     * function name to restrict the test to a single function. */
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
    SgAsmInterpretation *interp = interps.back();

    // Get the list of all functions except for the FUNC_LEFTOVERS
    std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        if ((*fi)->get_reason() & SgAsmFunction::FUNC_LEFTOVERS)
            *fi = NULL;
    }
    functions.erase(std::remove(functions.begin(), functions.end(), (SgAsmFunction*)0), functions.end());

    // Keep only functions matching user-specified criteria (address or name)
    if (algorithm.size()>1) {
        assert(':'==algorithm[1] && algorithm.size()>2);
        char *rest;
        rose_addr_t function_va = strtoull(algorithm.c_str()+2, &rest, 0);
        std::string function_name = *rest ? algorithm.substr(2) : "";
        for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            if ((function_va && function_va!=(*fi)->get_entry_va()) ||
                (!function_name.empty() && 0!=function_name.compare((*fi)->get_name())))
                *fi = NULL;
        }
        functions.erase(std::remove(functions.begin(), functions.end(), (SgAsmFunction*)0), functions.end());
        if (functions.empty()) {
            std::cerr <<"no function matching " <<algorithm.substr(2) <<"\n"
                      <<"available functions:\n"
                      <<AsmFunctionIndex(interp);
            exit(1);
        }
        algorithm = algorithm[0];
    }

    // Calculate the control flow graph over the entire interpretation.
    typedef BinaryAnalysis::ControlFlow::Graph CFG;
    typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;

    // Run the algorithm over each selected function
    static const std::string nameprefix = "testDominance-" + algorithm;
    int counter=0;
    for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        SgAsmFunction *func = *fi;
        std::string fname = nameprefix + StringUtility::numberToString(++counter) + ".dot";
        std::cout <<std::string(80, '=') <<"\n"
                  <<"test " <<fname <<" in function <" <<func->get_name() <<">"
                  <<" at " <<StringUtility::addrToString(func->get_entry_va()) <<"\n";
#if 0 // This doesn't work well for automated testing
        std::ofstream out(fname.c_str());
#else
        std::ostream &out = std::cout;
#endif

        if (algorithm=="A") {
            // Calculate immediate dominator graph from the control flow graph. Do this for each function.
            typedef BinaryAnalysis::Dominance::Graph DG;
            CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
            CFG_Vertex start = 0;
            assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            BinaryAnalysis::Dominance analyzer;
            DG dg = analyzer.build_idom_graph_from_cfg<DG>(cfg, start);
            boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));

        } else if (algorithm=="B") {
            // Calculate immediate dominator graph from dominator relation map. Do this for each function.
            typedef BinaryAnalysis::Dominance::Graph DG;
            typedef BinaryAnalysis::Dominance::RelationMap<CFG> RelMap;
            CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
            CFG_Vertex start = 0;
            assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            BinaryAnalysis::Dominance analyzer;
            RelMap rmap = analyzer.build_idom_relation_from_cfg(cfg, start);
            DG dg = analyzer.build_graph_from_relation<DG>(cfg, rmap);
            boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));

        } else if (algorithm=="C") {
            // Calculate immediate post dominator graph from the control flow graph. Do this for each function.
            typedef BinaryAnalysis::Dominance::Graph DG;
            CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
            CFG_Vertex start = 0;
            assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            BinaryAnalysis::Dominance analyzer;
            DG dg = analyzer.build_postdom_graph_from_cfg<DG>(cfg, start);
            boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));

        } else if (algorithm=="D") {
            // Calculate immediate post dominator graph from post dominator relation map. Do this for each function.
            typedef BinaryAnalysis::Dominance::Graph DG;
            typedef BinaryAnalysis::Dominance::RelationMap<CFG> RelMap;
            CFG cfg = BinaryAnalysis::ControlFlow().build_cfg_from_ast<CFG>(func);
            CFG_Vertex start = 0;
            assert(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            BinaryAnalysis::Dominance analyzer;
            RelMap rmap = analyzer.build_postdom_relation_from_cfg(cfg, start);
            DG dg = analyzer.build_graph_from_relation<DG>(cfg, rmap);
            boost::write_graphviz(out, dg, GraphvizVertexWriter<DG>(dg));

        } else {
            std::cerr <<"unknown algorithm: " <<algorithm <<"\n";
            exit(1);
        }
    }

    return 0;
}
