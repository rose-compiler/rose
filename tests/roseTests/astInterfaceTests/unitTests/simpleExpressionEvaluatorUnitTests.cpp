
#include "testSupport.h"

TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerLiteral42){
  SgExpression *expr = SageBuilder::buildIntVal(42);
  struct SgExpression::const_int_expr_t res = expr->evaluateConstIntegerExpression();
  EXPECT_EQ(res.hasValue_, true);
  EXPECT_EQ(res.value_, 42);
}

TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerAddition){
  SgExpression *lhs = SageBuilder::buildIntVal(42);
  SgExpression *rhs = SageBuilder::buildIntVal(21);
  SgExpression *bop = SageBuilder::buildAddOp(lhs, rhs);
  struct SgExpression::const_int_expr_t res = bop->evaluateConstIntegerExpression();
  EXPECT_EQ(res.hasValue_, true);
  EXPECT_EQ(res.value_, 63);
}

TEST(SimpleExpressionEvaluator, EvaluateSimpleIntegerAdditionAddition){
  SgExpression *lhs = SageBuilder::buildIntVal(42);
  SgExpression *rhs = SageBuilder::buildIntVal(21);
  SgExpression *iVal13 = SageBuilder::buildIntVal(13);
  SgExpression *bop = SageBuilder::buildAddOp(lhs, rhs);
  SgExpression *expr = SageBuilder::buildAddOp(bop, iVal13);
  struct SgExpression::const_int_expr_t res = expr->evaluateConstIntegerExpression();
  EXPECT_EQ(res.hasValue_, true);
  EXPECT_EQ(res.value_, 76);
}
