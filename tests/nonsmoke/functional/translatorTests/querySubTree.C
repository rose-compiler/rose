/*
 * Test if the hidden first non-defining declaration can be found by AST query. It should not.
by Liao, 12/14/2012
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
  int counter = 0;
  SgProject *project = frontend (argc, argv);
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(project, V_SgDeclarationStatement);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgFunctionDeclaration*decl= isSgFunctionDeclaration((*i));
    // cout<<"decl: "<<decl<< " "<< decl->unparseToString()<<endl;
    if (decl) 
      if (decl->get_name()==string("foo"))
        counter ++;
  }

  // We expect two references 
  if (counter !=1)
  {
    cerr<<"Error. We should find exactly one function declaration. But we found: "<< counter<<endl;
    ROSE_ASSERT (false);
  }

  return backend(project);
}
