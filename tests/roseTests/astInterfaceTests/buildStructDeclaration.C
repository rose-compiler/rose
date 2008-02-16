// Liao, 2/13/2008
// Demostrate how to build a struct declaration
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utitily tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // build a struct without any scope information
  // cannot do it here, since frontend() will traverse memery pool and find the dangling decl.
  //SgClassDeclaration * decl2 = buildStructDeclaration("bar");

  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);

  // build a struct without any scope information
  SgClassDeclaration * decl2 = buildStructDeclaration("bar");

  pushScopeStack (isSgScopeStatement (globalScope));

  // build struct declaration 
  SgClassDeclaration * decl = buildStructDeclaration("foo");

  // build member variables inside the structure
  SgClassDefinition *def = decl->get_definition();
  pushScopeStack (isSgScopeStatement (def));

  SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("i"), buildIntType());

 // Insert the  memeber variable
  appendStatement (varDecl);

   // build a member function of the construct
  SgInitializedName* arg1 = buildInitializedName(SgName("x"), buildIntType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList,arg1);

  SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration
                                          ("bar",buildVoidType(), paraList);
  appendStatement(funcdecl); 
  popScopeStack ();
  // insert the struct declaration
  appendStatement (decl);
    // This is a bottom up way for decl2
  insertStatementBefore (decl, decl2);

  // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}


