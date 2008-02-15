#include "rose.h"

class Visitor: public AstSimpleProcessing
   {
     public:
         virtual void visit(SgNode* n);
};

void
Visitor::visit(SgNode* n)
   {
     SgBasicBlock* basicBlock = isSgBasicBlock(n);
     if (basicBlock != NULL)
        {
          SgTreeCopy tc;
#if 1
          std::string basicBlockString = basicBlock->unparseToString();
          printf ("(before copy) forStatementString = %s \n",basicBlockString.c_str());
#endif
#if 1
          SgBasicBlock* copy = isSgBasicBlock(n->copy(tc));
          ROSE_ASSERT (copy != NULL);
#if 1
          basicBlockString = basicBlock->unparseToString();
          printf ("(after copy) forStatementString = %s \n",basicBlockString.c_str());
#endif
          SgScopeStatement* parentScope = isSgScopeStatement(basicBlock->get_parent());
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
