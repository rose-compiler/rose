/*
 * a strange bug related to missing the scope of the 2nd parameter of __builtin_prefetch()
 *  Liao
 *  8/26/2009
 */
#include "rose.h"
#include <iostream>
using namespace std;

int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);

  printf("Running ROSE...");
  Rose_STL_Container<SgNode*> initNames = NodeQuery::querySubTree(project, V_SgInitializedName);
  for (Rose_STL_Container<SgNode*>::const_iterator i = initNames.begin(); i != initNames.end(); ++i) {
    SgInitializedName* iName = isSgInitializedName(*i);
    string name = iName->get_qualified_name().str();
	printf(" Found initName : %s",name);
  }

  return backend (project);
}
