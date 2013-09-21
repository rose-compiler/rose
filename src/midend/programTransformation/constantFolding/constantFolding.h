// This file implements constant folding, but leverages the work done
// by the frontend and adds support for constant folding on any transformations
// introduced after the frontend processing (the transformations are typically
// not seen by the frontend, so we have to handle constant folding directly
// for transformations.  Since many transformations are introduced automatically
// there is a substantial requirement for constant folding to clean the generated 
// code up a bit.  However, since the backend compiler (vendor's compiler) 

#ifndef ROSE_CONSTANT_FOLDING_H
#define ROSE_CONSTANT_FOLDING_H

namespace ConstantFolding {

// Build an inherited attribute for the tree traversal to skip constant folded expressions
class ConstantFoldingInheritedAttribute
   {
     public:
          bool isConstantFoldedValue;
          bool isPartOfFoldedExpression;
          bool internalTestingAgainstFrontend;

      //! Specific constructors are required
          ConstantFoldingInheritedAttribute()
             : isConstantFoldedValue(false), 
               isPartOfFoldedExpression(false),
               internalTestingAgainstFrontend(false)
             {};

       // Need to implement the copy constructor
          ConstantFoldingInheritedAttribute ( const ConstantFoldingInheritedAttribute & X )
             : isConstantFoldedValue(X.isConstantFoldedValue), 
               isPartOfFoldedExpression(X.isPartOfFoldedExpression),
               internalTestingAgainstFrontend(X.internalTestingAgainstFrontend)
             {};
   };

class ConstantFoldingSynthesizedAttribute
   {
     public:
          SgValueExp* newValueExp;

          ConstantFoldingSynthesizedAttribute() : newValueExp(NULL) {};
          ConstantFoldingSynthesizedAttribute ( const ConstantFoldingSynthesizedAttribute & X )
             : newValueExp(X.newValueExp) {};
   };

class ConstantFoldingTraversal
   : public SgTopDownBottomUpProcessing<ConstantFoldingInheritedAttribute,ConstantFoldingSynthesizedAttribute>
   {
     public:
       // Functions required by the rewrite mechanism
          ConstantFoldingInheritedAttribute evaluateInheritedAttribute (
             SgNode* n, 
             ConstantFoldingInheritedAttribute inheritedAttribute );
          ConstantFoldingSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* n,
             ConstantFoldingInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };
//! This is the external interface of constant folding:
//It relies on the EDG frontend to do constant folding by default. 
// The original source code pass trough EDG will have all constant fold already  (not know how)
// Only new constant folding opportunities introduced by custom transformations will be 
// actually handled by this function itself.
// Note: It will fold children nodes of the input node to some constants, 
// not folding the input node itself. 
ROSE_DLL_API void constantFoldingOptimization(SgNode* n, bool internalTestingAgainstFrontend = false);

// ***************************************************************************
// Constant un-folding is implemented here as a test to verify the correctness 
// of the constant folding.
//    As an example: "int x = 3;" is transformed to be "int x = 1 + 2;"
// This transformation permits automated testing of the constant folding.
// ***************************************************************************

// Build a synthesized attribute for the tree traversal to unfold constants so that we can test the constant folding
class ConstantUnFoldingSynthesizedAttribute
   {
     public:
          SgExpression* newExp;

          ConstantUnFoldingSynthesizedAttribute() : newExp(NULL) {};
          ConstantUnFoldingSynthesizedAttribute ( const ConstantUnFoldingSynthesizedAttribute & X )
             : newExp(X.newExp) {};
   };

class ConstantUnFoldingTraversal
   : public SgBottomUpProcessing<ConstantUnFoldingSynthesizedAttribute>
   {
     public:
       // Functions required by the traversal mechanism
          ConstantUnFoldingSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* n,
          // ConstantUnFoldingInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

// This test transforms the AST to turn constant values into expressions that have the 
// same value. This is a test for the constant folding optimization implemented above.
ROSE_DLL_API void constantUnFoldingTest(SgNode* n);

// end of ConstantFolding namespace
}


#endif
