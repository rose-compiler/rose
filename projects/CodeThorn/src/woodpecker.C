// Author: Markus Schordan, 2013.

#include "rose.h"

#include "inliner.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "CommandLineOptions.h"
#include "AnalysisAbstractionLayer.h"
#include "AType.h"
#include "SgNodeHelper.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;

#include "ReachabilityResults.h"

static VariableIdSet variablesOfInterest;
static bool detailedOutput=0;
const char* csvAssertFileName=0;
const char* csvConstResultFileName=0;
ReachabilityResults reachabilityResults;

bool isVariableOfInterest(VariableId varId) {
  return variablesOfInterest.find(varId)!=variablesOfInterest.end();
}

bool trivialInline(SgFunctionCallExp* funCall) {
  /*
    0) check if it is a trivial function call (no return value, no params)
    1) find function to inline
    2) determine body of function to inline
    3) delete function call
    4) clone body of function to inline
    5) insert cloned body as block
  */
  string fname=SgNodeHelper::getFunctionName(funCall);
  SgFunctionDefinition* functionDef=isSgFunctionDefinition(SgNodeHelper::determineFunctionDefinition(funCall));
  if(!functionDef)
    return false;
  SgBasicBlock* functionBody=isSgBasicBlock(functionDef->get_body());
  if(!functionBody)
    return false;
  SgTreeCopy tc;
  SgBasicBlock* functionBodyClone=isSgBasicBlock(functionBody->copy(tc));
  // set current basic block as parent of body
  if(!functionBodyClone)
    return false;
  SgExprStatement* functionCallExprStmt=isSgExprStatement(funCall->get_parent());
  if(!functionCallExprStmt)
    return false;
  SgBasicBlock* functionCallBlock=isSgBasicBlock(functionCallExprStmt->get_parent());
  if(!functionCallBlock)
    return false;
  if(functionCallBlock->get_statements().size()>0) {
    SgStatement* oldStmt=functionCallExprStmt;
    SgStatement* newStmt=functionBodyClone;
    SageInterface::replaceStatement(oldStmt, newStmt,false);
    return true;
  }
  return false;
}

SgFunctionCallExp* isTrivialFunctionCall(SgNode* node) {
  if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
    SgExpressionPtrList& args=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(args.size()==0) {
      if(SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(funCall)) {
        SgType* returnType=SgNodeHelper::getFunctionReturnType(funDef);
        if(isSgTypeVoid(returnType)) {
          return funCall;
        }                
      }
    }
  }
  return 0;
}

list<SgFunctionCallExp*> trivialFunctionCalls(SgNode* node) {
  RoseAst ast(node);
  list<SgFunctionCallExp*> funCallList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgFunctionCallExp* funCall=isTrivialFunctionCall(*i)) {
      funCallList.push_back(funCall);
    }
  }
  return funCallList;
}

size_t numberOfFunctions(SgNode* node) {
  RoseAst ast(node);
  size_t num=0;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(isSgFunctionDefinition(*i))
      num++;
  }
  return num;
}

size_t inlineFunctionCalls(list<SgFunctionCallExp*>& funCallList) {
  size_t num=0;
  for(list<SgFunctionCallExp*>::iterator i=funCallList.begin();i!=funCallList.end();i++) {
    SgFunctionCallExp* funCall=*i;
	if(detailedOutput) cout<< "function call:"<<SgNodeHelper::nodeToString(*i)<<": ";
    bool success=trivialInline(funCall);
    if(success) {
      if(detailedOutput) cout<<"inlined."<<endl;
      num++;
    }
    else
      cout<<"not inlined."<<endl;
  }
  return num;
}

#include "FIConstAnalysis.C"
#include "DeadCodeEliminationOperators.C"

