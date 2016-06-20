// SageBuilder contains all high level buildXXX() functions,
// such as buildVariableDeclaration(), buildLabelStatement() etc.
// SageInterface contains high level AST manipulation and utility functions,
// e.g. appendStatement(), lookupFunctionSymbolInParentScopes() etc.
#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

class SimpleInstrumentation:public SgSimpleProcessing
{
public:
  void visit (SgNode * astNode);
};

void
SimpleInstrumentation::visit (SgNode * astNode)
{
  SgBasicBlock *block = isSgBasicBlock (astNode);
  if (block != NULL)
    {
      SgVariableDeclaration *variableDeclaration =
        	buildVariableDeclaration ("newVariable", buildIntType ());
      prependStatement (variableDeclaration, block);
    }
}

int
main (int argc, char *argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);

  SimpleInstrumentation treeTraversal;
  treeTraversal.traverseInputFiles (project, preorder);

  AstTests::runAllTests (project);
  return backend (project);
}
