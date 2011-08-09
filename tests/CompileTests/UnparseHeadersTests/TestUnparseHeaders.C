#include "rose.h"

#include "UnparseHeadersTransformVisitor.h"

int main(int argc, char* argv[]) {

    ROSE_ASSERT(argc > 1);

    SgProject* projectNode = frontend(argc, argv);
    
    //AstTests::runAllTests(projectNode);

    UnparseHeadersTransformVisitor transformVisitor;
    transformVisitor.traverse(projectNode, preorder);

    return backend(projectNode);
    
}