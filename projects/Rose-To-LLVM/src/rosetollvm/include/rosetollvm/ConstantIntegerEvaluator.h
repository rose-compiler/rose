#ifndef CONSTANT_INTEGER_EVALUATOR
#define CONSTANT_INTEGER_EVALUATOR

#include <rosetollvm/ConstantValue.h>
#include <rosetollvm/LLVMAstAttributes.h>

/**
 *
 */
class ConstantIntegerEvaluator: public AstBottomUpProcessing <ConstantValue> {
    LLVMAstAttributes *attributes;

public:
    ConstantIntegerEvaluator(LLVMAstAttributes *attributes_) : attributes(attributes_)
    {}

    /**
     *
     */
    ConstantValue getValueExpressionValue(SgValueExp *valExp);
    ConstantValue evaluateVariableReference(SgVarRefExp *ref);
    ConstantValue evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList);
};


#endif
