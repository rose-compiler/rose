/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 ****X*********************************************************/

#include "sage3basic.h"
#include "ExprAnalyzer.h"

using namespace CodeThorn;

ExprAnalyzer::ExprAnalyzer():_variableIdMapping(0),_skipSelectedFunctionCalls(false){
}

void ExprAnalyzer::setSkipSelectedFunctionCalls(bool skip) {
  _skipSelectedFunctionCalls=skip;
}

bool ExprAnalyzer::getSkipSelectedFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

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
  assert(node);
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    assert(_variableIdMapping);
#if 1
    SgSymbol* sym=varref->get_symbol();
    assert(sym);
    varId=_variableIdMapping->variableId(sym);
#else
    // MS: to investigate: even with the new var-sym-only case this does not work
    // MS: investigage getSymbolOfVariable
    varId=_variableIdMapping->variableId(varref);
#endif
    return true;
  } else {
    VariableId defaultVarId;
    varId=defaultVarId;
    return false;
  }
}

//////////////////////////////////////////////////////////////////////
// EVAL CONSTINT
//////////////////////////////////////////////////////////////////////
list<SingleEvalResultConstInt> listify(SingleEvalResultConstInt res) {
  list<SingleEvalResultConstInt> resList;
  resList.push_back(res);
  return resList;
}


