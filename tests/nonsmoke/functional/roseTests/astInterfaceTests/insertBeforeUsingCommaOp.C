#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);

  Rose_STL_Container<SgNode*> funcRefList = NodeQuery::querySubTree (project,V_SgFunctionRefExp);

  Rose_STL_Container<SgNode*>::iterator i = funcRefList.begin();
  while (i != funcRefList.end())
  {
    SgFunctionRefExp * func_ref = isSgFunctionRefExp(*i);
    ROSE_ASSERT (func_ref != NULL);
    SgFunctionSymbol* func_sym = isSgFunctionSymbol(func_ref->get_symbol_i());
    SgName functionName = func_sym->get_name();

    if (functionName == "fooA")
    {
      SgFunctionCallExp * func_call = isSgFunctionCallExp(func_ref->get_parent());
      insertBeforeUsingCommaOp(buildVarRefExp("a", getScope(func_ref)), func_call);
    }
    i++;
  }

  return backend (project);
}
