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

  ROSE_ASSERT(globalScope != NULL);

// DQ (9/28/2009): Tracking down GNU 4.0.x compiler problem!
// SageBuilder::clearScopeStack();
// SageBuilder::pushScopeStack (globalScope);

  // bottom up for volatile int j; no previous knowledge of target scope
  SgVariableDeclaration *varDecl0 = buildVariableDeclaration("j", 
    buildVolatileType(buildIntType()));

  //const int jc = 0;
  SgVariableDeclaration *varDecl0c = buildVariableDeclaration("jc", 
    buildConstType(buildIntType()), buildAssignInitializer(buildIntVal(0)));

  // int * restrict p;
  SgVariableDeclaration *varDecl0p = buildVariableDeclaration("jp", 
//    buildRestrictType(buildIntType())); 
    buildRestrictType(buildPointerType(buildIntType()))); 

  // top down for others; set implicit target scope info. in scope stack.
  pushScopeStack (isSgScopeStatement (globalScope));

  // extern int i;
  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), buildIntType());
  ((varDecl->get_declarationModifier()).get_storageModifier()).setExtern();
  appendStatement (varDecl);
// two ways to build a same declaration
  // int i;
#if 1
  SgVariableDeclaration *varDecl2 = buildVariableDeclaration
    (SgName ("i"), buildIntType());
#else
 // this one does not yet working,maybe related to copy mechanism
  SgVariableDeclaration *varDecl2 = isSgVariableDeclaration(deepCopy(varDecl));
  ((varDecl->get_declarationModifier()).get_storageModifier()).setDefault();
#endif 

  appendStatement (varDecl2);
  insertStatementAfter(varDecl2,varDecl0);
  prependStatement(varDecl0c);
  prependStatement(varDecl0p);

  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

