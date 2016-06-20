// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

#include <algorithm>
#include <functional>
#include <numeric>

typedef bool SynthesizedAttribute;

class visitorTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
   {
     public:
       // virtual function must be defined
          virtual SynthesizedAttribute evaluateSynthesizedAttribute ( 
                       SgNode* n, SynthesizedAttributesList childAttributes );
   };

SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute ( SgNode* n, SynthesizedAttributesList childAttributes )
   {
  // Fold up the list of child attributes using logical or, i.e. the local
  // result will be true iff one of the child attributes is true.
     SynthesizedAttribute localResult =
         std::accumulate(childAttributes.begin(), childAttributes.end(),
                         false, std::logical_or<bool>());

     if (isSgForStatement(n) != NULL)
        {
          printf ("Found a for loop ... \n");
          localResult = true;
        }

     return localResult;
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST 
     SynthesizedAttribute result = exampleTraversal.traverse(project);

     if (result == true)
        {
          printf ("The program contains at least one loop!\n");
        }

     return 0;
   }

