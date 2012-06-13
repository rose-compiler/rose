
#include "rose-utils.hpp"
#include "rose.h"

#include <stdint.h>

bool getIntFromConstExpression(SgExpression * exp, int * res) {
    if (isSgValueExp(exp)) {
        switch (exp->variantT()) {
          case V_SgIntVal:
            *res = isSgIntVal(exp)->get_value();
            return true;
          case V_SgLongLongIntVal:
            *res = isSgLongLongIntVal(exp)->get_value();
            return true;
          default:
            std::cout << "In getIntFromConstExpression() unrecognized value type: " << exp->class_name() << std::endl;
            return false;
        }
    }
    else if (isSgBinaryOp(exp)) {
        SgBinaryOp * bin_op = isSgBinaryOp(exp);

        int lhs;
        int rhs;

        bool valid = getIntFromConstExpression(bin_op->get_lhs_operand_i(), &lhs) && getIntFromConstExpression(bin_op->get_rhs_operand_i(), &rhs);

        if (!valid) return false;

        switch (exp->variantT()) {
          case V_SgMultiplyOp:
            *res = lhs * rhs;
            return true;
          case V_SgDivideOp:
            *res = lhs / rhs;
            return true;
          case V_SgAddOp:
            *res = lhs + rhs;
            return true;
          case V_SgSubtractOp:
            *res = lhs - rhs;
            return true;
          case V_SgLessThanOp:
            *res = lhs < rhs;
            return true;
          case V_SgLessOrEqualOp:
            *res = lhs <= rhs;
            return true;
          case V_SgGreaterThanOp:
            *res = lhs > rhs;
            return true;
          case V_SgGreaterOrEqualOp:
            *res = lhs >= rhs;
            return true;
          case V_SgAndOp:
            *res = lhs && rhs;
            return true;
          case V_SgOrOp:
            *res = lhs || rhs;
            return true;
          default:
            std::cout << "In getIntFromConstExpression() unrecognized binary operator: " << exp->class_name() << std::endl;
            return false;
        }
    }
    else if (isSgCastExp(exp)) {
        return getIntFromConstExpression(isSgCastExp(exp)->get_operand_i(), res);
    }
    else if (isSgMinusOp(exp)) {
      bool success = getIntFromConstExpression(isSgMinusOp(exp)->get_operand_i(), res);
      *res = -(*res);
      return success;
    }
    else if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp)) return false;
    else if (isSgUnaryOp(exp)) {
        std::cout << "In getIntFromConstExpression() unrecognized unary operator: " << exp->class_name() << std::endl;
        return false;
    }
    else if (isSgConditionalExp(exp)) {
        SgConditionalExp * cond = isSgConditionalExp(exp);

        int c_exp;
        int t_exp;
        int f_exp;
        bool c_bool = getIntFromConstExpression(cond->get_conditional_exp(), &c_exp);
        bool t_bool = getIntFromConstExpression(cond->get_true_exp(), &t_exp);
        bool f_bool = getIntFromConstExpression(cond->get_false_exp(), &f_exp);

        if (!c_bool) return false;
        else if (c_exp != 0 && !t_bool) return false;
        else if (c_exp == 0 && !f_bool) return false;
        else if (c_exp != 0 && t_bool) {
          *res = t_exp;
          return true;
        }
        else if (c_exp == 0 && f_bool) {
          *res = f_exp;
          return true;
        }
        else return false;
    }
    else if (isSgVarRefExp(exp)) return false; 
    else if (isSgFunctionCallExp(exp)) return false; 
    else {
        std::cout << "In getIntFromConstExpression() unrecognized expression: " << exp->class_name() << std::endl;
        return false;
    }
}

tri_state eval(SgExpression * exp) {
  SgBinaryOp * bin_op = isSgBinaryOp(exp);
  if (bin_op) {
    SgAndOp * and_op = isSgAndOp(bin_op);
    SgOrOp  * or_op  = isSgOrOp(bin_op);

    if (and_op || or_op) {
      tri_state lhs = eval(bin_op->get_lhs_operand_i());
      tri_state rhs = eval(bin_op->get_rhs_operand_i());

      if (and_op) {
        if (lhs == faux || rhs == faux) return faux;
        if (lhs == vrai && rhs == vrai) return vrai;
        return undefined;
      }

      if (or_op) {
        if (lhs == faux && rhs == faux) return faux;
        if (lhs == vrai || rhs == vrai) return vrai;
        return undefined;
      }
    } 

    SgLessThanOp * lt_op = isSgLessThanOp(bin_op);
    SgLessOrEqualOp * le_op = isSgLessOrEqualOp(bin_op);
    SgGreaterThanOp * gt_op = isSgGreaterThanOp(bin_op);
    SgGreaterOrEqualOp * ge_op = isSgGreaterOrEqualOp(bin_op);
    SgEqualityOp * eq_op = isSgEqualityOp(bin_op);

    if (lt_op || le_op || gt_op || ge_op || eq_op) {
      int rhs_val;
      int lhs_val;
      bool rhs = getIntFromConstExpression(bin_op->get_rhs_operand_i(), &rhs_val);
      bool lhs = getIntFromConstExpression(bin_op->get_lhs_operand_i(), &lhs_val);

      if (!rhs || !lhs) return undefined;

      switch (bin_op->variantT()) {
        case V_SgLessThanOp:
          return lhs_val < rhs_val ? vrai : faux;
        case V_SgLessOrEqualOp:
          return lhs_val <= rhs_val ? vrai : faux;
        case V_SgGreaterThanOp:
          return lhs_val > rhs_val ? vrai : faux;
        case V_SgGreaterOrEqualOp:
          return lhs_val >= rhs_val ? vrai : faux;
        case V_SgEqualityOp:
          return lhs_val == rhs_val ? vrai : faux;
        default:
          ROSE_ASSERT(false);
      }
    }

    return undefined;
  }

  if (isSgIntVal(exp)) {
    return isSgIntVal(exp)->get_value() == 0 ? faux : vrai;
  }

  return undefined;
}

