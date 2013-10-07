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

bool isEmptyBlock(SgNode* node) {
  if(node==0)
    return true;
  //cout<<"isEmptyBasicBlock:"<<node->class_name()<<endl;
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    const SgStatementPtrList& stmtList=block->get_statements(); 
    return stmtList.size()==0;
  }
  return false;
}

bool isEmptyIfStmt(SgIfStmt* ifStmt) {
  SgStatement* trueBody=ifStmt->get_true_body();
  SgStatement* falseBody=ifStmt->get_false_body();
  return isEmptyBlock(trueBody) && isEmptyBlock(falseBody);
}

size_t eliminateEmptyIfStmts(SgNode* node) {
  size_t numElim=0;
  list<SgIfStmt*> ifstmts;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgIfStmt* ifStmt=isSgIfStmt(*i)) {
      if(isEmptyIfStmt(ifStmt)) {
        //cout<<"-- Found empty if statement"<<endl;
        ifstmts.push_back(ifStmt);
      }
    }
  }
  for(list<SgIfStmt*>::iterator i=ifstmts.begin();i!=ifstmts.end();i++) {
    SageInterface::removeStatement (*i, false);
    numElim++;
  }
  cout<<"STATUS: Number of if-statements eliminated: "<<numElim<<endl;
  return numElim;
}

// analysis

typedef map<VariableId, set<CppCapsuleConstIntLattice> > VarConstSetMap;

ConstIntLattice analyzeAssignRhs(SgNode* rhs) {
  assert(isSgExpression(rhs));

  ConstIntLattice rhsIntVal=AType::Top();

  // TODO: -1 is OK, but not -(-1); yet.
  if(SgMinusOp* minusOp=isSgMinusOp(rhs)) {
    if(SgIntVal* intValNode=isSgIntVal(SgNodeHelper::getFirstChild(minusOp))) {
      // found integer on rhs
      rhsIntVal=ConstIntLattice(-((int)intValNode->get_value()));
    }
  }
  // extracted info: isRhsIntVal:rhsIntVal 
  if(SgIntVal* intValNode=isSgIntVal(rhs)) {
    // found integer on rhs
    rhsIntVal=ConstIntLattice((int)intValNode->get_value());
  }
  return rhsIntVal;
}

bool determineVariable(SgNode* node, VariableId& varId, VariableIdMapping& _variableIdMapping) {
  assert(node);
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    //assert(_variableIdMapping);
#if 1
    SgSymbol* sym=varref->get_symbol();
    assert(sym);
    varId=_variableIdMapping.variableId(sym);
#else
    // MS: to investigate: even with the new var-sym-only case this does not work
    // MS: investigage getSymbolOfVariable
    varId=_variableIdMapping.variableId(varref);
#endif
    return true;
  } else {
    VariableId defaultVarId;
    varId=defaultVarId;
    return false;
  }
}

class VariableValuePair {
public:
  VariableValuePair(){}
  VariableValuePair(VariableId varId, ConstIntLattice varValue):varId(varId),varValue(varValue){}
  VariableId varId;
  ConstIntLattice varValue;
  string toString(VariableIdMapping& varIdMapping) {
    string varNameString=varIdMapping.uniqueShortVariableName(varId);
    string varValueString=varValue.toString();
    return varNameString+"="+varValueString;
  }
};

bool analyzeAssignment(SgAssignOp* assignOp,VariableIdMapping& varIdMapping, VariableValuePair* result) {
  const VariableId varId;
  const ConstIntLattice varValue;
  SgNode* lhs=SgNodeHelper::getLhs(assignOp);
  SgNode* rhs=SgNodeHelper::getRhs(assignOp);
  ConstIntLattice rhsResult=analyzeAssignRhs(rhs);
  VariableId lhsVarId;
  bool isVariableLhs=determineVariable(lhs,lhsVarId,varIdMapping);
  if(isVariableLhs) {
    VariableValuePair p(lhsVarId,rhsResult);
    *result=p;
    return true;
  }
  return false;
}

VariableValuePair analyzeVariableDeclaration(SgVariableDeclaration* decl,VariableIdMapping& varIdMapping) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
      assert(initDeclVar);
      VariableId initDeclVarId=varIdMapping.variableId(initDeclVar);
      SgInitializer* initializer=initName->get_initializer();
      SgAssignInitializer* assignInitializer=0;
      if(initializer && (assignInitializer=isSgAssignInitializer(initializer))) {
        //cout << "initializer found:"<<endl;
        SgExpression* rhs=assignInitializer->get_operand_i();
        assert(rhs);
        return VariableValuePair(initDeclVarId,analyzeAssignRhs(rhs));
      } else {
        //cout << "no initializer (OK)."<<endl;
        return VariableValuePair(initDeclVarId,AType::Top());
      }
    } else {
      cerr << "Error: in declaration (@initializedName) no variable found ... bailing out."<<endl;
      exit(1);
    }
  } else {
    cerr << "Error: in declaration: no variable found ... bailing out."<<endl;
    exit(1);
  }
}

