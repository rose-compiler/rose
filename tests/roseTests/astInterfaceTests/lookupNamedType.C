// Liao, 3/12/2008
// Demostrate how to build variable declaration statements
// - topdown construction: go to the target scope, build vardecl with scope info.
//   - extern int i;
//   - int i;
// - bottomup construction: build vardecl directly, then insert it into scope
//   - int j;
//-------------------------------------------------------------------
#include "rose.h"
using namespace std;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = SageInterface::getFirstGlobalScope (project);

  SgType* sgtype = SageInterface::lookupNamedTypeInParentScopes("name3",globalScope);
  cout<<sgtype->unparseToString()<<endl;
	    
  return backend (project);
}