SgStatement * removeConstIf(SgStatement * stmt) {
  if (stmt == NULL) return NULL;
  SgStatement * res = NULL;
  switch (stmt->variantT()) {
    case V_SgBasicBlock:
    {
      SgBasicBlock * bb = isSgBasicBlock(stmt);
      std::vector<SgStatement *>::iterator it = bb->get_statements().begin();
      while (it != bb->get_statements().end()) {
        SgStatement * tmp = removeConstIf(*it);
        if (tmp != NULL) {
          *it = tmp;
          tmp->set_parent((*it)->get_parent());
          it++;
        }
        else
          it = bb->get_statements().erase(it);
      }
      res = stmt;
      break;
    }
    case V_SgForStatement:
    {
      SgForStatement * for_stmt = isSgForStatement(stmt);
      SgStatement * tmp = removeConstIf(for_stmt->get_loop_body());
      if (tmp != NULL) {
        for_stmt->set_loop_body(tmp);
        res = for_stmt;
      }
      else res = NULL;
      break;
    }
    case V_SgIfStmt:
    {
      SgIfStmt * if_stmt = isSgIfStmt(stmt);
      SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
      ROSE_ASSERT(cond_stmt != NULL);
      SgExpression * cond = cond_stmt->get_expression();
      switch (eval(cond)) {
        case vrai:
          res = removeConstIf(if_stmt->get_true_body());
          break;
        case faux:
          res = removeConstIf(if_stmt->get_false_body());
          break;
        case undefined:
          if_stmt->set_true_body(removeConstIf(if_stmt->get_true_body()));
          if_stmt->set_false_body(removeConstIf(if_stmt->get_false_body()));
          if (if_stmt->get_true_body() == NULL && if_stmt->get_false_body() == NULL)
            res = NULL;
          else
            res = if_stmt;
          break;
      }
      cond->set_parent(cond_stmt);
      break;
    }
    default:
      res = stmt;
  }

  SgBasicBlock * bb = isSgBasicBlock(res);
  while (bb != NULL) {
    if (bb->get_statements().size() == 0)
      res = NULL;
    else if (bb->get_statements().size() == 1)
      res = bb->get_statements()[0];
    else
      break;
    bb = isSgBasicBlock(res);
  }

  if (res != NULL)
    res->set_parent(stmt->get_parent());

  return res;
}

SgExpression * simplify(SgExpression * exp) {
  if (exp == NULL || isSgIntVal(exp)) {
    return exp;
  }

  {
    int value;
    if (getIntFromConstExpression(exp, &value)) {
      return SageBuilder::buildIntVal(value);
    }
  }

  SgBinaryOp * bin_op = isSgBinaryOp(exp);
  if (bin_op != NULL) {
    SgExpression * lhs = simplify(bin_op->get_lhs_operand_i());
    bin_op->set_lhs_operand_i(lhs);
    lhs->set_parent(bin_op);
    SgExpression * rhs = simplify(bin_op->get_rhs_operand_i());
    bin_op->set_rhs_operand_i(rhs);
    rhs->set_parent(bin_op);

    if (isSgAndOp(bin_op) || isSgOrOp(bin_op)) {
      int lhs_val;
      int rhs_val;
      bool lhs_bool = getIntFromConstExpression(lhs, &lhs_val);
      bool rhs_bool = getIntFromConstExpression(rhs, &rhs_val);
      if (lhs_bool && rhs_bool) ROSE_ASSERT(false);
      else if (lhs_bool) {
        if (lhs_val == 0) {
          if (isSgAndOp(bin_op)) return SageBuilder::buildIntVal(0);
          if (isSgOrOp(bin_op)) return rhs;
        }
        else {
          if (isSgAndOp(bin_op)) return rhs;
          if (isSgOrOp(bin_op)) return SageBuilder::buildIntVal(1);
        }
      }
      else if (rhs_bool) {
        if (rhs_val == 0) {
          if (isSgAndOp(bin_op)) return SageBuilder::buildIntVal(0);
          if (isSgOrOp(bin_op)) return lhs;
        }
        else {
          if (isSgAndOp(bin_op)) return lhs;
          if (isSgOrOp(bin_op)) return SageBuilder::buildIntVal(1);
        }
      }
    }

    return bin_op;
  }

  SgConditionalExp * cond_exp = isSgConditionalExp(exp);
  if (cond_exp != NULL) {

    int value;
    if (getIntFromConstExpression(cond_exp->get_conditional_exp(), &value)) {
      SgExpression * res = NULL;
      if (value == 0)
        res = simplify(cond_exp->get_false_exp());
      else
        res = simplify(cond_exp->get_true_exp());
      res->set_parent(cond_exp->get_parent());
      return res;
    }
    else {
      SgExpression * c_exp = simplify(cond_exp->get_conditional_exp());
        cond_exp->set_conditional_exp(c_exp);
        c_exp->set_parent(cond_exp);
      SgExpression * t_exp = simplify(cond_exp->get_true_exp());
        cond_exp->set_true_exp(t_exp);
        t_exp->set_parent(cond_exp);
      SgExpression * f_exp = simplify(cond_exp->get_false_exp());
        cond_exp->set_false_exp(f_exp);
        f_exp->set_parent(cond_exp);
      return cond_exp;
    }
  }

  SgFunctionCallExp * func_call = isSgFunctionCallExp(exp);
  if (func_call != NULL) {

    SgFunctionRefExp * func_ref = isSgFunctionRefExp(func_call->get_function());
    if (func_ref != NULL) {
      std::string func_name = func_ref->get_symbol_i()->get_name().getString();
      int value;
      if (func_name == "floor") {
        ROSE_ASSERT(func_call->get_args()->get_expressions().size() == 1);
        if (getIntFromConstExpression(func_call->get_args()->get_expressions()[0], &value))
          return SageBuilder::buildIntVal(value);
        else
          return simplify(func_call->get_args()->get_expressions()[0]);
      }
      else if (func_name == "ceil") {
        ROSE_ASSERT(func_call->get_args()->get_expressions().size() == 1);
        if (getIntFromConstExpression(func_call->get_args()->get_expressions()[0], &value))
          return SageBuilder::buildIntVal(value+1);
        else
          return simplify(func_call->get_args()->get_expressions()[0]);
      }
      else ROSE_ASSERT(false);
    }
  }


  SgMinusOp * minus_op = isSgMinusOp(exp);
  if (minus_op != NULL) {
    SgExpression * tmp_exp = simplify(minus_op->get_operand_i());
    minus_op->set_operand_i(tmp_exp);
    tmp_exp->set_parent(minus_op);
    return minus_op;
  }

  SgCastExp * cast_exp = isSgCastExp(exp);
  if (cast_exp != NULL) {
    return simplify(cast_exp->get_operand_i());
  }

  if (isSgVarRefExp(exp)) return exp;

  if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp)) return exp;

  std::cerr << exp->class_name() << std::endl;
  ROSE_ASSERT(false);
}

