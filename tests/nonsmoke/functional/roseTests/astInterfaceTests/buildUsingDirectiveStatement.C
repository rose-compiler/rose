#include "rose.h"
#include <string>
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  ROSE_ASSERT(globalScope != NULL);

  // find the first namespace declaration in the input file
  SgNamespaceDeclarationStatement* nsd = findDeclarationStatement<SgNamespaceDeclarationStatement>(project,"year", NULL, false );
  assert (nsd);
  SgUsingDirectiveStatement* decl= buildUsingDirectiveStatement(nsd); 

  appendStatement (decl, globalScope);
 
  AstTests::runAllTests(project);
  return backend (project);
}

