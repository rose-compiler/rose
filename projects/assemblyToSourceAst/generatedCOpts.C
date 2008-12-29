#include "rose.h"
#include "x86AssemblyToC.h"
#include "filteredCFG.h"
#include <fstream>
#include "generatedCOpts.h"

// This file implements general optimizations for the generate C code.

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace VirtualCFG;

// FIXME: put these in a common location
static SgType* lengthToType(size_t Len) {
  if (Len <= 1) return SgTypeBool::createType();
  if (Len <= 8) return SgTypeUnsignedChar::createType();
  if (Len <= 16) return SgTypeUnsignedShort::createType();
  if (Len <= 32) return SgTypeUnsignedInt::createType();
  if (Len <= 64) return SgTypeUnsignedLongLong::createType();
  cerr << "lengthToType(" << Len << ")" << endl; abort();
}

static SgExpression* constantOfLength(size_t Len, uintmax_t c) {
  if (Len <= 1) return buildBoolValExp((bool)c);
  if (Len <= 32) return buildUnsignedIntValHex(c);
  if (Len <= 64) return buildUnsignedLongLongIntValHex(c);
  cerr << "constantOfLength(" << Len << ")" << endl; abort();
}

static size_t typeToLength(SgType* t) {
  switch (t->variantT()) {
    case V_SgTypeBool: return 1;
    case V_SgTypeUnsignedChar: return 8;
    case V_SgTypeUnsignedShort: return 16;
    case V_SgTypeUnsignedInt: return 32;
    case V_SgTypeInt: return 32;
    case V_SgTypeUnsignedLongLong: return 64;
    case V_SgTypedefType: return typeToLength(isSgTypedefType(t)->get_base_type());
    default: {cerr << "typeToLength: " << t->class_name() << endl; abort();}
  }
}

unsigned long long getValue(SgExpression* e) {
  switch (e->variantT()) {
    case V_SgBoolValExp: return isSgBoolValExp(e)->get_value();
    case V_SgIntVal: return isSgIntVal(e)->get_value();
    case V_SgUnsignedIntVal: return isSgUnsignedIntVal(e)->get_value();
    case V_SgCharVal: return isSgCharVal(e)->get_value();
    case V_SgUnsignedCharVal: return isSgUnsignedCharVal(e)->get_value();
    case V_SgUnsignedShortVal: return isSgUnsignedShortVal(e)->get_value();
    case V_SgUnsignedLongVal: return isSgUnsignedLongVal(e)->get_value();
    case V_SgUnsignedLongLongIntVal: return isSgUnsignedLongLongIntVal(e)->get_value();
    default: cerr << "Unhandled variant " << e->class_name() << endl; abort();
  }
}

SgValueExp* buildConstantOfType(unsigned long long val, SgType* t) {
  t = t->stripType(); // Remove consts
  switch (t->variantT()) {
    case V_SgTypeUnsignedChar: return buildUnsignedCharValHex(val);
    case V_SgTypeUnsignedShort: return buildUnsignedShortValHex(val);
    case V_SgTypeInt: return buildIntValHex(val);
    case V_SgTypeUnsignedInt: return buildUnsignedIntValHex(val);
    case V_SgTypeUnsignedLong: return buildUnsignedLongValHex(val);
    case V_SgTypeUnsignedLongLong: return buildUnsignedLongLongIntValHex(val);
    case V_SgTypeBool: return buildBoolValExp((bool)val);
    default: cerr << "buildConstantOfType: " << t->class_name() << endl; ROSE_ASSERT (!"Bad type in buildConstantOfType");
  }
}

// Returns true only if top level expr was changed
bool simplifyExpression(SgExpression*& expr, bool& changed) {
  // cerr << "simplifyExpression " << expr->class_name() << endl;
  SgUnaryOp* uo = isSgUnaryOp(expr);
  SgBinaryOp* bo = isSgBinaryOp(expr);
  if (isSgValueExp(expr)) {
    return false;
  } else if (uo) {
    SgExpression* op = uo->get_operand();
    if (simplifyExpression(op, changed)) {
      uo->set_operand(op);
      op->set_parent(uo);
    }
    if (isSgCastExp(expr)) {
      SgType* exprt = expr->get_type()->stripType();
      SgType* opt = op->get_type()->stripType();
      if (isSgValueExp(op)) {
        expr = buildConstantOfType(getValue(op), exprt);
        changed = true;
        return true;
      } else if (isSgTypeUnsignedInt(exprt) && isSgTypeUnsignedInt(opt) ||
                 exprt == opt && isSgCastExp(op)) {
        expr = op;
        changed = true;
        return true;
      }
    } else if (isSgNotOp(expr) && isSgEqualityOp(op)) {
      expr = buildNotEqualOp(isSgEqualityOp(op)->get_lhs_operand(), isSgEqualityOp(op)->get_rhs_operand());
      changed = true;
      return true;
    } else if (isSgNotOp(expr) && isSgNotEqualOp(op)) {
      expr = buildEqualityOp(isSgNotEqualOp(op)->get_lhs_operand(), isSgNotEqualOp(op)->get_rhs_operand());
      changed = true;
      return true;
    } else if (isSgNotOp(expr) && isSgLessThanOp(op)) {
      expr = buildLessOrEqualOp(isSgLessThanOp(op)->get_rhs_operand(), isSgLessThanOp(op)->get_lhs_operand());
      changed = true;
      return true;
    } else if (isSgNotOp(expr) && isSgGreaterThanOp(op)) {
      expr = buildLessOrEqualOp(isSgGreaterThanOp(op)->get_lhs_operand(), isSgGreaterThanOp(op)->get_rhs_operand());
      changed = true;
      return true;
    } else if (isSgNotOp(expr) && isSgLessOrEqualOp(op)) {
      expr = buildGreaterThanOp(isSgLessOrEqualOp(op)->get_lhs_operand(), isSgLessOrEqualOp(op)->get_rhs_operand());
      changed = true;
      return true;
    } else if (isSgNotOp(expr) && isSgGreaterOrEqualOp(op)) {
      expr = buildLessThanOp(isSgGreaterOrEqualOp(op)->get_lhs_operand(), isSgGreaterOrEqualOp(op)->get_rhs_operand());
      changed = true;
      return true;
    }
    return false;
  } else if (isSgAssignInitializer(expr)) {
    SgExpression* op = isSgAssignInitializer(expr)->get_operand();
    if (simplifyExpression(op, changed)) {
      isSgAssignInitializer(expr)->set_operand(op);
      op->set_parent(isSgAssignInitializer(expr));
    }
    return false;
  } else if (bo) {
    SgExpression* op1 = bo->get_lhs_operand();
    if (simplifyExpression(op1, changed)) {
      bo->set_lhs_operand(op1);
      op1->set_parent(bo);
    }
    SgExpression* op2 = bo->get_rhs_operand();
    if (simplifyExpression(op2, changed)) {
      bo->set_rhs_operand(op2);
      op2->set_parent(bo);
    }
    SgType* op1type = op1->get_type()->stripType();
    // SgType* op2type = op2->get_type()->stripType();
    SgType* exprtype = expr->get_type()->stripType();
    if (isSgBitAndOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) & getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) &&
               isSgValueExp(op2) &&
               (getValue(op2) == (unsigned int)(~0UL)) &&
               isSgTypeUnsignedInt(op1type)) { // Remove unnecessary mask operations
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && isSgValueExp(op1) && getValue(op1) == 0) {
      expr = buildConstantOfType(0, exprtype);
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && isSgValueExp(op2) && getValue(op2) == 0) {
      expr = buildConstantOfType(0, exprtype);
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && isSgValueExp(op1)) { // Must be after the case for where both are value exprs
      bo->set_lhs_operand(op2);
      bo->set_rhs_operand(op1);
      return simplifyExpression(expr, changed);
    } else if (isSgBitAndOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && isSgBitOrOp(op1)) {
      expr = buildBitOrOp(buildBitAndOp(isSgBitOrOp(op1)->get_lhs_operand(), copyExpression(op2)), buildBitAndOp(isSgBitOrOp(op1)->get_rhs_operand(), op2));
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && isSgValueExp(op2) &&
               isSgBitAndOp(op1) && isSgValueExp(isSgBitAndOp(op1)->get_rhs_operand())) {
      expr = buildBitAndOp(isSgBitAndOp(op1)->get_lhs_operand(), buildConstantOfType(getValue(op2) & getValue(isSgBitAndOp(op1)->get_rhs_operand()), exprtype));
      changed = true;
      return true;
    } else if (isSgBitAndOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitOrOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) | getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgBitOrOp(expr) &&
               isSgValueExp(op2) &&
               (getValue(op2) == ~0UL || getValue(op2) == ~0ULL) &&
               isSgTypeUnsignedInt(op1type)) { // Remove ors that don't use their input
      expr = op2;
      changed = true;
      return true;
    } else if (isSgBitOrOp(expr) && isSgValueExp(op1) && getValue(op1) == 0) {
      expr = op2;
      changed = true;
      return true;
    } else if (isSgBitOrOp(expr) && isSgValueExp(op2) && getValue(op2) == 0) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitOrOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitXorOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) ^ getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgBitXorOp(expr) && isSgValueExp(op1) && getValue(op1) == 0) {
      expr = op2;
      changed = true;
      return true;
    } else if (isSgBitXorOp(expr) && isSgValueExp(op2) && getValue(op2) == 0) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgBitXorOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = buildConstantOfType(0, exprtype);
      changed = true;
      return true;
    } else if (isSgAndOp(expr) && isSgBoolValExp(op1)) {
      expr = getValue(op1) ? op2 : op1;
      changed = true;
      return true;
    } else if (isSgAndOp(expr) && isSgBoolValExp(op2)) {
      expr = getValue(op2) ? op1 : op2;
      changed = true;
      return true;
    } else if (isSgOrOp(expr) && isSgBoolValExp(op1)) {
      expr = getValue(op1) ? op1 : op2;
      changed = true;
      return true;
    } else if (isSgOrOp(expr) && isSgBoolValExp(op2)) {
      expr = getValue(op2) ? op2 : op1;
      changed = true;
      return true;
    } else if (isSgEqualityOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildBoolValExp(getValue(op1) == getValue(op2));
      changed = true;
      return true;
    } else if (isSgEqualityOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = buildBoolValExp(true);
      changed = true;
      return true;
    } else if (isSgEqualityOp(expr) && isSgSubtractOp(op1) && isSgValueExp(op2)) {
      expr = buildEqualityOp(isSgSubtractOp(op1)->get_lhs_operand(), buildAddOp(isSgSubtractOp(op1)->get_rhs_operand(), op2));
      changed = true;
      return true;
    } else if (isSgEqualityOp(expr) && isSgAddOp(op1) && isSgValueExp(isSgAddOp(op1)->get_rhs_operand()) && isSgValueExp(op2)) {
      expr = buildEqualityOp(isSgAddOp(op1)->get_lhs_operand(), buildConstantOfType(getValue(op2) - getValue(isSgAddOp(op1)->get_rhs_operand()), op1type));
      changed = true;
      return true;
    } else if (isSgNotEqualOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildBoolValExp(getValue(op1) != getValue(op2));
      changed = true;
      return true;
    } else if (isSgNotEqualOp(expr) && isSgBoolValExp(op2)) {
      expr = getValue(op2) ? buildNotOp(op1) : op1;
      changed = true;
      return true;
    } else if (isSgNotEqualOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = buildBoolValExp(false);
      changed = true;
      return true;
    } else if (isSgLessThanOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildBoolValExp(getValue(op1) < getValue(op2));
      changed = true;
      return true;
    } else if (isSgLessThanOp(expr) && isSgValueExp(op2) && getValue(op2) == 0 && (op1type->variantT() == V_SgTypeUnsignedInt || op1type->variantT() == V_SgTypeUnsignedLong || op1type->variantT() == V_SgTypeUnsignedLongLong || op1type->variantT() == V_SgTypeUnsignedChar || op1type->variantT() == V_SgTypeUnsignedShort)) {
      expr = buildBoolValExp(false);
      changed = true;
      return true;
    } else if (isSgAddOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) + getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgAddOp(expr) && isSgValueExp(op1) && getValue(op1) == 0) {
      expr = op2;
      changed = true;
      return true;
    } else if (isSgAddOp(expr) && isSgValueExp(op2) && getValue(op2) == 0) {
      expr = op1;
      changed = true;
      return true;
    } else if (isSgAddOp(expr) && isSgValueExp(op2) &&
               isSgAddOp(op1) && isSgValueExp(isSgAddOp(op1)->get_rhs_operand())) {
      expr = buildAddOp(isSgAddOp(op1)->get_lhs_operand(), buildConstantOfType(getValue(op2) + getValue(isSgAddOp(op1)->get_rhs_operand()), exprtype));
      changed = true;
      return true;
    } else if (isSgSubtractOp(expr) && isSgValueExp(op2)) {
      expr = buildCastExp(buildAddOp(op1, buildConstantOfType(-getValue(op2), exprtype)), exprtype);
      changed = true;
      simplifyExpression(expr, changed);
      return true;
    } else if (isSgSubtractOp(expr) && expressionTreeEqual(op1, op2)) {
      expr = buildConstantOfType(0, exprtype);
      changed = true;
      return true;
    } else if (isSgModOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) % getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgLshiftOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType((getValue(op1) << getValue(op2)) & IntegerOps::genMask<uintmax_t>(typeToLength(exprtype)), exprtype);
      changed = true;
      return true;
    } else if (isSgRshiftOp(expr) && isSgValueExp(op1) && isSgValueExp(op2)) {
      expr = buildConstantOfType(getValue(op1) >> getValue(op2), exprtype);
      changed = true;
      return true;
    } else if (isSgAndOp(expr) && isSgNotEqualOp(op1) && isSgGreaterOrEqualOp(op2) && expressionTreeEqual(isSgBinaryOp(op1)->get_lhs_operand(), isSgBinaryOp(op2)->get_lhs_operand()) && expressionTreeEqual(isSgBinaryOp(op1)->get_rhs_operand(), isSgBinaryOp(op2)->get_rhs_operand())) {
      expr = buildGreaterThanOp(isSgBinaryOp(op1)->get_lhs_operand(), isSgBinaryOp(op1)->get_rhs_operand());
      changed = true;
      return true;
    } else if (isSgOrOp(expr) && isSgEqualityOp(op1) && isSgLessThanOp(op2) && expressionTreeEqual(isSgBinaryOp(op1)->get_lhs_operand(), isSgBinaryOp(op2)->get_lhs_operand()) && expressionTreeEqual(isSgBinaryOp(op1)->get_rhs_operand(), isSgBinaryOp(op2)->get_rhs_operand())) {
      expr = buildLessOrEqualOp(isSgBinaryOp(op1)->get_lhs_operand(), isSgBinaryOp(op1)->get_rhs_operand());
      changed = true;
      return true;
    }
  } else if (isSgConditionalExp(expr)) {
    SgExpression* op1 = isSgConditionalExp(expr)->get_conditional_exp();
    if (simplifyExpression(op1, changed)) {
      isSgConditionalExp(expr)->set_conditional_exp(op1);
      op1->set_parent(isSgConditionalExp(expr));
    }
    SgExpression* op2 = isSgConditionalExp(expr)->get_true_exp();
    if (simplifyExpression(op2, changed)) {
      isSgConditionalExp(expr)->set_true_exp(op2);
      op2->set_parent(isSgConditionalExp(expr));
    }
    SgExpression* op3 = isSgConditionalExp(expr)->get_false_exp();
    if (simplifyExpression(op3, changed)) {
      isSgConditionalExp(expr)->set_false_exp(op3);
      op3->set_parent(isSgConditionalExp(expr));
    }
    if (isSgBoolValExp(op1) && isSgBoolValExp(op1)->get_value() == true) {
      expr = op2;
      changed = true;
      return true;
    } else if (isSgBoolValExp(op1) && isSgBoolValExp(op1)->get_value() == false) {
      expr = op3;
      changed = true;
      return true;
    } else {
      return false;
    }
  } else if (isSgFunctionCallExp(expr)) {
    SgExpression* op1 = isSgFunctionCallExp(expr)->get_function();
    if (simplifyExpression(op1, changed)) {
      isSgFunctionCallExp(expr)->set_function(op1);
      op1->set_parent(isSgFunctionCallExp(expr));
    }
    SgExpression* op2 = isSgFunctionCallExp(expr)->get_args();
    if (simplifyExpression(op2, changed)) {
      ROSE_ASSERT (isSgExprListExp(op2));
      isSgFunctionCallExp(expr)->set_args(isSgExprListExp(op2));
      op2->set_parent(isSgFunctionCallExp(expr));
    }
    SgFunctionRefExp* fr = isSgFunctionRefExp(op1);
    ROSE_ASSERT (fr);
    return false;
  } else if (isSgExprListExp(expr)) {
    SgExprListExp* el = isSgExprListExp(expr);
    for (size_t i = 0; i < el->get_expressions().size(); ++i) {
      SgExpression* e = el->get_expressions()[i];
      if (simplifyExpression(e, changed)) {
        el->get_expressions()[i] = e;
        e->set_parent(el);
      }
    }
    return false;
  }
  return false;
}

