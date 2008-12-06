// Liao, 2/13/2008
// Demonstrate how to build a struct declaration
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // build a struct without any scope information
  // cannot do it here, since frontend() will traverse memory pool and find the dangling decl.
  //SgClassDeclaration * decl2 = buildStructDeclaration("bar");

  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  ROSE_ASSERT (globalScope);

  // build a struct without any scope information
  SgClassDeclaration * decl2 = buildStructDeclaration("bar");
  ROSE_ASSERT (decl2);

  pushScopeStack (isSgScopeStatement (globalScope));
  ROSE_ASSERT (topScopeStack());

  // build a struct declaration with implicit scope information from the scope stack
  SgClassDeclaration * decl = buildStructDeclaration("foo");

  // build member variables inside the structure
  SgClassDefinition *def = decl->get_definition();
  pushScopeStack (isSgScopeStatement (def));
  ROSE_ASSERT (topScopeStack());

  SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("i"), buildIntType());

 // Insert the  member variable
  appendStatement (varDecl);

   // build a member function prototype of the construct
  SgInitializedName* arg1 = buildInitializedName(SgName("x"), buildIntType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList,arg1);

  SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration
                                          ("bar",buildVoidType(), paraList);
  appendStatement(funcdecl); 

  // build a defining member function 
  SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList)); 
  ROSE_ASSERT(paraList2);
  SgMemberFunctionDeclaration* funcdecl_2 = buildDefiningMemberFunctionDeclaration
                           ("bar2",buildVoidType(),paraList2);
  appendStatement(funcdecl_2);                         

  popScopeStack ();
  // insert the struct declaration
  appendStatement (decl);
    // This is a bottom up way for building decl2
  insertStatementBefore (decl, decl2);

  // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}


