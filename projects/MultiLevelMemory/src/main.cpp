#include "mlmapi.h"
using namespace std;
using namespace MLMAPIInsertion;
using namespace SageInterface;
vector<SgStatement*> DeletepragmasList;
vector<SgCallExpression*> callExprList;
vector<SgForStatement*> forStmtList;

int main (int argc, char** argv)
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc, argv);
    // Run internal consistency tests on AST
    AstTests::runAllTests(project);

    // Insert your own manipulations of the AST here...
    mlmFrontend mlmfrontend;
    mlmfrontend.traverse(project,preorder);
    mlmTransform mlmtransform;
    mlmtransform.insertHeaders(project);
    mlmtransform.traverse(project,preorder);

    // transform all the functonCallExpression
    for(vector<SgCallExpression*>::iterator i=callExprList.begin(); i!=callExprList.end(); ++i)
    {
      mlmtransform.transformCallExp(*i);
    }

    for(vector<SgForStatement*>::iterator i=forStmtList.begin(); i!=forStmtList.end(); ++i)
    {
      mlmtransform.transformForStmt(*i);
    }

    for(vector<SgStatement*>::iterator iter = DeletepragmasList.begin(); iter != DeletepragmasList.end(); iter++)
       removeStatement(*iter);
    // Generate source code from AST and invoke your
    // desired backend compiler
       return backend(project);
}