// This is a fixed point iteration.
void simplifyAllExpressions(SgNode* top) {
  while (true) {
    int changeCount = 0;
    // cerr << "simplifyAllExpressions" << endl;
    vector<SgNode*> exprs = NodeQuery::querySubTree(top, V_SgExpression);
    vector<SgExpression*> exprs2;
    for (size_t i = 0; i < exprs.size(); ++i) {
      SgExpression* e = isSgExpression(exprs[i]);
      if (isSgExpression(e->get_parent())) continue;
      exprs2.push_back(isSgExpression(exprs[i]));
    }
    for (size_t i = 0; i < exprs2.size(); ++i) {
      SgExpression* newE = exprs2[i];
      bool changed = false;
      if (simplifyExpression(newE, changed)) {
        replaceExpression(exprs2[i], newE);
      }
      if (changed) {
        ++changeCount;
      }
    }
    // cerr << "Changes = " << changeCount << endl;
    if (changeCount != 0) continue;
    break;
  }
}

void removeIfConstants(SgNode* top) {
  vector<SgNode*> ifs = NodeQuery::querySubTree(top, V_SgIfStmt);
  for (size_t i = 0; i < ifs.size(); ++i) {
    SgIfStmt* stmt = isSgIfStmt(ifs[i]);
    SgExprStatement* cond = isSgExprStatement(stmt->get_conditional());
    if (!cond) continue;
    SgExpression* conde = cond->get_expression();
    if (!isSgBoolValExp(conde)) continue;
    SgBasicBlock* bb = isSgBoolValExp(conde)->get_value() ? SageInterface::ensureBasicBlockAsTrueBodyOfIf(stmt) : SageInterface::ensureBasicBlockAsFalseBodyOfIf(stmt);
    insertStatementListBefore(stmt, bb->get_statements());
    isSgStatement(stmt->get_parent())->remove_statement(stmt);
  }
}

SgExpression* plugInDefsForExpression(SgExpression* e, const map<SgInitializedName*, SgExpression*>& defs) {
  if (isSgVarRefExp(e)) {
    SgInitializedName* in = isSgVarRefExp(e)->get_symbol()->get_declaration();
    map<SgInitializedName*, SgExpression*>::const_iterator it = defs.find(in);
    if (it == defs.end()) return e;
    delete e;
    return copyExpression(it->second);
  } else if (isSgUnaryOp(e)) {
    isSgUnaryOp(e)->set_operand(plugInDefsForExpression(isSgUnaryOp(e)->get_operand(), defs));
    isSgUnaryOp(e)->get_operand()->set_parent(e);
    return e;
  } else if (isSgBinaryOp(e)) {
    isSgBinaryOp(e)->set_lhs_operand(plugInDefsForExpression(isSgBinaryOp(e)->get_lhs_operand(), defs));
    isSgBinaryOp(e)->get_lhs_operand()->set_parent(e);
    isSgBinaryOp(e)->set_rhs_operand(plugInDefsForExpression(isSgBinaryOp(e)->get_rhs_operand(), defs));
    isSgBinaryOp(e)->get_rhs_operand()->set_parent(e);
    return e;
  } else if (isSgConditionalExp(e)) {
    isSgConditionalExp(e)->set_conditional_exp(plugInDefsForExpression(isSgConditionalExp(e)->get_conditional_exp(), defs));
    isSgConditionalExp(e)->get_conditional_exp()->set_parent(e);
    isSgConditionalExp(e)->set_true_exp(plugInDefsForExpression(isSgConditionalExp(e)->get_true_exp(), defs));
    isSgConditionalExp(e)->get_true_exp()->set_parent(e);
    isSgConditionalExp(e)->set_false_exp(plugInDefsForExpression(isSgConditionalExp(e)->get_false_exp(), defs));
    isSgConditionalExp(e)->get_false_exp()->set_parent(e);
    return e;
  } else if (isSgFunctionCallExp(e)) {
    isSgFunctionCallExp(e)->set_function(plugInDefsForExpression(isSgFunctionCallExp(e)->get_function(), defs));
    isSgFunctionCallExp(e)->get_function()->set_parent(e);
    isSgFunctionCallExp(e)->set_args(isSgExprListExp(plugInDefsForExpression(isSgFunctionCallExp(e)->get_args(), defs)));
    isSgFunctionCallExp(e)->get_args()->set_parent(e);
    return e;
  } else if (isSgExprListExp(e)) {
    SgExprListExp* el = isSgExprListExp(e);
    for (size_t i = 0; i < el->get_expressions().size(); ++i) {
      el->get_expressions()[i] = plugInDefsForExpression(el->get_expressions()[i], defs);
      el->get_expressions()[i]->set_parent(el);
    }
    return el;
  } else {
    return e;
  }
}

void plugInAllConstVarDefsForBlock(SgBasicBlock* bb, map<SgInitializedName*, SgExpression*>& defs, const CTranslationPolicy& conv);

void plugInAllConstVarDefs(SgNode* top, const CTranslationPolicy& conv) { // Const is not required anymore
  vector<SgNode*> blocks = NodeQuery::querySubTree(top, V_SgBasicBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    map<SgInitializedName*, SgExpression*> defs;
    SgBasicBlock* bb = isSgBasicBlock(blocks[i]);
    ROSE_ASSERT (bb);
    if (isSgBasicBlock(bb->get_parent())) continue; // Recursion is handled in plugInAllConstVarDefsForBlock
    plugInAllConstVarDefsForBlock(bb, defs, conv);
  }
}

