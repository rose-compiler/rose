#include "rose.h"

class Visitor: public AstSimpleProcessing
   {
     public:
         virtual void visit(SgNode* n);
};

void
Visitor::visit(SgNode* n)
   {
     SgForStatement* forStatement = isSgForStatement(n);
     if (forStatement != NULL)
        {
          SgTreeCopy tc;
#if 0
          std::string forStatementString = forStatement->unparseToString();
          printf ("(before copy) forStatementString = %s \n",forStatementString.c_str());
#endif
#if 1
          SgForStatement* copy = isSgForStatement(n->copy(tc));
          ROSE_ASSERT (copy != NULL);
#if 0
          forStatementString = forStatement->unparseToString();
          printf ("(after copy) forStatementString = %s \n",forStatementString.c_str());
#endif
          SgScopeStatement* parentScope = isSgScopeStatement(forStatement->get_parent());
          ROSE_ASSERT (parentScope);
          copy->set_parent(parentScope);
#endif
       }
   }

int main( int argc, char * argv[] )
   {
     SgProject* sageProject = frontend(argc,argv);
     AstTests::runAllTests(sageProject);
     Visitor v;
#if 1
     v.traverse(sageProject, postorder);
#endif

     generateAstGraph(sageProject, 4000);

     return 0;
   }
