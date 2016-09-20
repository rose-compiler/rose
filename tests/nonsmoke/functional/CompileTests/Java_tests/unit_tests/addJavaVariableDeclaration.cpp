// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottom of each basic block.
// Member functions of SAGE III AST node classes are directly used.
// So all details for Sg_File_Info, scope, parent, symbol tables have to be explicitly handled.

#include "rose.h"
#include <iostream>
using namespace std;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
SimpleInstrumentation::visit (SgNode* astNode) {
    static char suffix = 'a';
    SgBasicBlock* block = isSgBasicBlock(astNode);
    if (block != NULL && block->get_statements().size() > 0) {
        SgName name = string("newVariable_") + (suffix++);

        SgVariableDeclaration *variable_declaration = SageBuilder::buildVariableDeclaration(name, SgTypeInt::createType(), NULL, block);
        variable_declaration -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

        block -> get_statements().insert(block -> get_statements().begin(), variable_declaration);
        variable_declaration -> set_parent(block);
    }
}

int
main ( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles(project, preorder);
    
     AstTests::runAllTests(project);
     return backend(project);
   }

