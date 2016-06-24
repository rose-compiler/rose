// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.

#include "rose.h"

using namespace std;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
SimpleInstrumentation::visit ( SgNode* astNode )
   {
     SgBasicBlock* block = isSgBasicBlock(astNode);
     if (block != NULL)
        {
          const unsigned int SIZE_OF_BLOCK = 1;
          if (block->get_statements().size() > SIZE_OF_BLOCK)
             {
            // It is up to the user to link the implementations of these functions link time
               string codeAtTopOfBlock    = "void myTimerFunctionStart(); myTimerFunctionStart();";
               string codeAtBottomOfBlock = "void myTimerFunctionEnd(); myTimerFunctionEnd();";

            // Insert new code into the scope represented by the statement (applies to SgScopeStatements)
               MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::StatementScope;

            // Insert the new code at the top and bottom of the scope represented by block
               MiddleLevelRewrite::insert(block,codeAtTopOfBlock,scope,
                                          MidLevelCollectionTypedefs::TopOfCurrentScope);
               MiddleLevelRewrite::insert(block,codeAtBottomOfBlock,scope,
                                          MidLevelCollectionTypedefs::BottomOfCurrentScope);
             }
        }
   }

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );
     
     AstTests::runAllTests(project);
     return backend(project);
   }