#define MIN_INT 0xFFFFFFFF
#define MAX_INT 0x7FFFFFFF

std::pair<int,int> getVariableBounds(SgVariableSymbol * symbol, SgScopeStatement * scope) {
  int lb = MIN_INT;
  int ub = MAX_INT;

  int value;

  SgStatement * parent = scope;
  while (!isSgFunctionDefinition(parent)) {
    SgForStatement * for_stmt = isSgForStatement(parent);
    SgIfStmt * if_stmt = isSgIfStmt(parent);
    if (for_stmt != NULL) {
      bool direct = isSgPlusPlusOp(for_stmt->get_increment());
      {
        SgForInitStatement * for_init = for_stmt->get_for_init_stmt();
        if (for_init->get_init_stmt().size() != 1) {
          ROSE_ASSERT(for_init->get_init_stmt().size() == 0);
          SgForStatement * prev = isSgForStatement(SageInterface::getPreviousStatement(for_stmt));
          ROSE_ASSERT(prev != NULL);
          SgExprStatement * test_stmt = isSgExprStatement(prev->get_test());
          ROSE_ASSERT(test_stmt != NULL);
          SgBinaryOp * bin_op = isSgBinaryOp(test_stmt->get_expression());
          ROSE_ASSERT(bin_op != NULL);
          SgVarRefExp * it = NULL;
          SgCastExp * cast = isSgCastExp(bin_op->get_lhs_operand_i());
          if (cast != NULL) {
            it = isSgVarRefExp(cast->get_operand_i());
          }
          else {
            it = isSgVarRefExp(bin_op->get_lhs_operand_i());
          }
          ROSE_ASSERT(it != NULL);
          if (symbol == it->get_symbol()) {
            if (getIntFromConstExpression(bin_op->get_rhs_operand_i(), &value)) {
              switch (bin_op->variantT()) {
                case V_SgLessOrEqualOp:
                  value++;
                case V_SgLessThanOp:
                  if (direct)
                    lb = value < lb ? value : lb;
                  else
                    ROSE_ASSERT(false);
                  break;
                case V_SgGreaterOrEqualOp:
                  value--;
                case V_SgGreaterThanOp:
                  if (direct)
                    ROSE_ASSERT(false);
                  else
                    ub = value > ub ? value : ub;
                  break;
                default:
                  ROSE_ASSERT(false);
              }
            }
          }
        }
        else {
          SgExprStatement * init_stmt = isSgExprStatement(for_init->get_init_stmt()[0]);
          ROSE_ASSERT(init_stmt != NULL);
          SgAssignOp * assign_init = isSgAssignOp(init_stmt->get_expression());
          if (assign_init != NULL) {
            SgVarRefExp * it = isSgVarRefExp(assign_init->get_lhs_operand_i());
            ROSE_ASSERT(it != NULL);
            if (symbol == it->get_symbol()) {
              if (getIntFromConstExpression(assign_init->get_rhs_operand_i(), &value))
                if (direct)
                  lb = value > lb ? value : lb;
                else
                  ub = value < lb ? value : ub;
            }
          }
        }
      }
      {
        SgExprStatement * test_stmt = isSgExprStatement(for_stmt->get_test());
        ROSE_ASSERT(test_stmt != NULL);
        SgBinaryOp * bin_op = isSgBinaryOp(test_stmt->get_expression());
        if (bin_op != NULL) {
          SgVarRefExp * it = NULL;
          SgCastExp * cast = isSgCastExp(bin_op->get_lhs_operand_i());
          if (cast != NULL) {
            it = isSgVarRefExp(cast->get_operand_i());
          }
          else {
            it = isSgVarRefExp(bin_op->get_lhs_operand_i());
          }
          ROSE_ASSERT(it != NULL); // FIXME only valid because it is generated code (also for cleanly writted code !)
          if (symbol == it->get_symbol()) {
            if (getIntFromConstExpression(bin_op->get_rhs_operand_i(), &value)) {
              switch (bin_op->variantT()) {
                case V_SgLessThanOp:
                  value--;
                case V_SgLessOrEqualOp:
                  if (direct)
                    ub = value < ub ? value : ub;
                  else
                    ROSE_ASSERT(false);
                  break;
                case V_SgGreaterThanOp:
                  value++;
                case V_SgGreaterOrEqualOp:
                  if (direct)
                    ROSE_ASSERT(false);
                  else
                    lb = value > lb ? value : lb;
                  break;
                default:
                  ROSE_ASSERT(false);
              }
            }
          }
        }
      }
    }
    if (if_stmt != NULL) {
      SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
      ROSE_ASSERT(cond_stmt != NULL);
      SgExpression * tmp_exp = cond_stmt->get_expression();

      std::vector<SgExpression *> cond_list;
      if (isSgAndOp(tmp_exp)) {
        std::queue<SgAndOp *> and_queue;
        and_queue.push(isSgAndOp(tmp_exp));
        while (!and_queue.empty()) {
          tmp_exp = and_queue.front()->get_lhs_operand_i();
          if (isSgAndOp(tmp_exp)) and_queue.push(isSgAndOp(tmp_exp));
          else if (!isSgIntVal(tmp_exp)) cond_list.push_back(tmp_exp);
          else ROSE_ASSERT(isSgIntVal(tmp_exp)->get_value() != 0);
          tmp_exp = and_queue.front()->get_rhs_operand_i();
          if (isSgAndOp(tmp_exp)) and_queue.push(isSgAndOp(tmp_exp));
          else if (!isSgIntVal(tmp_exp)) cond_list.push_back(tmp_exp);
          else ROSE_ASSERT(isSgIntVal(tmp_exp)->get_value() != 0);
          and_queue.pop();
        }
      }
      else if (!isSgIntVal(tmp_exp)) cond_list.push_back(tmp_exp);

      std::vector<SgExpression *>::iterator cond_it;
      for (cond_it = cond_list.begin(); cond_it != cond_list.end(); cond_it++) {
        SgBinaryOp * bin_op = isSgBinaryOp(*cond_it);
        if (bin_op == NULL) std::cerr << (*cond_it)->class_name() << std::endl;
        ROSE_ASSERT(bin_op != NULL);
        SgVarRefExp * it = NULL;
        SgCastExp * cast = isSgCastExp(bin_op->get_lhs_operand_i());
        if (cast != NULL) {
          it = isSgVarRefExp(cast->get_operand_i());
          if (it == NULL) std::cout << "From cast -> " << cast->get_operand_i()->class_name() << std::endl;
        }
        else {
          it = isSgVarRefExp(bin_op->get_lhs_operand_i());
          if (it == NULL) std::cout << "From bin_op -> " << bin_op->get_lhs_operand_i()->class_name() << std::endl;
        }
        ROSE_ASSERT(it != NULL); // FIXME only valid because it is generated code (also for cleanly writted code !)
        if (symbol == it->get_symbol()) {
          if (getIntFromConstExpression(bin_op->get_rhs_operand_i(), &value)) {
            switch (bin_op->variantT()) {
              case V_SgLessThanOp:
                value--;
              case V_SgLessOrEqualOp:
                ub = value < ub ? value : ub;
                break;
              case V_SgGreaterThanOp:
                value++;
              case V_SgGreaterOrEqualOp:
                lb = value > lb ? value : lb;
                break;
              case V_SgEqualityOp:
                ub = lb = value;
              default:
                ROSE_ASSERT(false);
            }
          }
        }
      }
    }

    parent = isSgStatement(parent->get_parent());
    ROSE_ASSERT(parent != NULL);
  }

  return std::pair<int,int>(lb, ub);
}

