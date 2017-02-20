#ifndef UNTYPED_TRAVERSAL_H
#define UNTYPED_TRAVERSAL_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace SgUntyped {

typedef SgExpression* SynthesizedAttribute;

class UntypedTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
  {
    public :
      virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttrs);

      static SgExpression* convert_SgUntypedExpression      (SgUntypedExpression* untyped_expression);
      static SgValueExp*   convert_SgUntypedValueExpression (SgUntypedValueExpression* untyped_value);

      static SgUnaryOp*  convert_SgUntypedUnaryOperator (SgUntypedUnaryOperator* untyped_operator, SgExpression* expr);
      static SgBinaryOp* convert_SgUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

  };

} // namespace SgUntyped

// endif for UNTYPED_TRAVERSAL_H
#endif

