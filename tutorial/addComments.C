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
          string comment = string("Auto-comment function name: ") + 
                           functionDeclaration->get_name().str() + 
                           " is now a commented function";

       // Note that this function will add the "//" or "/* */" comment syntax as required for C or C++, or Fortran.
          SageInterface::attachComment(functionDeclaration,comment);
        }

     SgValueExp* valueExp = isSgValueExp(n);
     if (valueExp != NULL)
        {
       // Check if there is an expression tree from the original unfolded expression.
       // This is a trivial example ouf the output of an analysis result.
          string comment = string("Auto-comment value: ") + 
               ((valueExp->get_originalExpressionTree() != NULL) ? 
                    " this IS a constant folded value" : " this is NOT a constant folded value");

          SageInterface::attachComment(valueExp,comment);
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

