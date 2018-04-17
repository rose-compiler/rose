#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

SgProject*project;

class visitorTraversal : public AstSimpleProcessing
{
      bool gone;  //this variable is used to avoid multiple analysis
public:
      visitorTraversal();
      virtual void visit(SgNode* n);
      virtual void atTraversalEnd();
};

visitorTraversal::visitorTraversal()
{
      gone = false;
}

void visitorTraversal::visit (SgNode*n)
{
      SgIfStmt *s = isSgIfStmt(n);

      if (!s)
            return;
     
      if(gone)
      {
            gone = false;
            return;
      }

      SgBasicBlock *block = isSgBasicBlock(s->get_parent());

   // SgStatement *st = s->get_conditional();
      SgExpression *st = isSgExprStatement(s->get_conditional())->get_expression();
      if (!st)
            printf("That was not an expr!\n");

     
      pushScopeStack(block/*findMain(project)->get_definition()->get_body()*/);

   // SgStatementExpression *x = new SgStatementExpression(st);
      SgTreeCopy treeCopy;
      SgExpression *x = isSgExpression(st->copy(treeCopy));
      ROSE_ASSERT(x != NULL);

      SgExpression *expr = isSgExpression(x);
      if(!expr)
            printf("can't do that!\n");

      SgVariableDeclaration *dec = buildVariableDeclaration("tmp", buildIntType(), buildAssignInitializer(expr));

      insertStatementBefore(s/*getPreviousStatement(topScopeStack()), dec*/,dec);
     
      gone = true;

      popScopeStack();
}

void visitorTraversal::atTraversalEnd()
{
}


int main (int argc, char*argv[])
{
      if(SgProject::get_verbose()>0)
            printf("InvisitorTraversal.C:main()\n");
      project = frontend(argc,argv);
     
      visitorTraversal exampleTraversal;

      exampleTraversal.traverseInputFiles(project, preorder);

   // project->unparse();
   // return 0;
      return backend(project);
}