void getUsedVariables(SgExpression* e, set<SgInitializedName*>& vars);

bool isSimple(SgExpression* e) {
  return true; // Only called on expressions that don't have unsafe function calls or assignments
#if 0
  if (isSgValueExp(e)) return true;
  if (isSgVarRefExp(e)) return true;
#if 1
  if (isSgAssignOp(e)) return false;
  // No side-effecting operators allowed here
  if (isSgUnaryOp(e) && isSimple(isSgUnaryOp(e)->get_operand())) return true;
  if (isSgBinaryOp(e) && isSimple(isSgBinaryOp(e)->get_lhs_operand()) && isSgValueExp(isSgBinaryOp(e)->get_rhs_operand())) return true;
  if (isSgBinaryOp(e) && isSimple(isSgBinaryOp(e)->get_rhs_operand()) && isSgValueExp(isSgBinaryOp(e)->get_lhs_operand())) return true;
  if (isSgBinaryOp(e) && isSgVarRefExp(isSgBinaryOp(e)->get_rhs_operand()) && isSgVarRefExp(isSgBinaryOp(e)->get_lhs_operand())) return true;
  if (isSgLessThanOp(e) && isSgBitXorOp(isSgBinaryOp(e)->get_rhs_operand()) && isSgBitXorOp(isSgBinaryOp(e)->get_lhs_operand()) && isSimple(isSgBinaryOp(e)->get_lhs_operand()) && isSimple(isSgBinaryOp(e)->get_rhs_operand())) return true; // Special case for signed comparisons
#endif
  return false;
#endif
}

// Progate variable definitions to their uses (propates expressions that are in 
// variable definitions to their uses). This serves to eliminate variables.
void plugInAllConstVarDefsForBlock(SgBasicBlock* bb, map<SgInitializedName*, SgExpression*>& defs, const CTranslationPolicy& conv) {
  for (size_t j = 0; j < bb->get_statements().size(); ++j) {
    SgStatement* s = bb->get_statements()[j];
    if (isSgExprStatement(s)) {
      SgExprStatement* es = isSgExprStatement(s);
      SgExpression* e = es->get_expression();
      if (isSgAssignOp(e) && isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand())) {
        SgAssignOp* ao = isSgAssignOp(e);
        ao->set_rhs_operand(plugInDefsForExpression(ao->get_rhs_operand(), defs));
        ao->get_rhs_operand()->set_parent(ao);
        SgInitializedName* in = isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand())->get_symbol()->get_declaration();
        defs.erase(in);
        for (map<SgInitializedName*, SgExpression*>::iterator i = defs.begin(), inext;
             i != defs.end(); i = inext) {
          inext = i; ++inext;
          set<SgInitializedName*> usedVars;
          getUsedVariables(i->second, usedVars);
          if (usedVars.find(in) != usedVars.end()) {
            defs.erase(i);
          }
        }
      } else if (isSgFunctionCallExp(e) &&
                 isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function()) &&
                 conv.isMemoryWrite(isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function()))) {
        es->set_expression(plugInDefsForExpression(e, defs));
        es->get_expression()->set_parent(es);
        // Any writes to memory need to uncache reads from memory
        for (map<SgInitializedName*, SgExpression*>::iterator i = defs.begin(), inext;
             i != defs.end(); i = inext) {
          inext = i; ++inext;
          vector<SgNode*> funcrefs = NodeQuery::querySubTree(i->second, V_SgFunctionRefExp);
          for (size_t k = 0; k < funcrefs.size(); ++k) {
            if (conv.isMemoryRead(isSgFunctionRefExp(funcrefs[k]))) {
              defs.erase(i);
              break;
            }
          }
        }
      } else if (conv.isVolatileOperation(e)) {
        // Interrupts need to uncache everything
        defs.clear();
      } else {
        es->set_expression(plugInDefsForExpression(e, defs));
        es->get_expression()->set_parent(es);
      }
#if 0
      if (isSgAssignOp(e) && isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand())) {
        SgInitializedName* in = isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand())->get_symbol()->get_declaration();
        SgExpression* rhs = isSgAssignOp(e)->get_rhs_operand();
        set<SgInitializedName*> usedVars;
        getUsedVariables(rhs, usedVars);
        // Avoid a = a + b and such
        if (isSimple(rhs) && usedVars.find(in) == usedVars.end()) {
          // cerr << "Mapping " << in->get_name().getString() << " to " << rhs->unparseToString() << endl;
          defs[in] = rhs;
        }
      }
#endif
    } else if (isSgVariableDeclaration(s)) {
      ROSE_ASSERT (isSgVariableDeclaration(s)->get_variables().size() == 1);
      SgInitializedName* in = isSgVariableDeclaration(s)->get_variables()[0];
      SgAssignInitializer* ai = isSgAssignInitializer(in->get_initializer());
      if (ai) {
        ai->set_operand(plugInDefsForExpression(ai->get_operand(), defs));
        ai->get_operand()->set_parent(ai);
        // vector<SgNode*> deps = NodeQuery::querySubTree(ai, V_SgVarRefExp);
        if (isSimple(ai->get_operand())) {
          defs[in] = ai->get_operand();
        }
      }
    } else if (isSgBasicBlock(s)) {
      plugInAllConstVarDefsForBlock(isSgBasicBlock(s), defs, conv);
    } else if (isSgIfStmt(s)) {
      SgIfStmt* ifs = isSgIfStmt(s);
      SgExprStatement* es = isSgExprStatement(ifs->get_conditional());
      ROSE_ASSERT (es);
      es->set_expression(plugInDefsForExpression(es->get_expression(), defs));
      es->get_expression()->set_parent(es);
      map<SgInitializedName*, SgExpression*> defsSave = defs;
      plugInAllConstVarDefsForBlock(SageInterface::ensureBasicBlockAsTrueBodyOfIf(ifs), defsSave, conv);
      plugInAllConstVarDefsForBlock(SageInterface::ensureBasicBlockAsFalseBodyOfIf(ifs), defs, conv);
      defs.clear();
    } else if (isSgPragmaDeclaration(s)) {
      // Do nothing
    } else {
      defs.clear();
    }
    simplifyAllExpressions(s);
  }
  // Remove vars that are going out of scope
  SgInitializedNamePtrList initnames = findInitializedNamesInScope(bb);
  for (map<SgInitializedName*, SgExpression*>::iterator i = defs.begin(), inext;
      i != defs.end(); i = inext) {
    inext = i; ++inext;
    set<SgInitializedName*> usedVars;
    getUsedVariables(i->second, usedVars);
    for (size_t j = 0; j < initnames.size(); ++j) {
      if (usedVars.find(initnames[j]) != usedVars.end()) {
        defs.erase(i);
        break;
      }
    }
  }
}

void addDirectJumpsToSwitchCases(const CTranslationPolicy& conv) {
  map<uint64_t, SgLabelStatement*> addressToTargetMap;
  vector<SgNode*> cases = NodeQuery::querySubTree(conv.getSwitchBody(), V_SgCaseOptionStmt);
  vector<SgNode*> blocks = NodeQuery::querySubTree(conv.getWhileBody(), V_SgBasicBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgStatementPtrList& stmts = isSgBasicBlock(blocks[i])->get_statements();
    if (stmts.size() < 2) continue;
    if (!isSgContinueStmt(stmts.back())) continue;
    SgExprStatement* es = isSgExprStatement(*(stmts.end() - 2));
    if (!es) continue;
    SgAssignOp* ao = isSgAssignOp(es->get_expression());
    if (!ao) continue;
    if (!isSgVarRefExp(ao->get_lhs_operand()) ||
        isSgVarRefExp(ao->get_lhs_operand())->get_symbol()->get_declaration() != conv.getIPSymbol()->get_declaration()) continue;
    if (!isSgValueExp(ao->get_rhs_operand())) continue;
    uint64_t tgtAddr = getValue(ao->get_rhs_operand());
    if (conv.getLabelsForBlocks().find(tgtAddr) == conv.getLabelsForBlocks().end()) {
      continue;
    }
    SgLabelStatement* tgt = conv.getLabelsForBlocks().find(tgtAddr)->second;
    if (!tgt) continue;
    stmts.erase(stmts.end() - 2, stmts.end());
    appendStatement(buildGotoStatement(tgt), isSgBasicBlock(blocks[i]));
  }
  for (size_t i = 0; i < cases.size(); ++i) {
    SgCaseOptionStmt* c = isSgCaseOptionStmt(cases[i]);
    SgExpression* addrExpr = c->get_key();
    ROSE_ASSERT (isSgValueExp(addrExpr));
    if (conv.getExternallyVisibleBlocks().find(getValue(addrExpr)) == conv.getExternallyVisibleBlocks().end()) {
      // We only need the case label if there can be external jumps to this address
      isSgStatement(c->get_parent())->replace_statement(c, c->get_body());
    }
  }
}

SgStatement* getImmediatePredecessor(SgStatement* s) {
  SgStatement* p = isSgStatement(s->get_parent());
  ROSE_ASSERT (p);
  switch (p->variantT()) {
    case V_SgBasicBlock: {
      const vector<SgStatement*> stmts = isSgBasicBlock(p)->get_statements();
      size_t idx = std::find(stmts.begin(), stmts.end(), s) - stmts.begin();
      if (idx == 0) { // Either s is first or bb is empty
        return getImmediatePredecessor(isSgBasicBlock(p));
      } else {
        return stmts[idx - 1];
      }
    }
    case V_SgIfStmt: {
      if (s == isSgIfStmt(p)->get_conditional()) {
        return getImmediatePredecessor(p);
      } else {
        return isSgIfStmt(p)->get_conditional();
      }
    }
    default: {
      cerr << "getImmediatePredecessor " << p->class_name() << endl;
      abort();
    }
  }
}

void getPredecessorsFromEnd(SgStatement* s, set<SgStatement*>& result, const map<SgLabelStatement*, set<SgGotoStatement*> >& gotoMap);

void getPredecessorsFromBeginning(SgStatement* s, set<SgStatement*>& result, const map<SgLabelStatement*, set<SgGotoStatement*> >& gotoMap) {
  if (isSgLabelStatement(s)) {
    map<SgLabelStatement*, set<SgGotoStatement*> >::const_iterator i = gotoMap.find(isSgLabelStatement(s));
    if (i != gotoMap.end()) {
      const set<SgGotoStatement*>& gotos = i->second;
      result.insert(gotos.begin(), gotos.end());
      result.insert(getImmediatePredecessor(s));
    }
  } else {
    SgStatement* pred = getImmediatePredecessor(s);
    getPredecessorsFromEnd(pred, result, gotoMap);
  }
}

