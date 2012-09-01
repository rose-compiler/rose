#ifndef EVAL_EXPR_H
#define EVAL_EXPR_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <limits.h>
#include <string>
#include "StateRepresentation.h"

using namespace std;

// MS: temporary defines (to be removed)
#define ANALYZER_INT_TOP INT_MAX
#define ANALYZER_INT_BOT INT_MIN


class SingleEvalResult {
 public:
  EState eState;
  AType::BoolLattice result;
  bool isTop() {return result.isTop();}
  bool isTrue() {return result.isTrue();}
  bool isFalse() {return result.isFalse();}
  bool isBot() {return result.isBot();}
};

class SingleEvalResultConstInt {
 public:
  EState eState;
  ConstraintSet exprConstraints; // temporary during evaluation of expression
  AType::ConstIntLattice result;
  int intValue() {return result.getIntValue();}
  bool isConstInt() {return result.isConstInt();}
  bool isTop() {return result.isTop();}
  bool isTrue() {return result.isTrue();}
  bool isFalse() {return result.isFalse();}
  bool isBot() {return result.isBot();}
};

class ExprAnalyzer {
 public:
  SingleEvalResult eval(SgNode* node,EState eState);
  SingleEvalResultConstInt evalConstInt(SgNode* node,EState eState);
  bool childrenVarConst(string* var, int* con);
  bool childrenConstVar(int* con, string* var);
  // returns true if node is a VarRefExp and varName=name, otherwise false and varName="$".
  static bool variable(SgNode* node,VariableName& varName);
};

#endif
