// ROSE Translator to make sure that a single statement body (of for, if, etc.) after pragma is created properly
// Liao 11/26/2012
#include "rose.h"
using namespace SageInterface;

int main(int argc, char* argv[])
{
  SgProject* project = frontend(argc, argv);

  SgFunctionDeclaration* mainFunc= findMain(project);
  SgBasicBlock* body= mainFunc->get_definition()->get_body();

  std::vector<SgPragmaDeclaration*> pragmas= SageInterface::querySubTree<SgPragmaDeclaration>(body, V_SgPragmaDeclaration);
  ROSE_ASSERT (pragmas.size() ==1);
  SgPragmaDeclaration* decl = pragmas[0];
  ROSE_ASSERT (decl != NULL);
  SgStatement *n = getNextStatement(decl);
  ROSE_ASSERT (n != NULL);
}


