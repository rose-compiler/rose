// Liao, 11/9/2009
// Demonstrate how to build a struct declaration embedded into a variable decl
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  ROSE_ASSERT (globalScope);

  // Build an anonymous struct here
  // top-down construction, with scope ready
  SgClassDeclaration * decl = buildStructDeclaration("",globalScope);

  // build a member variable inside the structure
  SgClassDefinition *def = decl->get_definition();
  SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("i"), buildIntType(), NULL, def);
  // Insert the  member variable
  appendStatement (varDecl, def);

  // insert the struct declaration
  //  This is not necessary and will cause the struct to be unparsed twice,
  //  both from global scope and from the later variable declaration
  //  but SageInterface::setBaseTypeDefiningDeclaration() will take care of this
  //  even if you accidentally inserted it here.
  // appendStatement (decl,globalScope);

  //Declare a struct variable
  SgVariableDeclaration * varDecl2 = SageBuilder::buildVariableDeclaration("temp", decl->get_type(), NULL, globalScope);
  appendStatement(varDecl2, globalScope);

  // Set the struct declaration as its base type defining declaration
  SageInterface::setBaseTypeDefiningDeclaration(varDecl2, decl);

  AstTests::runAllTests(project);
  return backend (project);
}


