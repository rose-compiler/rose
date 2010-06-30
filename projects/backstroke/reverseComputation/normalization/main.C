#include "normalization.h"
#include <rose.h>

using namespace std;

class normalizationTraversal : public AstSimpleProcessing
{
    public:
        normalizationTraversal() 
            : AstSimpleProcessing()
    {}
        virtual void visit(SgNode* n);
};


void normalizationTraversal::visit(SgNode* n)
{
    if (SgExpression* exp = isSgExpression(n))
    {
#if 1
        exp = normalizeExpression(exp);
        splitCommaOpExp(exp);
#else
        SageInterface::splitExpressionIntoBasicBlock(exp);
#endif
    }
    else if (SgBasicBlock* body = isSgBasicBlock(n))
        removeUselessBraces(body);
}

int main(int argc, char * argv[])
{
    int i = 0;
    int a[10];
    a[i++] = i;

    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);

    normalizationTraversal norm;
    norm.traverseInputFiles(project,postorder);

    AstTests::runAllTests(project);
    return backend(project);
}