std::pair<int,int> evalExpressionBounds(SgExpression * exp, std::map<SgVariableSymbol *, std::pair<int,int> > bounds) {
  int lb = MIN_INT;
  int ub = MAX_INT;

  int value;

  if (getIntFromConstExpression(exp, &value)) {
    lb = value;
    ub = value;
  }
  else {
    SgBinaryOp * bin_op = isSgBinaryOp(exp);
    if (bin_op != NULL) {
      std::pair<int,int> lhs = evalExpressionBounds(bin_op->get_lhs_operand_i(), bounds);
      std::pair<int,int> rhs = evalExpressionBounds(bin_op->get_rhs_operand_i(), bounds);
      switch (bin_op->variantT()) {
        case V_SgAddOp:
          lb = lhs.first + rhs.first;
          ub = lhs.second + rhs.second;
          break;
        case V_SgSubtractOp:
          lb = lhs.first - rhs.second;
          ub = lhs.second - rhs.first;
          break;
        case V_SgMultiplyOp:
          lb = lhs.first * rhs.first;
          ub = lhs.second * rhs.second;
          break;
        case V_SgDivideOp:
          lb = lhs.first / rhs.second;
          ub = lhs.second / rhs.first;
          break;
        case V_SgModOp:
          ROSE_ASSERT(rhs.first == rhs.second);
          if (lhs.first == lhs.second) {
            lb = ub = lhs.first % rhs.first;
          }
          else {
            if (lhs.second - lhs.first < rhs.first) {
              int mod_lb = lhs.first % rhs.first;
              int mod_ub = lhs.second% rhs.first;
              if (mod_lb <= mod_ub) {
                lb = mod_lb;
                ub = mod_ub;
              }
              else {
                lb = 0;
                ub = rhs.first - 1;
              }
            }
            else {
              lb = 0;
              ub = rhs.first - 1;
            }
          }
          break;
        default:
          ROSE_ASSERT(false);
      }
    }
    else if (isSgMinusOp(exp)) {
      SgMinusOp * minus_op = isSgMinusOp(exp);
      std::pair<int,int> b = evalExpressionBounds(minus_op->get_operand_i(), bounds);
      lb = -b.second;
      ub = -b.first;
    }
    else if (isSgCastExp(exp)) {
      SgCastExp * cast = isSgCastExp(exp);
      return evalExpressionBounds(cast->get_operand_i(), bounds);
    }
    else if (isSgVarRefExp(exp)) {
      SgVarRefExp * var_ref = isSgVarRefExp(exp);
      std::map<SgVariableSymbol *, std::pair<int,int> >::iterator it = bounds.find(var_ref->get_symbol());
      ROSE_ASSERT(it != bounds.end());
      return it->second;
    }
    else if (isSgConditionalExp(exp)) {
      SgConditionalExp * cond_exp = isSgConditionalExp(exp);

      std::pair<int,int> true_bounds = evalExpressionBounds(cond_exp->get_true_exp(), bounds);
      std::pair<int,int> false_bounds = evalExpressionBounds(cond_exp->get_false_exp(), bounds);

      switch (eval(cond_exp->get_conditional_exp())) {
        case faux:
          return false_bounds;
        case vrai:
          return true_bounds;
        case undefined:
          lb = true_bounds.first < false_bounds.first ? true_bounds.first : false_bounds.first;
          ub = true_bounds.second > false_bounds.second ? true_bounds.second : false_bounds.second;
      }
    }
    else if (isSgIntVal(exp)) {
      ub = lb = isSgIntVal(exp)->get_value();
    }
    else {
      std::cerr << exp->class_name() << std::endl;
      ROSE_ASSERT(false);
    }
  }

  return std::pair<int,int>(lb, ub);
}

