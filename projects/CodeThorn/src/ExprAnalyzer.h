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
  list<SingleEvalResultConstInt> evalConstInt(SgNode* node,EState estate, bool useConstraints);
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
 public:
  //! returns true if node is a VarRefExp and sets varName=name, otherwise false and varName="$".
  static bool variable(SgNode* node,VariableName& varName);
  //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
  bool variable(SgNode* node,VariableId& varId);
 protected:
  AType::ConstIntLattice constIntLatticeFromSgValueExp(SgValueExp* valueExp);
  static list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res);

  // evaluation state
#ifdef EXPR_VISITOR
  SingleEvalResultConstInt res;
#endif
  // evaluation functions
  list<SingleEvalResultConstInt> evalConditionalExpr(SgConditionalExp* node, EState estate, bool useConstraints);

  list<SingleEvalResultConstInt> evalEqualOp(SgEqualityOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalNotEqualOp(SgNotEqualOp* node,
                                             SingleEvalResultConstInt lhsResult, 
                                             SingleEvalResultConstInt rhsResult,
                                             EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalAndOp(SgAndOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalOrOp(SgOrOp* node,
                                          SingleEvalResultConstInt lhsResult, 
                                          SingleEvalResultConstInt rhsResult,
                                          EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalAddOp(SgAddOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalSubOp(SgSubtractOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalMulOp(SgMultiplyOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalDivOp(SgDivideOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalModOp(SgModOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitAndOp(SgBitAndOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitOrOp(SgBitOrOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitXorOp(SgBitXorOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);

  list<SingleEvalResultConstInt> evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalGreaterThanOp(SgGreaterThanOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalLessOrEqualOp(SgLessOrEqualOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalLessThanOp(SgLessThanOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);

  list<SingleEvalResultConstInt> evalArrayReferenceOp(SgPntrArrRefExp* node,
                                                    SingleEvalResultConstInt lhsResult, 
                                                    SingleEvalResultConstInt rhsResult,
                                                    EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalNotOp(SgNotOp* node, 
                                           SingleEvalResultConstInt operandResult, 
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalUnaryMinusOp(SgMinusOp* node, 
                                                  SingleEvalResultConstInt operandResult, 
                                                  EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalCastOp(SgCastExp* node, 
                                            SingleEvalResultConstInt operandResult, 
                                            EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitComplementOp(SgBitComplementOp* node, 
                                              SingleEvalResultConstInt operandResult, 
                                              EState estate, bool useConstraints);
};

} // end of namespace CodeThorn

#endif
