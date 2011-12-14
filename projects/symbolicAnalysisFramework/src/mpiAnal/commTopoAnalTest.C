/*
 * Generic path-sensitive analysis that uses the PartitionedAnalysis framework to create a 
 * separate intra-procedural analysis for every if statement in the function
 */

#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "divAnalysis.h"
#include "sgnAnalysis.h"
//#include "intArithLogical.h"
#include "saveDotAnalysis.h"
#include "partitionedAnalysis.h"
#include "contProcMatchAnalysis.h"
#include "rankDepAnalysis.h"
#include "ranknpDepIfMeet.h"

class MPISideffectUses : public funcSideEffectUses
{
        MPIRankDepAnalysis* rda;
        
        public:
        MPISideffectUses(MPIRankDepAnalysis* rda) : rda(rda) {}
        
        // Returns the set of variables that are used in a call to the given function for which a body has not been provided
        set<varID> usedVarsInFunc(const Function& func, const DataflowNode& n, NodeState& state)
        {
                set<varID> uses;
                
                // If this is an MPI function
                if(func.get_name().getString() == "MPI_Init" || 
                        func.get_name().getString() == "MPI_Send" || 
                        func.get_name().getString() == "MPI_Recv" || 
                        func.get_name().getString() == "MPI_Isend" || 
                        func.get_name().getString() == "MPI_Irecv" || 
                        func.get_name().getString() == "MPI_Wait" || 
                        func.get_name().getString() == "MPI_Test" || 
                        func.get_name().getString() == "MPI_Comm_rank" || 
                        func.get_name().getString() == "MPI_Comm_size")
                {
                        FiniteVarsExprsProductLattice* rdaL = dynamic_cast<FiniteVarsExprsProductLattice*>(state.getLatticeBelow(rda, 0));
                        // Set varsToInclude to be the variables that are dependent on the rank or numproce, even if they're not live
                        
                        return rdaL->getAllVars();
                }
        }
};

int numFails=0;

int main( int argc, char * argv[] ) 
{
        printf("========== S T A R T ==========\n");
        
        // Build the AST used by ROSE
        SgProject* project = frontend(argc,argv);
        
        cfgUtils::initCFGUtils(project);

        initAnalysis(project);
        Dbg::init("Communication Topology Analysis Test", "/p/lscratcha/bronevet/dbg", "index.html");   
        
        SaveDotAnalysis sda;
        UnstructuredPassInterAnalysis upia_sda(sda);
        upia_sda.runAnalysis();
        
        Dbg::enterFunc("Rank Dependence Analysis");
        analysisDebugLevel = 0;
        MPIRankDepAnalysisDebugLevel = 0;
        /*printf("*************************************************************\n");
        printf("***************   MPI Rank Dependence Analysis **************\n");
        printf("*************************************************************\n");*/
        MPIRankDepAnalysis* rda = runMPIRankDepAnalysis(getCallGraph());
        
        printMPIRankDepAnalysisStates("");
        Dbg::exitFunc("Rank Dependence Analysis");
        
        Dbg::enterFunc("Live/Dead Variable Analysis");
        liveDeadAnalysisDebugLevel = 0;
        analysisDebugLevel = 0;
        /*printf("*************************************************************\n");
        printf("*****************   Live/Dead Variable Analysis   *****************\n");
        printf("*************************************************************\n");*/
        MPISideffectUses sideEffector(rda);
        LiveDeadVarsAnalysis ldva(project, &sideEffector);
        ContextInsensitiveInterProceduralDataflow ciipd_ldva(&ldva, getCallGraph());
        //UnstructuredPassInterDataflow ciipd_ldva(&ldva);
        ciipd_ldva.runAnalysis();
        //printLiveDeadVarsAnalysisStates(&ldva, "");
        Dbg::exitFunc("Live/Dead Variable Analysis");

        Dbg::enterFunc("Divisibility Analysis");
        analysisDebugLevel = 0;
        /*printf("*************************************************************\n");
        printf("*****************   Divisibility Analysis   *****************\n");
        printf("*************************************************************\n");*/
        DivAnalysis da(&ldva);
        //ContextInsensitiveInterProceduralDataflow ciipd_da(&da, getCallGraph());
        UnstructuredPassInterDataflow ciipd_da(&da);
        ciipd_da.runAnalysis();
        
        //printDivAnalysisStates(&da, "");
        Dbg::exitFunc("Divisibility Analysis");
        
        //analysisDebugLevel = 0;
        //
        //printf("*************************************************************\n");
        //printf("******************   Affine Inequalities   ******************\n");
        //printf("*************************************************************\n");
        //runAffineIneqPlacer(true);
        
        /*analysisDebugLevel = 1;
        
        printf("*************************************************************\n");
        printf("**********   Integer-Arithmetic-Logical Expressions *********\n");
        printf("*************************************************************\n");
        runIntArithLogicalPlacer(true);*/

        /*analysisDebugLevel = 0;
        
        printf("*************************************************************\n");
        printf("************** MPI Rank IfMeetDetector Analysis *************\n");
        printf("*************************************************************\n");
        
        runRankDepIfMeetDetector(false);*/
        
        Dbg::enterFunc("IfMeetDetector Analysis");
        analysisDebugLevel = 0;
        
        /*printf("*************************************************************\n");
        printf("**************     IfMeetDetector Analysis      *************\n");
        printf("*************************************************************\n");*/
        
        runIfMeetDetector(false);
        Dbg::exitFunc("IfMeetDetector Analysis");
        
        analysisDebugLevel = 1;
        MPIAnalysisDebugLevel = 1;
        
        /*{
                set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
                // iterate over all functions with bodies
                for(set<FunctionState*>::iterator fState=allFuncs.begin(); fState!=allFuncs.end(); fState++)
                {       
                        cout << "Function "<<(*fState)->func.str()<<"\n";
                        if((*fState)->func.get_definition()) {
                                DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG((*fState)->func.get_definition());
                                cout << "    ldva state="<<NodeState::getNodeState(funcCFGStart)->str(NULL, "    ")<<"\n";
                        }
                }
        }*/
                
        Dbg::enterFunc("Communication Topology Dataflow Analysis");     
        /*printf("***************************************************************\n");
        printf("********** Communication Topology Dataflow Analysis ***********\n");
        printf("***************************************************************\n");*/
        /*commTopoPartitionedAnalysis sfwpa(&ldva, &da);
        UnstructuredPassInterAnalysis upia_sfwpa(sfwpa);
        upia_sfwpa.runAnalysis();*/
        pCFG_contProcMatchAnalysis sfwpa(&ldva, &da, rda);
        UnstructuredPassInterAnalysis upia_sfwpa(sfwpa);
        upia_sfwpa.runAnalysis();
        
        vector<int> factNames;
        vector<int> latticeNames;
        latticeNames.push_back(0);
        //printf("Master Analysis = %p\n", sfwpa.getMasterDFAnalysis());
        //printAnalysisStates pas(sfwpa.getMasterDFAnalysis(), factNames, latticeNames, printAnalysisStates::below, ":");
        printAnalysisStates pas(&sfwpa, factNames, latticeNames, printAnalysisStates::below, ":");
        UnstructuredPassInterAnalysis upia_pas(pas);
        upia_pas.runAnalysis();
        Dbg::enterFunc("Communication Topology Dataflow Analysis");     

        if(numFails==0)
                printf("PASS\n");
        else
                printf("FAIL!\n");
        
        printf("==========  E  N  D  ==========\n");
        
        // Unparse and compile the project (so this can be used for testing)
        return backend(project);
}
