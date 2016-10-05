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
 // SgNullStatement cannot be inserted into a global scope, 
 // which expects declaration statement only!  
 
 // SgGlobal *globalScope = isSgGlobal (astNode);
 // if (globalScope != NULL)
    SgBasicBlock* block = isSgBasicBlock(astNode);
  if (block != NULL)
    {
       SgNullStatement* stmt = buildNullStatement();
       attachArbitraryText(stmt,"int UPCR_TLD_DEFINE (counter2, 4, 4) = 100;");
       prependStatement(stmt,block);
    }
}

int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);

  SimpleInstrumentation treeTraversal;
  treeTraversal.traverseInputFiles (project, preorder);

  return backend (project);
}