void determineVarConstValueSet(SgNode* node, VariableIdMapping& varIdMapping, VarConstSetMap& map) {
  // TODO: // traverse the AST now and collect information
  RoseAst ast(node);
  cout<< "STATUS: Collecting information."<<endl;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      VariableValuePair res=analyzeVariableDeclaration(varDecl,varIdMapping);
      if(detailedOutput) cout<<"analyzing variable declaration :"<<res.toString(varIdMapping)<<endl;
      //update map
    }
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      VariableValuePair res;
      bool hasLhsVar=analyzeAssignment(assignOp,varIdMapping,&res);
      if(hasLhsVar) {
        //cout<<"analyzing variable assignment  :"<<res.toString(varIdMapping)<<endl;
        map[res.varId].insert(res.varValue);
      } else {
        // not handled yet (the new def-use sets allow to handle this better)
        cerr<<"Warning: ignoring assignment."<<endl;
      }
    }
    // ignore everything else (as of now)
  }
}

// does not support -inf, +inf yet
class VariableValueRangeInfo {
public:
  VariableValueRangeInfo(ConstIntLattice min, ConstIntLattice max);
  VariableValueRangeInfo(ConstIntLattice value);
  bool isTop() const { return _width.isTop(); }
  bool isBot() const { return _width.isBot(); }
  bool isEmpty() const { return (_width==0).isTrue(); }
  ConstIntLattice minValue() const { return _min; }
  ConstIntLattice maxValue() const { return _max; }
  int minIntValue() const { assert(_min.isConstInt()); return _min.getIntValue(); }
  int maxIntValue() const { assert(_max.isConstInt()); return _max.getIntValue(); }
  ConstIntLattice width() const { return _width; }
  string toString() const {
    if(isBot())
      return "bot";
    if(isTop())
      return "top";
    return string("[")+_min.toString()+","+_max.toString()+"]";
  }
private:
  ConstIntLattice _width;
  ConstIntLattice _min;
  ConstIntLattice _max;
};

VariableValueRangeInfo::VariableValueRangeInfo(ConstIntLattice min0, ConstIntLattice max0) {
  assert(min0.isConstInt() && max0.isConstInt());
  _width=max0-min0;
  _min=min0;
  _max=max0;
  if((_width<0).isTrue())
    _width=ConstIntLattice(0);
  ConstIntLattice one=AType::ConstIntLattice(1);
  _width=(VariableValueRangeInfo::_width+one);
}

VariableValueRangeInfo::VariableValueRangeInfo(ConstIntLattice value) {
  _min=value;
  _max=value;
  if(value.isTop()) {
    _width=ConstIntLattice(Top());
	return;
  }
  if(value.isBot()) {
    _width=ConstIntLattice(Bot());
	return;
  }
  _width=1;
}

VariableValueRangeInfo createVariableValueRangeInfo(VariableId varId, VarConstSetMap& map) {
  set<CppCapsuleConstIntLattice> cppCapsuleSet=map[varId];
  AType::ConstIntLattice minVal;
  AType::ConstIntLattice maxVal;
  // in case the set of collected assignments is empty, bot is returned (min and max remain bot).
  if(cppCapsuleSet.size()==0)
	return VariableValueRangeInfo(AType::ConstIntLattice(AType::Bot()));
  for(set<CppCapsuleConstIntLattice>::iterator i=cppCapsuleSet.begin();i!=cppCapsuleSet.end();++i) {
    AType::ConstIntLattice aint=(*i).getValue();
    if(aint.isTop()) {
      return VariableValueRangeInfo(AType::ConstIntLattice(AType::Top()));
    }
 
    if(minVal.isBot() && maxVal.isBot()) { minVal=aint; maxVal=aint; continue; }
    if((aint<minVal).isTrue())
      minVal=aint;
    if((aint>maxVal).isTrue())
      maxVal=aint;
  }
  if(minVal.isBot()||maxVal.isBot())
    return VariableValueRangeInfo(AType::ConstIntLattice(AType::Bot()));
  return VariableValueRangeInfo(minVal,maxVal);
}

