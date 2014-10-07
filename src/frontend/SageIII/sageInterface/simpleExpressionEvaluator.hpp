#ifndef TYPEEQUIVALENCE_SIMPLE_EXPRESSION_EVALUATER_INCLUDE_GUARD
# define TYPEEQUIVALENCE_SIMPLE_EXPRESSION_EVALUATER_INCLUDE_GUARD

//# include "rose.h"
#include "sage3basic.h"
#include <cassert>

// XXX Should we have this as template, so that it is extendable?

class EvaluatorExpressionRepresentation {

 public:
  EvaluatorExpressionRepresentation();
  EvaluatorExpressionRepresentation(long long value);
  bool isEvalutable();
  void setEvalutable(bool evalutable);

  long long getValue();
  void setValue(long long value);

  bool operator==(const EvaluatorExpressionRepresentation &eer);

 private:
  long long value_;
  bool evalutable_;
};



class SimpleExpressionEvaluator: public AstBottomUpProcessing <
  EvaluatorExpressionRepresentation > {

 public:
  SimpleExpressionEvaluator();  // XXX Do I need attributes here?

 protected:
  EvaluatorExpressionRepresentation evaluateSynthesizedAttribute(SgNode *node,
      SynthesizedAttributesList synList);

 private:
  EvaluatorExpressionRepresentation getValueExpressionValue(SgValueExp *valExp);
  EvaluatorExpressionRepresentation evaluateVariableReference(SgVarRefExp *vRef);
};


namespace SageInterface {
  long long evaluateSimpleIntegerExpression(SgExpression *expr);
}

#endif
