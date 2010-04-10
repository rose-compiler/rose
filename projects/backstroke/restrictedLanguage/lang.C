#include "rose.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          void visit(SgNode* n);
   };

void visitorTraversal::visit(SgNode* node)
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
     if (functionDeclaration != NULL)
        {
       // Detect restricted language events
        }
   }


// Typical main function for ROSE translator
int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Build the traversal object
     visitorTraversal restrictionTraversal;

  // Call the traversal starting at the project node of the AST
     restrictionTraversal.traverseInputFiles(project,preorder);

     return backend(project);
   }