// returns the number of eliminated expressions
int eliminateDeadCodePhase1(SgNode* root,SgFunctionDefinition* mainFunctionRoot,
					   VariableIdMapping* variableIdMapping,
					   VariableConstInfo& vci) {
  RoseAst ast1(root);

  // eliminate variables with one value only
  // 1) eliminate declaration of variable
  // 2) eliminate assignment to variable
  // 3) replace use of variable
  cout<<"STATUS: Dead code elimination phase 1: Eliminating variables."<<endl;
  cout<<"STATUS: Collecting variables, assignments, and expressions."<<endl;
  list<SgVariableDeclaration*> toDeleteVarDecls;
  list<SgAssignOp*> toDeleteAssignments;
  list<pair<SgExpression*,SgExpression*> > toReplaceExpressions;
  for(RoseAst::iterator i=ast1.begin();i!=ast1.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      VariableId declVarId=variableIdMapping->variableId(varDecl);
      if(isVariableOfInterest(declVarId) && vci.isUniqueConst(declVarId)) {
        toDeleteVarDecls.push_back(varDecl);
      }
    }
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      VariableValuePair varValPair;
      bool found=analyzeAssignment(assignOp,*variableIdMapping, &varValPair);
      if(found) {
        VariableId varId=varValPair.varId;
        if(isVariableOfInterest(varId) && vci.isUniqueConst(varId)) {
          toDeleteAssignments.push_back(assignOp);
        }
      }
    }
    if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
      VariableId varId;
      bool found=determineVariable(varRef, varId, *variableIdMapping);
      if(found) {
        if(isVariableOfInterest(varId) && vci.isUniqueConst(varId)) {
          SgIntVal* newSgIntValExpr=SageBuilder::buildIntVal(vci.uniqueConst(varId));
          toReplaceExpressions.push_back(make_pair(varRef,newSgIntValExpr));
        }
      }
    }
  }
  
  cout<<"STATUS: eliminating declarations."<<endl;
  int elimVar=0;
  for(list<SgVariableDeclaration*>::iterator i=toDeleteVarDecls.begin();
      i!=toDeleteVarDecls.end();
      ++i) {
    elimVar++;
    if(detailedOutput) cout<<"Eliminating dead variable's declaration: "<<(*i)->unparseToString()<<endl;
    SageInterface::removeStatement(*i, false);
  }
  cout<<"STATUS: eliminating assignments."<<endl;
  int elimAssignment=0;
  for(list<SgAssignOp*>::iterator i=toDeleteAssignments.begin();
      i!=toDeleteAssignments.end();
      ++i) {
    SgExprStatement* exprStatAssign=isSgExprStatement(SgNodeHelper::getParent(*i));
    if(!exprStatAssign) {
      cerr<<"Error: assignments inside expressions are not supported yet.";
      cerr<<"Problematic assignment: "<<(*i)->unparseToString()<<endl;
      exit(1);
    }
    elimAssignment++;
    if(detailedOutput) cout<<"Eliminating dead variable assignment: "<<(*i)->unparseToString()<<endl;
    SageInterface::removeStatement(exprStatAssign, false);
  }
  
  cout<<"STATUS: eliminating expressions."<<endl;
  int elimVarUses=0;
  for(list<pair<SgExpression*,SgExpression*> >::iterator i=toReplaceExpressions.begin();
      i!=toReplaceExpressions.end();
      ++i) {
    elimVarUses++;
    if(detailedOutput) cout<<"Replacing use of variable with constant: "<<(*i).first->unparseToString()<<" replaced by "<<(*i).second->unparseToString()<<endl;
    SageInterface::replaceExpression((*i).first, (*i).second);
  }
  
  cout<<"STATUS: Eliminated "<<elimVar<<" variable declarations."<<endl;
  cout<<"STATUS: Eliminated "<<elimAssignment<<" variable assignments."<<endl;
  cout<<"STATUS: Replaced "<<elimVarUses<<" uses of variables with constant."<<endl;
  cout<<"STATUS: Eliminated "<<elimVar<<" dead variables."<<endl;
  cout<<"STATUS: Dead code elimination phase 1: finished."<<endl;
  return elimVar+elimAssignment+elimVarUses;
}