list<SingleEvalResultConstInt> ExprAnalyzer::evalConstInt(SgNode* node,EState estate, bool useConstraints, bool safeConstraintPropagation) {
  assert(estate.pstate()); // ensure state exists
  SingleEvalResultConstInt res;
  // initialize with default values from argument(s)
  res.estate=estate;
  res.result=AType::ConstIntLattice(AType::Bot());
  if(SgNodeHelper::isPostfixIncDecOp(node)) {
    cout << "INFO: incdec-op found!"<<endl;
  }
  if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
	list<SingleEvalResultConstInt> resultList;
	SgExpression* cond=condExp->get_conditional_exp();
	list<SingleEvalResultConstInt> condResultList=evalConstInt(cond,estate,useConstraints,safeConstraintPropagation);
	if(condResultList.size()!=1) {
	  cerr<<"Error: non-const condition in conditional operator inside expressions not supported yet."<<endl;
	  exit(1);
	}
	SingleEvalResultConstInt singleResult=*condResultList.begin();
	if(singleResult.result.isTrue()) {
	  SgExpression* trueBranch=condExp->get_true_exp();
	  list<SingleEvalResultConstInt> trueBranchResultList=evalConstInt(trueBranch,estate,useConstraints,safeConstraintPropagation);
	  return trueBranchResultList;
	}
	if(singleResult.result.isFalse()) {
	  SgExpression* falseBranch=condExp->get_false_exp();
	  list<SingleEvalResultConstInt> falseBranchResultList=evalConstInt(falseBranch,estate,useConstraints,safeConstraintPropagation);
	  return falseBranchResultList;
	}
	cerr<<"Error: condition evaluating to top in conditional operator inside expressions not supported yet."<<endl;
	exit(1);
	// dummy return value to avoid compiler warning
	return resultList;
  }
  if(dynamic_cast<SgBinaryOp*>(node)) {
    //cout << "BinaryOp:"<<SgNodeHelper::nodeToString(node)<<endl;

    SgNode* lhs=SgNodeHelper::getLhs(node);
    list<SingleEvalResultConstInt> lhsResultList=evalConstInt(lhs,estate,useConstraints,safeConstraintPropagation);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    list<SingleEvalResultConstInt> rhsResultList=evalConstInt(rhs,estate,useConstraints,safeConstraintPropagation);
    //assert(lhsResultList.size()==1);
    //assert(rhsResultList.size()==1);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
        liter!=lhsResultList.end();
        ++liter) {
      for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
          riter!=rhsResultList.end();
          ++riter) {
        SingleEvalResultConstInt lhsResult=*liter;
        SingleEvalResultConstInt rhsResult=*riter;

        switch(node->variantT()) {
        case V_SgEqualityOp: {
          res.result=(lhsResult.result==rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          // record new constraint
          VariableId varId;
          if(variable(lhs,varId) && rhsResult.isConstInt()) {
            // if var is top add two states with opposing constraints
            if(!res.estate.pstate()->varIsConst(varId)) {
              SingleEvalResultConstInt tmpres1=res;
              SingleEvalResultConstInt tmpres2=res;
              tmpres1.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,rhsResult.value()));
              tmpres1.result=true;
              tmpres2.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,rhsResult.value()));
              tmpres2.result=false;
              resultList.push_back(tmpres1);
              resultList.push_back(tmpres2);
              //return resultList; MS: removed 3/11/2014
              break;
            }
          }
          if(lhsResult.isConstInt() && variable(rhs,varId)) {
            // only add the equality constraint if no constant is bound to the respective variable
            if(!res.estate.pstate()->varIsConst(varId)) {
              SingleEvalResultConstInt tmpres1=res;
              SingleEvalResultConstInt tmpres2=res;
              tmpres1.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,lhsResult.value()));
              tmpres1.result=true;
              tmpres2.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,lhsResult.value()));
              tmpres2.result=false;
              resultList.push_back(tmpres1);
              resultList.push_back(tmpres2);
              break;
            }
          }
          resultList.push_back(res);
          break;
        }
        case V_SgNotEqualOp: {
          res.result=(lhsResult.result!=rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          // record new constraint
          VariableId varId;
          if(variable(lhs,varId) && rhsResult.isConstInt()) {
            // only add the equality constraint if no constant is bound to the respective variable
            if(!res.estate.pstate()->varIsConst(varId)) {
              SingleEvalResultConstInt tmpres1=res;
              SingleEvalResultConstInt tmpres2=res;
              tmpres1.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,rhsResult.value()));
              tmpres1.result=true;
              tmpres2.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,rhsResult.value()));
              tmpres2.result=false;
              resultList.push_back(tmpres1);
              resultList.push_back(tmpres2);
              break;
            }
          }
          if(lhsResult.isConstInt() && variable(rhs,varId)) {
            // only add the equality constraint if no constant is bound to the respective variable
            if(!res.estate.pstate()->varIsConst(varId)) {
              SingleEvalResultConstInt tmpres1=res;
              SingleEvalResultConstInt tmpres2=res;
              tmpres1.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,lhsResult.value()));
              tmpres1.result=true;
              tmpres2.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,lhsResult.value()));
              tmpres2.result=false;
              resultList.push_back(tmpres1);
              resultList.push_back(tmpres2);
              break;
            }
          }
          resultList.push_back(res);
          break;
        }
        case V_SgAndOp: {
          //cout << "SgAndOp: "<<lhsResult.result.toString()<<"&&"<<rhsResult.result.toString()<<" ==> ";
          res.result=(lhsResult.result&&rhsResult.result);
          //cout << res.result.toString()<<endl;
#if 0
          cout << lhsResult.exprConstraints.toString();
          cout << "&&";
          cout << rhsResult.exprConstraints.toString();
          cout << " == > ";
#endif
          if(lhsResult.result.isFalse()) {
            res.exprConstraints=lhsResult.exprConstraints;
            // rhs is not considered due to short-circuit AND semantics
          }
          if(lhsResult.result.isTrue() && rhsResult.result.isFalse()) {
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
            // nothing to do
          }
          if(lhsResult.result.isTrue() && rhsResult.result.isTrue()) {
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          }
          
          // in case of top we do not propagate constraints
          if(lhsResult.result.isTop() && !safeConstraintPropagation) {
            res.exprConstraints+=lhsResult.exprConstraints;
          }
          if(rhsResult.result.isTop() && !safeConstraintPropagation) {
            res.exprConstraints+=rhsResult.exprConstraints;
          }
          resultList.push_back(res);
          //          cout << res.exprConstraints.toString();
          //cout << endl;
          break;
        }
        case V_SgOrOp: {
          res.result=(lhsResult.result||rhsResult.result);
          // we encode short-circuit CPP-OR semantics here!
          if(lhsResult.result.isTrue()) {
            res.result=lhsResult.result;
            res.exprConstraints=lhsResult.exprConstraints;
          }
          if(lhsResult.result.isFalse() && rhsResult.result.isFalse()) {
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          }
          if(lhsResult.result.isFalse() && rhsResult.result.isTrue()) {
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          }
          // in case of top we do not propagate constraints
          if(lhsResult.result.isTop() && !safeConstraintPropagation) {
            res.exprConstraints+=lhsResult.exprConstraints;
          }
          if(rhsResult.result.isTop() && !safeConstraintPropagation) {
            res.exprConstraints+=rhsResult.exprConstraints;
          }
          resultList.push_back(res);
          break;
        }
        case V_SgAddOp: {
          res.result=(lhsResult.result+rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgSubtractOp: {
          res.result=(lhsResult.result-rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgMultiplyOp: {
          res.result=(lhsResult.result*rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgDivideOp: {
          res.result=(lhsResult.result/rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgModOp: {
          res.result=(lhsResult.result%rhsResult.result);
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgGreaterOrEqualOp: {
          res.result=(lhsResult.result>=rhsResult.result);
          if(boolOptions["relop-constraints"]) {
            if(res.result.isTop())
              throw "Error: Top found in relational operator (not supported yet).";
          }
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgGreaterThanOp: {
          res.result=(lhsResult.result>rhsResult.result);
          if(boolOptions["relop-constraints"]) {
            if(res.result.isTop())
              throw "Error: Top found in relational operator (not supported yet).";
          }
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgLessThanOp: {
          res.result=(lhsResult.result<rhsResult.result);
          if(boolOptions["relop-constraints"]) {
            if(res.result.isTop())
              throw "Error: Top found in relational operator (not supported yet).";
          }
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
        case V_SgLessOrEqualOp: {
          res.result=(lhsResult.result<=rhsResult.result);
          if(boolOptions["relop-constraints"]) {
            if(res.result.isTop())
              throw "Error: Top found in relational operator (not supported yet).";
          }
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
          break;
        }
		case V_SgPntrArrRefExp: {
		  // assume top for array elements (array elements are not stored in state)
		  res.result=AType::Top();
          res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
          resultList.push_back(res);
		  break;
		}
        default:
			cerr << "Binary Op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
          throw "Error: evalConstInt::unkown binary operation.";
        }
      }
    }
    return resultList;
  }
  
  if(dynamic_cast<SgUnaryOp*>(node)) {
    SgNode* child=SgNodeHelper::getFirstChild(node);
    list<SingleEvalResultConstInt> operandResultList=evalConstInt(child,estate,useConstraints,safeConstraintPropagation);
    //assert(operandResultList.size()==1);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator oiter=operandResultList.begin();
        oiter!=operandResultList.end();
        ++oiter) {
      SingleEvalResultConstInt operandResult=*oiter;
      switch(node->variantT()) {
      case V_SgNotOp:
        res.result=!operandResult.result;
        // we do NOT invert the constraints, instead we negate the operand result (TODO: investigate)
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
      break;
      case V_SgCastExp: {
        // TODO: model effect of cast when sub language is extended
        //SgCastExp* castExp=isSgCastExp(node);
        res.result=operandResult.result;
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
        break;
      }
        // unary minus
      case V_SgMinusOp: {
        res.result=-operandResult.result; // using overloaded operator
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
        break;
      }
      default:
        cerr << "@NODE:"<<node->sage_class_name()<<endl;
        throw "Error: evalConstInt::unknown unary operation.";
      } // end switch
    }
    return  resultList;
  }
  
  assert(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));
  
  // ALL REMAINING CASES DO NOT GENERATE CONSTRAINTS
  // EXPRESSION LEAF NODES
  switch(node->variantT()) {
  case V_SgBoolValExp: {
    SgBoolValExp* boolValExp=isSgBoolValExp(node);
    assert(boolValExp);
    int boolVal= boolValExp->get_value();
    if(boolVal==0) {
      res.result=false;
      return listify(res);
    }
    if(boolVal==1) {
      res.result=true;
      return listify(res);
    }
    break;
  }
  case V_SgDoubleVal: {
    //SgDoubleVal* doubleValNode=isSgDoubleVal(node);
	// floating point values are currently not computed
	res.result=AType::Top();
    return listify(res);
  }
  case V_SgIntVal: {
    SgIntVal* intValNode=isSgIntVal(node);
    int intVal=intValNode->get_value();
    res.result=intVal;
    return listify(res);
  }
  case V_SgVarRefExp: {
    VariableId varId;
    bool isVar=ExprAnalyzer::variable(node,varId);
    assert(isVar);
    const PState* pstate=estate.pstate();
    if(pstate->varExists(varId)) {
      PState pstate2=*pstate; // also removes constness
      res.result=pstate2[varId].getValue();
      if(res.result.isTop() && useConstraints) {
        // in case of TOP we try to extract a possibly more precise value from the constraints
        AType::ConstIntLattice val=res.estate.constraints()->varConstIntLatticeValue(varId);
        //if(!val.isTop())
        // TODO: we will want to monitor this for statistics!
        //  cout << "DEBUG: extracing more precise value from constraints: "<<res.result.toString()<<" ==> "<<val.toString()<<endl;
        res.result=val;
      }
      return listify(res);
    } else {
      res.result=AType::Top();
      //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueLongVariableName(varId)<<"). Initialized with top."<<endl;
      return listify(res);
    }
    break;
  }
  case V_SgFunctionCallExp: {
    if(getSkipSelectedFunctionCalls()) {
      // return default value
      return listify(res);
    } else {
      throw "Error: evalConstInt::function call inside expression.";
    }

  }

  default:
    cerr << "@NODE:"<<node->sage_class_name()<<endl;
    throw "Error: evalConstInt::unknown operation failed.";
  } // end of switch
  throw "Error: evalConstInt failed.";
}

/////////////////////////////////////////////////////////////////////////////////////////////////