void getPredecessorsFromEnd(SgStatement* s, set<SgStatement*>& result, const map<SgLabelStatement*, set<SgGotoStatement*> >& gotoMap) {
  switch (s->variantT()) {
    case V_SgBasicBlock: {
      if (isSgBasicBlock(s)->get_statements().empty()) {
        getPredecessorsFromBeginning(s, result, gotoMap);
      } else {
        getPredecessorsFromEnd(isSgBasicBlock(s)->get_statements().back(), result, gotoMap);
      }
      break;
    }
    case V_SgExprStatement:
    case V_SgVariableDeclaration: {result.insert(s); break;}

    case V_SgContinueStmt: break; // No predecessors from end

    case V_SgGotoStatement:
    case V_SgLabelStatement: {
      getPredecessorsFromBeginning(s, result, gotoMap);
      break;
    }

    case V_SgIfStmt: {
      getPredecessorsFromEnd(isSgIfStmt(s)->get_true_body(), result, gotoMap);
      getPredecessorsFromEnd(SageInterface::ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(s)), result, gotoMap);
      break;
    }

    default: {
      cerr << "getPredecessorsFromEnd " << s->class_name() << endl;
      abort();
    }
  }
}

#if 0
set<SgInitializedName*> makeAllPossibleVars(const X86AssemblyToCWithVariables& conv) {
  set<SgInitializedName*> result;
  for (size_t i = 0; i < 16; ++i) {
    if (conv.gprSym[i]) {
      result.insert(conv.gprSym[i]->get_declaration());
    }
  }
  for (size_t i = 0; i < 16; ++i) {
    if (conv.flagsSym[i]) {
      result.insert(conv.flagsSym[i]->get_declaration());
    }
  }
  // result.insert(conv.ipSym->get_declaration());
  result.insert(conv.sf_xor_ofSym->get_declaration());
  result.insert(conv.zf_or_cfSym->get_declaration());
  return result;
}
#endif

void getUsedVariables(SgExpression* e, set<SgInitializedName*>& vars) {
  ROSE_ASSERT (e);
  if (isSgVarRefExp(e)) {
    vars.insert(isSgVarRefExp(e)->get_symbol()->get_declaration());
  } else {
    vector<SgNode*> succs = e->get_traversalSuccessorContainer();
    for (size_t i = 0; i < succs.size(); ++i) {
      if (isSgExpression(succs[i])) {
        getUsedVariables(isSgExpression(succs[i]), vars);
      }
    }
  }
}

ostream& operator<<(ostream& o, const std::set<SgInitializedName*>& s) {
  for (set<SgInitializedName*>::const_iterator i = s.begin(); i != s.end(); ++i) {
    if (i != s.begin()) o << ", ";
    o << (*i)->get_name().getString();
  }
  return o;
}

typedef map<SgLabelStatement*, set<SgGotoStatement*> > GotoMap;
GotoMap buildGotoMap(SgBasicBlock* bb) {
  GotoMap gotoMap;
  vector<SgNode*> gotos = NodeQuery::querySubTree(bb, V_SgGotoStatement);
  for (size_t i = 0; i < gotos.size(); ++i) {
    gotoMap[isSgGotoStatement(gotos[i])->get_label()].insert(isSgGotoStatement(gotos[i]));
  }
  return gotoMap;
}

#if 0
set<SgInitializedName*> computeLiveVars(SgStatement* stmt, const X86AssemblyToCWithVariables& conv, map<SgLabelStatement*, set<SgInitializedName*> >& liveVarsForLabels, set<SgInitializedName*> currentLiveVars, bool actuallyRemove) {
  switch (stmt->variantT()) {
    case V_SgBasicBlock: {
      const SgStatementPtrList& stmts = isSgBasicBlock(stmt)->get_statements();
      for (size_t i = stmts.size(); i > 0; --i) {
        currentLiveVars = computeLiveVars(stmts[i - 1], conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
      }
      return currentLiveVars;
    }
    case V_SgPragmaDeclaration: return currentLiveVars;
    case V_SgDefaultOptionStmt: return currentLiveVars;
    case V_SgCaseOptionStmt: {
      return computeLiveVars(isSgCaseOptionStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
    }
    case V_SgLabelStatement: {
      liveVarsForLabels[isSgLabelStatement(stmt)] = currentLiveVars;
      return currentLiveVars;
    }
    case V_SgGotoStatement: {
      return liveVarsForLabels[isSgGotoStatement(stmt)->get_label()];
    }
    case V_SgSwitchStatement: {
      SgSwitchStatement* s = isSgSwitchStatement(stmt);
      SgBasicBlock* swBody = s->get_body();
      ROSE_ASSERT (swBody);
      const SgStatementPtrList& bodyStmts = swBody->get_statements();
      set<SgInitializedName*> liveForBody; // Assumes any statement in the body is possible
      for (size_t i = 0; i < bodyStmts.size(); ++i) {
        setUnionInplace(liveForBody, computeLiveVars(bodyStmts[i], conv, liveVarsForLabels, currentLiveVars, actuallyRemove));
      }
      return computeLiveVars(s->get_item_selector(), conv, liveVarsForLabels, liveForBody, actuallyRemove);
    }
    case V_SgContinueStmt: {
      return makeAllPossibleVars(conv);
    }
    case V_SgIfStmt: {
      set<SgInitializedName*> liveForBranches = computeLiveVars(isSgIfStmt(stmt)->get_true_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove);
      setUnionInplace(liveForBranches, (isSgIfStmt(stmt)->get_false_body() != NULL ? computeLiveVars(isSgIfStmt(stmt)->get_false_body(), conv, liveVarsForLabels, currentLiveVars, actuallyRemove) : set<SgInitializedName*>()));
      return computeLiveVars(isSgIfStmt(stmt)->get_conditional(), conv, liveVarsForLabels, liveForBranches, actuallyRemove);
    }
    case V_SgWhileStmt: {
      while (true) {
        set<SgInitializedName*> liveVarsSave = currentLiveVars;
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, false);
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_condition(), conv, liveVarsForLabels, currentLiveVars, false);
        setUnionInplace(currentLiveVars, liveVarsSave);
        if (liveVarsSave == currentLiveVars) break;
      }
      if (actuallyRemove) {
        set<SgInitializedName*> liveVarsSave = currentLiveVars;
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_body(), conv, liveVarsForLabels, currentLiveVars, true);
        currentLiveVars = computeLiveVars(isSgWhileStmt(stmt)->get_condition(), conv, liveVarsForLabels, currentLiveVars, true);
        setUnionInplace(currentLiveVars, liveVarsSave);
      }
      return currentLiveVars;
    }
    case V_SgBreakStmt: return set<SgInitializedName*>();
    case V_SgExprStatement: {
      SgExpression* e = isSgExprStatement(stmt)->get_expression();
      switch (e->variantT()) {
        case V_SgAssignOp: {
          SgVarRefExp* lhs = isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand());
          ROSE_ASSERT (lhs);
          SgInitializedName* in = lhs->get_symbol()->get_declaration();
          if (currentLiveVars.find(in) == currentLiveVars.end()) {
            if (actuallyRemove) {
              // cerr << "Removing assignment " << e->unparseToString() << endl;
              isSgStatement(stmt->get_parent())->remove_statement(stmt);
            }
            return currentLiveVars;
          } else {
            currentLiveVars.erase(in);
            getUsedVariables(isSgAssignOp(e)->get_rhs_operand(), currentLiveVars);
            return currentLiveVars;
          }
        }
        case V_SgFunctionCallExp: {
          getUsedVariables(e, currentLiveVars);
          SgFunctionRefExp* fr = isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function());
          ROSE_ASSERT (fr);
          if (fr->get_symbol()->get_declaration() == conv.interruptSym->get_declaration()) {
            setUnionInplace(currentLiveVars, makeAllPossibleVars(conv));
            return currentLiveVars;
          } else {
            return currentLiveVars;
          }
        }
        default: {
          getUsedVariables(e, currentLiveVars);
          return currentLiveVars;
        }
      }
    }
    case V_SgVariableDeclaration: {
      ROSE_ASSERT (isSgVariableDeclaration(stmt)->get_variables().size() == 1);
      SgInitializedName* in = isSgVariableDeclaration(stmt)->get_variables()[0];
      bool isConst = isConstType(in->get_type());
      if (currentLiveVars.find(in) == currentLiveVars.end() && isConst) {
        if (actuallyRemove) {
          // cerr << "Removing decl " << stmt->unparseToString() << endl;
          isSgStatement(stmt->get_parent())->remove_statement(stmt);
        }
        return currentLiveVars;
      } else {
        currentLiveVars.erase(in);
        if (in->get_initializer()) {
          getUsedVariables(in->get_initializer(), currentLiveVars);
        }
        return currentLiveVars;
      }
    }
    default: cerr << "computeLiveVars: " << stmt->class_name() << endl; abort();
  }
}

void removeDeadStores(SgBasicBlock* switchBody, const X86AssemblyToCWithVariables& conv) {
  map<SgLabelStatement*, set<SgInitializedName*> > liveVars;
  while (true) {
    map<SgLabelStatement*, set<SgInitializedName*> > liveVarsSave = liveVars;
    computeLiveVars(switchBody, conv, liveVars, set<SgInitializedName*>(), false);
    if (liveVars == liveVarsSave) break;
  }
  computeLiveVars(switchBody, conv, liveVars, set<SgInitializedName*>(), true);
}
#endif

void removeEmptyBasicBlocks(SgNode* top) {
  bool changed = true;
  while (changed) {
    changed = false;
    vector<SgNode*> blocks = NodeQuery::querySubTree(top, V_SgBasicBlock);
    for (size_t i = 0; i < blocks.size(); ++i) {
      SgBasicBlock* bb = isSgBasicBlock(blocks[i]);
      if (!bb->get_statements().empty()) continue;
      if (isSgBasicBlock(bb->get_parent())) {
        changed = true;
        isSgBasicBlock(bb->get_parent())->remove_statement(bb);
      }
    }
    vector<SgNode*> ifs = NodeQuery::querySubTree(top, V_SgIfStmt);
    for (size_t i = 0; i < ifs.size(); ++i) {
      if (!isSgBasicBlock(isSgIfStmt(ifs[i])->get_true_body())) continue;
      if (!isSgBasicBlock(isSgIfStmt(ifs[i])->get_true_body())->get_statements().empty()) continue;
      if (!isSgBasicBlock(isSgIfStmt(ifs[i])->get_false_body())) continue;
      if (!isSgBasicBlock(isSgIfStmt(ifs[i])->get_false_body())->get_statements().empty()) continue;
      isSgStatement(ifs[i]->get_parent())->remove_statement(isSgIfStmt(ifs[i]));
    }
  }
}

SgLabelStatement* getPreviousLabel(SgStatement* s) {
  ROSE_ASSERT (s);
  SgBasicBlock* bb = isSgBasicBlock(s->get_parent());
  if (!bb) return getPreviousLabel(isSgStatement(s->get_parent()));
  const SgStatementPtrList& stmts = bb->get_statements();
  size_t idx = std::find(stmts.begin(), stmts.end(), s) - stmts.begin();
  ROSE_ASSERT (idx != stmts.size());
  while (idx != 0) {
    --idx;
    if (isSgLabelStatement(stmts[idx])) return isSgLabelStatement(stmts[idx]);
  }
  ROSE_ASSERT (idx == 0);
  return getPreviousLabel(bb);
}

