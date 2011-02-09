/*
 * Test if the clause variables can be visited during an AST traversal
by Liao, 9/27/2010
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  SgFunctionDeclaration* func_decl = SageInterface::findDeclarationStatement<SgFunctionDeclaration> 
     (project, "foo", NULL, true);
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(func_decl->get_definition(), V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp((*i));
    cout<<"varRefExp: "<< vRef->unparseToString()<<endl;
  }

  // We expect two references 
  // from input_ompVariableCollecting.C
  if (nodeList.size() !=2)
  {
    cerr<<"Error. We should find exactly two variable references."<<endl;
  }
  ROSE_ASSERT (nodeList.size() ==2);

  return backend(project);
}
