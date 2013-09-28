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

using namespace std;
using namespace CodeThorn;
using namespace AType;

static VariableIdSet variablesOfInterest;

static bool detailedOutput=0;

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
    cout<< "function call:"<<SgNodeHelper::nodeToString(*i)<<": ";
    bool success=trivialInline(funCall);
    if(success) {
      cout<<"inlined."<<endl;
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
  cout<<"Number of if-statements eliminated: "<<numElim<<endl;
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
  assert(width.isConstInt());
  return width.getIntValue();
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
  determineVarConstValueSet(mainFunctionRoot,variableIdMapping,varConstIntMap);
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
  cout<<"STATUS: Eliminating variables."<<endl;
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

  int elimVar=0;
  for(list<SgVariableDeclaration*>::iterator i=toDeleteVarDecls.begin();
	  i!=toDeleteVarDecls.end();
	  ++i) {
	elimVar++;
	if(detailedOutput) cout<<"Eliminating dead variable's declaration: "<<(*i)->unparseToString()<<endl;
	SageInterface::removeStatement(*i, false);
  }
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
  cout<<"STATUS: Replaced "<<elimVarUses<<" uses of variable with constant."<<endl;
  cout<<"STATUS: Eliminated "<<elimVar<<" dead variables."<<endl;

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
		<<" isMultiConst:"<<vci.isMultiConst(varId)<<"##";
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


int main(int argc, char* argv[]) {
  cout << "INIT: Parsing and creating AST."<<endl;
  boolOptions.registerOption("arith-top",false); // temporary
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions
  list<SgFunctionCallExp*> funCallList=trivialFunctionCalls(root);
  cout<<"Number of trivial function calls (with existing function bodies): "<<funCallList.size()<<endl;

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* mainFunctionRoot=completeast.findFunctionByName(funtofind);
  if(!mainFunctionRoot) {
    cerr<<"No main function available. "<<endl;
    exit(1);
  } else {
    cout << "Found main function."<<endl;
  }
  list<SgFunctionCallExp*> remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
  while(remainingFunCalls.size()>0) {
    size_t numFunCall=remainingFunCalls.size();
    cout<<"Remaing function calls in main function: "<<numFunCall<<endl;
    if(numFunCall>0)
      inlineFunctionCalls(remainingFunCalls);
    remainingFunCalls=trivialFunctionCalls(mainFunctionRoot);
  }
  
  size_t num=0;
  size_t numTotal=num;
  do {
    num=eliminateEmptyIfStmts(mainFunctionRoot);
    numTotal+=num;
  } while(num>0);
  cout<<"Total number of if-statements eliminated: "<<numTotal<<endl;
  
  //TODO: create ICFG and compute non reachable functions (from main function)
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();i++) {
    string funName=SgNodeHelper::getFunctionName(*i);
    SgFunctionDeclaration* funDecl=(*i)->get_declaration();
    if(funName!="main") {
      cout<<"Deleting function: "<<funName<<endl;
      SgStatement* stmt=funDecl;
      SageInterface::removeStatement (stmt, false);
      //SageInterface::deleteAST(funDef);
    }
  }

  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(root);
  VarConstSetMap varConstSetMap=computeVarConstValues(root, mainFunctionRoot, variableIdMapping);
  if(detailedOutput) printResult(variableIdMapping,varConstSetMap);
  VariableConstInfo vci(&variableIdMapping, &varConstSetMap); // use vci as PState for dead code elimination
  eliminateDeadCodePhase1(root,mainFunctionRoot,&variableIdMapping,vci);
#if 0
  rdAnalyzer->determineExtremalLabels(startFunRoot);
  rdAnalyzer->run();
#endif
  cout << "Remaining functions in program: "<<numberOfFunctions(root)<<endl;
  cout << "INFO: generating transformed source code."<<endl;
  root->unparse(0,0);
  cout<< "STATUS: finished."<<endl;
  return 0;
}
