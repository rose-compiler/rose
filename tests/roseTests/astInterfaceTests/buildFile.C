// Liao, 4/8/2008
// Demonstrate how to build a SgFile using the interface
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  //absolute filename
 // std::string fileName="/home/liao6/buildrose/tests/roseTests/astInterfaceTests/testfile525.cpp";
  std::string fileName="testfile525.cpp";
  std::string fileName2="testfile626.cpp";

//case 1. Calling it with project available 
  SgProject *project = frontend (argc, argv);

  SgFile *sgfile = buildFile(fileName,project);

  // try to add something into the new file
  SgGlobal* global = sgfile->get_globalScope();
  SgVariableDeclaration *varDecl = buildVariableDeclaration("j", buildIntType());
  appendStatement (varDecl,isSgScopeStatement(global));

  AstTests::runAllTests(project);
  project->unparse();
// case 2. Calling it without the project parameter  
  SgFile *sgfile2 = buildFile(fileName2);

  // try to add something into the new file
  SgGlobal* global2 = sgfile2->get_globalScope();
  SgVariableDeclaration *varDecl2 = buildVariableDeclaration("x", buildIntType());
  appendStatement (varDecl2,isSgScopeStatement(global2));

  SgProject * project2= sgfile2->get_project();
  AstTests::runAllTests(project2);
  project2->unparse();

  return 0;
}

