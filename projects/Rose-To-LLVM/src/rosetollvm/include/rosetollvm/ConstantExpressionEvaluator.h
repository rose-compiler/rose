#ifndef CONSTANT_EXPRESSION_EVALUATOR
#define CONSTANT_EXPRESSION_EVALUATOR

#include <rosetollvm/ConstantValue.h>
#include <rosetollvm/LLVMAstAttributes.h>

/**
 *
 */
class ConstantExpressionEvaluator: public AstBottomUpProcessing <ConstantValue> {
protected:
    LLVMAstAttributes *attributes;
    
public:
    ConstantExpressionEvaluator(LLVMAstAttributes *attributes_) : attributes(attributes_)
    {}

    ConstantValue getValueExpressionValue(SgValueExp *valExp);
    ConstantValue evaluateVariableReference(SgVarRefExp *ref);
    ConstantValue evaluateSynthesizedAttribute(SgNode *some_node, SynthesizedAttributesList synList);
};


#endif
