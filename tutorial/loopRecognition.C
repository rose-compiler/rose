// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

class InheritedAttribute
   {
     public:
          int loopNestDepth;

          InheritedAttribute () : loopNestDepth(0) {};
          InheritedAttribute ( const InheritedAttribute & X ) {};
   };

class SynthesizedAttribute
   {
     public:
         SynthesizedAttribute() {};
   };

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
     switch(astNode->variantT())
        {
          case V_SgForStatement:
             {
               printf ("Found a SgForStatement \n");

            // This loop is one deepper than the depth of the parent's inherited attribute
               inheritedAttribute.loopNestDepth++;

               break;
             }

          default:
             {
            // g++ needs a block here
             }
        }

     return inheritedAttribute;
   }

SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     SynthesizedAttribute returnAttribute;

     switch(astNode->variantT())
        {
          case V_SgForStatement:
             {
               
               break;
             }

          default:
             {
            // g++ needs a block here
             }
        }

     return returnAttribute;
   }



int
main ( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

     Traversal myTraversal;

  // Call the traversal starting at the sageProject node of the AST
     myTraversal.traverseInputFiles(project,inheritedAttribute);

     return 0;
   }






