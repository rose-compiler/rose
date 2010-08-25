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
#include "CreateSliceSet.h"

#include <list>
#include <set>
#include <iostream>

#define DEBUG 1
using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

int main(int argc, char *argv[])
{
	std::string filename;

	SgProject *project = frontend(argc, argv);
	std::vector<InterproceduralInfo*> ip;
#ifdef NEWDU
	// Create the global def-use analysis
	EDefUse *defUseAnalysis=new EDefUse(project);
	if (defUseAnalysis->run(false)==1)
	{
		std::cerr<<"createSDG:: DFAnalysis failed!  (defUseAnalysis->run(false)==0"<<endl;
		exit(0);
	}
#endif
	string outputFileName=(*(*project->get_fileList()).begin())->get_sourceFileNameWithoutPath ();


	SystemDependenceGraph *sdg = new SystemDependenceGraph;
	// for all function-declarations in the AST
	list < SgNode * >functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);

	for (list < SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
	{
		ControlDependenceGraph *cdg;
		DataDependenceGraph *ddg;
	//	FunctionDependenceGraph * pdg;
		InterproceduralInfo *ipi;

		SgFunctionDeclaration *fDec = isSgFunctionDeclaration(*i);

		ROSE_ASSERT(fDec != NULL);

		// CI (01/08/2007): A missing function definition is an indicator to a 
		// 
		// 
		// librarycall. 
		// * An other possibility would be a programmer-mistake, which we
		// don't treat at this point.  // I assume librarycall
		if (fDec->get_definition() == NULL)
		{/*
//			if (fDec->get_file_info()->isCompilerGenerated()) continue;
			// treat librarycall -> iterprocedualInfo must be created...
			// make all call-parameters used and create a function stub for
			// the graph
			ipi=new InterproceduralInfo(fDec);
			ipi->addExitNode(fDec);
			sdg->addInterproceduralInformation(ipi);
			if (sdg->isKnownLibraryFunction(fDec))
			{
				sdg->createConnectionsForLibaryFunction(fDec);
			}
			else
			{
				sdg->createSafeConfiguration(fDec);
			}
			ip.push_back(ipi);*/

			// This is somewhat a waste of memory and a more efficient approach might generate this when needed, but at the momenent everything is created...
		}
		else
		{
			// get the control depenence for this function
			ipi=new InterproceduralInfo(fDec);

			ROSE_ASSERT(ipi != NULL);

			// get control dependence for this function defintion
			cdg = new ControlDependenceGraph(fDec->get_definition(), ipi);
                        cdg->computeAdditionalFunctioncallDepencencies();
			cdg->computeInterproceduralInformation(ipi);

			// get the data dependence for this function
			ddg = new DataDependenceGraph(fDec->get_definition(), defUseAnalysis,ipi);
			
			sdg->addFunction(cdg,ddg);
			sdg->addInterproceduralInformation(ipi);
			ip.push_back(ipi);
		}   
		// else if (fD->get_definition() == NULL)

	}
	// now all function-declarations have been process as well have all function-definitions
	filename = (outputFileName) + ".no_ii.sdg.dot";
	sdg->writeDot((char *)filename.c_str());
	
	// perform interproceduralAnalysys
	sdg->performInterproceduralAnalysis();

	filename = (outputFileName)+".deadEnds.sdg.dot";
	sdg->writeDot((char *)filename.c_str());			
	set<SgNode*> preserve;
	if (sdg->getMainFunction()!=NULL);
	preserve.insert(sdg->getMainFunction());
	sdg->cleanUp(preserve);
	filename = (outputFileName)+".final.sdg.dot";
	sdg->writeDot((char *)filename.c_str());			
}
