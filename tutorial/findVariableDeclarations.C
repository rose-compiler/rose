// ROSE is a tool for building translators, this file is an example translator built with ROSE.
// Specifically it shows the analysis of a code to report all variable declarations in each block.

#include "rose.h"

class FindVariableDeclarations : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
FindVariableDeclarations::visit ( SgNode* astNode )
   {
     SgBasicBlock* block = isSgBasicBlock(astNode);
     if (block != NULL)
        {
          SgStatementPtrList & listOfStatements = block->get_statements();
          for (size_t i = 0; i < listOfStatements.size(); i++)
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(listOfStatements[i]);
               if (variableDeclaration != NULL)
                  {
                    printf ("Found a variable decaration in a SgBasicBlock at: \n");
                    variableDeclaration->get_file_info()->display("Found a variable decaration in a SgBasicBlock");
                  }
             }
        }
   }

int
main ( int argc, char * argv[] )
   {
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);

     FindVariableDeclarations treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

     return backend(project);
   }

