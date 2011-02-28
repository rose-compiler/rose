#include "rose.h"

#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"

#include "SlicingInfo.h"
#include "CreateSlice.h"
#include "ControlFlowGraph.h"
#include "DominatorTree.h"


#include <list>
#include <set>
#include <iostream>
#include "DefUseAnalysis.h"

#define DEBUG 1
using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

int main(int argc, char *argv[])
{
        std::string filename;

    SgProject *project = frontend(argc, argv);
#ifdef NEWDU
                // Create the global def-use analysis
                DFAnalysis *defUseAnalysis=new DefUseAnalysis(project);
                if (defUseAnalysis->run(false)==1)
                {
                        std::cerr<<"testDDG:: DFAnalysis failed! -- defUseAnalysis->run(false)==0"<<endl;
                        exit(0);
                }
#endif
                std::vector<InterproceduralInfo*> ip;

    list < SgNode * >functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);

    for (list < SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
    {
        DataDependenceGraph *ddg;
        InterproceduralInfo *ipi;

        SgFunctionDeclaration *fD = isSgFunctionDeclaration(*i);

        // SGFunctionDefinition * fDef;
        ROSE_ASSERT(fD != NULL);

        // CI (01/08/2007): A missing function definition is an indicator to a 
        // 
        // 
        // librarycall. 
        // * An other possibility would be a programmer-mistake, which we
        // don't treat at this point.  // I assume librarycall
        if (fD->get_definition() == NULL)
        {
        }
        else
        {
            // get the control depenence for this function
                                                ipi=new InterproceduralInfo(fD);

            ROSE_ASSERT(ipi != NULL);

            // get the data dependence for this function
#ifdef NEWDU
            ddg = new DataDependenceGraph(fD->get_definition(), defUseAnalysis);
#else
            ddg = new DataDependenceGraph(fD->get_definition());
#endif
                                                //printf("DDG for %s:\n", fD->get_name().str());
                                                
            filename = (fD->get_definition()->get_file_info()->get_filenameString ()) + "." +
                (fD->get_name().getString()) + ".ddg.dot";
            ddg->writeDot((char *)filename.c_str());

        }   
    }
                return 0;
}
