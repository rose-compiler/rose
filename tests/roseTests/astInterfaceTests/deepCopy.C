// Liao 4/22/2011
// test deepCopy on function declarations
#include <rose.h>
#include <stdio.h>
using namespace SageInterface;

int main(int argc, char** argv)
{
  SgProject* project = frontend(argc, argv);
  AstTests::runAllTests(project);


  // Copy a defining function declaration
  SgFunctionDeclaration* func= findDeclarationStatement<SgFunctionDeclaration> (project, "bar", NULL, true);
  ROSE_ASSERT (func != NULL);


  SgFunctionDeclaration* func_copy = isSgFunctionDeclaration(copyStatement (func));
  func_copy->set_name("bar_copy");
  SgGlobal * glb = getFirstGlobalScope(project);
  appendStatement (func_copy,glb);

#if 0 // this has been merged into fixStatement() called by appendStatement()
  SgFunctionSymbol *func_symbol =  glb->lookup_function_symbol ("bar_copy", func_copy->get_type()); 
  if (func_symbol == NULL);
  {
    func_symbol = new SgFunctionSymbol (func_copy);
    glb ->insert_symbol("bar_copy", func_symbol);
  }
#endif

  //copy a non-defining function declaration

  SgFunctionDeclaration* nfunc= findDeclarationStatement<SgFunctionDeclaration> (project, "foo", NULL, false);
  ROSE_ASSERT (nfunc != NULL);
  func_copy = isSgFunctionDeclaration(copyStatement (nfunc));
  glb = getFirstGlobalScope(project);
  appendStatement (func_copy,glb);
  //copy another non-defining function declaration

  nfunc= findDeclarationStatement<SgFunctionDeclaration> (project, "bar", NULL, false);
  ROSE_ASSERT (nfunc != NULL);
  func_copy = isSgFunctionDeclaration(copyStatement (nfunc));
  glb = getFirstGlobalScope(project);
  appendStatement (func_copy,glb);


  AstTests::runAllTests(project);
  backend(project);   
  return 0;
}
