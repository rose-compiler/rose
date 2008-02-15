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
          string comment = string("// Auto-comment function name: ") + 
                           functionDeclaration->get_name().str() + 
                           " is now a commented function";
          PreprocessingInfo* commentInfo = 
               new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
               comment,"user-generated",0, 0, 0, PreprocessingInfo::before, false, true);
          functionDeclaration->addToAttachedPreprocessingInfo(commentInfo);
        }
   }

int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,preorder);

     return backend(project);
   }

