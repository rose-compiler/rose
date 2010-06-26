#include "../normalization.h"
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
        normalizeExpression(exp);
}

int main(int argc, char * argv[])
{
    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);

    normalizationTraversal norm;
    norm.traverseInputFiles(project,postorder);

    //AstTests::runAllTests(project);
    return backend(project);
}


