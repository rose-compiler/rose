#include "rose.h"

#include "AstInterface.h"
#include "StmtInfoCollect.h"
#include "ReachingDefinition.h"
#include "DefUseChain.h"
#include "DirectedGraph.h"
#include "DependenceGraph.h"

#include "SlicingInfo.h"
#include "CreateSlice.h"

#include <set>
#include <iostream>
using namespace std;

int main(int argc, char * argv[]) {

  SgProject * project = frontend(argc, argv);

  SlicingInfo si;
  si.traverse(project, preorder);

  SgFunctionDeclaration * func = si.getTargetFunction();
  printf("Target Function: %p (%s)\n", si.getTargetFunction(), 
	 si.getTargetFunction()->get_name().str());

  SgStatement * target = si.getSlicingCriterion();
  printf("Target Statement: %p (%s)\n", si.getSlicingCriterion(),
	 si.getSlicingCriterion()->sage_class_name());

//   GenerateCDG g_cdg;

//   g_cdg.traverse(func);

//   g_cdg.getCDG()->printGraph();

//   GenerateDDG g_ddg;
//   g_ddg.buildDefUseChains(func);
//   g_ddg.buildDDG();

//   // DependenceGraph PDG;

// //   PDG.merge(g_cdg.getCDG());
// //   PDG.merge(g_ddg.getDDG());

// //   set<SgNode *> nodes = PDG.getReachable(target, DependenceGraph::BACKWARD);
// //   //set<SgNode *> nodes = PDG.getReachable(target, DependenceGraph::FORWARD);

//   ProgramDependenceGraph PDG(g_cdg.getCDG(), g_ddg.getDDG());

//   set<SgNode *> nodes = PDG.getBackwardSlice(target);

//   set<SgNode *>::iterator i;
//   printf("\n");
//   for (i = nodes.begin(); i != nodes.end(); i++) {
//     printf("Affected node: %p (%s)\n", *i, (*i)->sage_class_name());
//   }

//   CreateSlice cs(nodes);
//   cs.traverse(func, preorder);

  //project->unparse();

  printf("Building DDG\n");
  DataDependenceGraph * ddg;
  ddg = new DataDependenceGraph(func->get_definition());
  ddg->printGraph();
  //project->unparse();

  printf("Building CFG\n");
  ControlDependenceGraph * cdg;
  cdg = new ControlDependenceGraph(func->get_definition());
  cdg->printGraph();
  //project->unparse();

  printf("Building PDG\n");
  ProgramDependenceGraph * pdg = new ProgramDependenceGraph(cdg, ddg);
  pdg->printGraph();
  //project->unparse();

  printf("Finding reachable\n");
  DependenceNode * sliceStart = pdg->getNode(target);
  ROSE_ASSERT(sliceStart != NULL);
  set < SimpleDirectedGraphNode *> sliceNodes = pdg->getReachable(sliceStart,GraphAccess::EdgeIn);
  //project->unparse();

  printf("SLICE:\n");

  set <SgNode *> sliceSgNodes;
  for (set< SimpleDirectedGraphNode *>::iterator i = sliceNodes.begin(); i != sliceNodes.end(); i++) 
	{
    DependenceNode * dN = dynamic_cast<DependenceNode *>(*i);
    dN->writeOut(cout);
    ROSE_ASSERT(dN->getNode() != NULL);
    sliceSgNodes.insert(dN->getNode());
    cout << endl;
  }
  //project->unparse();

  printf("Setting up for unparse\n");
  CreateSlice cs(sliceSgNodes);
  cs.traverse(func);
  project->unparse();

  return 0;

  /*  DirectedGraph<int> * g1 = new DirectedGraph<int>;
  DirectedGraph<int> * g2 = new DirectedGraph<int>;

  g1->addLink(1, 2);
  g1->addLink(1, 3);

  cout << "G1:\n\n";
  g1->printGraph();

  g2->addLink(2, 4);
  g2->addLink(2, 3);
  cout << "G2:\n\n";
  g2->printGraph();

  g1->merge(g2);
  cout << "Merge:\n\n";
  g1->printGraph();

  set<int> blah = g1->getReachable(4, DirectedGraph<int>::BACKWARD);
  set<int>::iterator i;
  for (i = blah.begin(); i != blah.end(); i++) {
    printf("%d ", *i);
  }
  printf("\n");
  */
}
