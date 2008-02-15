// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.

#include "rose.h"

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
       // Mark this as a transformation (required)
          Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
          ROSE_ASSERT(sourceLocation != NULL);

          SgType* type = new SgTypeInt();
          ROSE_ASSERT(type != NULL);

          SgName name = "newVariable";

          SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration(sourceLocation,name,type);
          ROSE_ASSERT(variableDeclaration != NULL);

          SgInitializedName* initializedName = *(variableDeclaration->get_variables().begin());

       // DQ (6/18/2007): The unparser requires that the scope be set (for name qualification to work).
          initializedName->set_scope(block);

          ROSE_ASSERT(block->get_statements().size() > 0);

          block->insert_statement(block->get_statements().begin(),variableDeclaration);

       // Add a symbol to the sybol table for the new variable
          SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
          block->insert_symbol(name,variableSymbol);
        }
   }

int
main ( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

     return backend(project);
   }

