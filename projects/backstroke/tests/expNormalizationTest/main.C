#include <normalizations/expNormalization.h>

class normalizationTraversal : public AstSimpleProcessing
{
    public:
        virtual void visit(SgNode* n);
};


void normalizationTraversal::visit(SgNode* n)
{
#if 0
    if (SgExpression* exp = isSgExpression(n))
    {
#if 1
        exp = normalizeExpression(exp);
        exp = extendCommaOpExp(exp);
        splitCommaOpExp(exp);
#else
        SageInterface::splitExpressionIntoBasicBlock(exp);
#endif
    }
    else if (SgBasicBlock* body = isSgBasicBlock(n))
        removeUselessBraces(body);
#endif
    if (SgFunctionDefinition* func = isSgFunctionDefinition(n))
        normalizeEvent(func);
}

int main(int argc, char * argv[])
{
    SgProject* project = frontend(argc, argv);

    normalizationTraversal norm;
    norm.traverseInputFiles(project,postorder);

    AstTests::runAllTests(project);
    return backend(project);
}