bool endsWithGoto(SgStatement* s) {
  switch (s->variantT()) {
    case V_SgGotoStatement: return true;
    case V_SgContinueStmt: return true;
    case V_SgExprStatement: return false;
    case V_SgVariableDeclaration: return false;
    case V_SgLabelStatement: return false;
    case V_SgPragmaDeclaration: return false;
    case V_SgCaseOptionStmt: {
      return endsWithGoto(isSgCaseOptionStmt(s)->get_body());
    }
    case V_SgDefaultOptionStmt: return false;
    case V_SgWhileStmt: return false;
    case V_SgBasicBlock: {
      const SgStatementPtrList& stmts = isSgBasicBlock(s)->get_statements();
      if (stmts.empty()) {
        return false;
      } else {
        return endsWithGoto(stmts.back());
      }
    }
    case V_SgIfStmt: {
      return endsWithGoto(isSgIfStmt(s)->get_true_body()) &&
             isSgIfStmt(s)->get_false_body() != NULL &&
             endsWithGoto(isSgIfStmt(s)->get_false_body());
    }
    default: cerr << "endsWithGoto: " << s->class_name() << endl; abort();
  }
}

void structureCode(SgBasicBlock* switchBody) {
  bool changed = true;
  while (changed) {
    changed = false;
    GotoMap gotoMap = buildGotoMap(switchBody);
    vector<SgNode*> gotos = NodeQuery::querySubTree(switchBody, V_SgGotoStatement);
    for (size_t i = 0; i < gotos.size(); ++i) {
      SgGotoStatement* gs = isSgGotoStatement(gotos[i]);
      SgLabelStatement* ls = gs->get_label();
      if (gotoMap[ls].size() != 1) continue;
      if (isSgSwitchStatement(gs->get_parent()->get_parent()->get_parent()->get_parent()) || getPreviousLabel(gs) == ls) continue; // Prevent loops in AST
      changed = true;
      cerr << "Working on goto " << i + 1 << " of " << gotos.size() << endl;
      ROSE_ASSERT (isSgBasicBlock(ls->get_statement()));
      ROSE_ASSERT (isSgBasicBlock(ls->get_statement())->get_statements().empty());
      SgBasicBlock* lsParent = isSgBasicBlock(ls->get_parent());
      ROSE_ASSERT (lsParent);
      SgStatementPtrList& stmts = lsParent->get_statements();
      size_t idx = std::find(stmts.begin(), stmts.end(), ls) - stmts.begin();
      ROSE_ASSERT(idx != stmts.size());
      ROSE_ASSERT(idx + 1 != stmts.size());
      cerr << "Appending initial statement " << stmts[idx]->unparseToString() << endl;
      SgBasicBlock* lsBody = buildBasicBlock(stmts[idx]); // Body also includes ls itself
      stmts.erase(stmts.begin() + idx);
      while (idx < stmts.size() && !endsWithGoto(stmts[idx])) {
        // ROSE_ASSERT (!isSgLabelStatement(stmts[idx]));
        cerr << "Appending statement " << stmts[idx]->unparseToString() << endl;
        appendStatement(stmts[idx], lsBody);
        stmts.erase(stmts.begin() + idx);
      }
      if (idx < stmts.size()) {
        cerr << "Appending goto statement " << stmts[idx]->unparseToString() << endl;
        appendStatement(stmts[idx], lsBody);
        stmts.erase(stmts.begin() + idx);
        ROSE_ASSERT (endsWithGoto(lsBody));
      }
      insertStatementListBefore(gs, lsBody->get_statements());
      isSgStatement(gs->get_parent())->remove_statement(gs);
    }
    vector<SgNode*> ifs = NodeQuery::querySubTree(switchBody, V_SgIfStmt);
    for (size_t i = 0; i < ifs.size(); ++i) {
      SgIfStmt* ifstmt = isSgIfStmt(ifs[i]);
      const SgStatementPtrList& stmtsTrue = SageInterface::ensureBasicBlockAsTrueBodyOfIf(ifstmt)->get_statements();
      const SgStatementPtrList& stmtsFalse = SageInterface::ensureBasicBlockAsFalseBodyOfIf(ifstmt)->get_statements();
      if (stmtsTrue.empty()) continue;
      if (stmtsFalse.empty()) continue;
      SgGotoStatement* gsTrue = isSgGotoStatement(stmtsTrue.back());
      SgGotoStatement* gsFalse = isSgGotoStatement(stmtsFalse.back());
      if (!gsTrue || !gsFalse) continue;
      SgLabelStatement* ls = gsTrue->get_label();
      if (gsFalse->get_label() != ls) continue;
      changed = true;
      ifstmt->get_true_body()->remove_statement(gsTrue);
      if (ifstmt->get_false_body() != NULL) {
        ifstmt->get_false_body()->remove_statement(gsFalse);
      }
      insertStatementAfter(ifstmt, gsTrue);
    }
  }
}

void flattenBlocksWithoutVariables(SgNode* top) {
  vector<SgNode*> blocks = NodeQuery::querySubTree(top, V_SgBasicBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgBasicBlock* bb = isSgBasicBlock(blocks[i]);
    const SgStatementPtrList& stmts = bb->get_statements();
    bool hasVariableDecls = false;
    for (size_t j = 0; j < stmts.size(); ++j) {
      if (isSgVariableDeclaration(stmts[j])) {
        hasVariableDecls = true;
        break;
      }
    }
    if (hasVariableDecls) continue;
    if (!isSgBasicBlock(bb->get_parent())) continue;
    SgStatementPtrList& pstmts = isSgBasicBlock(bb->get_parent())->get_statements();
    bool found = false;
    for (size_t j = 0; j < pstmts.size(); ++j) {
      if (pstmts[j] != bb) continue;
      insertStatementListBefore(bb, stmts);
      isSgBasicBlock(bb->get_parent())->remove_statement(bb);
      found = true;
      break;
    }
    ROSE_ASSERT (found);
  }
}

#if 0
class TrackVariableDefs {
  public:
  const X86AssemblyToCWithVariables& conv;

  typedef map<SgInitializedName*, SgVariableSymbol*> DefMap;

  DefMap makeInitialMap() const;
  vector<SgStatement*> flush(const DefMap& m) const;
  int go(SgStatement* s, DefMap& lastDef) const; // Returns number of statements inserted
  static void applyVarMap(SgExpression* e, const DefMap& m); // Does update in place within e

  public:
  TrackVariableDefs(const X86AssemblyToCWithVariables& conv): conv(conv) {}
  void go(SgStatement* s) const {
    DefMap m;
    go(s, m);
    ROSE_ASSERT (m.empty());
    // insertStatementAfter(s, flush(m));
  }
};

void trackVariableDefs(SgStatement* top, const X86AssemblyToCWithVariables& conv) { // Mini version of SSA
  TrackVariableDefs(conv).go(top);
}

TrackVariableDefs::DefMap TrackVariableDefs::makeInitialMap() const {
  DefMap result;
  for (size_t i = 0; i < 16; ++i) {
    if (conv.gprSym[i]) {
      result[conv.gprSym[i]->get_declaration()] = conv.gprSym[i];
    }
#if 0
    if (conv.gprLowByteSym[i]) {
      result[conv.gprLowByteSym[i]->get_declaration()] = conv.gprLowByteSym[i];
    }
#endif
  }
  for (size_t i = 0; i < 16; ++i) {
    if (conv.flagsSym[i]) {
      result[conv.flagsSym[i]->get_declaration()] = conv.flagsSym[i];
    }
  }
  // result[conv.ipSym->get_declaration()] = conv.ipSym;
  result[conv.sf_xor_ofSym->get_declaration()] = conv.sf_xor_ofSym;
  result[conv.zf_or_cfSym->get_declaration()] = conv.zf_or_cfSym;
  return result;
}

vector<SgStatement*> TrackVariableDefs::flush(const TrackVariableDefs::DefMap& m) const {
  TrackVariableDefs::DefMap origMap = makeInitialMap();
  vector<SgStatement*> result;
  for (TrackVariableDefs::DefMap::const_iterator i = m.begin(); i != m.end(); ++i) {
    if (i->first == i->second->get_declaration()) continue;
#if 0
    if (origMap.find(i->first) == origMap.end()) {
      cerr << "Bad variable " << i->first->get_name().getString() << " in TrackVariableDefs::flush" << endl;
      abort();
    }
#endif
    SgStatement* newStmt = buildAssignStatement(buildVarRefExp(new SgVariableSymbol(i->first)), buildVarRefExp(i->second));
    result.push_back(newStmt);
  }
  return result;
}

void TrackVariableDefs::applyVarMap(SgExpression* e, const TrackVariableDefs::DefMap& m) {
  if (isSgVarRefExp(e)) {
    SgInitializedName* in = isSgVarRefExp(e)->get_symbol()->get_declaration();
    TrackVariableDefs::DefMap::const_iterator it = m.find(in);
    if (it != m.end()) {
      ROSE_ASSERT (it->second);
      isSgVarRefExp(e)->set_symbol(it->second);
    }
  } else if (isSgValueExp(e) || isSgFunctionRefExp(e)) {
    // Do nothing
  } else if (isSgUnaryOp(e)) {
    applyVarMap(isSgUnaryOp(e)->get_operand(), m);
  } else if (isSgBinaryOp(e)) {
    applyVarMap(isSgBinaryOp(e)->get_lhs_operand(), m);
    applyVarMap(isSgBinaryOp(e)->get_rhs_operand(), m);
  } else if (isSgConditionalExp(e)) {
    applyVarMap(isSgConditionalExp(e)->get_conditional_exp(), m);
    applyVarMap(isSgConditionalExp(e)->get_true_exp(), m);
    applyVarMap(isSgConditionalExp(e)->get_false_exp(), m);
  } else if (isSgFunctionCallExp(e)) {
    applyVarMap(isSgFunctionCallExp(e)->get_function(), m);
    applyVarMap(isSgFunctionCallExp(e)->get_args(), m);
  } else if (isSgExprListExp(e)) {
    SgExpressionPtrList& exprs = isSgExprListExp(e)->get_expressions();
    for (size_t i = 0; i < exprs.size(); ++i) {applyVarMap(exprs[i], m);}
  } else {
    cerr << "applyVarMap: " << e->class_name() << endl;
    abort();
  }
}

