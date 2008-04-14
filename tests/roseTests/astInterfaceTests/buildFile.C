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
 std::string fileName="/home/liao6/buildrose/tests/roseTests/astInterfaceTests/testfile525.cpp";
#if 1
  SgProject *project = frontend (argc, argv);
//  SgGlobal *globalScope = getFirstGlobalScope (project);

// TODO relative path case
  SgFile *sgfile = buildFile(fileName,project);

// try to add something into the new file
  SgGlobal* global = sgfile->get_globalScope();
  SgVariableDeclaration *varDecl = buildVariableDeclaration("j", buildIntType());
   appendStatement (varDecl,isSgScopeStatement(global));

  AstTests::runAllTests(project);
  return backend (project);
#else  
  SgFile *sgfile = buildFile(fileName);
#endif   
}