bool areEquivalent(SgExpression * e1, SgExpression * e2) {
  if (e1->variantT() != e2->variantT()) return false;

  if (isSgBinaryOp(e1)) {
    SgBinaryOp * op1 = isSgBinaryOp(e1);
    SgBinaryOp * op2 = isSgBinaryOp(e2);

    return areEquivalent(op1->get_lhs_operand_i(), op2->get_lhs_operand_i())
        && areEquivalent(op1->get_rhs_operand_i(), op2->get_rhs_operand_i());
  }

  if (isSgUnaryOp(e1)) {
    SgUnaryOp * op1 = isSgUnaryOp(e1);
    SgUnaryOp * op2 = isSgUnaryOp(e2);

    return areEquivalent(op1->get_operand_i(), op2->get_operand_i());
  }

  if (isSgIntVal(e1)) {
    SgIntVal * v1 = isSgIntVal(e1);
    SgIntVal * v2 = isSgIntVal(e2);

    return v1->get_value() == v2->get_value();
  }

  if (isSgVarRefExp(e1)) {
    SgVarRefExp * ref1 = isSgVarRefExp(e1);
    SgVarRefExp * ref2 = isSgVarRefExp(e2);

    return ref1->get_symbol() == ref2->get_symbol();
  }

  return false;
  std::cout << e1->class_name() << std::endl;
  ROSE_ASSERT(false);
}

bool isMinMax(SgNode * n) {
  SgExpression * exp = isSgExpression(n);
  if (exp == NULL) return false;
  SgConditionalExp * cond_exp = isSgConditionalExp(exp);
  if (cond_exp == NULL) return false;
  SgBinaryOp * bin_op = isSgBinaryOp(cond_exp->get_conditional_exp());
  if (bin_op == NULL) return false;
  return areEquivalent(bin_op->get_lhs_operand_i(), cond_exp->get_true_exp())
      && areEquivalent(bin_op->get_rhs_operand_i(), cond_exp->get_false_exp());
}

void aggresivelySimplifyBounds(
  SgExpression * exp,
  std::map<SgVariableSymbol *, std::pair<int,int> > bounds_val,
  std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > bounds_exp
) {
  std::vector<SgExpression *> cond_list;
  std::queue<SgExpression *> queue;
  queue.push(exp);
  while (!queue.empty()) {
    SgExpression * tmp_exp = queue.front();

    if (isSgAndOp(tmp_exp)) {
      SgAndOp * and_op = isSgAndOp(tmp_exp);
      queue.push(and_op->get_lhs_operand_i());
      queue.push(and_op->get_rhs_operand_i());
    }
    else if (isSgConditionalExp(tmp_exp)) {
      SgConditionalExp * cond_exp = isSgConditionalExp(tmp_exp);
      queue.push(cond_exp->get_conditional_exp());
      queue.push(cond_exp->get_true_exp());
      queue.push(cond_exp->get_false_exp());
    }
    else if (isSgLessThanOp(tmp_exp) || isSgLessOrEqualOp(tmp_exp) || isSgGreaterThanOp(tmp_exp) || isSgGreaterOrEqualOp(tmp_exp)) {
      cond_list.push_back(tmp_exp);
    }
    else if (isSgCastExp(tmp_exp)) queue.push(isSgUnaryOp(tmp_exp)->get_operand_i());
    else {
      std::cerr << "aggresivelySimplifyBounds skips: " << tmp_exp->class_name() << std::endl;
    }

    queue.pop();
  }

  std::vector<SgExpression *>::iterator cond_it;
  for (cond_it = cond_list.begin(); cond_it != cond_list.end(); cond_it++) {
    SgBinaryOp * bin_op = isSgBinaryOp(*cond_it);
    ROSE_ASSERT(bin_op != NULL);

    std::pair<int,int> lhs = evalExpressionBounds(bin_op->get_lhs_operand_i(), bounds_val);
    std::pair<int,int> rhs = evalExpressionBounds(bin_op->get_rhs_operand_i(), bounds_val);

    tri_state res = undefined;
    switch (bin_op->variantT()) {
      case V_SgLessThanOp:
        if (!isMinMax(bin_op->get_parent())) {
          if (lhs.second < rhs.first) res = vrai;
          else if (lhs.first >= rhs.second) res = faux;
          break;
        }
      case V_SgLessOrEqualOp:
        if (lhs.second <= rhs.first) res = vrai;
        else if (lhs.first > rhs.second) res = faux;
        break;
      case V_SgGreaterThanOp:
        if (!isMinMax(bin_op->get_parent())) {
          if (lhs.first > rhs.second) res = vrai;
          else if (lhs.second <= rhs.first) res = faux;
          break;
        }
      case V_SgGreaterOrEqualOp:
        if (lhs.first >= rhs.second) res = vrai;
        else if (lhs.second < rhs.first) res = faux;
        break;
      default:
        ROSE_ASSERT(false);
    }

    switch (res) {
      case vrai:
        SageInterface::replaceExpression(bin_op, SageBuilder::buildIntVal(1), true);
        break;
      case faux:
        SageInterface::replaceExpression(bin_op, SageBuilder::buildIntVal(0), true);
        break;
      case undefined:
        break;
    }
  }
}

