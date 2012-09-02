/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "ExprAnalyzer.h"

bool ExprAnalyzer::variable(SgNode* node, string& varName) {
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
	// found variable
	SgVariableSymbol* varsym=varref->get_symbol();
	varName=varsym->get_name().getString();
	return true;
  } else {
	varName="$";
	return false;
  }
}
bool ExprAnalyzer::variable(SgNode* node, VariableId& varId) {
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
	// found variable
	SgVariableSymbol* varsym=varref->get_symbol();
	varId=VariableId(varsym);
	return true;
  } else {
	VariableId defaultVarId;
	varId=defaultVarId;
	return false;
  }
}
bool ExprAnalyzer::childrenVarConst(string* var, int* con) {
  return false;
}

bool ExprAnalyzer::childrenConstVar(int* con, string* var) {
  return false;
}

//////////////////////////////////////////////////////////////////////
// EVAL BOOL
//////////////////////////////////////////////////////////////////////
SingleEvalResult ExprAnalyzer::eval(SgNode* node,EState eState) {
  SingleEvalResult tmp;
  tmp.eState=eState;
  switch(node->variantT()) {
  case V_SgAndOp: {
	SgNode* lhs=SgNodeHelper::getLhs(node);
	SgNode* rhs=SgNodeHelper::getRhs(node);
	SingleEvalResult lhsResult=eval(lhs,eState);
	SingleEvalResult rhsResult=eval(rhs,eState);
	tmp.result=lhsResult.result&&rhsResult.result;
	return tmp;
  }
  case V_SgOrOp: {
	SgNode* lhs=SgNodeHelper::getLhs(node);
	SgNode* rhs=SgNodeHelper::getRhs(node);
	SingleEvalResult lhsResult=eval(lhs,eState);
	SingleEvalResult rhsResult=eval(rhs,eState);
	tmp.result=lhsResult.result||rhsResult.result;
	return tmp;
  }
  case V_SgNotOp: {
	SgNode* child=SgNodeHelper::getFirstChild(node);
	SingleEvalResult operandResult=eval(child,eState);
	tmp.result=!operandResult.result;
	return tmp;
  }
  case V_SgBoolValExp: {
	SgBoolValExp* boolValExp=isSgBoolValExp(node);
	assert(boolValExp);
	int boolVal= boolValExp->get_value();
	if(boolVal==0) {
	  tmp.result=false;
	  return tmp;
	}
	if(boolVal==1) {
	  tmp.result=true;
	  return tmp;
	}
	break;
  }
  } // end of switch
  tmp.result=AType::Top();
  return tmp;
}