int TrackVariableDefs::go(SgStatement* s, TrackVariableDefs::DefMap& lastDef) const {
  SgScopeStatement* sc = s->get_scope();
  ROSE_ASSERT (sc);
  SgStatement* p = isSgStatement(s->get_parent());
  ROSE_ASSERT (p);
  ROSE_ASSERT (isSgBasicBlock(s) || isSgBasicBlock(p));
  switch (s->variantT()) {
    case V_SgBasicBlock: {
      set<SgInitializedName*> localVars;
      const SgStatementPtrList& stmts = isSgBasicBlock(s)->get_statements();
      for (size_t i = 0; i < stmts.size(); ) {
        size_t stmtsAdded = go(stmts[i], lastDef);
        size_t stmtsToScan = stmtsAdded + 1;
        for (; stmtsToScan > 0 && i < stmts.size(); --stmtsToScan, ++i) {
          if (isSgVariableDeclaration(stmts[i])) {
            ROSE_ASSERT (isSgVariableDeclaration(stmts[i])->get_variables().size() == 1);
            localVars.insert(isSgVariableDeclaration(stmts[i])->get_variables()[0]);
          }
        }
      }
      DefMap toFlush;
      DefMap newLastDef = lastDef;
      for (DefMap::const_iterator i = lastDef.begin(); i != lastDef.end(); ++i) {
        if (localVars.find(i->second->get_declaration()) != localVars.end()) {
          toFlush.insert(*i);
          newLastDef.erase(newLastDef.find(i->first));
        }
      }
      appendStatementList(flush(toFlush), isSgBasicBlock(s));
      lastDef = newLastDef;
      return 0;
    }
    case V_SgGotoStatement:
    case V_SgContinueStmt:
    case V_SgSwitchStatement:
    case V_SgLabelStatement: {
      insertStatementListBefore(s, flush(lastDef));
      lastDef.clear();
      return 1;
    }
    case V_SgDefaultOptionStmt: ROSE_ASSERT (lastDef.empty()); return 0;
    case V_SgCaseOptionStmt: {
      insertStatementListBefore(s, flush(lastDef));
      lastDef.clear();
      go(isSgCaseOptionStmt(s)->get_body(), lastDef);
      return 1;
    }
    case V_SgIfStmt: {
#if 1
      ROSE_ASSERT (isSgExprStatement(isSgIfStmt(s)->get_conditional()));
      SgExpression* test = isSgExprStatement(isSgIfStmt(s)->get_conditional())->get_expression();
      applyVarMap(test, lastDef);
      DefMap mTrue = lastDef;
      DefMap mFalse = lastDef;
      go(isSgIfStmt(s)->get_true_body(), mTrue);
      if (isSgIfStmt(s)->get_false_body() != NULL) {
        go(isSgIfStmt(s)->get_false_body(), mFalse);
      }
      DefMap mCommon;
      for (DefMap::const_iterator i = mTrue.begin(); i != mTrue.end(); ++i) {
        DefMap::const_iterator j = mFalse.find(i->first);
        if (j == mFalse.end()) continue;
        if (j->second == i->second) {
          mCommon.insert(*i);
        }
      }
      for (DefMap::const_iterator i = mCommon.begin(); i != mCommon.end(); ++i) {
        mTrue.erase(i->first);
        mFalse.erase(i->first);
      }
      appendStatementList(flush(mTrue), SageInterface::ensureBasicBlockAsTrueBodyOfIf(isSgIfStmt(s)));
      appendStatementList(flush(mFalse), SageInterface::ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(s)));
      lastDef = mCommon;
#else
      insertStatementListBefore(s, flush(lastDef));
      lastDef.clear();
      go(isSgIfStmt(s)->get_true_body(), lastDef);
      appendStatementList(flush(lastDef), SageInterface::ensureBasicBlockAsTrueBodyOfIf(isSgIfStmt(s)));
      lastDef.clear();
      ROSE_ASSERT (isSgIfStmt(s)->get_false_body());
      go(isSgIfStmt(s)->get_false_body(), lastDef);
      appendStatementList(flush(lastDef), SageInterface::ensureBasicBlockAsFalseBodyOfIf(isSgIfStmt(s)));
      lastDef.clear();
#endif
      return 1;
    }
    case V_SgWhileStmt: {
      insertStatementListBefore(s, flush(lastDef));
      lastDef.clear();
      return 1;
    }
    case V_SgPragmaDeclaration: {
      return 0;
    }
    case V_SgExprStatement: {
      SgExpression* e = isSgExprStatement(s)->get_expression();
      switch (e->variantT()) {
        case V_SgAssignOp: {
          SgVarRefExp* vr = isSgVarRefExp(isSgAssignOp(e)->get_lhs_operand());
          ROSE_ASSERT (vr);
          SgExpression* rhs = isSgAssignOp(e)->get_rhs_operand();
          applyVarMap(rhs, lastDef);
          SgInitializedName* in = vr->get_symbol()->get_declaration();
          static int nameCounter = 0;
          string name = in->get_name().getString() + "_" + StringUtility::numberToString(++nameCounter);
          SgModifierType* constT = new SgModifierType(vr->get_type()->stripType());
          constT->get_typeModifier().get_constVolatileModifier().setConst();
          SgVariableDeclaration* decl = buildVariableDeclaration(name, constT, buildAssignInitializer(rhs), sc);
          insertStatementBefore(s, decl);
          p->remove_statement(s);
          lastDef[in] = getFirstVarSym(decl);
          return 0;
        }
        case V_SgFunctionCallExp: {
          SgFunctionRefExp* fr = isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function());
          ROSE_ASSERT (fr);
          SgFunctionDeclaration* fd = fr->get_symbol()->get_declaration();
          if (fd == conv.memoryWriteByteSym->get_declaration() ||
              fd == conv.memoryWriteWordSym->get_declaration() ||
              fd == conv.memoryWriteDWordSym->get_declaration() ||
              fd == conv.memoryWriteQWordSym->get_declaration()) {
            applyVarMap(e, lastDef);
            return 0;
          } else {
            insertStatementListBefore(s, flush(lastDef));
            lastDef.clear();
            return 1;
          }
        }
        default: {
          cerr << "Bad expr " << e->class_name() << endl;
          abort();
        }
      }
      break;
    }
    case V_SgVariableDeclaration: {
      ROSE_ASSERT (isSgVariableDeclaration(s)->get_variables().size() == 1);
      SgInitializedName* in = isSgVariableDeclaration(s)->get_variables().front();
      if (isSgAssignInitializer(in->get_initializer())) {
        applyVarMap(isSgAssignInitializer(in->get_initializer())->get_operand(), lastDef);
      }
      return 0;
    }
    default: cerr << "TrackVariableDefs " << s->class_name() << endl; abort();
  }
}
#endif

SgStatement* getBodyOfLabel(SgLabelStatement* l) {
  SgBasicBlock* bb = isSgBasicBlock(l->get_parent());
  ROSE_ASSERT (bb);
  const SgStatementPtrList& stmts = bb->get_statements();
  SgStatementPtrList::const_iterator it = std::find(stmts.begin(), stmts.end(), l);
  while (it != stmts.end() && isSgLabelStatement(*it)) {++it;}
  ROSE_ASSERT (it != stmts.end());
  return *it;
}

#if 0
void doSimpleSSA(SgBasicBlock* top, const X86AssemblyToCWithVariables& conv) {
  map<SgLabelStatement*, map<SgInitializedName*, SgVariableSymbol*> > localVarMap;
  vector<SgNode*> labels = NodeQuery::querySubTree(top, V_SgLabelStatement);
  TrackVariableDefs::DefMap registerVars = TrackVariableDefs(conv).makeInitialMap();
  for (size_t i = 0; i < labels.size(); ++i) {
    SgLabelStatement* ls = isSgLabelStatement(labels[i]);
    map<SgInitializedName*, SgVariableSymbol*>& locals = localVarMap[ls];
    SgBasicBlock* lsBody = isSgBasicBlock(getBodyOfLabel(ls));
    if (!lsBody) {
      cerr << "Bad lsBody " << getBodyOfLabel(ls)->class_name() << endl;
    }
    ROSE_ASSERT (lsBody);
    for (TrackVariableDefs::DefMap::const_iterator j = registerVars.begin(); j != registerVars.end(); ++j) {
      SgVariableDeclaration* decl = buildVariableDeclaration(j->first->get_name().getString() + "__" + ls->get_name().getString(), j->first->get_type(), NULL, top);
      SgVariableSymbol* sym = getFirstVarSym(decl);
      prependStatement(decl, top);
      locals[j->first] = sym;
      prependStatement(buildAssignStatement(buildVarRefExp(j->second), buildVarRefExp(sym)), lsBody);
    }
  }
  vector<SgNode*> gotos = NodeQuery::querySubTree(top, V_SgGotoStatement);
  for (size_t i = 0; i < gotos.size(); ++i) {
    SgLabelStatement* ls = isSgGotoStatement(gotos[i])->get_label();
    map<SgInitializedName*, SgVariableSymbol*>& locals = localVarMap[ls];
    for (TrackVariableDefs::DefMap::const_iterator j = registerVars.begin(); j != registerVars.end(); ++j) {
      SgVariableSymbol* sym = locals[j->first];
      insertStatementBefore(isSgGotoStatement(gotos[i]), buildAssignStatement(buildVarRefExp(sym), buildVarRefExp(j->second)));
    }
  }
  for (size_t i = 0; i < labels.size(); ++i) {
    SgLabelStatement* ls = isSgLabelStatement(labels[i]);
    map<SgInitializedName*, SgVariableSymbol*>& locals = localVarMap[ls];
    SgBasicBlock* lsBody = isSgBasicBlock(getBodyOfLabel(ls));
    ROSE_ASSERT (lsBody);
    vector<SgNode*> varRefs = NodeQuery::querySubTree(lsBody, V_SgVarRefExp);
    for (size_t j = 0; j < varRefs.size(); ++j) {
      SgVarRefExp* vr = isSgVarRefExp(varRefs[j]);
      TrackVariableDefs::DefMap::const_iterator it = locals.find(vr->get_symbol()->get_declaration());
      if (it == locals.end()) continue;
      vr->set_symbol(it->second);
    }
  }
  vector<SgNode*> continues = NodeQuery::querySubTree(top, V_SgContinueStmt);
  for (size_t i = 0; i < continues.size(); ++i) {
    SgContinueStmt* c = isSgContinueStmt(continues[i]);
    SgLabelStatement* ls = getPreviousLabel(c);
    map<SgInitializedName*, SgVariableSymbol*>& locals = localVarMap[ls];
    for (TrackVariableDefs::DefMap::const_iterator j = registerVars.begin(); j != registerVars.end(); ++j) {
      SgVariableSymbol* sym = locals[j->first];
      insertStatementBefore(c, buildAssignStatement(buildVarRefExp(j->second), buildVarRefExp(sym)));
    }
  }
}
#endif

void unparseAsSExpressions(ostream& o, SgType* t) {
  switch (t->variantT()) {
    case V_SgModifierType: unparseAsSExpressions(o, isSgModifierType(t)->get_base_type()); break;
    case V_SgTypeUnsignedChar: o << "uint8_t"; break;
    case V_SgTypeUnsignedShort: o << "uint16_t"; break;
    case V_SgTypeInt: o << "int32_t"; break;
    case V_SgTypeUnsignedInt: o << "uint32_t"; break;
    case V_SgTypeUnsignedLong: o << "uint32_t"; break;
    case V_SgTypeUnsignedLongLong: o << "uint64_t"; break;
    default: cerr << "unparseAsSExpressions: bad type " << t->class_name() << endl; abort();
  }
}

