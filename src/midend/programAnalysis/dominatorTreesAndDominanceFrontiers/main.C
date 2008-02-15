
THIS CODE IS NOT COMPILED!

#include "rose.h"
#include "DominatorTree.h"
#include "DominanceFrontier.h"

int main (int argc, char * argv[]) {

  SgProject * project = frontend(argc, argv);

  list<SgNode *> functionDefinitions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);

  // printf("Getting function definitions\n");

  for (list<SgNode *>::iterator i = functionDefinitions.begin(); i != functionDefinitions.end(); i++) {
    SgFunctionDefinition * fD = isSgFunctionDefinition(*i);
    ROSE_ASSERT(fD != NULL);

    // printf("Building CFG\n");

    DominatorTree dt(fD, DominatorTree::POST);
    dt.printCFG();
    //dt.printDominators();

    dt.getCFG()->outputCFGImpl();

    DominanceFrontier df(&dt);
    df.printFrontier();
  }

  return 0;
}
