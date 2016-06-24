// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

class PreAndPostOrderTraversal : public AstPrePostProcessing
   {
     public:
          virtual void preOrderVisit(SgNode* n);
          virtual void postOrderVisit(SgNode* n);
   };

void PreAndPostOrderTraversal::preOrderVisit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Entering for loop ... \n");
        }
   }

void PreAndPostOrderTraversal::postOrderVisit(SgNode* n)
   {
     if (isSgForStatement(n) != NULL)
        {
          printf ("Leaving for loop ... \n");
        }
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     if (SgProject::get_verbose() > 0)
          printf ("In prePostTraversal.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     PreAndPostOrderTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project);

     return 0;
   }

