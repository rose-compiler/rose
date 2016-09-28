#include <rose.h>
#include <iostream>

bool unparse_all = false;

class TypeUnparser: public ROSE_VisitTraversal {
public:
    void visit(SgNode *n) {
        if (SgType *type= isSgType(n))
            std::cout << type->unparseToString() << std::endl;
    }
};

class TypeUnparserOnAST: public AstSimpleProcessing {
public:
    void visit(SgNode *n) {
        if (unparse_all) {
            if (SgFunctionDeclaration *d = isSgFunctionDeclaration(n))
                std::cout << "function declaration, name = " << d->get_name().getString() << std::endl;
            std::cout <<n->unparseToString() <<std::endl;
        }

        if (SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n)) {
            SgInitializedNamePtrList &varList = variableDeclaration->get_variables();
            for (SgInitializedNamePtrList::iterator i=varList.begin(); i!=varList.end(); ++i)
                std::cout << (*i)->get_typeptr()->unparseToString() << std::endl;
        }
    }
};

int main(int argc, char **argv)
{
    // Parse command line args for things we recognize
    for (int argno=1; argno<argc; ++argno) {
        if (!strcmp(argv[argno], "--all")) {
            unparse_all = true;
            memmove(argv+argno, argv+argno+1, (argc-argno)*sizeof(argv[0]));
            --argno; --argc;
        }
    }

    // The frontend() will parse the rest of the arguments
    SgProject *p= frontend( argc, argv );
    ROSE_ASSERT(p);

    TypeUnparserOnAST t1;
    t1.traverse(p,preorder);

    TypeUnparser t2;
    t2.traverseMemoryPool();
}