//////////////////////////////////////////////////////////////////////
// EVAL CONSTINT
//////////////////////////////////////////////////////////////////////
SingleEvalResultConstInt ExprAnalyzer::evalConstInt(SgNode* node,EState eState) {
  assert(eState.state); // ensure state exists
  SingleEvalResultConstInt res;
  // initialize with default values from argument(s)
  res.eState=eState;
  res.result=AType::ConstIntLattice(AType::Bot());
  if(dynamic_cast<SgBinaryOp*>(node)) {
	SgNode* lhs=SgNodeHelper::getLhs(node);
	SgNode* rhs=SgNodeHelper::getRhs(node);
	SingleEvalResultConstInt lhsResult=evalConstInt(lhs,eState);
	SingleEvalResultConstInt rhsResult=evalConstInt(rhs,eState);
	// union constraintSet from lhs and rhs

	switch(node->variantT()) {
	case V_SgEqualityOp: {
	  res.result=(lhsResult.result==rhsResult.result);
	  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
	  // record new constraint
	  VariableId varId;
	  if((variable(lhs,varId) && rhsResult.isConstInt()) || (lhsResult.isConstInt() && variable(rhs,varId))) {
		// only add the equality constraint if no constant is bound to the respective variable
		if(!res.eState.state->varIsConst(varId)) {
		  res.exprConstraints.insert(Constraint(Constraint::EQ_VAR_CONST,varId,rhsResult.intValue()));
		}
	  }
	  return res;
	}
	case V_SgNotEqualOp: {
	  res.result=(lhsResult.result!=rhsResult.result);
	  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
	  // record new constraint
	  VariableId varId;
	  if((variable(lhs,varId) && rhsResult.isConstInt()) || (lhsResult.isConstInt() && variable(rhs,varId))) {
		// only add the inequality constraint if no constant is bound to the respective variable
		if(!res.eState.state->varIsConst(varId)) {
		  res.exprConstraints.insert(Constraint(Constraint::NEQ_VAR_CONST,varId,rhsResult.intValue()));
		}
	  }
	  return res;
	}
	case V_SgAndOp:
	  res.result=(lhsResult.result&&rhsResult.result);
	  // we encode CPP-AND semantics here!
	  if(res.result.isTrue()||res.result.isTop())
		res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
	  else {
		// do not up-proagate collected constraints (exprConstraints remains empty) because (lhs AND rhs)=false
	  }
	  return res;
	case V_SgOrOp:
	  res.result=(lhsResult.result||rhsResult.result);
	  // we encode CPP-OR semantics here!
	  if(lhsResult.isTrue()||lhsResult.isTop())
		res.exprConstraints=lhsResult.exprConstraints; // we do not propagte rhs constraints (because of short-circuit evaluation semantics)
	  else if(rhsResult.isTrue()||rhsResult.isTop()) {
		res.exprConstraints=rhsResult.exprConstraints; // we do not propagte lhs constraints (because of short-circuit evaluation semantics and lhs is FALSE)
	  } else {
		// do not up-proagate collected constraints (exprConstraints remains empty) because (lhs OR rhs)=false
	  }
	  return res;
	default:
	  throw "Error: evalConstInt::binary operation failed.";
	}
  }
  if(dynamic_cast<SgUnaryOp*>(node)) {
	SgNode* child=SgNodeHelper::getFirstChild(node);
	SingleEvalResultConstInt operandResult=evalConstInt(child,eState);
	switch(node->variantT()) {
	case V_SgNotOp:
	  res.result=!operandResult.result;
	  // we propagate the inverted constraints
	  res.exprConstraints=operandResult.exprConstraints.invertedConstraints();
	  return res;
	case V_SgCastExp: {
	  // no constraint handling necessary, as all constraints get computed and propagated for child
	  SgCastExp* castExp=isSgCastExp(node);
	  // TODO: model effect of cast when sub language is extended
	  res.exprConstraints=operandResult.exprConstraints;
	  return evalConstInt(SgNodeHelper::getFirstChild(castExp),eState);
	}
	case V_SgMinusOp: {
	  res.result=-operandResult.result; // using overloaded operator
	  res.exprConstraints=operandResult.exprConstraints;
	  return res;
	}
	default:
	  cerr << "@NODE:"<<node->sage_class_name()<<endl;
	  throw "Error: evalConstInt::unary operation failed.";
	} // end switch
  }
  assert(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));

  // ALL REMAINING CASES DO NOT GENERATE CONSTRAINTS
  switch(node->variantT()) {
  case V_SgBoolValExp: {
	SgBoolValExp* boolValExp=isSgBoolValExp(node);
	assert(boolValExp);
	int boolVal= boolValExp->get_value();
	if(boolVal==0) {
	  res.result=false;
	  return res;
	}
	if(boolVal==1) {
	  res.result=true;
	  return res;
	}
	break;
  }
  case V_SgIntVal: {
	SgIntVal* intValNode=isSgIntVal(node);
	int intVal=intValNode->get_value();
	res.result=intVal;
	return res;
  }
  case V_SgVarRefExp: {
	VariableId varId;
	bool isVar=ExprAnalyzer::variable(node,varId);
	assert(isVar);
	const State* state=eState.state;
	if(state->varExists(varId)) {
	  State state2=*state; // also removes constness
	  int tmpres=state2[varId]; // MS: TODO: replace int in state with ConstIntLattice
	  if(tmpres==ANALYZER_INT_TOP)
		res.result=AType::Top();
	  else if(tmpres==ANALYZER_INT_BOT)
		res.result=AType::Bot();
	  else
		res.result=tmpres;
		
	  if(res.result.isTop()) {
		// in case of TOP we try to extract a possibly more precise value from the constraints
		AType::ConstIntLattice val=res.eState.constraints.varConstIntLatticeValue(varId);
		//if(!val.isTop())
		//  cout << "DEBUG: extracing more precise value from constraints: "<<res.result.toString()<<" ==> "<<val.toString()<<endl;
		res.result=val;
	  }
	  return res;
	} else {
	  res.result=AType::Top();
	  cerr << "WARNING: variable not in State (var="<<varId.longVariableName()<<"). Initialized with top."<<endl;
	  return res;
	}
	break;
  }
  default:
	cerr << "@NODE:"<<node->sage_class_name()<<endl;
	throw "Error: evalConstInt::unknown operation failed.";
  } // end of switch
  throw "Error: evalConstInt failed.";
}
