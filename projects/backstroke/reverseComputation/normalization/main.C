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

const int& foo()
{
    return 1;
}

int main(int argc, char * argv[])
{
    int i = 0;
    int a[10];
    a[i++] = i;
    a[0] = i++ + i++;

    //foo();
    //return 0;

    //cout << (const_cast<char*>("hello")[0] = 'a') << endl;

    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);

    normalizationTraversal norm;
    norm.traverseInputFiles(project,postorder);

    AstTests::runAllTests(project);
    return backend(project);
}


