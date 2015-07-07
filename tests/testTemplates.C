// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"

#include "RoseAst.h"

bool isTemplateInstantiationNode(SgNode* node) 
   {
     return isSgTemplateInstantiationDecl(node)
         || isSgTemplateInstantiationDefn(node)
         || isSgTemplateInstantiationFunctionDecl(node)
         || isSgTemplateInstantiationMemberFunctionDecl(node)
         || isSgTemplateInstantiationTypedefDeclaration(node)
         || isSgTemplateInstantiationDirectiveStatement(node)
         ;
   }

void markNodeToBeUnparsed(SgNode* node) {
  Sg_File_Info* fileInfo=node->get_file_info();
  if(fileInfo) {
    fileInfo->setTransformation();
    fileInfo->setOutputInCodeGeneration();

    SgLocatedNode* locatedNode = isSgLocatedNode(node);
    if (locatedNode != NULL)
       {
      // DQ (7/7/2015): Make the subtree as transformed.
         locatedNode->setTransformation();
         locatedNode->setOutputInCodeGeneration();

         markTransformationsForOutput(node);
       }
  }
}

int markAllTemplateInstantiationsToBeUnparsed(SgProject* root) {
  RoseAst ast(root);
  int n=0;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isTemplateInstantiationNode(*i)) {
      markNodeToBeUnparsed(*i);
      n++;
    }
  }
  return n;
}


int main( int argc, char * argv[] )
   {
#if 0
  // Output the ROSE specific predefined macros.
     outputPredefinedMacros();
#endif

  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     markAllTemplateInstantiationsToBeUnparsed(project);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT ( *project );
     // generateAstGraph(project, 2000);
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     SgNode::get_globalTypeTable()->print_typetable();
#endif

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