void unparseAsSExpressions(ostream& o, SgExpression* e) {
  switch (e->variantT()) {
    case V_SgFunctionCallExp: {
      SgFunctionRefExp* fr = isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function());
      ROSE_ASSERT (fr);
      const SgExpressionPtrList& args = isSgFunctionCallExp(e)->get_args()->get_expressions();
      o << "(" << fr->get_symbol()->get_declaration()->get_name().getString();
      for (size_t i = 0; i < args.size(); ++i) {
        o << " ";
        unparseAsSExpressions(o, args[i]);
      }
      o << ")";
      break;
    }
    case V_SgConditionalExp: {
      o << "(if-e ";
      unparseAsSExpressions(o, isSgConditionalExp(e)->get_conditional_exp());
      o << " ";
      unparseAsSExpressions(o, isSgConditionalExp(e)->get_true_exp());
      o << " ";
      unparseAsSExpressions(o, isSgConditionalExp(e)->get_false_exp());
      o << ")";
      break;
    }
    case V_SgAssignOp: {
      o << "(assign ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgAddOp: {
      o << "(add ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgSubtractOp: {
      o << "(subtract ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgMultiplyOp: {
      o << "(multiply ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgLshiftOp: {
      o << "(left-shift ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgRshiftOp: {
      o << "(right-shift ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgEqualityOp: {
      o << "(equal ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgNotEqualOp: {
      o << "(not-equal ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgLessThanOp: {
      o << "(less-than ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgLessOrEqualOp: {
      o << "(less-than-or-equal ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgGreaterThanOp: {
      o << "(greater-than ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgGreaterOrEqualOp: {
      o << "(greater-than-or-equal ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgBitAndOp: {
      o << "(and ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgBitOrOp: {
      o << "(or ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgBitXorOp: {
      o << "(xor ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgAndOp: {
      o << "(logical-and ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgOrOp: {
      o << "(logical-or ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgCommaOpExp: {
      o << "(choose ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_lhs_operand());
      o << " ";
      unparseAsSExpressions(o, isSgBinaryOp(e)->get_rhs_operand());
      o << ")";
      break;
    }
    case V_SgMinusOp: {
      o << "(minus ";
      unparseAsSExpressions(o, isSgUnaryOp(e)->get_operand());
      o << ")";
      break;
    }
    case V_SgNotOp: {
      o << "(logical-not ";
      unparseAsSExpressions(o, isSgUnaryOp(e)->get_operand());
      o << ")";
      break;
    }
    case V_SgBitComplementOp: {
      o << "(not ";
      unparseAsSExpressions(o, isSgUnaryOp(e)->get_operand());
      o << ")";
      break;
    }
    case V_SgCastExp: {
      o << "(";
      unparseAsSExpressions(o, e->get_type());
      o << " ";
      unparseAsSExpressions(o, isSgUnaryOp(e)->get_operand());
      o << ")";
      break;
    }
    case V_SgVarRefExp: {
      o << e->unparseToString();
      break;
    }
    case V_SgBoolValExp: o << (getValue(e) ? "#t" : "#f"); break;
    case V_SgUnsignedCharVal: o << getValue(e); break;
    case V_SgUnsignedShortVal: o << getValue(e); break;
    case V_SgIntVal: o << getValue(e); break;
    case V_SgUnsignedIntVal: o << getValue(e); break;
    case V_SgUnsignedLongVal: o << getValue(e); break;
    case V_SgUnsignedLongLongIntVal: o << getValue(e); break;
    case V_SgAssignInitializer: unparseAsSExpressions(o, isSgAssignInitializer(e)->get_operand()); break;
    default: cerr << "Bad expr type " << e->class_name() << " in unparseAsSExpressions" << endl; abort();
  }
}

void unparseAsSExpressions(ostream& o, SgStatement* s) {
  switch (s->variantT()) {
    case V_SgBasicBlock: {
      size_t numCloseParens = 0;
      const SgStatementPtrList& stmts = isSgBasicBlock(s)->get_statements();
      for (size_t i = 0; i < stmts.size(); ++i) {
        if (isSgVariableDeclaration(stmts[i])) {
          SgVariableDeclaration* decl = isSgVariableDeclaration(stmts[i]);
          ROSE_ASSERT (decl->get_variables().size() == 1);
          SgInitializedName* in = decl->get_variables()[0];
          o << "(let ((";
          unparseAsSExpressions(o, in->get_type());
          o << " " << in->get_name().getString() << " ";
          if (in->get_initializer()) {
            unparseAsSExpressions(o, in->get_initializer());
          } else {
            o << "(void)";
          }
          o << "))\n";
          ++numCloseParens;
        } else if (i == stmts.size() - 1) {
          unparseAsSExpressions(o, stmts[i]);
        } else {
          o << "(begin ";
          unparseAsSExpressions(o, stmts[i]);
          ++numCloseParens;
        }
      }
      o << string(numCloseParens, ')') << "\n";
      break;
    }
    case V_SgGotoStatement: {
      o << "(goto " << isSgGotoStatement(s)->get_label()->get_name().getString() << ")\n";
      break;
    }
    case V_SgLabelStatement: {
      o << "(label " << isSgLabelStatement(s)->get_name().getString() << ")\n";
      break;
    }
    case V_SgIfStmt: {
      o << "(if ";
      unparseAsSExpressions(o, isSgIfStmt(s)->get_conditional());
      unparseAsSExpressions(o, isSgIfStmt(s)->get_true_body());
      unparseAsSExpressions(o, (isSgIfStmt(s)->get_false_body() != NULL ? isSgIfStmt(s)->get_false_body() : SageBuilder::buildBasicBlock()));
      o << ")\n";
      break;
    }
    case V_SgWhileStmt: {
      o << "(while ";
      unparseAsSExpressions(o, isSgWhileStmt(s)->get_condition());
      unparseAsSExpressions(o, isSgWhileStmt(s)->get_body());
      o << ")\n";
      break;
    }
    case V_SgContinueStmt: {
      o << "(dispatch-next-instruction)\n";
      break;
    }
    case V_SgBreakStmt: {
      o << "(break)\n";
      break;
    }
    case V_SgSwitchStatement: {
      // Special case
      SgBasicBlock* body = isSgBasicBlock(isSgSwitchStatement(s)->get_body());
      ROSE_ASSERT (body);
      const SgStatementPtrList& stmts = body->get_statements();
      o << "(case ";
      unparseAsSExpressions(o, isSgSwitchStatement(s)->get_item_selector());
      o << "\n";
      for (size_t i = 0; i < stmts.size(); ++i) {
        SgCaseOptionStmt* co = isSgCaseOptionStmt(stmts[i]);
        if (!co) continue;
        o << "((" << getValue(co->get_key()) << ") ";
        unparseAsSExpressions(o, co->get_body());
        o << ")\n";
      }
      o << ")\n";
      break;
    }
    case V_SgPragmaDeclaration: break;
    case V_SgVariableDeclaration: {
      SgVariableDeclaration* decl = isSgVariableDeclaration(s);
      ROSE_ASSERT (decl->get_variables().size() == 1);
      SgInitializedName* in = decl->get_variables()[0];
      if (in->get_initializer()) {
        o << "(var " << in->get_name().getString() << " ";
        unparseAsSExpressions(o, in->get_initializer());
        o << ")\n";
      }
      break;
    }
    case V_SgExprStatement: {
      o << "(expr ";
      unparseAsSExpressions(o, isSgExprStatement(s)->get_expression());
      o << ")\n";
      break;
    }
    default: cerr << "Unknown statement in unparseAsSExpressions: " << s->class_name() << endl; abort();
  }
}

void unparseAsSExpressionsTop(ostream& o, SgBasicBlock* bb) {
  unparseAsSExpressions(o, bb);
  return;
#if 0
  o << "(";
  const SgStatementPtrList& stmts = bb->get_statements();
  for (size_t i = 0; i < stmts.size(); ++i) {
    SgStatement* s = stmts[i];
    switch (s->variantT()) {
#if 0
      case V_SgBasicBlock: {
        const SgStatementPtrList& stmts2 = isSgBasicBlock(s)->get_statements();
        for (size_t i = 0; i < stmts2.size(); ++i) {
          o << " ";
          unparseAsSExpressions(o, stmts2[i]);
        }
        break;
      }
#endif
      case V_SgGotoStatement: {
        o << "(goto " << isSgGotoStatement(s)->get_label()->get_name().getString() << ")\n";
        break;
      }
      case V_SgLabelStatement: {
        ROSE_ASSERT (i + 1 != stmts.size());
        if (isSgLabelStatement(stmts[i + 1])) break; // Skip this one
        o << "(label " << isSgLabelStatement(s)->get_name().getString() << " ";
        unparseAsSExpressions(o, stmts[i + 1]);
        ++i;
        o << ")\n";
        break;
      }
      case V_SgSwitchStatement: {
        // Special case
        SgBasicBlock* body = isSgSwitchStatement(s)->get_body();
        const SgStatementPtrList& stmts = body->get_statements();
        o << "(externally-visible";
        for (size_t i = 0; i < stmts.size(); ++i) {
          SgCaseOptionStmt* co = isSgCaseOptionStmt(stmts[i]);
          if (!co) continue;
          o << " " << getValue(co->get_key());
        }
        o << ")\n";
        break;
      }
      case V_SgVariableDeclaration: {
        SgVariableDeclaration* decl = isSgVariableDeclaration(s);
        ROSE_ASSERT (decl->get_variables().size() == 1);
        SgInitializedName* in = decl->get_variables()[0];
        if (in->get_initializer()) {
          o << "(var " << in->get_name().getString() << " ";
          unparseAsSExpressions(o, in->get_initializer());
          o << ")\n";
        }
        break;
      }
      case V_SgExprStatement: break;
      default: cerr << "Unknown statement in unparseAsSExpressionsTop: " << s->class_name() << endl; abort();
    }
  }
  o << ")\n";
#endif
}

set<SgInitializedName*> getVariablesUsedInExpression(SgExpression* e) {
  vector<SgVariableSymbol*> syms = getSymbolsUsedInExpression(e);
  set<SgInitializedName*> result;
  for (size_t j = 0; j < syms.size(); ++j) {
    result.insert(syms[j]->get_declaration());
  }
  return result;
}

bool isStoppingPointStatement(SgNode* n) {
  return isSgExprStatement(n) || isSgVariableDeclaration(n);
}

void getReachingStatements(CFGNode start, ReachingStatementMap& result) {
  set<CFGNode> worklist;
  map<CFGNode, set<CFGNode> > sources;
  worklist.insert(start);
  vector<CFGEdge> inEdges = start.inEdges();
  for (size_t i = 0; i < inEdges.size(); ++i) {
    sources[start].insert(inEdges[i].source());
  }
  while (!worklist.empty()) {
    // cerr << "getReachingStatements worklist size = " << worklist.size() << endl;
    CFGNode n = *worklist.begin();
    // cerr << n.toString() << endl;
    worklist.erase(worklist.begin());
    set<SgStatement*> newReachingStatements;
    for (set<CFGNode>::const_iterator i = sources[n].begin(); i != sources[n].end(); ++i) {
      CFGNode n2 = *i;
      if (isStoppingPointStatement(n2.getNode())) {
        newReachingStatements.insert(isSgStatement(n2.getNode()));
      } else {
        newReachingStatements.insert(result[n2].begin(), result[n2].end());
      }
    }
    bool entryIsNew = (result.find(n) == result.end());
    bool changed = newReachingStatements != result[n];
    if (changed) result[n] = newReachingStatements;
    if (entryIsNew) { // Only create source list once
      vector<CFGEdge> outEdges = n.outEdges();
      for (size_t i = 0; i < outEdges.size(); ++i) {
        sources[outEdges[i].target()].insert(n);
      }
    }
    if (entryIsNew /* Go past stopping points only on the first iteration */ ||
        (changed && !isStoppingPointStatement(n.getNode()))) {
      vector<CFGEdge> outEdges = n.outEdges();
      for (size_t i = 0; i < outEdges.size(); ++i) {
        worklist.insert(outEdges[i].target());
      }
    }
  }
}

