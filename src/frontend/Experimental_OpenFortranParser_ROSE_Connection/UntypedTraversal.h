#ifndef UNTYPED_TRAVERSAL_H
#define UNTYPED_TRAVERSAL_H

extern SgSourceFile* OpenFortranParser_globalFilePointer;

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedTraversal class is used to traverse SgUntypedNodes and
// convert them to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace Fortran {
namespace Untyped {

typedef SgScopeStatement*  InheritedAttribute;
typedef SgExpression*      SynthesizedAttribute;

class UntypedTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
  {
    public :

      virtual InheritedAttribute   evaluateInheritedAttribute   (SgNode* n, InheritedAttribute inheritedAttribute );
      virtual SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, InheritedAttribute inheritedAttribute
                                                                          , SynthesizedAttributesList childAttrs  );

      static SgExpression* convert_SgUntypedExpression      (SgUntypedExpression* untyped_expression);
      static SgValueExp*   convert_SgUntypedValueExpression (SgUntypedValueExpression* untyped_value);

      static SgUnaryOp*  convert_SgUntypedUnaryOperator (SgUntypedUnaryOperator* untyped_operator, SgExpression* expr);
      static SgBinaryOp* convert_SgUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs);

  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_TRAVERSAL_H
#endif

