// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

class visitorTraversal : public AstSimpleProcessing
   {
     public:
          visitorTraversal();
          virtual void visit(SgNode* n);
          virtual void atTraversalEnd();
   };

visitorTraversal::visitorTraversal()
   {
   }

void visitorTraversal::visit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Found a for loop ... \n");
        }
   }

void visitorTraversal::atTraversalEnd()
   {
     printf ("Traversal ends here. \n");
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In visitorTraversal.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal function (member function of AstSimpleProcessing)
  // starting at the project node of the AST, using a preorder traversal.
     exampleTraversal.traverseInputFiles(project,preorder);

     return 0;
   }