void findIteratorBounds(
  std::map<SgVariableSymbol *, std::pair<int,int> > & bounds_val,
  std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > & bounds_exp,
  SgStatement * context, SgStatement * limit
) {
  std::cerr << ">" << std::endl;
  std::vector<SgForStatement *> for_list;
  std::vector<SgBinaryOp *> if_conds;
  SgStatement * current = isSgStatement(context->get_parent());
  while (current != NULL && current != limit) {
    if (isSgForStatement(current)) {
      for_list.insert(for_list.begin(), isSgForStatement(current));
    }

    if (isSgIfStmt(current)) {
      std::queue<SgExpression *> queue;
      queue.push(isSgExprStatement(isSgIfStmt(current)->get_conditional())->get_expression());
      while (!queue.empty()) {
        if (isSgAndOp(queue.front())) {
          queue.push(isSgAndOp(queue.front())->get_lhs_operand_i());
          queue.push(isSgAndOp(queue.front())->get_rhs_operand_i());
        }
        else if (
                  isSgLessThanOp(queue.front()) || isSgLessOrEqualOp(queue.front()) ||
                  isSgGreaterThanOp(queue.front()) || isSgGreaterOrEqualOp(queue.front())
        ) {
          if_conds.push_back(isSgBinaryOp(queue.front()));
        }
        queue.pop();
      }
    }

    current = isSgStatement(current->get_parent());
  }

  std::vector<SgForStatement *>::iterator it_for;
  for (it_for = for_list.begin(); it_for != for_list.end(); it_for++) {
    SgVariableSymbol * it_sym = NULL;
    SgExpression * lb_exp = NULL;
    SgExpression * ub_exp = NULL;
    
    {
      bool direct = true;
      SgExpression * inc_for = (*it_for)->get_increment();
      if (isSgMinusMinusOp(inc_for)) direct = false;
      else if (isSgPlusPlusOp(inc_for)) direct = true;
      else ROSE_ASSERT(false);

      SgVarRefExp * it_ref = isSgVarRefExp(isSgUnaryOp(inc_for)->get_operand_i());
      ROSE_ASSERT(it_ref != NULL);

      it_sym = it_ref->get_symbol();

      SgExpression * tmp_init = NULL;
      SgForInitStatement * init_stmt_for = (*it_for)->get_for_init_stmt();
      if (init_stmt_for->get_init_stmt().size() == 1) {
        SgExprStatement * expr_stmt = isSgExprStatement(init_stmt_for->get_init_stmt()[0]);
        ROSE_ASSERT(expr_stmt != NULL);
        SgAssignOp * assign_op = isSgAssignOp(expr_stmt->get_expression());
        ROSE_ASSERT(assign_op != NULL);
        it_ref = isSgVarRefExp(assign_op->get_lhs_operand_i());
        ROSE_ASSERT(it_ref != NULL);
        ROSE_ASSERT(it_sym == it_ref->get_symbol());
        tmp_init = SageInterface::deepCopy(assign_op->get_rhs_operand_i());
      }
      else if (init_stmt_for->get_init_stmt().size() == 0) {
        SgForStatement * prev = isSgForStatement(SageInterface::getPreviousStatement(*it_for));
        ROSE_ASSERT(prev != NULL);
        SgExprStatement * expr_stmt = isSgExprStatement(prev->get_test());
        ROSE_ASSERT(expr_stmt != NULL);
        SgBinaryOp * bin_op = isSgBinaryOp(expr_stmt->get_expression());
        if (direct) {
          ROSE_ASSERT(isSgLessThanOp(bin_op) || isSgLessOrEqualOp(bin_op));
        }
        else {
          ROSE_ASSERT(isSgGreaterThanOp(bin_op) || isSgGreaterOrEqualOp(bin_op));
        }
        it_ref = isSgVarRefExp(bin_op->get_lhs_operand_i());
        ROSE_ASSERT(it_ref != NULL);
        ROSE_ASSERT(it_sym == it_ref->get_symbol());
        tmp_init = SageInterface::deepCopy(bin_op->get_rhs_operand_i());
        if (isSgLessOrEqualOp(bin_op))
          tmp_init = SageBuilder::buildAddOp(tmp_init, SageBuilder::buildIntVal(1));
        if (isSgGreaterOrEqualOp(bin_op))
          tmp_init = SageBuilder::buildSubtractOp(tmp_init, SageBuilder::buildIntVal(1));
      }
      else ROSE_ASSERT(false);

      if (direct)
        lb_exp = tmp_init;
      else
        ub_exp = tmp_init;

      SgExprStatement * expr_stmt = isSgExprStatement((*it_for)->get_test());
      ROSE_ASSERT(expr_stmt != NULL);
      SgBinaryOp * bin_op = isSgBinaryOp(expr_stmt->get_expression());
      if (direct) {
        ROSE_ASSERT(isSgLessThanOp(bin_op) || isSgLessOrEqualOp(bin_op));
      }
      else {
        ROSE_ASSERT(isSgGreaterThanOp(bin_op) || isSgGreaterOrEqualOp(bin_op));
      }
      it_ref = isSgVarRefExp(bin_op->get_lhs_operand_i());
      ROSE_ASSERT(it_ref != NULL);
      ROSE_ASSERT(it_sym == it_ref->get_symbol());
      SgExpression * tmp_test = SageInterface::deepCopy(bin_op->get_rhs_operand_i());
      if (isSgLessThanOp(bin_op))
        tmp_test = SageBuilder::buildSubtractOp(tmp_test, SageBuilder::buildIntVal(1));
      if (isSgGreaterThanOp(bin_op))
        tmp_test = SageBuilder::buildAddOp(tmp_test, SageBuilder::buildIntVal(1));
      if (direct)
        ub_exp = tmp_test;
      else
        lb_exp = tmp_test;
    }
    ROSE_ASSERT(it_sym != NULL && lb_exp != NULL && ub_exp != NULL);

    std::cerr << it_sym->get_name().getString() << " -> " << lb_exp->unparseToString() << " / " << ub_exp->unparseToString() << std::endl;

    bounds_exp.insert(std::pair<SgVariableSymbol *, std::pair<SgExpression *,SgExpression *> >(
                         it_sym , std::pair<SgExpression *,SgExpression *>(lb_exp, ub_exp)
    ));

    std::pair<int,int> lb_bound = evalExpressionBounds(lb_exp, bounds_val);
    std::pair<int,int> ub_bound = evalExpressionBounds(ub_exp, bounds_val);

    bounds_val.insert(std::pair<SgVariableSymbol *, std::pair<int,int> >(it_sym , std::pair<int,int>(lb_bound.first, ub_bound.second)));
  }

  // TODO apply 'if' conds...

  std::cerr << "<" << std::endl;
}

