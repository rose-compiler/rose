#include <rose.h>

class NodeTypeCounter: public AstSimpleProcessing {
public:
    NodeTypeCounter(enum VariantT variant, std::string typeName)
        : myVariant(variant), typeName(typeName), count(0) {
    }

protected:
    virtual void visit(SgNode *node) {
        if (node->variantT() == myVariant) {
            std::cout << "Found " << typeName << std::endl;
            count++;
        }
    }

    virtual void atTraversalEnd() {
        std::cout << typeName << " total: " << count << std::endl;
    }

private:
    enum VariantT myVariant;
    std::string typeName;
    unsigned int count;
};

int main(int argc, char **argv) {
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

    SgProject *project = frontend(argc, argv);

    std::cout << "sequential execution of traversals" << std::endl;
    NodeTypeCounter forStatementCounter(V_SgForStatement, "for loop");
    NodeTypeCounter intValueCounter(V_SgIntVal, "int constant");
    NodeTypeCounter varDeclCounter(V_SgVariableDeclaration, "variable declaration");
    // three calls to traverse, executed sequentially
    forStatementCounter.traverseInputFiles(project, preorder);
    intValueCounter.traverseInputFiles(project, preorder);
    varDeclCounter.traverseInputFiles(project, preorder);
    std::cout << std::endl;

    std::cout << "combined execution of traversals" << std::endl;
    AstCombinedSimpleProcessing combinedTraversal;
    combinedTraversal.addTraversal(new NodeTypeCounter(V_SgForStatement, "for loop"));
    combinedTraversal.addTraversal(new NodeTypeCounter(V_SgIntVal, "int constant"));
    combinedTraversal.addTraversal(new NodeTypeCounter(V_SgVariableDeclaration, "variable declaration"));
    // one call to traverse, execution is interleaved
    combinedTraversal.traverseInputFiles(project, preorder);
}
