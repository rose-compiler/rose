#ifndef FIXUP_CONSTANT_FOLDED_VALUES_H
#define FIXUP_CONSTANT_FOLDED_VALUES_H

/*! \brief This traversal can either replace the constant folded values with the original expression tree or leave the constant folded value and remove the original expression tree.

    This traversal uses the Memory Pool traversal to fixup SgValueExp IR nodes that contain valid pointers to original expression trees.
 */

// ******************************************
// This is the API function we use externally
// ******************************************
void resetConstantFoldedValues( SgNode* node );


// DQ (9/17/2011): Make this a traversal over the Memory pool so that we will easily catch all expression (especially where they are not a part of a traversal such as in array types).
class RemoveOriginalExpressionTrees : public ROSE_VisitTraversal
   {
     public:
          virtual ~RemoveOriginalExpressionTrees() {};

      //! Required traversal function
          void visit (SgNode* node);
   };

// This is the internal function we use in resetConstantFoldedValues().
void removeOriginalExpressionTrees( SgNode* node );







//! Synthesized attribute required for MarkTemplateSpecializationsForOutputSupport class.
class RemoveConstantFoldedValueSynthesizedAttribute
   {
     public:
       // This is the AST node associated with the evaluation of the synthesized attributes.
          SgNode* node;

       // This appears to be required
          RemoveConstantFoldedValueSynthesizedAttribute() : node(NULL) {}

       // This is used in evaluateSynthesizedAttribute()
          RemoveConstantFoldedValueSynthesizedAttribute(SgNode* n);
   };


// class RemoveConstantFoldedValue : public AstSimpleProcessing
class RemoveConstantFoldedValue : public SgBottomUpProcessing<RemoveConstantFoldedValueSynthesizedAttribute>
   {
     public:
          virtual ~RemoveConstantFoldedValue() {};

      //! Required traversal function
          RemoveConstantFoldedValueSynthesizedAttribute evaluateSynthesizedAttribute ( SgNode* node, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };


// class RemoveConstantFoldedValue : public AstSimpleProcessing
class RemoveConstantFoldedValueViaParent : public ROSE_VisitTraversal
   {
     public:
          virtual ~RemoveConstantFoldedValueViaParent() {};

      //! Required traversal function
          void visit (SgNode* node);
   };




// This is part of the mechanism to support replacing values in the AST via references to their pointers.
// It was built to support the AST merge andspecifically the AST fixup associated with merging ASTs.
struct ConstantFoldedValueReplacer : public SimpleReferenceToPointerHandler
   {
  // This class captures how we want to have pointers be reset for any IR node.  It is a general
  // mechanism and better than the alternative special case handling for each data member in each IR node.

  // This turns on tracing for debugging support.
     bool traceReplacement;

  // This is the IR node that we want to have be replaced in the IR node calling:
  // "SgNode::processDataMemberReferenceToPointers(ConstantFoldedValueReplacer* r)".
     SgNode* targetNode;

  // Constructor.
     ConstantFoldedValueReplacer(bool traceReplacement, SgNode* node) : traceReplacement(traceReplacement), targetNode(node) {}

  // Put the work to be done into the "()" operator. This will be called for each data member where 
  // "SgNode::processDataMemberReferenceToPointers(ConstantFoldedValueReplacer* r)" is called.
     virtual void operator()(SgNode*& key, const SgName & debugStringName, bool /* traverse */ traceReplacement);
   };



// This is the internal function we use in resetConstantFoldedValues().
void removeConstantFoldedValue( SgNode* node );




// **************************************************************
// **************************************************************
//    Verification of all original expression trees being reset
// **************************************************************
// **************************************************************

class DetectOriginalExpressionTreeTraversal : public AstSimpleProcessing
   {
  // This is a traveral over the AST.
  // This traversal is used to verify that all of the original expression trees in the AST 
  // have been either deleted (optional) or used to replace the constant folded values.
     public:
          virtual ~DetectOriginalExpressionTreeTraversal() {};

      //! Required traversal function
          void visit (SgNode* node);
   };

class DetectHiddenOriginalExpressionTreeTraversal : public ROSE_VisitTraversal
   {
  // This is a traveral over the memory pool, so that we can see all types etc.
  // This traversal is used to find hidden AST subtrees (in SgArrayType and bitfield specifications)
  // and verify that all of the original expression trees in these subtrees have been either deleted
  // (optional) or used to replace the constant folded values.

  // This traversal calls the DetectOriginalExpressionTreeTraversal to detect the original exxpression trees.

     public:
          virtual ~DetectHiddenOriginalExpressionTreeTraversal() {};

      //! Required traversal function
          void visit (SgNode* node);
   };

// This is a hard test to always pass.
class VerifyOriginalExpressionTreesSetToNull : public ROSE_VisitTraversal
   {
  // This is a traveral over the memory pool, so that we can see all expressions.

  // This is a more aggressive test since it will detect original expression trees in 
  // orphaned expressions (which should not exist and are rare, but are in some code 
  // using array type expressions with constant folding).

  // This traversal is used to verify that all of the original expression trees in the AST 
  // have been either deleted (optional) or used to replace the constant folded values.

     public:
          virtual ~VerifyOriginalExpressionTreesSetToNull() {};

      //! Required traversal function
          void visit (SgNode* node);
   };


// This is the internal function we use in resetConstantFoldedValues().
void verifyOriginalExpressionTreesSetToNull( SgNode* node );

// endif for FIXUP_CONSTANT_FOLDED_VALUES_H
#endif
