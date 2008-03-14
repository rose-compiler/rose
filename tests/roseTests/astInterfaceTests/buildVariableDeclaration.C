// Liao, 1/18/2008
// Demostrate how to build variable declaration statements
// - topdown construction: go to the target scope, build vardecl with scope info.
//   - extern int i;
//   - int i;
// - bottomup construction: build vardecl directly, then insert it into scope
//   - int j;
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utitily tools 
//-------------------------------------------------------------------
#include "rose.h"
#include <string>
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);

  // bottom up for int j; no previous knowledge of target scope
  SgVariableDeclaration *varDecl0 = buildVariableDeclaration("j", buildIntType());

  // top down for others; set implicit target scope info. in scope stack.
  pushScopeStack (isSgScopeStatement (globalScope));

  // extern int i;
  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());
  ((varDecl->get_declarationModifier()).get_storageModifier()).setExtern();
  appendStatement (varDecl);
// two ways to build a same declaration
  // int i;
#if 1
  SgVariableDeclaration *varDecl2 = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());
#else
 // this one does not yet working,maybe related to copy mechanism
  SgVariableDeclaration *varDecl2 = isSgVariableDeclaration(deepCopy(varDecl));
  ((varDecl->get_declarationModifier()).get_storageModifier()).setDefault();
#endif 

  appendStatement (varDecl2);
  insertStatementAfter(varDecl2,varDecl0);
  //prependStatement(varDecl0);

  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