// returns the error_XX label number or -1
//  error_0 is a valid label number (therefore -1 is returned if no label is found)
int isIfWithLabeledAssert(SgNode* node) {
  if(isSgIfStmt(node)) {
    node=SgNodeHelper::getTrueBranch(node);
    RoseAst block(node);
    for(RoseAst::iterator i=block.begin();i!=block.end();++i) {
      SgNode* node2=*i;
      if(SgExprStatement* exp=isSgExprStatement(node2))
        node2=SgNodeHelper::getExprStmtChild(exp);
      if(isSgLabelStatement(node2)) {
        RoseAst::iterator next=i;
        next++;
        if(SgNodeHelper::Pattern::matchAssertExpr(*next)) {
          //		  cout<<"ASSERT FOUND with Label found:"<<endl;
          SgLabelStatement* labStmt=isSgLabelStatement(*i);
          assert(labStmt);
          string name=SgNodeHelper::getLabelName(labStmt);
          if(name=="globalError")
			name="error_60";
          name=name.substr(6,name.size()-6);
          std::istringstream ss(name);
          int num;
          ss>>num;
          return num;
        }
      }
    }
  }
  return -1;
}
// ===================== EVALUATION ========================

// to become a template/pattern
typedef ConstIntLattice EvalValueType;
EvalValueType evalSgBoolValExp(SgExpression* node) {
  EvalValueType res;
  SgBoolValExp* boolValExp=isSgBoolValExp(node);
  assert(boolValExp);
  int boolVal= boolValExp->get_value();
  if(boolVal==0) {
	res=false;
	return res;
  }
  if(boolVal==1) {
	res=true;
	return res;
  }
  cerr<<"Error: boolean value different to 0 and 1.";
  assert(0);
}

EvalValueType evalSgIntVal(SgExpression* node) {
  EvalValueType res;
  SgIntVal* intValNode=isSgIntVal(node);
  int intVal=intValNode->get_value();
  res=intVal;
  return res;
}

// uses global environment (for this prototype only)
// putting eval functions into an evaluator-class will model this properly
// globals (for this function only):
// global_xxx, global_variableIdMappingPtr
VariableIdMapping* global_variableIdMappingPtr=0;
VariableConstInfo* global_variableConstInfo=0;
bool global_option_multiconstanalysis=false;
EvalValueType evalSgVarRefExp(SgExpression* node) {
  assert(global_variableIdMappingPtr);
  assert(global_variableConstInfo);
  VariableId varId;
  bool isVar=determineVariable(node, varId, *global_variableIdMappingPtr);
  assert(isVar);
  // varId is now VariableId of VarRefExp
  if(global_variableConstInfo->isUniqueConst(varId)) {
	return ConstIntLattice(global_variableConstInfo->uniqueConst(varId));
  } else {
	return ConstIntLattice(AType::Top());
  }
}

bool isRelationalOperator(SgExpression* node) {
  switch(node->variantT()) {
  case V_SgEqualityOp:
  case V_SgNotEqualOp:
  case V_SgGreaterOrEqualOp:
  case V_SgGreaterThanOp:
  case V_SgLessThanOp:
  case V_SgLessOrEqualOp:
	return true;
  default: return false;
  }
}

EvalValueType evalSgAndOp(EvalValueType lhsResult,EvalValueType rhsResult) {
  EvalValueType res;
  // short-circuit CPP-AND semantics
  if(lhsResult.isFalse()) {
	res=lhsResult;
  } else {
	res=(lhsResult&&rhsResult);
  }
  return res;
}

EvalValueType evalSgOrOp(EvalValueType lhsResult,EvalValueType rhsResult) {
  EvalValueType res;
  // short-circuit CPP-OR semantics
  if(lhsResult.isTrue()) {
	res=lhsResult;
  } else {
	res=(lhsResult||rhsResult);
  }
  return res;
}

