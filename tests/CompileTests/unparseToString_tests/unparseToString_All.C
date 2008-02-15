#include <rose.h>
#include <iostream>

using namespace std;

class TypeUnparser : public ROSE_VisitTraversal {
public:
    void visit( SgNode *n );
};

void TypeUnparser::visit( SgNode *n )
   {
     SgType *type= dynamic_cast<SgType*>(n);
     if (!type) return;

     cout << type->unparseToString() << endl;
   }

class TypeUnparserOnAST : public AstSimpleProcessing {
public:
    void visit( SgNode *n );
};

void TypeUnparserOnAST::visit( SgNode *n )
   {
// #ifndef __CYGWIN__
     // PC (9/18/2006): skipping on cygwin
     cout << "calling unparseToString(" << n << ") of type " << n->class_name() << endl;
     if (SgFunctionDeclaration *d = isSgFunctionDeclaration(n)) {
        cout << "function declaration, name = " << d->get_name().getString() << endl;
     }
     string s = n->unparseToString();
     cout << s << endl;
// #endif
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
     if (variableDeclaration != NULL)
        {
          SgInitializedNamePtrList & varList = variableDeclaration->get_variables();
          SgInitializedNamePtrList::iterator i = varList.begin();
          while (i != varList.end())
             {
               cout << (*i)->get_typeptr()->unparseToString() << endl;
               i++;
             }
        }
   }


int main( int argc, char **argv )
   {
     SgProject *p= frontend( argc, argv );
     ROSE_ASSERT(p);

     TypeUnparserOnAST t1;
     t1.traverse(p,preorder);

#if 1
     TypeUnparser t2;
     t2.traverseMemoryPool();
#endif

     return backend(p);
   }

