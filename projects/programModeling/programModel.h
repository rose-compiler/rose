#ifndef PROGRAM_MODEL_H
#define PROGRAM_MODEL_H

/* OpenMaple routines are defined here */
#include "mapleSupport.h"

/*! \brief This attribute is used a a bread crumb to mark the occuraces of things in the AST.

   Models are automatically generated using symbolic algebra to cound the occurances of 
   properties in the AST.  There are two types of models: control flow base and non-control 
   flow based models.

   Where these properties occur in loops the occurance is expressed 
   as an equation involving the loop base and bounds.
 */

class ModelingAttribute : public AstAttribute
   {
     public:
      //! Models can be used in different ways
          enum ModelTypeEnum 
             {
               unknownModel        = 0,
               controlFlowBased    = 1, //! counts occurances as function of loop trip equations
               nonControlFlowBased = 2, //! counts occurances (independend of loop trip equations)
               LAST_MODEL_TYPE
             };

          ModelTypeEnum modelType;

          ModelingAttribute (ModelTypeEnum modelTypeParameter = nonControlFlowBased ) : modelType(modelTypeParameter) {}
   };

// ***********************************************************************
//    Traversal to collect modeling attributes and build equations to
//    define flow-sensitive or non-flow-sensitive models of program
//    elements that where the modeling attibute is used.
// ***********************************************************************

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class ProgramModelAttributeCollectorInheritedAttribute
   {
     public:
      //! Specific constructors are required
          ProgramModelAttributeCollectorInheritedAttribute () {};
          ProgramModelAttributeCollectorInheritedAttribute ( const ProgramModelAttributeCollectorInheritedAttribute & X ) {};
   };

// Build a synthesized attribute for the tree traversal to test the rewrite mechanism
class ProgramModelAttributeCollectorSynthesizedAttribute
   {
     public:
         ProgramModelAttributeCollectorSynthesizedAttribute() {};
   };

// tree traversal to test the rewrite mechanism
 /*! A specific AST processing class is used (built from SgTopDownBottomUpProcessing)
  */
class ProgramModelAttributeCollectorTraversal
   : public SgTopDownBottomUpProcessing<ProgramModelAttributeCollectorInheritedAttribute,ProgramModelAttributeCollectorSynthesizedAttribute>
   {
     public:
       // This value is a temporary data member to allow us to output the number of 
       // nodes traversed so that we can relate this number to the numbers printed 
       // in the AST graphs output via DOT.
          int traversalNodeCounter;

          ProgramModelAttributeCollectorTraversal (): traversalNodeCounter(0) {};

       // Functions required by the rewrite mechanism
          ProgramModelAttributeCollectorInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode,
             ProgramModelAttributeCollectorInheritedAttribute inheritedAttribute );

          ProgramModelAttributeCollectorSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             ProgramModelAttributeCollectorInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

namespace ProgramModeling
   {
     void computeModelEquations(SgProject* project);
   }

// PROGRAM_MODEL_H
#endif