// returns true if is in set
// returns false if not in set
// returns top if set contains top
ConstIntLattice isConstInSet(ConstIntLattice val, set<CppCapsuleConstIntLattice> valSet) {
  if(valSet.find(CppCapsuleConstIntLattice(ConstIntLattice(AType::Top())))!=valSet.end()) {
    return ConstIntLattice(AType::Top());
  }
  if(valSet.find(CppCapsuleConstIntLattice(val))!=valSet.end()) {  
    return ConstIntLattice(true);
  }
  return ConstIntLattice(false);
}

class VariableConstInfo {
public:
  VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map);
  bool isAny(VariableId);
  bool isUniqueConst(VariableId);
  bool isMultiConst(VariableId);
  int width(VariableId);
  bool isInConstSet(VariableId varId, int varVal);
  int uniqueConst(VariableId);
  int minConst(VariableId);
  int maxConst(VariableId);
private:
  VariableIdMapping* _variableIdMapping;
  VarConstSetMap* _map;
};

VariableConstInfo::VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map):_variableIdMapping(variableIdMapping),_map(map) {
}
bool VariableConstInfo::isAny(VariableId varId) {
  return createVariableValueRangeInfo(varId,*_map).isTop();
}
bool VariableConstInfo::isUniqueConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && (vri.width()==ConstIntLattice(1)).isTrue();
}
bool VariableConstInfo::isMultiConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && (vri.width()>ConstIntLattice(1)).isTrue();
}
int VariableConstInfo::width(VariableId varId) {
  ConstIntLattice width=createVariableValueRangeInfo(varId,*_map).width();
  if(!width.isConstInt())
	return 0;
  else
	return width.getIntValue();
}
int VariableConstInfo::minConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  assert(!(vri.isTop()||vri.isBot()));
  return vri.minIntValue();
}
int VariableConstInfo::maxConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  assert(!(vri.isTop()||vri.isBot()));
  return vri.maxIntValue();
}
bool VariableConstInfo::isInConstSet(VariableId varId, int varVal) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return isConstInSet(ConstIntLattice(varVal),(*_map)[varId]).isTrue();
}
int VariableConstInfo::uniqueConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  assert(!vri.isTop() && !vri.isBot() && vri.minIntValue()==vri.maxIntValue());
  return vri.minIntValue();
}

// intra-procedural; ignores function calls
VarConstSetMap computeVarConstValues(SgProject* project, SgFunctionDefinition* mainFunctionRoot, VariableIdMapping& variableIdMapping) {
  VarConstSetMap varConstIntMap;  

  VariableIdSet varIdSet=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);

  // initialize map such that it is resized to number of variables of interest

  for(VariableIdSet::iterator i=varIdSet.begin();i!=varIdSet.end();++i) {
    set<CppCapsuleConstIntLattice> emptySet;
    varConstIntMap[*i]=emptySet;
  }
  cout<<"STATUS: Initialized const map for "<<varConstIntMap.size()<< " variables."<<endl;

  cout << "STATUS: Number of global variables: ";
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  cout << globalVars.size()<<endl;
  VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);
  cout << "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;
  int filteredVars=0;
  set<CppCapsuleConstIntLattice> emptySet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    VariableId globalVarId=variableIdMapping.variableId(*i);
    if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
      VariableValuePair p=analyzeVariableDeclaration(*i,variableIdMapping);
      ConstIntLattice varValue=p.varValue;
      varConstIntMap[p.varId]=emptySet; // create mapping
      varConstIntMap[p.varId].insert(CppCapsuleConstIntLattice(varValue));
	  variablesOfInterest.insert(p.varId);
      //set<CppCapsuleConstIntLattice>& myset=varConstIntMap[p.varId];
    } else {
      filteredVars++;
    }
  }
  cout << "STATUS: Number of filtered variables for initial state: "<<filteredVars<<endl;

  // traverse the AST now and collect information

  if(boolOptions["inline"]) {
	determineVarConstValueSet(mainFunctionRoot,variableIdMapping,varConstIntMap);
  } else {
	// compute value-sets for entire program (need to cover all functions without inlining)
	determineVarConstValueSet(project,variableIdMapping,varConstIntMap);
  }
  return varConstIntMap;
}

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

