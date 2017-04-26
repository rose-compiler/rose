#ifndef CT_EXPR_ANALYZER_H
#define CT_EXPR_ANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <limits.h>
#include <string>
#include "EState.h"
#include "VariableIdMapping.h"
#include "AbstractValue.h"
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
  CodeThorn::BoolLattice result;
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
  void init(EState estate, ConstraintSet exprConstraints, AbstractValue result);
  EState estate;
  ConstraintSet exprConstraints; // temporary during evaluation of expression
  AbstractValue result;
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
  //! Evaluates an expression using AbstractValue and returns a list of all evaluation-results.
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
  void setExternalFunctionSemantics(bool flag);
  bool getExternalFunctionSemantics();

  bool checkArrayBounds(VariableId arrayVarId,int accessIndex);
  VariableId resolveToAbsoluteVariableId(AbstractValue abstrValue) const;
  AbstractValue readFromMemoryLocation(const PState* pState, AbstractValue abstrValue) const;
  void writeToMemoryLocation(PState& pState, AbstractValue abstractMemLoc, AbstractValue abstrValue);
private:
  //! This function turn a single result into a one-elment list with
  //! this one result. This function is used to combine cases where the result
  //! might be empty or have multiple results as well.
  VariableIdMapping* _variableIdMapping;

  // Options
  bool _skipSelectedFunctionCalls;
  bool _skipArrayAccesses;
  bool _externalFunctionSemantics;

 public:
  //! returns true if node is a VarRefExp and sets varName=name, otherwise false and varName="$".
  static bool variable(SgNode* node,VariableName& varName);
  //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
  bool variable(SgNode* node,VariableId& varId);

  list<SingleEvalResultConstInt> evalFunctionCall(SgFunctionCallExp* node, EState estate, bool useConstraints);

 protected:
  AbstractValue constIntLatticeFromSgValueExp(SgValueExp* valueExp);
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
  list<SingleEvalResultConstInt> evalBitwiseAndOp(SgBitAndOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitwiseOrOp(SgBitOrOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitwiseXorOp(SgBitXorOp* node,
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
  list<SingleEvalResultConstInt> evalBitwiseShiftLeftOp(SgLshiftOp* node,
                                           SingleEvalResultConstInt lhsResult, 
                                           SingleEvalResultConstInt rhsResult,
                                           EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitwiseShiftRightOp(SgRshiftOp* node,
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
  list<SingleEvalResultConstInt> evalDereferenceOp(SgPointerDerefExp* node, 
                                                   SingleEvalResultConstInt operandResult, 
                                                   EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalBitwiseComplementOp(SgBitComplementOp* node, 
                                              SingleEvalResultConstInt operandResult, 
                                              EState estate, bool useConstraints);

  // use of Variable as rvalue (not as lvalue; lvalues are handled in the transfer function of assignments)
  list<SingleEvalResultConstInt> evalRValueVarExp(SgVarRefExp* node, EState estate, bool useConstraints);
  list<SingleEvalResultConstInt> evalValueExp(SgValueExp* node, EState estate, bool useConstraints);

  list<SingleEvalResultConstInt> evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate, bool useConstraints);
};

} // end of namespace CodeThorn

#endif
