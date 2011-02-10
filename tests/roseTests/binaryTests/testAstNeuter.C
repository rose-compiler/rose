#include "rose.h"

/* Neuters an AST, preventing the backend from generating a binary file. */
struct Neuter: public SgSimpleProcessing {
    Neuter(SgProject *p) {
        traverse(p, preorder);
    }
    void visit(SgNode *node) {
        if (isSgAsmGenericFile(node))
            isSgAsmGenericFile(node)->set_neuter(true);
    }
};

int
main(int argc, char *argv[])
{
    std::string ast_file_name = "testAstNeuter.ast";

    SgProject *p1 = frontend(argc, argv);
    ROSE_ASSERT(p1!=NULL);
    Neuter x(p1);
    AST_FILE_IO::startUp(p1);
    AST_FILE_IO::writeASTToFile(ast_file_name);
    AST_FILE_IO::clearAllMemoryPools();

    SgProject *p2 = AST_FILE_IO::readASTFromFile(ast_file_name);
    ROSE_ASSERT(p2!=NULL);

    return backend(p2);
}

    