bool isRelationalOp(SgExpression* node) {
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

EvalValueType eval(SgExpression* node) {
  EvalValueType res;
  stringstream watch;
  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(node));
    assert(lhs);
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(node));
    assert(rhs);
    watch<<"(";
    EvalValueType lhsResult=eval(lhs);
    watch<<",";
    EvalValueType rhsResult=eval(rhs);
    watch<<")";
    switch(node->variantT()) {
    case V_SgAndOp: watch<<"and";res=evalSgAndOp(lhsResult,rhsResult);break;
    case V_SgOrOp : watch<<"or" ;res=evalSgOrOp(lhsResult,rhsResult);break;
      
    case V_SgEqualityOp: watch<<"==";res=(lhsResult==rhsResult);break;
    case V_SgNotEqualOp: watch<<"!=";res=(lhsResult!=rhsResult);break;
    case V_SgGreaterOrEqualOp: watch<<">=";res=(lhsResult>=rhsResult);break;
    case V_SgGreaterThanOp: watch<<">";res=(lhsResult>rhsResult);break;
    case V_SgLessThanOp: watch<<"<";res=(lhsResult<rhsResult);break;
    case V_SgLessOrEqualOp: watch<<"<=";res=(lhsResult<=rhsResult);break;
      
    default:watch<<"#1:";watch<<node->class_name();watch<<"#";assert(0);
    }
  } else if(dynamic_cast<SgUnaryOp*>(node)) {
    SgExpression* child=isSgExpression(SgNodeHelper::getFirstChild(node));
    watch<<"(";
    EvalValueType childVal=eval(child);
    watch<<")";
    assert(child);
    switch(node->variantT()) {
    case V_SgNotOp:watch<<"!";res=!childVal;break;
    case V_SgCastExp:watch<<"C";res=childVal;break; // requires refinement for different types
	case V_SgMinusOp:watch<<"-";res=-childVal; break;
    default:watch<<"#2:";watch<<node->class_name();watch<<"#";assert(0);
    }
  } else {
    // ALL REMAINING CASES ARE EXPRESSION LEAF NODES
    switch(node->variantT()) {
    case V_SgBoolValExp: watch<<"B";res=evalSgBoolValExp(node);break;
    case V_SgIntVal:watch<<"I";res=evalSgIntVal(node);break;
    case V_SgVarRefExp:watch<<"V";res=evalSgVarRefExp(node);break;
    default:watch<<"#3:";watch<<node->class_name();watch<<"#";assert(0);
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
#if 0
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
	  int mywidth=vci.width(varId);
	  assert(mywidth==vci.maxConst(varId)-vci.minConst(varId)+1);
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
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ("inline",po::value< string >(), "perform inlining ([yes]|no).")
    ("eliminate-empty-if",po::value< string >(), "eliminate if-statements with empty branches in main function ([yes]/no).")
    ("eliminate-dead-code",po::value< string >(), "eliminate dead code (variables and expressions) ([yes]|no).")
    ("csv-const-result",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
    ("generate-transformed-code",po::value< string >(), "generate transformed code with prefix rose_ ([yes]|no).")
    ("verbose",po::value< string >(), "print detailed output during analysis and transformation (yes|[no]).")
    ("csv-assert",po::value< string >(), "name of csv file with reachability assert results")
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
  boolOptions.registerOption("precision-intbool",false); // temporary
  // regular options
  boolOptions.registerOption("inline",true);
  boolOptions.registerOption("eliminate-empty-if",true);
  boolOptions.registerOption("eliminate-dead-code",true);
  boolOptions.registerOption("generate-transformed-code",true);
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

  

  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* mainFunctionRoot=completeast.findFunctionByName(funtofind);
  if(!mainFunctionRoot) {
    cerr<<"Error: No main function available. "<<endl;
    exit(1);
  } else {
    cout << "STATUS: Found main function."<<endl;
  }
  if(boolOptions["inline"]) {
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
  
  if(boolOptions["eliminate-empty-if"]) {
    cout<<"STATUS: Eliminating empty if-statements in main function."<<endl;
    size_t num=0;
    size_t numTotal=num;
    do {
      num=eliminateEmptyIfStmts(mainFunctionRoot);
      numTotal+=num;
    } while(num>0);
    cout<<"STATUS: Total number of empty if-statements in main function eliminated: "<<numTotal<<endl;
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
  
  cout<<"STATUS: performing flow-insensitive const analysis."<<endl;
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(root);

  VarConstSetMap varConstSetMap;
  varConstSetMap=computeVarConstValues(root, mainFunctionRoot, variableIdMapping);
  

  if(detailedOutput) printResult(variableIdMapping,varConstSetMap);
  VariableConstInfo vci(&variableIdMapping, &varConstSetMap); // use vci as PState for dead code elimination

  printResult(variableIdMapping, varConstSetMap);

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
