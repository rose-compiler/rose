// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottom of each basic block.
// Member functions of SAGE III AST node classes are directly used.
// So all details for Sg_File_Info, scope, parent, symbol tables have to be explicitly handled.

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
          initializedName->set_file_info(Sg_File_Info::generateDefaultFileInfoForTransformationNode());

       // DQ (6/18/2007): The unparser requires that the scope be set (for name qualification to work).
          initializedName->set_scope(block);

       // Liao (2/13/2008): AstTests requires this to be set
          variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);

          ROSE_ASSERT(block->get_statements().size() > 0);

          block->get_statements().insert(block->get_statements().begin(),variableDeclaration);
          variableDeclaration->set_parent(block);

       // Add a symbol to the sybol table for the new variable
          SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
          block->insert_symbol(name,variableSymbol);
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