EvalValueType evalWithMultiConst(SgNode* op, SgVarRefExp* var, EvalValueType val) {
  assert(op);
  assert(var);

  assert(global_variableIdMappingPtr);
  assert(global_variableConstInfo);

  assert(!(val.isTop()||val.isBot()));

    EvalValueType res=AType::Top(); // default if no more precise result can be determined

  int constVal=val.getIntValue();
  VariableId varId;
  bool isVar=determineVariable(var, varId, *global_variableIdMappingPtr);
  assert(isVar);
  if(detailedOutput) cout<<"evalWithMultiConst:"<<op->unparseToString();
  bool myIsMultiConst=global_variableConstInfo->isMultiConst(varId);
  if(myIsMultiConst) {
	bool myIsInConstSet=global_variableConstInfo->isInConstSet(varId,constVal);
	int myMinConst=global_variableConstInfo->minConst(varId);
	int myMaxConst=global_variableConstInfo->maxConst(varId);
	if(detailedOutput) {
	  cout<<" isMC:"<<myIsMultiConst;
	  cout<<" isInConstSet:"<<myIsInConstSet;
	  cout<<" min:"<<myMinConst;
	  cout<<" max:"<<myMaxConst;
	}
	//cout<<endl;
	
	// it holds here: val *is* a multi const
	// handle all cases with 3-valued logic
	switch(op->variantT()) {
	case V_SgEqualityOp:
	  if(!myIsInConstSet) res=EvalValueType(false);
	  else res=AType::Top();
	  break;
	case V_SgNotEqualOp: 
	  if(!myIsInConstSet) res=EvalValueType(true);
	  else res=AType::Top();
	  break;
	case V_SgGreaterOrEqualOp:
	  if(myMaxConst>=constVal) res=EvalValueType(true);
	  else res=EvalValueType(false);
	  break;
	case V_SgGreaterThanOp:
	  if(myMaxConst>constVal) res=EvalValueType(true);
	  else res=EvalValueType(false);
	  break;
	case V_SgLessThanOp:
	  if(myMinConst<constVal) res=EvalValueType(true);
	  else res=EvalValueType(false);
	  break;
	case V_SgLessOrEqualOp:
	  if(myMinConst<=constVal) res=EvalValueType(true);
	  else res=EvalValueType(false);
	  break;
	default:
	  cerr<<"Error: evalWithMultiConst: unknown operator."<<endl;
	  assert(0);
	}
  } else {
	if(detailedOutput) cout<<" not MC.";
  }

  if(detailedOutput) cout<<" Result: "<<res.toString()<<endl;
  return res;
}

bool isConstVal(SgExpression* node) {
  return isSgBoolValExp(node)||isSgIntVal(node);
}

