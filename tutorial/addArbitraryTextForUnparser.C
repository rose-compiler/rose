// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* n)
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
     if (functionDeclaration != NULL)
        {
       // This is an example of a XYZ tool specific annotation
          string compilerSpecificDirective = "\n#if XYZ_TOOL \n   \"builtin\"\n#endif\n";
          SageInterface::addTextForUnparser(functionDeclaration,compilerSpecificDirective,AstUnparseAttribute::e_before);
        }

     SgValueExp* valueExp = isSgValueExp(n);
     if (valueExp != NULL)
        {
       // Add a backend specific compiler directive
          string compilerSpecificDirective = "\n#if CRAY \n   cray_specific_attribute \n#endif\n";
          SageInterface::addTextForUnparser(valueExp,compilerSpecificDirective,AstUnparseAttribute::e_before);
        }

   }

// Typical main function for ROSE translator
int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

     return backend(project);
   }

