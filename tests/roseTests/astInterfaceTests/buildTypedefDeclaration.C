// Liao, 11/9/2009
// Demonstrate how to build a typedef declaration
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

// top down building  : typedef int MYINT;
  SgTypedefDeclaration * decl_1 = buildTypedefDeclaration("MYINT", buildIntType(), globalScope);
  appendStatement (decl_1,globalScope);

// typedef struct frame {int x;} frame

  // Build struct frame {} first
  SgClassDeclaration* struct_1 = buildStructDeclaration("frame", globalScope); 
  SgClassDefinition *def = struct_1->get_definition();
  SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("x"), buildIntType(), NULL, def);
  appendStatement (varDecl, def);

 // should not append struct here if we want it show up within the typedef declaration
 // otherwise duplicated definition for the same struct
 // appendStatement(struct_1, globalScope);

 // build typedef from struct frame then 
  SgTypedefDeclaration * decl_2 = buildTypedefDeclaration("frame",struct_1->get_type(),globalScope);
  // make sure the defining struct declaration will show up
  decl_2->set_typedefBaseTypeContainsDefiningDeclaration(true);

  appendStatement (decl_2,globalScope);

  AstTests::runAllTests(project);
  return backend (project);
}


