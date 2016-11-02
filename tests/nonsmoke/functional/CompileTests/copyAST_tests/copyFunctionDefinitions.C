#include "rose.h"

class Visitor: public AstSimpleProcessing
   {
     public:
         virtual void visit(SgNode* n);
};

void
Visitor::visit(SgNode* n)
   {
     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(n);
     if (functionDefinition != NULL)
        {
          SgTreeCopy tc;
#if 1
          std::string functionDefinitionString = functionDefinition->unparseToString();
          printf ("(before copy) functionDefinitionString = %s \n",functionDefinitionString.c_str());
#endif
#if 1
          SgFunctionDefinition* copy = isSgFunctionDefinition(n->copy(tc));
          ROSE_ASSERT (copy != NULL);
#if 1
          functionDefinitionString = functionDefinition->unparseToString();
          printf ("(after copy) functionDefinitionString = %s \n",functionDefinitionString.c_str());
#endif
          SgFunctionDeclaration* parentScope = isSgFunctionDeclaration(functionDefinition->get_parent());
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
