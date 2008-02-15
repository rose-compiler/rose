// Liao, 1/18/2008
// Demostrate how to build variable declaration statements
//  extern int i;
//  int i;
// HighLevel_Sage_Builder_Interface contains the AST nodes/subtrees builders
// SageInterface contains any other AST utitily tools 
//-------------------------------------------------------------------
#include "rose.h"
#include "sageBuilder.h"

using namespace HighLevel_Sage_Builder_Interface;
using namespace SageInterface;

static  SgTreeCopy treeCopy;
int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack (isSgScopeStatement (globalScope));

  // extern int i;
  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());
  ((varDecl->get_declarationModifier()).get_storageModifier()).setExtern();
  append_statement (varDecl);
// two ways to build a same declaration
  // int i;
#if 1
  SgVariableDeclaration *varDecl2 = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());
#else
 // this one is not yet working,may be related to copy mechanism
  SgVariableDeclaration *varDecl2 = isSgVariableDeclaration(deepCopy(varDecl));
  ((varDecl->get_declarationModifier()).get_storageModifier()).setDefault();
#endif 

  append_statement (varDecl2);

  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

