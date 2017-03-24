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

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgWcharVal* wcharValue = buildWcharVal(42);
   SgExprStatement* exprStatement = buildExprStatement(wcharValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgDoubleVal* real_doubleValue      = buildDoubleVal(42.0);
   SgDoubleVal* imaginary_doubleValue = buildDoubleVal(43.0);
   SgComplexVal* complexValue         = buildComplexVal (real_doubleValue,imaginary_doubleValue);
   SgExprStatement* exprStatement_2   = buildExprStatement(complexValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_2);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgDoubleVal* imaginary_doubleValue_2 = buildDoubleVal(42.0);
   SgComplexVal* imaginaryValue         = buildImaginaryVal (imaginary_doubleValue_2);
   SgExprStatement* exprStatement_3     = buildExprStatement(imaginaryValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_3);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgComplexVal* imaginaryValue_2       = buildImaginaryVal (42.0);
   SgExprStatement* exprStatement_4     = buildExprStatement(imaginaryValue_2);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_4);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgDoubleVal* imaginary_doubleValue_3 = buildDoubleVal(42.0);
   SgComplexVal* imaginaryValue_3       = buildImaginaryVal_nfi (imaginary_doubleValue_3,"42.0i");
   SgExprStatement* exprStatement_5     = buildExprStatement(imaginaryValue_3);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_5);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgIntVal* hexValue               = buildIntValHex (42);
   SgExprStatement* exprStatement_6 = buildExprStatement(hexValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_6);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgLongLongIntVal* longlongValue               = buildLongLongIntVal (42);
   SgExprStatement* exprStatement_7 = buildExprStatement(longlongValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_7);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedCharVal* unsignedCharValue   = buildUnsignedCharVal (42);
   SgExprStatement* exprStatement_8 = buildExprStatement(unsignedCharValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_8);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedCharVal* hexUnsignedCharValue   = buildUnsignedCharValHex (42);
   SgExprStatement* exprStatement_9 = buildExprStatement(hexUnsignedCharValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_9);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgShortVal* shortValue   = buildShortVal (42);
   SgExprStatement* exprStatement_10 = buildExprStatement(shortValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_10);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgShortVal* hexShortValue   = buildShortValHex (42);
   SgExprStatement* exprStatement_11 = buildExprStatement(hexShortValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_11);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedShortVal* unsignedShortValue   = buildUnsignedShortVal (42);
   SgExprStatement* exprStatement_12 = buildExprStatement(unsignedShortValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_12);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedIntVal* unsignedIntValue   = buildUnsignedIntVal (42);
   SgExprStatement* exprStatement_13 = buildExprStatement(unsignedIntValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_13);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedIntVal* hexUnsignedIntValue   = buildUnsignedIntValHex (42);
   SgExprStatement* exprStatement_14 = buildExprStatement(hexUnsignedIntValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_14);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedLongVal* hexUnsignedLongValue   = buildUnsignedLongValHex (42);
   SgExprStatement* exprStatement_15 = buildExprStatement(hexUnsignedLongValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_15);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedLongLongIntVal* unsignedLongLongIntValue   = buildUnsignedLongLongIntVal (42);
   SgExprStatement* exprStatement_16 = buildExprStatement(unsignedLongLongIntValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_16);

// DQ (3/20/2017): Build value expressions not tested elsewhere.
   SgUnsignedLongLongIntVal* hexUnsignedLongLongIntValue   = buildUnsignedLongLongIntValHex (42);
   SgExprStatement* exprStatement_17 = buildExprStatement(hexUnsignedLongLongIntValue);
   laststmt = getLastStatement(topScopeStack());
   insertStatementBefore(laststmt,exprStatement_17);


  popScopeStack();
  AstTests::runAllTests(project);

  //invoke backend compiler to generate object/binary files
   return backend (project);
}


