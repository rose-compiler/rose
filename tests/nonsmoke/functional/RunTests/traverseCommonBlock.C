/*
 * A test to make sure variable references in Fortran common block are traversable
 * Liao 12/9/2010
 * */
#include "rose.h"

#include <iostream>
using namespace std;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(project,V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp((*i));
    cout<<"Found a variable reference ! "<<endl;
  }

  ROSE_ASSERT (nodeList.size() != 0);

  return backend (project);
}


