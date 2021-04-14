/*
 * Test move function declaration from one namespace to another
by Liao, 1/20/2021
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(project, V_SgNamespaceDefinitionStatement);

  SgNamespaceDefinitionStatement *src= isSgNamespaceDefinitionStatement(nodeList[0]);
  SgNamespaceDefinitionStatement *dest= isSgNamespaceDefinitionStatement(nodeList[1]);

  SageInterface::moveStatementsBetweenBlocks(src, dest);
  return backend(project);
}
