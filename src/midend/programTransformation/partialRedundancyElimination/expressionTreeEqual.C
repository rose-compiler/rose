// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "expressionTreeEqual.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// using namespace PRE;


bool isFunctionSideEffectFree(SgExpression* decl) {
  return false; // FIXME
}

bool expressionTreeEqualStar(const SgExpressionPtrList& as,
                             const SgExpressionPtrList& bs) {
  SgExpressionPtrList::const_iterator i, j;
  for (i = as.begin(), j = bs.begin(); i != as.end() && j != bs.end(); ++i, ++j)
    if (!expressionTreeEqual(*i, *j))
      return false;
  if (i != as.end() || j != bs.end())
    return false; // Mismatched lengths
  return true;
}

bool expressionTreeEqual(SgExpression* a, SgExpression* b) {
  // cout << "Comparing " << a->unparseToString() << " of type " << a->sage_class_name() << " with " << b->unparseToString() << " of type " << b->sage_class_name() << endl;
  if (a->variantT() != b->variantT())
    return false;
  if (isSgBinaryOp(a)) {
    assert (isSgBinaryOp(b));
    return expressionTreeEqual(isSgBinaryOp(a)->get_lhs_operand(),
                               isSgBinaryOp(b)->get_lhs_operand()) &&
           expressionTreeEqual(isSgBinaryOp(a)->get_rhs_operand(),
                               isSgBinaryOp(b)->get_rhs_operand());
  }
  if (isSgUnaryOp(a)) {
    assert (isSgUnaryOp(b));
    return expressionTreeEqual(isSgUnaryOp(a)->get_operand(),
                               isSgUnaryOp(b)->get_operand());
  }
  if (isSgConditionalExp(a)) {
    assert (isSgConditionalExp(b));
    return expressionTreeEqual(isSgConditionalExp(a)->get_conditional_exp(),
                               isSgConditionalExp(b)->get_conditional_exp()) &&
           expressionTreeEqual(isSgConditionalExp(a)->get_true_exp(),
                               isSgConditionalExp(b)->get_true_exp()) &&
           expressionTreeEqual(isSgConditionalExp(a)->get_false_exp(),
                               isSgConditionalExp(b)->get_false_exp());
  }
  if (isSgDeleteExp(a)) {
    assert (isSgDeleteExp(b));
    return false;
  }
  if (isSgExprListExp(a)) {
    assert (isSgExprListExp(b));
    return expressionTreeEqualStar(isSgExprListExp(a)->get_expressions(),
                                   isSgExprListExp(b)->get_expressions());
  }
  if (isSgFunctionCallExp(a)) {
    assert (isSgFunctionCallExp(b));
    return expressionTreeEqual(isSgFunctionCallExp(a)->get_function(),
                               isSgFunctionCallExp(b)->get_function()) &&
           expressionTreeEqual(isSgFunctionCallExp(a)->get_args(),
                               isSgFunctionCallExp(b)->get_args()) &&
           isFunctionSideEffectFree(isSgFunctionCallExp(a)->get_function());
  }
  if (isSgFunctionRefExp(a)) {
    assert (isSgFunctionRefExp(b));
    return (isSgFunctionRefExp(a)->get_symbol()->get_declaration() ==
            isSgFunctionRefExp(b)->get_symbol()->get_declaration());
  }
  if (isSgAssignInitializer(a)) {
    assert (isSgAssignInitializer(b));
    return expressionTreeEqual(isSgAssignInitializer(a)->get_operand(),
                               isSgAssignInitializer(b)->get_operand());
  }
  if (isSgAggregateInitializer(a)) {
    assert (isSgAggregateInitializer(b));
    return expressionTreeEqual(isSgAggregateInitializer(a)->get_initializers(),
                               isSgAggregateInitializer(b)->get_initializers());
  }
  if (isSgConstructorInitializer(a)) {
    assert (isSgConstructorInitializer(b));
    return false; // FIXME
  }
  if (isSgMemberFunctionRefExp(a)) {
    assert (isSgMemberFunctionRefExp(b));
    return (isSgMemberFunctionRefExp(a)->get_symbol()->get_declaration() ==
            isSgMemberFunctionRefExp(b)->get_symbol()->get_declaration());
  }
  if (isSgNewExp(a)) {
    assert (isSgNewExp(b));
    return false;
  }
  if (isSgRefExp(a)) {
    assert (isSgRefExp(b));
    assert (!"FIXME");
  }
  if (isSgSizeOfOp(a)) {
    assert (isSgSizeOfOp(b));
    return expressionTreeEqual(isSgSizeOfOp(a)->get_operand_expr(),
                               isSgSizeOfOp(b)->get_operand_expr());
  }
  if (isSgThisExp(a)) {
    assert (isSgThisExp(b));
    return true;
  }
  if (isSgValueExp(a)) {
    assert (isSgValueExp(b));
    switch (a->variantT()) {
#define HANDLEVAL(type) \
      case V_##type: \
      return is##type(a)->get_value() == is##type(b)->get_value();

      HANDLEVAL(SgBoolValExp);
      HANDLEVAL(SgCharVal);
      HANDLEVAL(SgDoubleVal);
      HANDLEVAL(SgEnumVal);
      HANDLEVAL(SgFloatVal);
      HANDLEVAL(SgIntVal);
      HANDLEVAL(SgLongDoubleVal);
      HANDLEVAL(SgLongIntVal);
      HANDLEVAL(SgLongLongIntVal);
      HANDLEVAL(SgShortVal);
      HANDLEVAL(SgStringVal);
      HANDLEVAL(SgUnsignedCharVal);
      HANDLEVAL(SgUnsignedIntVal);
      HANDLEVAL(SgUnsignedLongLongIntVal);
      HANDLEVAL(SgUnsignedLongVal);
      HANDLEVAL(SgUnsignedShortVal);
      HANDLEVAL(SgWcharVal);
#undef HANDLEVAL

      default: assert (!"Bad SgValueExp");
    }
  }
  if (isSgVarRefExp(a)) {
    assert (isSgVarRefExp(b));
    return (isSgVarRefExp(a)->get_symbol()->get_declaration() ==
            isSgVarRefExp(b)->get_symbol()->get_declaration());
  }

  cerr << a->sage_class_name() << endl;
  ROSE_ASSERT (!"FIXME");

  /* Avoid MSVC warning */
     return false;
}