EvalValueType eval(SgExpression* node) {
  EvalValueType res;
  stringstream watch;

  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(node));
    assert(lhs);
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(node));
    assert(rhs);

	if(global_option_multiconstanalysis) {
	  // refinement for special cases handled by multi-const analysis
	  if(isRelationalOperator(node)) {
		EvalValueType res2;
		if(isSgVarRefExp(lhs) && isConstVal(rhs))
		  res2=evalWithMultiConst(node,isSgVarRefExp(lhs),eval(rhs));
		if(isConstVal(lhs) && isSgVarRefExp(rhs))
		  res2=evalWithMultiConst(node,isSgVarRefExp(rhs),eval(lhs));
		if(!res2.isTop()) {
		  // found a more precise result with multi-const analysis results
		  return res2;
		}
	  }
	  // otherwise we continue with all other cases
	}
    EvalValueType lhsResult=eval(lhs);
    EvalValueType rhsResult=eval(rhs);
    switch(node->variantT()) {
    case V_SgAndOp: res=evalSgAndOp(lhsResult,rhsResult);break;
    case V_SgOrOp : res=evalSgOrOp(lhsResult,rhsResult);break;
    case V_SgEqualityOp: res=(lhsResult==rhsResult);break;
    case V_SgNotEqualOp: res=(lhsResult!=rhsResult);break;
    case V_SgGreaterOrEqualOp: res=(lhsResult>=rhsResult);break;
    case V_SgGreaterThanOp: res=(lhsResult>rhsResult);break;
    case V_SgLessThanOp: res=(lhsResult<rhsResult);break;
    case V_SgLessOrEqualOp: res=(lhsResult<=rhsResult);break;

    case V_SgPntrArrRefExp: res=AType::Top();break;
    default:cerr<<"EvalValueType:unknown binary operator:"<<node->class_name()<<"::"<<node->unparseToString()<<endl; res=AType::Top();break;
    }
  } else if(dynamic_cast<SgUnaryOp*>(node)) {
    SgExpression* child=isSgExpression(SgNodeHelper::getFirstChild(node));
    assert(child);
    EvalValueType childVal=eval(child);
    switch(node->variantT()) {
    case V_SgNotOp: res=!childVal;break;
    case V_SgCastExp: res=childVal;break; // requires refinement for different types
    case V_SgMinusOp: res=-childVal; break;
    case V_SgPointerDerefExp: res=AType::Top();break;
    default:cerr<<"EvalValueType:unknown unary operator:"<<node->class_name()<<"::"<<node->unparseToString()<<endl; res=AType::Top();break;
    }
  } else {
    // ALL REMAINING CASES ARE EXPRESSION LEAF NODES
    switch(node->variantT()) {
    case V_SgBoolValExp: res=evalSgBoolValExp(node);break;
    case V_SgIntVal: res=evalSgIntVal(node);break;
    case V_SgVarRefExp: res=evalSgVarRefExp(node);break;
    default: cerr<<"EvalValueType:unknown operator:"<<node->class_name()<<"::"<<node->unparseToString()<<endl; res=AType::Top();break;
    }
  }
  return res;
}


int eliminateDeadCodePhase2(SgNode* root,SgFunctionDefinition* mainFunctionRoot,
                            VariableIdMapping* variableIdMapping,
                            VariableConstInfo& vci) {
  // temporary global var
  global_variableIdMappingPtr=variableIdMapping;
  global_variableConstInfo=&vci;

  RoseAst ast1(root);

  cout<<"STATUS: Dead code elimination phase 2: Eliminating sub expressions."<<endl;
  list<pair<SgExpression*,SgExpression*> > toReplaceExpressions;
  for(RoseAst::iterator i=ast1.begin();i!=ast1.end();++i) {
    // determine expressions of blocks/ifstatements
    SgExpression* exp=0;
    // we exclude ?-operator because this would eliminate all assert(0) expressions
    // NOTE: assert needs to be handled with exit(int) to allow such operations
    if(isSgIfStmt(*i)||isSgWhileStmt(*i)||isSgDoWhileStmt(*i)) {
      SgNode* node=SgNodeHelper::getCond(*i);
      if(isSgExprStatement(node)) {
        node=SgNodeHelper::getExprStmtChild(node);
      }
      //cout<<node->class_name()<<";";
      exp=isSgExpression(node);
      if(exp) {
        ConstIntLattice res=eval(exp);
        int assertCode=isIfWithLabeledAssert(*i);
        if(assertCode>=0) {
          if(res.isTrue()) {
            reachabilityResults.reachable(assertCode);
          }
          if(res.isFalse()) {
            reachabilityResults.nonReachable(assertCode);
          }
        }
        //cout<<"\nELIM:"<<res.toString()<<":"<<exp->unparseToString()<<endl;
      }
    }
  }
  cout<<"STATUS: Dead code elimination phase 2: finished."<<endl;
  return 0;
}

