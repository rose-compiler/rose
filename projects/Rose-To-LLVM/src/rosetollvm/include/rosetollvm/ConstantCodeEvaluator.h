#ifndef CONSTANT_CODE_EVALUATOR
#define CONSTANT_CODE_EVALUATOR

#include <rosetollvm/ConstantExpressionEvaluator.h>

/**
 *
 */
class ConstantCodeEvaluator: public ConstantExpressionEvaluator {
public:
    ConstantCodeEvaluator(LLVMAstAttributes *attributes_) : ConstantExpressionEvaluator(attributes_)
    {}

    ConstantValue evaluateVariableReference(SgVarRefExp *ref);
    ConstantValue evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList);
};

#endif
