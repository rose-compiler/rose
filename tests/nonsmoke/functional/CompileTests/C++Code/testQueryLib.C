// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure
#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <iomanip>

#include "AstTests.h"

#include <algorithm>

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class dqInheritedAttribute
   {
     public:
      //! Specific constructors are required
          dqInheritedAttribute () {};
          dqInheritedAttribute ( const dqInheritedAttribute & X ) {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class dqSynthesizedAttribute
   {
     public:
         dqSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class dqTraversal
   : public SgTopDownBottomUpProcessing<dqInheritedAttribute,dqSynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

       // list of types that have been traversed
          static list<SgNode*> listOfTraversedTypes;
         
          dqTraversal (): traversalNodeCounter(0) {};

       // Functions required by the rewrite mechanism
          dqInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             dqInheritedAttribute inheritedAttribute );

          dqSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             dqInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// Allocation of space for listOfTraversedTypes declared in dqTraversal
list<SgNode*> dqTraversal::listOfTraversedTypes;

// Functions required by the tree traversal mechanism
dqInheritedAttribute
dqTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     dqInheritedAttribute inheritedAttribute )
   {
     traversalNodeCounter++;

     switch(astNode->variantT())
        {
        }

  // This query is not longer used and so we have removed it and we no longer need the test!
  // bool isInAClass = BooleanQuery::isContainedInSubtreeOfType(astNode,V_SgClassDeclaration);
     bool isInAClass = false;
     printf ("Is in a class declaration = %s \n",(isInAClass == true) ? "true" : "false");

     return inheritedAttribute;
   }

dqSynthesizedAttribute
dqTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     dqInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     dqSynthesizedAttribute returnAttribute;

     switch(astNode->variantT())
        {
        }

     return returnAttribute;
   }

int
main ( int argc, char* argv[] )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

     AstTests::runAllTests(const_cast<SgProject*>(project));

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

#if 1
     dqTraversal treeTraversal;
     dqInheritedAttribute inheritedAttribute;

  // Ignore the return value since we don't need it
     treeTraversal.traverseInputFiles(project,inheritedAttribute);
#endif

     return backend(project);
  // return backend(frontend(argc,argv));
   }











