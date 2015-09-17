/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 ****X*********************************************************/

#include "sage3basic.h"
#include "ExprAnalyzer.h"

using namespace CodeThorn;
using namespace SPRAY;

ExprAnalyzer::ExprAnalyzer():_variableIdMapping(0),_skipSelectedFunctionCalls(false),_skipArrayAccesses(false){
}

void ExprAnalyzer::setSkipSelectedFunctionCalls(bool skip) {
  _skipSelectedFunctionCalls=skip;
}

bool ExprAnalyzer::getSkipSelectedFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

void ExprAnalyzer::setSkipArrayAccesses(bool skip) {
  _skipArrayAccesses=skip;
}

bool ExprAnalyzer::getSkipArrayAccesses() {
  return _skipArrayAccesses;
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
  if(SgNodeHelper::isArrayAccess(node)) {
    // 1) array variable id
    // 2) eval array-index expr
    // 3) if const then compute variable id otherwise return non-valid var id (would require set)
#if 0
    VariableId arrayVarId;
    SgExpression* arrayIndexExpr=0;
    int arrayIndexInt=-1;
    //cout<<"DEBUG: ARRAY-ACCESS"<<astTermWithNullValuesToString(node)<<endl;
    if(false) {
      varId=_variableIdMapping->variableIdOfArrayElement(arrayVarId,arrayIndexInt);
    }
#endif
    return false;
  }
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    assert(_variableIdMapping);
#if 0
    SgSymbol* sym=varref->get_symbol();
    assert(sym);
    varId=_variableIdMapping->variableId(sym);
#else
    // MS: to investigate: even with the new var-sym-only case this does not work
    // MS: investigage getSymbolOfVariable
    // MS: 9/6/2014: this seems to work now
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
  //cout<<"DEBUG: evalConstInt: "<<node->unparseToString()<<astTermWithNullValuesToString(node)<<endl;
  // guard: for floating-point expression: return immediately with most general result
  // TODO: refine to walk the tree, when assignments are allowed in sub-expressions
  // MS: 2014-06-27: this cannot run in parallel because exp->get_type() seg-faults 
#if 0
  if(SgExpression* exp=isSgExpression(node)) {
    bool isFloatingPointType;
    // ROSE workaround. get_type cannot be run in parallel
    #pragma omp critical
    {
      isFloatingPointType=SgNodeHelper::isFloatingPointType(exp->get_type());
    }
    if(isFloatingPointType) {
      res.estate=estate;
      res.result=AType::ConstIntLattice(AType::Top());
      return listify(res);
    }
  }
#endif
  // initialize with default values from argument(s)
  res.estate=estate;
  res.result=AType::ConstIntLattice(AType::Bot());

  if(SgNodeHelper::isPostfixIncDecOp(node)) {
    cout << "Error: incdec-op not supported in conditions yet."<<endl;
    exit(1);
  }
  if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
    list<SingleEvalResultConstInt> resultList;
    SgExpression* cond=condExp->get_conditional_exp();
    list<SingleEvalResultConstInt> condResultList=evalConstInt(cond,estate,useConstraints,safeConstraintPropagation);
    if(condResultList.size()==0) {
      cerr<<"Error: evaluating condition of conditional operator inside expressions gives no result."<<endl;
      exit(1);
    }
    if(condResultList.size()==2) {
      list<SingleEvalResultConstInt>::iterator i=condResultList.begin();
      SingleEvalResultConstInt singleResult1=*i;
      ++i;
      SingleEvalResultConstInt singleResult2=*i;
      if((singleResult1.value()==singleResult2.value()).isTrue()) {
        cout<<"Info: evaluating condition of conditional operator gives two equal results"<<endl;
      }
    }
    if(condResultList.size()>1) {
      cerr<<"Error: evaluating condition of conditional operator gives more than one result. Not supported yet."<<endl;
      exit(1);
    }
    SingleEvalResultConstInt singleResult=*condResultList.begin();
    if(singleResult.result.isTop()) {
      SgExpression* trueBranch=condExp->get_true_exp();
      list<SingleEvalResultConstInt> trueBranchResultList=evalConstInt(trueBranch,estate,useConstraints,safeConstraintPropagation);
      SgExpression* falseBranch=condExp->get_false_exp();
      list<SingleEvalResultConstInt> falseBranchResultList=evalConstInt(falseBranch,estate,useConstraints,safeConstraintPropagation);
      // append falseBranchResultList to trueBranchResultList (moves elements), O(1).
      trueBranchResultList.splice(trueBranchResultList.end(), falseBranchResultList); 
      return trueBranchResultList;
    }
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
    // dummy return value to avoid compiler warning
    cerr<<"Error: evaluating conditional operator inside expressions - unknown behavior (condition may have evaluated to bot)."<<endl;
    exit(1);
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
          //cout<<"DEBUG: ARRAY-ACCESS2: ARR"<<node->unparseToString()<<"Index:"<<rhsResult.value()<<"skip:"<<getSkipArrayAccesses()<<endl;
          if(rhsResult.value().isTop()||getSkipArrayAccesses()==true) {
            // set result to top when index is top
            res.result=AType::Top();
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
            resultList.push_back(res);
            break;
          } else {
            if(SgVarRefExp* varRefExp=isSgVarRefExp(lhs)) {
              const PState* pstate=estate.pstate();
              PState pstate2=*pstate; // also removes constness
              VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
              // two cases
              if(_variableIdMapping->hasArrayType(arrayVarId)) {
                // has already correct id
                // nothing to do
              } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
                // in case it is a pointer retrieve pointer value
                //cout<<"DEBUG: pointer-array access!"<<endl;
                if(pstate->varExists(arrayVarId)) {
                  AValue aValuePtr=pstate2[arrayVarId].getValue();
                  // convert integer to VariableId
                  // TODO (topify mode: does read this as integer)
                  if(!aValuePtr.isConstInt()) {
                    res.result=AType::Top();
                    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
                    resultList.push_back(res);
                    return resultList;
                  }
                  int aValueInt=aValuePtr.getIntValue();
                  // change arrayVarId to refered array!
                  //cout<<"DEBUG: defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
                  arrayVarId=_variableIdMapping->variableIdFromCode(aValueInt);
                  //cout<<" to "<<_variableIdMapping->variableName(arrayVarId)<<endl;//DEBUG
                } else {
                  cerr<<"Error: pointer variable does not exist in PState."<<endl;
                  exit(1);
                }
              } else {
                cerr<<"Error: unkown type of array or pointer."<<endl;
                exit(1);
              }
              VariableId arrayElementId;
              AValue aValue=rhsResult.value();
              int index=-1;
              if(aValue.isConstInt()) {
                index=aValue.getIntValue();
                arrayElementId=_variableIdMapping->variableIdOfArrayElement(arrayVarId,index);
                //cout<<"DEBUG: arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
              } else {
                cerr<<"Error: array index cannot be evaluated to a constant. Not supported yet."<<endl;
                cerr<<"expr: "<<varRefExp->unparseToString()<<endl;
                exit(1);
              }
              ROSE_ASSERT(arrayElementId.isValid());
              // read value of variable var id (same as for VarRefExp - TODO: reuse)
              // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
              if(pstate->varExists(arrayElementId)) {
                res.result=pstate2[arrayElementId].getValue();
                //cout<<"DEBUG: retrieved array element value:"<<res.result<<endl;
                if(res.result.isTop() && useConstraints) {
                  AType::ConstIntLattice val=res.estate.constraints()->varConstIntLatticeValue(arrayElementId);
                  res.result=val;
                }
                return listify(res);
              } else {
                // check that array is constant array (it is therefore ok that it is not in the state)
                if(_variableIdMapping->isConstantArray(arrayVarId)) {
                  SgExpressionPtrList& initList=_variableIdMapping->getInitializerListOfArrayVariable(arrayVarId);
                  int elemIndex=0;
                  // TODO: slow linear lookup (TODO: pre-compute all values and provide access function)
                  for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
                    //VariableId arrayElemId=_variableIdMapping->variableIdOfArrayElement(initDeclVarId,elemIndex);
                    SgExpression* exp=*i;
                    SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
                    SgIntVal* intValNode=0;
                    if(assignInit && (intValNode=isSgIntVal(assignInit->get_operand_i()))) {
                      int intVal=intValNode->get_value();
                      //cout<<"DEBUG:initializing array element:"<<arrayElemId.toString()<<"="<<intVal<<endl;
                      //newPState.setVariableToValue(arrayElemId,CodeThorn::CppCapsuleAValue(AType::ConstIntLattice(intVal)));
                      if(elemIndex==index) {
                        AType::ConstIntLattice val=AType::ConstIntLattice(intVal);
                        res.result=val;
                        return listify(res);
                      }
                    } else {
                      cerr<<"Error: unsupported array initializer value:"<<exp->unparseToString()<<" AST:"<<SPRAY::AstTerm::astTermWithNullValuesToString(exp)<<endl;
                      exit(1);
                    }
                    elemIndex++;
                  }
                  cerr<<"Error: access to element of constant array (not in state). Not supported yet."<<endl;
                  exit(1);
                } else {
                  cerr<<"Error: Array Element does not exist (out of array access?)"<<endl;
                  cerr<<"array-element-id: "<<arrayElementId.toString()<<" name:"<<_variableIdMapping->variableName(arrayElementId)<<endl;
                  cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
                  cerr<<"AST: "<<node->unparseToString()<<endl;
                  exit(1);
                }
              }
            } else {
              cerr<<"Error: array-access uses expr for denoting the array. Not supported yet."<<endl;
              cerr<<"expr: "<<lhs->unparseToString()<<endl;
              cerr<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
              exit(1);
            }
          }
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
  
  ROSE_ASSERT(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));
  
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
    //cout<<"DEBUG: EvalConstInt: V_SgVarRefExp: isVar:"<<isVar<<", varIdcode:"<<varId.getIdCode()<<"Source:"<<node->unparseToString()<<endl;
    assert(isVar);
    const PState* pstate=estate.pstate();
    if(pstate->varExists(varId)) {
      PState pstate2=*pstate; // also removes constness

      if(_variableIdMapping->hasArrayType(varId)) {
        // CODE-POINT-1
        // for arrays (by default the address is used) return its pointer value (the var-id-code)
        res.result=AType::ConstIntLattice(varId.getIdCode());
      } else {
        res.result=pstate2[varId].getValue(); // this include assignment of pointer values
      }
      if(res.result.isTop() && useConstraints) {
        // in case of TOP we try to extract a possibly more precise value from the constraints
        AType::ConstIntLattice val=res.estate.constraints()->varConstIntLatticeValue(varId);
        // TODO: TOPIFY-MODE: most efficient here
        res.result=val;
      }
      return listify(res);
    } else {
      if(_variableIdMapping->isConstantArray(varId) && boolOptions["rersmode"]) {
        res.result=AType::ConstIntLattice(varId.getIdCode());
        return listify(res);
      } else {
        res.result=AType::Top();
        cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueLongVariableName(varId)<<"). Initialized with top."<<endl;
        return listify(res);
      }
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
