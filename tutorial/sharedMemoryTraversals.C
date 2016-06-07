#include <rose.h>

#ifdef _REENTRANT                                       // Does user want ROSE to be thread-aware?

class NodeTypeTraversal: public AstSimpleProcessing {
public:
    NodeTypeTraversal(enum VariantT variant, std::string typeName)
        : myVariant(variant), typeName(typeName) {
    }

protected:
    virtual void visit(SgNode *node) {
        if (node->variantT() == myVariant) {
            std::cout << "Found " << typeName;
            if (SgLocatedNode *loc = isSgLocatedNode(node)) {
                Sg_File_Info *fi = loc->get_startOfConstruct();
                if (fi->isCompilerGenerated()) {
                    std::cout << ": compiler generated";
                } else {
                    std::cout << ": " << fi->get_filenameString()
                              << ":" << fi->get_line();
                }
            }
            std::cout << std::endl;
        }
    }

private:
    enum VariantT myVariant;
    std::string typeName;
};

int main(int argc, char **argv) {
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

    SgProject *project = frontend(argc, argv);

    std::cout << "combined execution of traversals" << std::endl;
    AstSharedMemoryParallelSimpleProcessing parallelTraversal(5);
    parallelTraversal.addTraversal(new NodeTypeTraversal(V_SgForStatement, "for loop"));
    parallelTraversal.addTraversal(new NodeTypeTraversal(V_SgIntVal, "int constant"));
    parallelTraversal.addTraversal(new NodeTypeTraversal(V_SgVariableDeclaration, "variable declaration"));
    parallelTraversal.traverse(project, preorder);
    std::cout << std::endl;

    std::cout << "shared-memory parallel execution of traversals" << std::endl;
    parallelTraversal.traverseInParallel(project, preorder);
}

#else

int main() {
    std::cout <<"parallel traversal is not supported in this configuration (user does not want ROSE to be thread-aware)\n";
}

#endif