SgBasicBlock * cleanPoCC(SgBasicBlock * bb) {
  // TODO remove unused declaration and the register storage qualifiers...

  {
    std::vector<SgExpression *> expr_list = SageInterface::querySubTree<SgExpression>(bb);
    std::vector<SgExpression *>::iterator it_expr;
    std::vector<SgExpression *> expr_list_2;
    for (it_expr = expr_list.begin(); it_expr != expr_list.end(); it_expr++)
      if (isSgExpression(*it_expr) && !isSgExpression((*it_expr)->get_parent()))
        expr_list_2.push_back(*it_expr);
    for (it_expr = expr_list_2.begin(); it_expr != expr_list_2.end(); it_expr++) {
      SgExpression * tmp_exp = simplify(*it_expr);
      SageInterface::replaceExpression(*it_expr, SageInterface::deepCopy(tmp_exp), true);
    }
  }
  
  bb = isSgBasicBlock(removeConstIf(bb));
  ROSE_ASSERT(bb != NULL);

  {
    std::vector<SgIfStmt *> if_stmts = SageInterface::querySubTree<SgIfStmt>(bb);
    std::vector<SgIfStmt *>::iterator it_if;
    for (it_if = if_stmts.begin(); it_if != if_stmts.end(); it_if++) {
      SgExprStatement * cond_stmt = isSgExprStatement((*it_if)->get_conditional());
      ROSE_ASSERT(cond_stmt != NULL);
      SgExpression * cond = cond_stmt->get_expression();
      ROSE_ASSERT(cond != NULL);

      std::map<SgVariableSymbol *, std::pair<int,int> > bounds_val;
      std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > bounds_exp;
      findIteratorBounds(bounds_val, bounds_exp, *it_if, bb);
      aggresivelySimplifyBounds(cond, bounds_val, bounds_exp);
    }
  }

  {
    std::vector<SgExpression *> expr_list = SageInterface::querySubTree<SgExpression>(bb);
    std::vector<SgExpression *>::iterator it_expr;
    std::vector<SgExpression *> expr_list_2;
    for (it_expr = expr_list.begin(); it_expr != expr_list.end(); it_expr++)
      if (isSgExpression(*it_expr) && !isSgExpression((*it_expr)->get_parent()))
        expr_list_2.push_back(*it_expr);
    for (it_expr = expr_list_2.begin(); it_expr != expr_list_2.end(); it_expr++) {
      SgExpression * tmp_exp = simplify(*it_expr);
      SageInterface::replaceExpression(*it_expr, SageInterface::deepCopy(tmp_exp), true);
    }
  }

  bb = isSgBasicBlock(removeConstIf(bb));
  ROSE_ASSERT(bb != NULL);

  {
    std::vector<SgForStatement *> for_stmts = SageInterface::querySubTree<SgForStatement>(bb);
    std::vector<SgForStatement *>::iterator it_for;
    for (it_for = for_stmts.begin(); it_for != for_stmts.end(); it_for++) {
      SgExprStatement * cond_stmt = isSgExprStatement((*it_for)->get_test());
      ROSE_ASSERT(cond_stmt != NULL);
      SgExpression * cond = cond_stmt->get_expression();
      ROSE_ASSERT(cond != NULL);

      std::map<SgVariableSymbol *, std::pair<int,int> > bounds_val;
      std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > bounds_exp;
      findIteratorBounds(bounds_val, bounds_exp, *it_for, bb);
      aggresivelySimplifyBounds(cond, bounds_val, bounds_exp);
    }
  }

  return bb;
}