void printResult(VariableIdMapping& variableIdMapping, VarConstSetMap& map) {
  cout<<"Result:"<<endl;
  VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=map.begin();i!=map.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueShortVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
    set<CppCapsuleConstIntLattice> valueSet=(*i).second;
    stringstream setstr;
    setstr<<"{";
    for(set<CppCapsuleConstIntLattice>::iterator i=valueSet.begin();i!=valueSet.end();++i) {
      if(i!=valueSet.begin())
        setstr<<",";
      setstr<<(*i).getValue().toString();
    }
    setstr<<"}";
    cout<<variableName<<"="<<setstr.str()<<";";
#if 1
    cout<<"Range:"<<createVariableValueRangeInfo(varId,map).toString();
    cout<<" width: "<<createVariableValueRangeInfo(varId,map).width().toString();
	cout<<" top: "<<createVariableValueRangeInfo(varId,map).isTop();
	cout<<endl;
#endif
	cout<<" isAny:"<<vci.isAny(varId)
		<<" isUniqueConst:"<<vci.isUniqueConst(varId)
		<<" isMultiConst:"<<vci.isMultiConst(varId);
	if(vci.isUniqueConst(varId)||vci.isMultiConst(varId)) {
	  cout<<" width:"<<vci.width(varId);
	} else {
	  cout<<" width:unknown";
	}
	cout<<" Test34:"<<vci.isInConstSet(varId,34);
    cout<<endl;
  }
  cout<<"---------------------"<<endl;
}

/* format: varname, isAny, isUniqueconst, isMultiConst, width(>=1 or 0 or -1 (for any)), min, max, numBits, "{...}"
*/
void writeCvsConstResult(VariableIdMapping& variableIdMapping, VarConstSetMap& map, const char* filename) {
  ofstream myfile;
  myfile.open(filename);

  //  cout<<"Result:"<<endl;
  VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=map.begin();i!=map.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueShortVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
    myfile<<variableName;
	myfile<<",";
    myfile<<vci.isAny(varId);
	myfile<<",";
	myfile<<vci.isUniqueConst(varId);
	myfile<<",";
	myfile<<vci.isMultiConst(varId);
	myfile<<",";
    if(vci.isUniqueConst(varId)||vci.isMultiConst(varId)) {
	  myfile<<vci.minConst(varId);
	  myfile<<",";
	  myfile<<vci.maxConst(varId);
	  size_t mywidth=vci.width(varId);
	  assert(mywidth==(size_t)vci.maxConst(varId)-vci.minConst(varId)+1);
	  int mylog2=log2(mywidth);
	  // compute upper whole number
	  int bits=-1;
	  if(mywidth==pow(2,mylog2)) {
		if(mylog2==0)
		  bits=1;
		else
		  bits=mylog2;
	  } else {
		bits=mylog2+1;
	  }
	  assert(bits!=-1);
	  myfile<<",";
	  myfile<<bits;
    } else {
      myfile<<INT_MIN
			<<","
			<<INT_MAX
			<<","
			<<sizeof(int)*8;
    }
	myfile<<",";

	// TODO: print array size
	// myfile<<",";
	SgType* varType=variableIdMapping.getType(varId);
	if(isSgArrayType(varType))
	  myfile<<"CA_ARRAY";
	else if(isSgPointerType(varType))
	  myfile<<"CA_PTR";
	else if(isSgTypeInt(varType))
	  myfile<<"CA_INT";
	else
	  myfile<<"CA_UNKNOWN";
	myfile<<",";	
#if 1
    set<CppCapsuleConstIntLattice> valueSet=(*i).second;
    stringstream setstr;
    myfile<<"{";
    for(set<CppCapsuleConstIntLattice>::iterator i=valueSet.begin();i!=valueSet.end();++i) {
      if(i!=valueSet.begin())
        myfile<<",";
      myfile<<(*i).getValue().toString();
    }
    myfile<<"}";
#endif
    myfile<<endl;
  }
  myfile.close();
}

