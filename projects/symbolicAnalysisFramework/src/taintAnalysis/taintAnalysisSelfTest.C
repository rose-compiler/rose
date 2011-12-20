#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
#include "genericDataflowCommon.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "constantPropagation.h"
#include "taintAnalysis.h"

class EvaluationPass: public UnstructuredPassIntraAnalysis
{
public:
    ConstantPropagationAnalysis* cpa;
    TaintAnalysis *ta;
    int pass, fail;
    string indent;
    bool failure;
    EvaluationPass(ConstantPropagationAnalysis* that_cpa, TaintAnalysis* that_ta, string _indent="")
        : cpa(that_cpa), ta(that_ta), indent(_indent)
    {
        pass = 0;
        fail = 0;
        failure = false;
    }

    void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

void EvaluationPass::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
    FiniteVarsExprsProductLattice *cpa_lat = dynamic_cast<FiniteVarsExprsProductLattice*> (state.getLatticeAbove(cpa)[0]);
    FiniteVarsExprsProductLattice *ta_lat = dynamic_cast<FiniteVarsExprsProductLattice*> (state.getLatticeAbove(ta)[0]);

    // NOTE: Don't see why the vars should be different
    // below assertion should hold true
    ROSE_ASSERT(cpa_lat->getAllVars().size() == ta_lat->getAllVars().size());
    set<varID> constLatVars = cpa_lat->getAllVars();
    set<varID> taLatVars = ta_lat->getAllVars();

    set<varID>::iterator i, j;
    for(i = constLatVars.begin(), j = taLatVars.begin();
        i != constLatVars.end() && j != taLatVars.end();
        i++, j++) {
        
        // check if we are testing for the same varID
        ROSE_ASSERT((i->str().compare(j->str())) == 0);
        ConstantPropagationLattice *cpaVarLattice = dynamic_cast<ConstantPropagationLattice*> (cpa_lat->getVarLattice(*i));
        TaintLattice *taVarLattice = dynamic_cast<TaintLattice*> (ta_lat->getVarLattice(*j));

        //NOTE: our only taint source is a function returning constant
        if(taVarLattice->getLevel() == TaintLattice::taintyes) {
            if(cpaVarLattice->getLevel() == ConstantPropagationLattice::constantValue) {
                failure = failure || false;
                pass++;
            }
            else {
                failure = true;
                fail++;
            }
            
        }
    }    
}

int main( int argc, char * argv[] ) 
{
    printf("========== S T A R T ==========\n");
        
    // Build the AST used by ROSE
    SgProject* project = frontend(argc,argv);
        
    initAnalysis(project);
    Dbg::init("Taint Analysis", ".", "index.html");

    bool constTaintAnalysisTest = false;

    // get file name of the test
    string filename = (project->getAbsolutePathFileNames())[0];
    size_t ispresent = filename.find("taintFlowTests");

    // if its in the directory taintFlowTests, enable contant + taint analysis testing mechanism
    if(ispresent != string::npos) {
        constTaintAnalysisTest = true;
    }
        
    analysisDebugLevel = 2;

    LiveDeadVarsAnalysis ldva(project);
    UnstructuredPassInterDataflow upid_ldva(&ldva);
    upid_ldva.runAnalysis();

    CallGraphBuilder cgb(project);
    cgb.buildCallGraph();
    SgIncidenceDirectedGraph* graph = cgb.getGraph();    

    SecureFunctionTypeTraversal sftype_traversal;

    if(constTaintAnalysisTest) {
        sftype_traversal.addToUntrustedFunc("constTaintFunc1");
        sftype_traversal.addToUntrustedFunc("constTaintFunc2");
    }
    sftype_traversal.traverse(project, preorder);

    // SecureFunctionTypeTraversalTest sftype_test;
    // sftype_test.traverse(project, preorder);

    TaintAnalysis ta(&ldva);
    ContextInsensitiveInterProceduralDataflow ciipd_ta(&ta, graph);
    ciipd_ta.runAnalysis();    

    // NOTE: Not the best of testing the analysis
    // Method: 1. Create a function returning constant
    //         2. Mark it as taint using SecureFunctionTypeTraversal::addToUntrustedFunc
    //         3. In an evaluation pass, check if the constant lattice and taint lattice match
    // only in test mode, perform the constant propagation analysis
    if(constTaintAnalysisTest) {
        //std::cout << "Testing Taint Analysis using Constant Propagation Analysis..\n";
        ConstantPropagationAnalysis cpa(&ldva);
        ContextInsensitiveInterProceduralDataflow ciipd_cpa(&cpa, graph);
        ciipd_cpa.runAnalysis();
           
        EvaluationPass ep(&cpa, &ta, " ");
        UnstructuredPassInterAnalysis upia_ep(ep);
        upia_ep.runAnalysis();

        if(ep.failure) {
            cout << "TEST FAIL\n";
            cout << ep.fail << " lattice did not match the lattice\n";
        }
        else {
            cout << "TEST PASSED\n";
        }
    }
                      
    return 0;
}
