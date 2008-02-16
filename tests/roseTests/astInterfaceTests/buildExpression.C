/*! \brief  test sage builders for expressions
*   It shows 
*    - the recommended bottomup approach for building expressions.
8    - topdown approach for building expressions
* Liao, 2/12/2008
*/
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  // go to the function body
  SgFunctionDeclaration* mainFunc= findMain(project);

  SgBasicBlock* body= mainFunc->get_definition()->get_body();
  pushScopeStack(body);
  
 // bottomup: build operands first, create expression later on
  SgExpression * init_exp =  
            buildMultiplyOp(buildDoubleVal(2.0),
                 buildSubtractOp(buildDoubleVal(1.0), 
                      buildMultiplyOp (buildVarRefExp("gama"),buildVarRefExp("gama")
                                      )));
  SgVariableDeclaration* decl = buildVariableDeclaration("result",buildDoubleType(),buildAssignInitializer(init_exp));

  SgStatement* laststmt = getLastStatement(topScopeStack());
  insertStatementBefore(laststmt,decl);

 // topdown: build expression first, set operands later on
  SgExpression * init_exp2 = buildMultiplyOp();
  setLhsOperand(init_exp2,buildVarRefExp("alpha"));
  setRhsOperand(init_exp2,buildVarRefExp("beta"));

  SgVariableDeclaration* decl2 = buildVariableDeclaration("result2",buildDoubleType(),buildAssignInitializer(init_exp2));
  laststmt = getLastStatement(topScopeStack());
  insertStatementBefore(laststmt,decl2);

  popScopeStack();
  AstTests::runAllTests(project);

  //invoke backend compiler to generate object/binary files
   return backend (project);
}