#if 0
void renumberVariableDefinitions(SgNode* top, const X86AssemblyToCWithVariables& conv) {
  vector<SgNode*> stmtsRaw = NodeQuery::querySubTree(top, V_SgStatement);
  vector<SgStatement*> stmts;
  for (size_t i = 0; i < stmtsRaw.size(); ++i) {
    if (isSgExprStatement(stmtsRaw[i]) || isSgVariableDeclaration(stmtsRaw[i])) {
      stmts.push_back(isSgStatement(stmtsRaw[i]));
    }
  }
  map<SgStatement*, set<SgInitializedName*> > varsDefined, varsUsed;
  cerr << "Building local def and use sets" << endl;
  for (size_t i = 0; i < stmts.size(); ++i) {
    SgStatement* stmt = stmts[i];
    switch (stmt->variantT()) {
      case V_SgExprStatement: {
        if (isSgAssignOp(isSgExprStatement(stmt)->get_expression())) {
          SgAssignOp* ao = isSgAssignOp(isSgExprStatement(stmt)->get_expression());
          SgVarRefExp* lhs = isSgVarRefExp(ao->get_lhs_operand());
          ROSE_ASSERT (lhs);
          varsDefined[stmt].clear();
          varsDefined[stmt].insert(lhs->get_symbol()->get_declaration());
          varsUsed[stmt] = getVariablesUsedInExpression(ao->get_rhs_operand());
        } else {
          varsDefined[stmt].clear();
          varsUsed[stmt] = getVariablesUsedInExpression(isSgExprStatement(stmt)->get_expression());
        }
        break;
      }
      case V_SgVariableDeclaration: {
        ROSE_ASSERT (isSgVariableDeclaration(stmt)->get_variables().size() == 1);
        SgInitializedName* in = isSgVariableDeclaration(stmt)->get_variables().front();
        varsDefined[stmt].clear();
        varsDefined[stmt].insert(in);
        if (in->get_initializer()) {
          varsUsed[stmt] = getVariablesUsedInExpression(in->get_initializer());
        } else {
          varsUsed[stmt].clear();
        }
        break;
      }

      default: {
        cerr << "Bad statement kind " << stmt->class_name() << endl;
        abort();
      }
    }
#if 0
    cerr << "For statement " << stmt->unparseToString() << " got localDefs=";
    for (set<SgInitializedName*>::const_iterator i = varsDefined[stmt].begin(); i != varsDefined[stmt].end(); ++i) {
      if (i != varsDefined[stmt].begin()) cerr << ",";
      cerr << (*i)->get_name().getString();
    }
    cerr << " localUses=";
    for (set<SgInitializedName*>::const_iterator i = varsUsed[stmt].begin(); i != varsUsed[stmt].end(); ++i) {
      if (i != varsUsed[stmt].begin()) cerr << ",";
      cerr << (*i)->get_name().getString();
    }
    cerr << endl;
#endif
  }
  cerr << "Caching possible predecessors" << endl;
  ReachingStatementMap reachingStatements;
  map<SgStatement*, vector<SgStatement*> > possiblePredecessors;
  getReachingStatements(top->cfgForBeginning(), reachingStatements);
  for (size_t i = 0; i < stmts.size(); ++i) {
    SgStatement* stmt = stmts[i];
    // if (i % 100 == 0) cerr << "On " << i + 1 << " of " << stmts.size() << endl;
    getReachingStatements(stmt->cfgForBeginning(), reachingStatements);
    ROSE_ASSERT (reachingStatements.find(stmt->cfgForBeginning()) != reachingStatements.end());
    vector<SgStatement*> preds(reachingStatements[stmt->cfgForBeginning()].begin(), reachingStatements[stmt->cfgForBeginning()].end());
    possiblePredecessors[stmt] = preds;
  }
  map<SgStatement*, map<SgInitializedName*, set<SgStatement*> > > duBefore, duAfter;
  cerr << "Doing main def-use loop" << endl;
  bool changed = true;
  while (changed) {
    cerr << "Iteration" << endl;
    changed = false;
    for (size_t i = 0; i < stmts.size(); ++i) {
      SgStatement* stmt = stmts[i];
      // cerr << "Working on statement " << stmt->unparseToString() << endl;
      map<SgInitializedName*, set<SgStatement*> > localDefsBefore;
      ROSE_ASSERT (possiblePredecessors.find(stmt) != possiblePredecessors.end());
      const vector<SgStatement*>& preds = possiblePredecessors[stmt];
      // cerr << "Found " << preds.size() << " pred(s)" << endl;
      for (vector<SgStatement*>::const_iterator j = preds.begin(); j != preds.end(); ++j) {
        const map<SgInitializedName*, set<SgStatement*> >& localDefsToAdd = duAfter[*j];
        for (map<SgInitializedName*, set<SgStatement*> >::const_iterator k = localDefsToAdd.begin(); k != localDefsToAdd.end(); ++k) {
          localDefsBefore[k->first].insert(k->second.begin(), k->second.end());
        }
      }
      if (localDefsBefore != duBefore[stmt]) {
        changed = true;
        duBefore[stmt] = localDefsBefore;
      }
      map<SgInitializedName*, set<SgStatement*> > localDefsAfter = localDefsBefore;
      for (set<SgInitializedName*>::const_iterator j = varsDefined[stmt].begin(); j != varsDefined[stmt].end(); ++j) {
        localDefsAfter[*j].clear();
        localDefsAfter[*j].insert(stmt);
      }
      if (localDefsAfter != duAfter[stmt]) {
        changed = true;
        duAfter[stmt] = localDefsAfter;
      }
#if 0
      cerr << "duBefore: ";
      for (map<SgInitializedName*, set<SgStatement*> >::const_iterator j = localDefsBefore.begin(); j != localDefsBefore.end(); ++j) {
        if (j != localDefsBefore.begin()) cerr << ", ";
        cerr << j->first->get_name().getString() << ":{";
        for (set<SgStatement*>::const_iterator k = j->second.begin(); k != j->second.end(); ++k) {
          if (k != j->second.begin()) cerr << ",";
          cerr << (*k)->class_name() << "@" << StringUtility::intToHex((uintptr_t)(*k));
        }
        cerr << "}";
      }
      cerr << " // ";
      cerr << "duAfter: ";
      for (map<SgInitializedName*, set<SgStatement*> >::const_iterator j = localDefsAfter.begin(); j != localDefsAfter.end(); ++j) {
        if (j != localDefsAfter.begin()) cerr << ", ";
        cerr << j->first->get_name().getString() << ":{";
        for (set<SgStatement*>::const_iterator k = j->second.begin(); k != j->second.end(); ++k) {
          if (k != j->second.begin()) cerr << ",";
          cerr << (*k)->class_name() << "@" << StringUtility::intToHex((uintptr_t)(*k));
        }
        cerr << "}";
      }
      cerr << endl;
      cerr << "Changed = " << changed << endl;
#endif
    }
  }
  cerr << "Making new vars" << endl;
  map<SgStatement*, SgVariableSymbol*> newVarsForAssignments;
  vector<SgNode*> assignments = NodeQuery::querySubTree(top, V_SgAssignOp);
  for (size_t i = 0; i < assignments.size(); ++i) {
    SgAssignOp* ao = isSgAssignOp(assignments[i]);
    SgVarRefExp* vr = isSgVarRefExp(ao->get_lhs_operand());
    ROSE_ASSERT (vr);
    SgInitializedName* in = vr->get_symbol()->get_declaration();
    SgExpression* rhs = ao->get_rhs_operand();
    SgStatement* stmt = getEnclosingStatement(ao);
    SgVariableDeclaration* decl = buildVariableDeclaration(in->get_name().getString() + "__" + StringUtility::intToHex((uintptr_t)ao), vr->get_type(), NULL, conv.whileBody);
    SgVariableSymbol* sym = getFirstVarSym(decl);
    newVarsForAssignments[stmt] = sym;
    prependStatement(decl, conv.whileBody);
    ao->set_lhs_operand(buildVarRefExp(sym));
    ao->get_lhs_operand()->set_parent(ao);
  }
  cerr << "Replacing var refs" << endl;
  vector<SgNode*> varrefs = NodeQuery::querySubTree(top, V_SgVarRefExp);
  for (size_t i = 0; i < varrefs.size(); ++i) {
    SgVarRefExp* vr = isSgVarRefExp(varrefs[i]);
    if (isSgAssignOp(vr->get_parent()) && isSgAssignOp(vr->get_parent())->get_lhs_operand() == vr) continue;
    cerr << "Working on var ref " << vr->unparseToString() << endl;
    SgStatement* stmt = getEnclosingStatement(vr);
    if (duBefore.find(stmt) == duBefore.end()) {
      cerr << "Did not find def-use list for " << stmt->unparseToString() << endl;
    }
    const set<SgStatement*>& defs = duBefore[stmt][vr->get_symbol()->get_declaration()];
    cerr << "Defs size is " << defs.size() << endl;
    SgNode* origParent = vr->get_parent();
    SgExpression* newExp = NULL;
    bool includeCurrentVarRef = false;
    for (set<SgStatement*>::const_iterator j = defs.begin(); j != defs.end(); ++j) {
      cerr << "Found def " << (*j)->unparseToString() << endl;
      if (isSgVariableDeclaration(*j)) {
        includeCurrentVarRef = true;
        continue;
      }
      SgVariableSymbol* sym = newVarsForAssignments[*j];
      ROSE_ASSERT (sym);
      if (!newExp) {
        newExp = buildVarRefExp(sym);
      } else {
        newExp = buildCommaOpExp(newExp, buildVarRefExp(sym));
      }
    }
    if (includeCurrentVarRef || !newExp) {
      if (newExp) {
        newExp = buildCommaOpExp(newExp, vr);
      } else {
        newExp = vr;
      }
    }
    cerr << "newExp = " << newExp->unparseToString() << endl;
    if (isSgExprStatement(origParent)) {
      isSgExprStatement(origParent)->set_expression(newExp);
      newExp->set_parent(origParent);
    } else {
      ROSE_ASSERT (isSgExpression(origParent));
      isSgExpression(origParent)->replace_expression(vr, newExp);
    }
  }
}
#endif

void moveVariableDeclarationsToTop(SgBasicBlock* top) {
  vector<SgNode*> varDecls = NodeQuery::querySubTree(top, V_SgVariableDeclaration);
  for (size_t i = 0; i < varDecls.size(); ++i) {
    SgVariableDeclaration* varDecl = isSgVariableDeclaration(varDecls[i]);
    ROSE_ASSERT (varDecl);
    if (varDecl->get_scope() == top) continue;
    const SgInitializedNamePtrList& vars = varDecl->get_variables();
    for (size_t j = 0; j < vars.size(); ++j) {
      SgInitializedName* var = vars[j];
      ROSE_ASSERT (var);
      SgAssignInitializer* init = isSgAssignInitializer(var->get_initializer());
      if (!init) continue;
      var->set_type(removeConst(var->get_type()));
      convertInitializerIntoAssignment(init);
    }
    isSgScopeStatement(varDecl->get_parent())->remove_statement(varDecl);
    prependStatement(varDecl, top);
  }
}
