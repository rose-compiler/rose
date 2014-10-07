
#include "testSupport.h"
#include "simpleExpressionEvaluator.hpp"


TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerLiteral42){
  SgExpression *expr = SageBuilder::buildIntVal(42);
  SimpleExpressionEvaluator see;
  int res = see.traverse(expr).getValue();
  EXPECT_EQ(res, 42);
}

TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerAddition){
  SgExpression *lhs = SageBuilder::buildIntVal(42);
  SgExpression *rhs = SageBuilder::buildIntVal(21);
  SgExpression *bop = SageBuilder::buildAddOp(lhs, rhs);
  SimpleExpressionEvaluator see;
  int res = see.traverse(bop).getValue();
  EXPECT_EQ(res, 63);
}

TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerAdditionAddition){
  SgExpression *lhs = SageBuilder::buildIntVal(42);
  SgExpression *rhs = SageBuilder::buildIntVal(21);
  SgExpression *iVal13 = SageBuilder::buildIntVal(13);
  SgExpression *bop = SageBuilder::buildAddOp(lhs, rhs);
  SgExpression *expr = SageBuilder::buildAddOp(bop, iVal13);
  SimpleExpressionEvaluator see;
  int res = see.traverse(expr).getValue();
  EXPECT_EQ(res, 76);
}