void printCodeStatistics(SgNode* root) {
  SgProject* project=isSgProject(root);
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(project);
  VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"Statistics:"<<endl;
  cout<<"Number of empty if-statements: "<<listOfEmptyIfStmts(root).size()<<endl;
  cout<<"Number of functions          : "<<SgNodeHelper::listOfFunctionDefinitions(project).size()<<endl;
  cout<<"Number of global variables   : "<<SgNodeHelper::listOfGlobalVars(project).size()<<endl;
  cout<<"Number of used variables     : "<<setOfUsedVars.size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"VariableIdMapping-size       : "<<variableIdMapping.getVariableIdSet().size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
}


int main(int argc, char* argv[]) {
  try {
	if(argc==1) {
	  cout << "Error: wrong command line options."<<endl;
	  exit(1);
	}
#if 0
	if(argc==3) {
	  csvAssertFileName=argv[2];
	  argc=2; // don't confuse ROSE command line
	  cout<< "INIT: CSV-output file: "<<csvAssertFileName<<endl;
	}
#endif
  // Command line option handling.
	namespace po = boost::program_options;
  po::options_description desc
    ("Woodpecker V0.1\n"
     "Written by Markus Schordan\n"
     "Supported options");
  
  desc.add_options()
    ("help,h", "produce this help message.")
    ("rose-help", "show help for compiler frontend options.")
    ("version,v", "display the version.")
    ("stats", "display code statistics.")
    ("inline",po::value< string >(), "perform inlining ([yes]|no).")
    ("eliminate-empty-if",po::value< string >(), "eliminate if-statements with empty branches in main function ([yes]/no).")
    ("eliminate-dead-code",po::value< string >(), "eliminate dead code (variables and expressions) ([yes]|no).")
    ("csv-const-result",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
    ("generate-transformed-code",po::value< string >(), "generate transformed code with prefix rose_ ([yes]|no).")
    ("verbose",po::value< string >(), "print detailed output during analysis and transformation (yes|[no]).")
    ("csv-assert",po::value< string >(), "name of csv file with reachability assert results'")
	("enable-multi-const-analysis",po::value< string >(), "enable multi-const analysis.")
	;
  //    ("int-option",po::value< int >(),"option info")


  po::store(po::command_line_parser(argc, argv).
			options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
	cout << "woodpecker <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    return 0;
  }
  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  if (args.count("version")) {
    cout << "Woodpecker version 0.1\n";
    cout << "Written by Markus Schordan 2013\n";
    return 0;
  }
  if (args.count("csv-assert")) {
	csvAssertFileName=args["csv-assert"].as<string>().c_str();
  }
  if (args.count("csv-const-result")) {
	csvConstResultFileName=args["csv-const-result"].as<string>().c_str();
  }
  
  
  boolOptions.init(argc,argv);
  // temporary fake optinos
  boolOptions.registerOption("arith-top",false); // temporary
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary
  // regular options
  boolOptions.registerOption("inline",true);
  boolOptions.registerOption("eliminate-empty-if",true);
  boolOptions.registerOption("eliminate-dead-code",true);
  boolOptions.registerOption("generate-transformed-code",true);
  boolOptions.registerOption("enable-multi-const-analysis",false);
  boolOptions.registerOption("verbose",false);
  boolOptions.processOptions();

  if(boolOptions["verbose"])
	detailedOutput=1;

  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--csv-assert" 
        || string(argv[i]) == "--csv-const-result"
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
      assert(i+1<argc);
        argv[i+1] = strdup("");
    }
  }

  global_option_multiconstanalysis=boolOptions["enable-multi-const-analysis"];
#if 0
  if(global_option_multiconstanalysis) {
	cout<<"INFO: Using flow-insensitive multi-const-analysis."<<endl;
  } else {
	cout<<"INFO: Using flow-insensitive unique-const-analysis."<<endl;
  }
#endif

  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions

  if(args.count("stats")) {
    printCodeStatistics(root);
    exit(0);
  }

  SgFunctionDefinition* mainFunctionRoot=0;
  if(boolOptions["inline"]) {
    std::string funtofind="main";
    RoseAst completeast(root);
    mainFunctionRoot=completeast.findFunctionByName(funtofind);
    if(!mainFunctionRoot) {
      cerr<<"Error: No main function available. "<<endl;
      exit(1);
    } else {
      cout << "STATUS: Found main function."<<endl;
    }
    list<SgFunctionCallExp*> funCallList=trivialFunctionCalls(root);
    cout<<"STATUS: Inlining: Number of trivial function calls (with existing function bodies): "<<funCallList.size()<<endl;
    list<SgFunctionCallExp*> remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
    while(remainingFunCalls.size()>0) {
      size_t numFunCall=remainingFunCalls.size();
      cout<<"INFO: Remaining function calls in main function: "<<numFunCall<<endl;
      if(numFunCall>0)
        inlineFunctionCalls(remainingFunCalls);
      remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
    }
    cout<<"INFO: Remaining function calls in main function: 0"<<endl;
  } else {
    cout<<"INFO: Inlining: turned off."<<endl;
  }
  //TODO: create ICFG and compute non reachable functions (from main function)
  if(boolOptions["inline"]) {
    cout<<"STATUS: deleting inlined functions."<<endl;
    list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
    for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();i++) {
      string funName=SgNodeHelper::getFunctionName(*i);
      SgFunctionDeclaration* funDecl=(*i)->get_declaration();
      if(funName!="main") {
        if(detailedOutput) cout<<"Deleting function: "<<funName<<endl;
        SgStatement* stmt=funDecl;
        SageInterface::removeStatement (stmt, false);
        //SageInterface::deleteAST(funDef);
      }
    }
  }
  
  if(boolOptions["eliminate-empty-if"]) {
     cout<<"STATUS: Eliminating empty if-statements."<<endl;
    size_t num=0;
    size_t numTotal=num;
    do {
      num=eliminateEmptyIfStmts(root);
      cout<<"INFO: Number of if-statements eliminated: "<<num<<endl;
      numTotal+=num;
    } while(num>0);
    cout<<"STATUS: Total number of empty if-statements eliminated: "<<numTotal<<endl;
  }
  
  cout<<"STATUS: performing flow-insensitive const analysis."<<endl;
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(root);

  VarConstSetMap varConstSetMap;
  varConstSetMap=computeVarConstValues(root, mainFunctionRoot, variableIdMapping);

  if(detailedOutput) printResult(variableIdMapping,varConstSetMap);
  VariableConstInfo vci(&variableIdMapping, &varConstSetMap); // use vci as PState for dead code elimination

  //printResult(variableIdMapping, varConstSetMap);

  if(csvConstResultFileName) {
    writeCvsConstResult(variableIdMapping, varConstSetMap,csvConstResultFileName);
  }

  if(boolOptions["eliminate-dead-code"]) {
    cout<<"STATUS: performing dead code elimination."<<endl;
    eliminateDeadCodePhase1(root,mainFunctionRoot,&variableIdMapping,vci);
    eliminateDeadCodePhase2(root,mainFunctionRoot,&variableIdMapping,vci);
  } else {
    cout<<"STATUS: Dead code elimination: turned off."<<endl;
  }
  if(csvAssertFileName) {
    cout<<"STATUS: generating file "<<csvAssertFileName<<endl;
    reachabilityResults.write2013File(csvAssertFileName,true);
  }
#if 0
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
#endif
  cout << "INFO: Remaining functions in program: "<<numberOfFunctions(root)<<endl;
  if(boolOptions["generate-transformed-code"]) {
	cout << "STATUS: generating transformed source code."<<endl;
	root->unparse(0,0);
  }

  cout<< "STATUS: finished."<<endl;

  // main function try-catch
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  }
  return 0;
}
