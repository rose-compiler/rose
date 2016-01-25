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
  //! returns true if node is a VarRefExp and sets varName=name, otherwise false and varName="$".
  static bool variable(SgNode* node,VariableName& varName);
  //! returns true if node is a VarRefExp and sets varId=id, otherwise false and varId=0.
  bool variable(SgNode* node,VariableId& varId);

  void setVariableIdMapping(VariableIdMapping* variableIdMapping) { _variableIdMapping=variableIdMapping; }
  void setSkipSelectedFunctionCalls(bool skip);
  bool getSkipSelectedFunctionCalls();
  void setSkipArrayAccesses(bool skip);
  bool getSkipArrayAccesses();
  AType::ConstIntLattice constIntLatticeFromSgValueExp(SgValueExp* valueExp);
 private:
  //! This function turn a single result into a one-elment list with
  //! this one result. This function is used to combine cases where the result
  //! might be empty or have multiple results as well.
  static list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res);
  VariableIdMapping* _variableIdMapping;
  bool _skipSelectedFunctionCalls;
  bool _skipArrayAccesses;
};

} // end of namespace CodeThorn

#endif
