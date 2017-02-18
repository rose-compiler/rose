#ifndef CT_EXPR_ANALYZER_H
#define CT_EXPR_ANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <limits.h>
#include <string>
#include "StateRepresentations.h"
#include "VariableIdMapping.h"
#include "AType.h"
#include "AstTerm.h"

using namespace std;

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class SingleEvalResult {
 public:
  EState estate;
  AType::BoolLattice result;
  bool isTop() {return result.isTop();}
  bool isTrue() {return result.isTrue();}
  bool isFalse() {return result.isFalse();}
  bool isBot() {return result.isBot();}
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class SingleEvalResultConstInt {
 public:
  void init(EState estate, ConstraintSet exprConstraints, AType::ConstIntLattice result);
  EState estate;
  ConstraintSet exprConstraints; // temporary during evaluation of expression
  AType::ConstIntLattice result;
  AValue value() {return result;}
  bool isConstInt() {return result.isConstInt();}
  bool isTop() {return result.isTop();}
  bool isTrue() {return result.isTrue();}
  bool isFalse() {return result.isFalse();}
  bool isBot() {return result.isBot();}
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
//#define EXPR_VISITOR
class ExprAnalyzer {
 public:
  ExprAnalyzer();
  //SingleEvalResult eval(SgNode* node,EState estate);
  //! Evaluates an expression using ConstIntLattice and returns a list of all evaluation-results.
  //! There can be multiple results if one of the variables was bound to top as we generate
  //! two different states and corresponding constraints in this case, one representing the
  //! true-case the other one representing the false-case.
  //! When the option useConstraints is set to false constraints are not used when determining the
  //! values of top-variables. 
  list<SingleEvalResultConstInt> evalConstInt(SgNode* node,EState estate, bool useConstraints, bool safeConstraintPropagation);
  void setVariableIdMapping(VariableIdMapping* variableIdMapping) { _variableIdMapping=variableIdMapping; }
  void setSkipSelectedFunctionCalls(bool skip);
  bool getSkipSelectedFunctionCalls();
  void setSkipArrayAccesses(bool skip);
  bool getSkipArrayAccesses();
 private:
  //! This function turn a single result into a one-elment list with
  //! this one result. This function is used to combine cases where the result
  //! might be empty or have multiple results as well.
  VariableIdMapping* _variableIdMapping;
  bool _skipSelectedFunctionCalls;
  bool _skipArrayAccesses;
 protected:
  //! returns true if node is a VarRefExp and sets varName=name, otherwise false and varName="$".
  static bool variable(SgNode* node,VariableName& varName);
  //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
  bool variable(SgNode* node,VariableId& varId);
  AType::ConstIntLattice constIntLatticeFromSgValueExp(SgValueExp* valueExp);
  static list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res);

  // evaluation state
#ifdef EXPR_VISITOR
  SingleEvalResultConstInt res;

  // evaluation functions
  list<SingleEvalResultConstInt> evalConditionalExpr(SgNode* node);
  list<SingleEvalResultConstInt> evalEqualOp(SgNode* node);
  list<SingleEvalResultConstInt> evalNotEqualOp(SgNode* node);
  list<SingleEvalResultConstInt> evalGreaterThanOp(SgNode* node);
  list<SingleEvalResultConstInt> evalGreaterOrEqualOp(SgNode* node);
  list<SingleEvalResultConstInt> evalLessThanOp(SgNode* node);
  list<SingleEvalResultConstInt> evalLessOrEqualOp(SgNode* node);
  list<SingleEvalResultConstInt> evalAndOp(SgNode* node);
  list<SingleEvalResultConstInt> evalOrOp(SgNode* node);
  list<SingleEvalResultConstInt> evalNotOp(SgNode* node);
  list<SingleEvalResultConstInt> evalBitAndOp(SgNode* node);
  list<SingleEvalResultConstInt> evalBitOrOp(SgNode* node);
  list<SingleEvalResultConstInt> evalBitXorOp(SgNode* node);
  list<SingleEvalResultConstInt> evalBitNotOp(SgNode* node);
  list<SingleEvalResultConstInt> evalUnaryMinusOp(SgNode* node);
  list<SingleEvalResultConstInt> evalAddOp(SgNode* node);
  list<SingleEvalResultConstInt> evalSubOp(SgNode* node);
  list<SingleEvalResultConstInt> evalMulOp(SgNode* node);
  list<SingleEvalResultConstInt> evalDivOp(SgNode* node);
  list<SingleEvalResultConstInt> evalModOp(SgNode* node);
  list<SingleEvalResultConstInt> evalCastOp(SgNode* node);
  list<SingleEvalResultConstInt> evalArrayReference(SgNode* node);
#endif

};

} // end of namespace CodeThorn

#endif
