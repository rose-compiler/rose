#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rose.h"

#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>

//#include "DirectedGraph.h"
#include "DependenceGraph.h"

#include "SlicingInfo.h"
#include "CreateSlice.h"

int main (int argc, char * argv[]) {

  SgProject * project = frontend(argc, argv);
  
  SlicingInfo si;
  si.traverse(project, preorder);
  
  SgFunctionDeclaration * func = si.getTargetFunction();
  printf("Target Function: %p (%s)\n", si.getTargetFunction(), 
	 si.getTargetFunction()->get_name().str());
  
  SgStatement * target = si.getSlicingCriterion();
  printf("Target Statement: %p (%s)\n", si.getSlicingCriterion(),
	 si.getSlicingCriterion()->sage_class_name());
  
  list<SgNode *> functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  
  SystemDependenceGraph * sdg = new SystemDependenceGraph;
  
  for (list<SgNode *>::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++) {
    SgFunctionDeclaration * fD = isSgFunctionDeclaration(*i);
    ROSE_ASSERT(fD != NULL);
    
    if (fD->get_definition() == NULL) continue;
    
    ControlDependenceGraph * cdg;
    cdg = new ControlDependenceGraph(fD->get_definition(), new InterproceduralInfo);
    
    DataDependenceGraph * ddg;
    ddg = new DataDependenceGraph(fD->get_definition(), cdg->getInterprocedural());
    //ddg = new DataDependenceGraph(fD->get_definition());
    
    
    printf("CDG for %s:\n", fD->get_name().str());
    cdg->printGraph();
    printf("DDG for %s:\n", fD->get_name().str());
    ddg->printGraph();
    printf("PDG for: %s\n", fD->get_name().str());
    ProgramDependenceGraph * pdg = new ProgramDependenceGraph(cdg, ddg, cdg->getInterprocedural());
    pdg->printGraph();
    
    sdg->addFunction(pdg);
  }
  
  sdg->process();
  printf("\n\nSDG:\n");
  sdg->printGraph();
  sdg->writeDot("sdg.dot");
  
  set<DependenceNode *> slice = sdg->getSlice(sdg->getNode(target));
  set<SgNode *> sgslice = sdg->slice(target);

  printf("Slice: \n");
  for(set<DependenceNode *>::iterator i = slice.begin(); i != slice.end(); i++) {
    DependenceNode * node = *i;
    node->writeOut(cout);
    cout << endl;
  }

  CreateSlice cs(sgslice);
  cs.traverse(project);
  project->unparse();

}

