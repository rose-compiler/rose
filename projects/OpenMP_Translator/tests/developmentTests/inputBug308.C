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

          SgStatement* variableDeclaration = new SgVariableDeclaration(sourceLocation,name,type);
          ROSE_ASSERT(variableDeclaration != NULL);

          ROSE_ASSERT(block->get_statements().size() > 0);

//          block->insert_statement(block->get_statements().begin(),variableDeclaration);
	SgStatement *firststmt = block->get_statements().begin();
    	list<SgStatement * > list_to_insert;
      	 list_to_insert.push_back(variableDeclaration);
	 LowLevelRewrite::insert(firststmt,list_to_insert,false);
    
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

