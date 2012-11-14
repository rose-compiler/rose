// Tests the TaintAnalysis genericDataflow features. See src/midend/programAnalysis/genericDataflow/simpleAnalysis/taintAnalysis.h
// for details.
//
// This test is newer than taintedFlowAnalysisTest.C in this same directory.  Also, it uses the taint analysis built into the
// ROSE library rather than the taintedFlowAnalysis.[Ch] features defined in this test directory.

#include "sage3basic.h"
#include "taintAnalysis.h"

static void
usage(const char *arg0, int exit_status)
{
    const char *slash = strrchr(arg0, '/');
    if (slash && slash[1])
        arg0 = slash+1;
    if (!strncmp(arg0, "lt-", 3) && arg0[3])
        arg0 += 3;

    std::ostream &o = exit_status ? std::cerr : std::cout;
    o <<"usage: " <<arg0 <<" [--help] [--debug] [ROSE_AND_COMPILER_SWITCHES] SOURCE_FILES...\n";
    if (exit_status)
        exit(exit_status);
    //---################################################################################ (80 columns)
    o <<"  The --debug switch will cause quite a bit of information to be written to\n"
      <<"  the standard error stream. Most of this information comes from the\n"
      <<"  TaintAnalysis::transfer() method as it is driven by the genericDataflow\n"
      <<"  framework.\n"
      <<"\n"
      <<"  Additional information (regardless of --debug switch) is dumped by the\n"
      <<"  framework into files named index.html, summary.html, and detail.html and a\n"
      <<"  directory named dbg_imgs.  These names are beyond the control of higher\n"
      <<"  layers, so use caution when running tests in parallel in the same directory.\n"
      <<"\n"
      <<"  The results emitted on standard output consist of lines like\n"
      <<"      Function FUNCTION_NAME returns VERTEX value\n"
      <<"  where FUNCTION_NAME is the name of each function that had a return statemnt\n"
      <<"  and VERTEX is the name of the TaintLattice vertex, one of \"bottom\",\n"
      <<"  \"untainted\", or \"tainted\".  A separate line is produced for each\n"
      <<"  return statement.\n";
    exit(0);
}

// Override the transfer() method of TaintAnalysis so we can produce output for each SgReturnStmt.  This is the output we will
// compare against known answers during testing.
class TaintAnalysisTest: public TaintAnalysis {
public:
    TaintAnalysisTest(LiveDeadVarsAnalysis *ldv_analysis): TaintAnalysis(ldv_analysis) {}

    bool transfer(const Function &func, const DataflowNode &node, NodeState &state,
                  const std::vector<Lattice*> &dfInfo) /*override*/ {
        bool modified = TaintAnalysis::transfer(func, node, state, dfInfo);
        SgReturnStmt *xop = isSgReturnStmt(node.getNode());
        if (xop) {
            varID id = SgExpr2Var(xop->get_expression());
            assert(!dfInfo.empty());
            FiniteVarsExprsProductLattice *prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(dfInfo.front());
            assert(prodLat!=NULL);
            TaintLattice *lattice = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(id));
            assert(lattice!=NULL);
            std::cout <<"Function " <<func.get_name() <<" returns " <<lattice->to_string() <<" value\n";
        }
        return modified;
    }
};

int
main(int argc, char *argv[])
{
    // Parse command-line args and remove those that we understand. Pass everything else to ROSE's frontend()
    bool do_debug = false;
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?") || !strcmp(argv[i], "--help")) {
            usage(argv[0], 0);
        } else if (!strcmp(argv[i], "--debug")) {
            do_debug = true;
            memmove(argv+i, argv+i+1, (argc-- -i)*sizeof(*argv)); // argv has argc+1 elements
            --i;
        } else {
            break;
        }
    }
    if (argc<2) // we must have at least one more arg (the specimen name). Depend on frontend() to report other problems.
        usage(argv[0], 1);

    SgProject *project = frontend(argc, argv);
    initAnalysis(project);
    Dbg::init("Taint analysis", ".", "index.html");

    // Generate the call graph
    CallGraphBuilder cg_analyzer(project);
    cg_analyzer.buildCallGraph();
    SgIncidenceDirectedGraph *cg = cg_analyzer.getGraph();

    // Live/Dead variables analysis
    LiveDeadVarsAnalysis ldv_analysis(project);
    UnstructuredPassInterDataflow ldv_dataflow(&ldv_analysis);
    ldv_dataflow.runAnalysis();

    // Taint analysis
    TaintAnalysisTest taint_analysis(&ldv_analysis);
    if (do_debug)
        taint_analysis.set_debug(&std::cerr);
    ContextInsensitiveInterProceduralDataflow taint_interproc(&taint_analysis, cg);
    taint_interproc.runAnalysis();

    return 0;
}
