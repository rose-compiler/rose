#include <rose.h>
#include <iostream>

using namespace std;

bool unparse_all = false;

class TypeUnparser : public ROSE_VisitTraversal {
public:
    void visit( SgNode *n );
};

void TypeUnparser::visit( SgNode *n )
{
    SgType *type= dynamic_cast<SgType*>(n);
    if (!type)
        return;
    cout << type->unparseToString() << endl;
}

class TypeUnparserOnAST : public AstSimpleProcessing {
public:
    void visit( SgNode *n );
};

void TypeUnparserOnAST::visit( SgNode *n )
{
    if (unparse_all) {
        if (SgFunctionDeclaration *d = isSgFunctionDeclaration(n)) {
            cout << "function declaration, name = " << d->get_name().getString() << endl;
        }
        string s = n->unparseToString();
        cout << s << endl;
    }

    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
    if (variableDeclaration != NULL) {
        SgInitializedNamePtrList & varList = variableDeclaration->get_variables();
        SgInitializedNamePtrList::iterator i = varList.begin();
        while (i != varList.end()) {
            cout << (*i)->get_typeptr()->unparseToString() << endl;
            i++;
        }
    }
}

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

