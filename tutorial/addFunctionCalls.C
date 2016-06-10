// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottom of each basic block.

#include "rose.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

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
      const unsigned int SIZE_OF_BLOCK = 1;
      if (block->get_statements ().size () > SIZE_OF_BLOCK)
	{
          SgName name1("myTimerFunctionStart");
	  // It is up to the user to link the implementations of these functions link time
          SgFunctionDeclaration *decl_1 = buildNondefiningFunctionDeclaration
              (name1,buildVoidType(),buildFunctionParameterList(),block);
          ((decl_1->get_declarationModifier()).get_storageModifier()).setExtern();

          SgExprStatement* callStmt_1 = buildFunctionCallStmt
              (name1,buildVoidType(), buildExprListExp(),block);
          
          prependStatement(callStmt_1,block);
          prependStatement(decl_1,block);

          SgName name2("myTimerFunctionEnd");
	  // It is up to the user to link the implementations of these functions link time
          SgFunctionDeclaration *decl_2 = buildNondefiningFunctionDeclaration
              (name2,buildVoidType(),buildFunctionParameterList(),block);
          ((decl_2->get_declarationModifier()).get_storageModifier()).setExtern();

          SgExprStatement* callStmt_2 = buildFunctionCallStmt
              (name2,buildVoidType(), buildExprListExp(),block);

          appendStatement(decl_2,block);
          appendStatement(callStmt_2,block);
	}
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
