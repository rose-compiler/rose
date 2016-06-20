// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"

#include <algorithm>
#include <functional>
#include <numeric>

typedef bool InheritedAttribute;
typedef bool SynthesizedAttribute;

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
       // Functions required
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             InheritedAttribute inheritedAttribute );

          SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             InheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

InheritedAttribute
Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
     if (isSgFunctionDefinition(astNode))
        {
       // The inherited attribute is true iff we are inside a function.
          return true;
        }
     return inheritedAttribute;
   }

SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
     if (inheritedAttribute == false)
        {
       // The inherited attribute is false, i.e. we are not inside any
       // function, so there can be no loops here.
          return false;
        }
     else
        {
       // Fold up the list of child attributes using logical or, i.e. the local
       // result will be true iff one of the child attributes is true.
          SynthesizedAttribute localResult =
          std::accumulate(childAttributes.begin(), childAttributes.end(),
                          false, std::logical_or<bool>());
          if (isSgFunctionDefinition(astNode) && localResult == true)
             {
               printf ("Found a function containing a for loop ...\n");
             }
          if (isSgForStatement(astNode))
             {
               localResult = true;
             }
          return localResult;
        }
   }

int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute = false;

  // Define the traversal
     Traversal myTraversal;

  // Call the traversal starting at the project (root) node of the AST
     myTraversal.traverseInputFiles(project,inheritedAttribute);

  // This program only does analysis, so it need not call the backend to generate code.
     return 0;
   }