// Assume (+ (* () ()) ... (* () ()) ()), add (can be sub) are in tree form: recurse with coef
void expToMap(SgExpression * exp, std::map<SgVariableSymbol *, int> & map, int coef = 1) {
  if (isSgAddOp(exp)) {
    SgAddOp * add = isSgAddOp(exp);
    expToMap(add->get_lhs_operand_i(), map, coef);
    expToMap(add->get_rhs_operand_i(), map, coef);
  }
  else if (isSgSubtractOp(exp)) {
    SgSubtractOp * sub = isSgSubtractOp(exp);
    expToMap(sub->get_lhs_operand_i(), map, coef);
    expToMap(sub->get_rhs_operand_i(), map, -coef);
  }
  else if (isSgMultiplyOp(exp)) {
    SgVarRefExp * var = isSgVarRefExp(isSgMultiplyOp(exp)->get_rhs_operand_i());
    SgIntVal * val = NULL;
    if (var == NULL)
      var = isSgVarRefExp(isSgMultiplyOp(exp)->get_lhs_operand_i());
    else
      val = isSgIntVal(isSgMultiplyOp(exp)->get_lhs_operand_i());
    ROSE_ASSERT(var != NULL);
    if (val == NULL)
      val = isSgIntVal(isSgMultiplyOp(exp)->get_rhs_operand_i());
    ROSE_ASSERT(val != NULL);

    SgVariableSymbol * sym = var->get_symbol();
    std::map<SgVariableSymbol *, int>::iterator it = map.find(sym);
    if (it == map.end())
      map.insert(std::pair<SgVariableSymbol *, int>(sym, coef * val->get_value()));
    else
      it->second += coef * val->get_value(); 
  }
  else if (isSgVarRefExp(exp)) {
    SgVariableSymbol * sym = isSgVarRefExp(exp)->get_symbol();
    std::map<SgVariableSymbol *, int>::iterator it = map.find(sym);
    if (it == map.end())
      map.insert(std::pair<SgVariableSymbol *, int>(sym, coef));
    else
      it->second += coef;
  }
  else if (isSgIntVal(exp)) {
    std::map<SgVariableSymbol *, int>::iterator it = map.find(NULL);
    if (it == map.end())
      map.insert(std::pair<SgVariableSymbol *, int>(NULL, coef * isSgIntVal(exp)->get_value()));
    else
      it->second += coef * isSgIntVal(exp)->get_value();
  }
  else {
    std::cout << exp->class_name() << std::endl;
    ROSE_ASSERT(false);
  }
}

SgExpression * mapToExp(std::map<SgVariableSymbol *, int> & map) {
  SgExpression * res = NULL;
  std::map<SgVariableSymbol *, int>::iterator it;
  for (it = map.begin(); it != map.end(); it++) {
    if (it->second == 0) continue;
    if (it->first == NULL)
      if (res == NULL)
        res = SageBuilder::buildIntVal(it->second);
      else
        res = SageBuilder::buildAddOp(res, SageBuilder::buildIntVal(it->second));
    else {
      SgVarRefExp * var_ref = SageBuilder::buildVarRefExp(it->first);
      if (it->second == 1)
        if (res == NULL)
          res = var_ref;
        else
          res = SageBuilder::buildAddOp(res, var_ref);
      else
        if (res == NULL) 
          res = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(it->second), var_ref);
        else 
          res = SageBuilder::buildAddOp(res, SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(it->second), var_ref));
    }
  }
  if (res == NULL) res = SageBuilder::buildIntVal(0);
  return res;
}

SgExpression * substract(SgExpression * lhs_exp, SgExpression * rhs_exp, int add) {
  std::map<SgVariableSymbol *, int> lhs;
  std::map<SgVariableSymbol *, int> rhs;
  expToMap(lhs_exp, lhs);
  expToMap(rhs_exp, rhs);

  std::map<SgVariableSymbol *, int> res;
  std::map<SgVariableSymbol *, int>::iterator it;
  for (it = lhs.begin(); it != lhs.end(); it++) {
    std::map<SgVariableSymbol *, int>::iterator it_ = rhs.find(it->first);
    if (it_ == rhs.end())
      res.insert(std::pair<SgVariableSymbol *, int>(it->first, it->second));
    else {
      res.insert(std::pair<SgVariableSymbol *, int>(it->first, it->second - it_->second));
      rhs.erase(it_);
    }
  }
  for (it = rhs.begin(); it != rhs.end(); it++) {
    std::map<SgVariableSymbol *, int>::iterator it_ = lhs.find(it->first);
    if (it_ == lhs.end())
      res.insert(std::pair<SgVariableSymbol *, int>(it->first, -it->second));
    else
      res.insert(std::pair<SgVariableSymbol *, int>(it->first, it_->second - it->second));
  }
 
  if (add != 0) {
    std::map<SgVariableSymbol *, int>::iterator it = res.find(NULL);
    if (it == res.end())
      res.insert(std::pair<SgVariableSymbol *, int>(NULL, add));
    else
      it->second += add;
  }

  return mapToExp(res);
}

