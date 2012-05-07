
#include "toolboxes/rose-toolbox.hpp"

#include "rose.h"

SgExpression * vecToExp(std::vector<std::pair<RoseVariable, int> > & vec) {
  if (vec.size() == 0)
    return SageBuilder::buildIntVal(0);

  SgScopeStatement * scope = SageBuilder::topScopeStack();
  SgExpression * res = NULL;
  if (isConstant(vec[0].first))
    res = SageBuilder::buildIntVal(vec[0].second);
  else
    res = SageBuilder::buildMultiplyOp(
      SageBuilder::buildIntVal(vec[0].second),
      vec[0].first.generate(scope)
    );

  if (vec.size() == 1)
    return res;

  std::vector<std::pair<RoseVariable, int> >::iterator it;
  for (it = vec.begin()+1; it != vec.end(); it++) {
    SgExpression * tmp = NULL;
    if (isConstant(it->first))
      tmp = SageBuilder::buildIntVal(it->second);
    else
      tmp = SageBuilder::buildMultiplyOp(
        SageBuilder::buildIntVal(it->second),
        it->first.generate(scope)
      );
    res = SageBuilder::buildAddOp(res, tmp);
  }

  return res;
}

SgExpression * genAnd(std::vector<SgExpression *> & terms) {
  if (terms.size() == 0)
    return SageBuilder::buildIntVal(1);

  SgExpression * res = terms[0];

  for (unsigned i = 1; i < terms.size(); i++)
    res = SageBuilder::buildAndOp(res, terms[i]);

  return res;
}

SgExpression * genMin(std::vector<SgExpression *> & terms) {
  assert(false); // TODO
  return NULL;
}

SgExpression * genMax(std::vector<SgExpression *> & terms) {
  assert(false); // TODO
  return NULL;
}

SgExpression * simplify(SgExpression * exp) {
  // TODO create a real one
  SgExpression * res = NULL;

  SgCastExp * cast_exp = isSgCastExp(exp);
  if (cast_exp != NULL) return cast_exp->get_operand_i();

  SgBinaryOp * bin_op = isSgBinaryOp(exp);
  if (bin_op != NULL) {
    SgValueExp * lhs = isSgValueExp(simplify(bin_op->get_lhs_operand_i()));
    SgValueExp * rhs = isSgValueExp(simplify(bin_op->get_rhs_operand_i()));
    if (lhs != NULL && rhs != NULL) {
      long lhs_val = 0;
      switch (lhs->variantT()) {
        case V_SgIntVal:           lhs_val = isSgIntVal(lhs)->get_value();          break;
        case V_SgUnsignedIntVal:   lhs_val = isSgUnsignedIntVal(lhs)->get_value();  break;
        case V_SgLongIntVal:       lhs_val = isSgLongIntVal(lhs)->get_value();      break;
        case V_SgUnsignedLongVal:  lhs_val = isSgUnsignedLongVal(lhs)->get_value(); break;
        default:
          assert(false);
      }
      long rhs_val = 0;
      switch (rhs->variantT()) {
        case V_SgIntVal:           rhs_val = isSgIntVal(rhs)->get_value();          break;
        case V_SgUnsignedIntVal:   rhs_val = isSgUnsignedIntVal(rhs)->get_value();  break;
        case V_SgLongIntVal:       rhs_val = isSgLongIntVal(rhs)->get_value();      break;
        case V_SgUnsignedLongVal:  rhs_val = isSgUnsignedLongVal(rhs)->get_value(); break;
        default:
          assert(false);
      }
      long res_val = 0;
      switch (bin_op->variantT()) {
        case V_SgAddOp:      res_val = lhs_val + rhs_val; break;
        case V_SgSubtractOp: res_val = lhs_val - rhs_val; break;
        case V_SgMultiplyOp: res_val = lhs_val * rhs_val; break;
        case V_SgDivideOp:   res_val = lhs_val / rhs_val; break;
        default:
          assert(false);
      }
      res = SageBuilder::buildLongIntVal(res_val);
    }
    else {
      res = exp;
    }
  }
  else {
    res = exp;
  }

  return res;
}

