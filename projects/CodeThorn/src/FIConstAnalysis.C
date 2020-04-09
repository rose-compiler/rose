// analysis
#include <rose.h>
#include "FIConstAnalysis.h"
#include "AbstractValue.h"
#include "CPAstAttribute.h"
#include "AnalysisAbstractionLayer.h"
#include "AstTerm.h"

using namespace CodeThorn;

FIConstAnalysis::FIConstAnalysis(VariableIdMapping* variableIdMapping):
  global_variableIdMapping(variableIdMapping),
  global_variableConstInfo(0),
  option_multiconstanalysis(false),
  detailedOutput(false)
 {
}

void FIConstAnalysis::runAnalysis(SgProject* root) {
  SgFunctionDefinition* mainFunctionRoot=0;
  runAnalysis(root,mainFunctionRoot);
}

void FIConstAnalysis::runAnalysis(SgProject* root, SgFunctionDefinition* mainFunctionRoot) {
  _varConstSetMap=computeVarConstValues(root, mainFunctionRoot, *global_variableIdMapping);
  cout<<"INFO: varConstSetMap: size: "<<_varConstSetMap.size()<<endl;
  global_variableConstInfo=new VariableConstInfo(global_variableIdMapping,&_varConstSetMap);
}
VariableConstInfo* FIConstAnalysis::getVariableConstInfo() {
  return global_variableConstInfo;
}

VariableIdMapping::VariableIdSet
FIConstAnalysis::determinedConstantVariables() {
  return variablesOfInterest;
}

AbstractValue FIConstAnalysis::analyzeAssignRhs(SgNode* rhs) {
  assert(isSgExpression(rhs));

  AbstractValue rhsIntVal=CodeThorn::Top();

  // TODO: -1 is OK, but not -(-1); yet.
  if(SgMinusOp* minusOp=isSgMinusOp(rhs)) {
    if(SgIntVal* intValNode=isSgIntVal(SgNodeHelper::getFirstChild(minusOp))) {
      // found integer on rhs
      rhsIntVal=AbstractValue(-((int)intValNode->get_value()));
    }
  }
  // extracted info: isRhsIntVal:rhsIntVal 
  if(SgIntVal* intValNode=isSgIntVal(rhs)) {
    // found integer on rhs
    rhsIntVal=AbstractValue((int)intValNode->get_value());
  }
  return rhsIntVal;
}

bool FIConstAnalysis::determineVariable(SgNode* node, VariableId& varId, VariableIdMapping& _variableIdMapping) {
  assert(node);
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    //assert(_variableIdMapping);
#if 1
    SgSymbol* sym=varref->get_symbol();
    ROSE_ASSERT(sym);
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

bool FIConstAnalysis::analyzeAssignment(SgExpression* assignOp,VariableIdMapping& varIdMapping, VariableValuePair* result) {
  ROSE_ASSERT(isSgAssignOp(assignOp)||isSgCompoundAssignOp(assignOp));
  const VariableId varId;
  const AbstractValue varValue;
  SgNode* lhs=SgNodeHelper::getLhs(assignOp);
  SgNode* rhs=SgNodeHelper::getRhs(assignOp);
  AbstractValue rhsResult=analyzeAssignRhs(rhs);
  VariableId lhsVarId;
  bool isVariableLhs=determineVariable(lhs,lhsVarId,varIdMapping);
  if(isVariableLhs) {
    VariableValuePair p(lhsVarId,rhsResult);
    *result=p;
    return true;
  }
  return false;
}

VariableValuePair FIConstAnalysis::analyzeVariableDeclaration(SgVariableDeclaration* decl,VariableIdMapping& varIdMapping) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
      assert(initDeclVar);
      VariableId initDeclVarId=varIdMapping.variableId(initDeclVar);
      SgInitializer* initializer=initName->get_initializer();
      SgAssignInitializer* assignInitializer=0;
      if(initializer && (assignInitializer=isSgAssignInitializer(initializer))) {
        if(detailedOutput) cout << "initializer found:"<<endl;
        SgExpression* rhs=assignInitializer->get_operand_i();
        assert(rhs);
        return VariableValuePair(initDeclVarId,analyzeAssignRhs(rhs));
      } else {
        if(detailedOutput) cout << "no initializer (OK)."<<endl;
        return VariableValuePair(initDeclVarId,CodeThorn::Top());
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

void FIConstAnalysis::determineVarConstValueSet(SgNode* node, VariableIdMapping& varIdMapping, VarConstSetMap& map) {
  // TODO: // traverse the AST now and collect information
  RoseAst ast(node);
  cout<< "STATUS: Collecting information."<<endl;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      VariableValuePair res=analyzeVariableDeclaration(varDecl,varIdMapping);
      if(detailedOutput) cout<<"INFO: analyzing variable declaration :"<<res.toString(varIdMapping)<<endl;
      //update map
      map[res.varId].insert(res.varValue);
    }
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      VariableValuePair res;
      bool hasLhsVar=analyzeAssignment(assignOp,varIdMapping,&res);
      if(hasLhsVar) {
        if(detailedOutput) cout<<"INFO: analyzing variable assignment (SgAssignOp)  :"<<res.toString(varIdMapping)<<endl;
        map[res.varId].insert(res.varValue);
      } else {
        // handle some special case, fail on all others
        SgDotExp* dotExp=isSgDotExp(SgNodeHelper::getLhs(assignOp));
        if(dotExp && isSgVarRefExp(SgNodeHelper::getLhs(dotExp)) && isSgVarRefExp(SgNodeHelper::getRhs(dotExp))) {
          VariableId id1=varIdMapping.variableId(isSgVarRefExp(SgNodeHelper::getLhs(dotExp)));
          VariableId id2=varIdMapping.variableId(isSgVarRefExp(SgNodeHelper::getRhs(dotExp)));
          if(!id1.isValid()||!id2.isValid()) {
            cerr<<"Error: FIConstAnalysis: special case v1.v2=... failed:"<<SgNodeHelper::sourceFilenameLineColumnToString(assignOp)<<":"<<assignOp->unparseToString()<<":::"<<AstTerm::astTermWithNullValuesToString(assignOp)<<endl;
          }
          map[id1].insert(AbstractValue::createTop());
          map[id2].insert(AbstractValue::createTop());
        } else {
          // TODO: all vars have to go to top and all additional entries must be top
          // not handled yet (the new def-use sets allow to handle this better)
          cout<<"Error: unknown lhs of assignment:"<<SgNodeHelper::sourceFilenameLineColumnToString(assignOp)<<":"<<assignOp->unparseToString()<<":::"<<AstTerm::astTermWithNullValuesToString(assignOp)<<endl;
          exit(1);
        }
      }
    }
    // check for operators: +=, -=, ...
    if(SgCompoundAssignOp* assignOp=isSgCompoundAssignOp(*i)) {
      VariableValuePair res;
      bool hasLhsVar=analyzeAssignment(assignOp,varIdMapping,&res);
      if(hasLhsVar) {
        if(detailedOutput) cout<<"INFO: analyzing variable assignment (SgCompoundAssignOp)  :"<<res.toString(varIdMapping)<<endl;
        // set properly to Top (update of variable)
#if 0
        AbstractValue topVal(CodeThorn::Top);
#endif
        map[res.varId].insert(AbstractValue(Top()));
      } else {
        cerr<<"Warning: unknown lhs of compound assignment."<<endl;
        // TODO: all vars have to go to top and all additional entries must be top
        exit(1);
      }
    }

    // ignore everything else (as of now)
  }
}


void
VariableValueRangeInfo::setArraySize(int asize) {
  _asize=asize;
}

int
VariableValueRangeInfo::arraySize() {
  return _asize;
}

VariableValueRangeInfo::VariableValueRangeInfo(AbstractValue min0, AbstractValue max0) {
  assert(min0.isConstInt() && max0.isConstInt());
  _width=max0-min0;
  _min=min0;
  _max=max0;
  if((_width.operatorLess(0)).isTrue())
    _width=AbstractValue(0);
  AbstractValue one=AbstractValue(1);
  _width=(VariableValueRangeInfo::_width+one);
  _asize=1;
}

VariableValueRangeInfo::VariableValueRangeInfo(AbstractValue value) {
  _min=value;
  _max=value;
  if(value.isTop()) {
    _width=AbstractValue(Top());
    return;
  }
  if(value.isBot()) {
    _width=AbstractValue(Bot());
    return;
  }
  _width=1;
}

VariableValueRangeInfo VariableConstInfo::createVariableValueRangeInfo(VariableId varId, VarConstSetMap& map) {
  ROSE_ASSERT(map.size()>0);
  ROSE_ASSERT(varId.isValid());
  set<AbstractValue> intSet=map[varId];
  AbstractValue minVal;
  AbstractValue maxVal;
  // in case the set of collected assignments is empty, bot is returned (min and max remain bot).
  if(intSet.size()==0)
    return VariableValueRangeInfo(AbstractValue(CodeThorn::Bot()));
  for(set<AbstractValue>::iterator i=intSet.begin();i!=intSet.end();++i) {
    AbstractValue aint=(*i);
    if(aint.isTop()) {
      return VariableValueRangeInfo(AbstractValue(CodeThorn::Top()));
    }
 
    if(minVal.isBot() && maxVal.isBot()) { minVal=aint; maxVal=aint; continue; }
    if((aint.operatorLess(minVal)).isTrue())
      minVal=aint;
    if((aint.operatorMore(maxVal)).isTrue())
      maxVal=aint;
  }
  if(minVal.isBot()||maxVal.isBot())
    return VariableValueRangeInfo(AbstractValue(CodeThorn::Bot()));
  return VariableValueRangeInfo(minVal,maxVal);
}

// returns true if is in set
// returns false if not in set
// returns top if set contains top
AbstractValue VariableConstInfo::isConstInSet(AbstractValue val, set<AbstractValue> valSet) {
  if(valSet.find(AbstractValue(AbstractValue(CodeThorn::Top())))!=valSet.end()) {
    return AbstractValue(CodeThorn::Top());
  }
  if(valSet.find(AbstractValue(val))!=valSet.end()) {  
    return AbstractValue(true);
  }
  return AbstractValue(false);
}

VariableConstInfo::VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map):_variableIdMapping(variableIdMapping),_map(map) {
}

int VariableConstInfo::arraySize(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return vri.arraySize();
}

bool VariableConstInfo::haveEmptyIntersection(VariableId varId1,VariableId varId2) {
  set<AbstractValue> var1Set=(*_map)[varId1];
  set<AbstractValue> var2Set=(*_map)[varId2];
  for(set<AbstractValue>::iterator i=var1Set.begin();
      i!=var1Set.end();
      ++i) {
    if(var2Set.find(*i)!=var2Set.end()) {
      return false;
    }
  }
  return true;
}

bool VariableConstInfo::isAny(VariableId varId) {
  return createVariableValueRangeInfo(varId,*_map).isTop();
}
bool VariableConstInfo::isUniqueConst(VariableId varId) {
  ROSE_ASSERT(_map);
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && !vri.width().isBot() && !vri.width().isTop() && (vri.width().operatorEq(AbstractValue(1))).isTrue();
}
bool VariableConstInfo::isMultiConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && !vri.width().isBot() && !vri.width().isTop() && (vri.width().operatorMore(AbstractValue(1))).isTrue();
}
size_t VariableConstInfo::width(VariableId varId) {
  AbstractValue width=createVariableValueRangeInfo(varId,*_map).width();
  if(!width.isConstInt())
    return ((size_t)INT_MAX)-INT_MIN;
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
  //VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return isConstInSet(AbstractValue(varVal),(*_map)[varId]).isTrue();
}
int VariableConstInfo::uniqueConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  assert(!vri.isTop() && !vri.isBot() && vri.minIntValue()==vri.maxIntValue());
  return vri.minIntValue();
}

VarConstSetMap FIConstAnalysis::computeVarConstValues(SgProject* project, SgFunctionDefinition* mainFunctionRoot, VariableIdMapping& variableIdMapping) {
  VarConstSetMap varConstIntMap;  

  VariableIdSet varIdSet=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);

  // initialize map such that it is resized to number of variables of interest
  for(VariableIdSet::iterator i=varIdSet.begin();i!=varIdSet.end();++i) {
    set<AbstractValue> emptySet;
    varConstIntMap[*i]=emptySet;
  }
  cout<<"STATUS: Initialized const map for "<<varConstIntMap.size()<< " variables."<<endl;

  cout << "STATUS: Number of global variables: ";
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  cout << globalVars.size()<<endl;
  VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);
  cout << "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;
#if 0
  int filteredVars=0;
  set<AbstractValue> emptySet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    VariableId globalVarId=variableIdMapping.variableId(*i);
    if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
      VariableValuePair p=analyzeVariableDeclaration(*i,variableIdMapping);
      AbstractValue varValue=p.varValue;
      varConstIntMap[p.varId]=emptySet; // create mapping
      varConstIntMap[p.varId].insert(AbstractValue(varValue));
      variablesOfInterest.insert(p.varId);
      //set<AbstractValue>& myset=varConstIntMap[p.varId];
    } else {
      filteredVars++;
    }
  }
  cout << "STATUS: Number of filtered variables for initial state: "<<filteredVars<<endl;
#endif
  variablesOfInterest=setOfUsedVars;
  if(mainFunctionRoot!=0) {
    determineVarConstValueSet(mainFunctionRoot,variableIdMapping,varConstIntMap);
  } else {
    // compute value-sets for entire program (need to cover all functions without inlining)
    determineVarConstValueSet(project,variableIdMapping,varConstIntMap);
  }
  return varConstIntMap;
}

// =======================================================================================================
// ===================== EVALUATION ========================

// to become a template/pattern

EvalValueType FIConstAnalysis::evalSgBoolValExp(SgExpression* node) {
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

EvalValueType FIConstAnalysis::evalSgIntVal(SgExpression* node) {
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
//VariableIdMapping* global_variableIdMappingPtr=0;
//VariableConstInfo* global_variableConstInfo=0;
//bool global_option_multiconstanalysis=false;
EvalValueType FIConstAnalysis::evalSgVarRefExp(SgExpression* node) {
  ROSE_ASSERT(global_variableIdMapping);
  ROSE_ASSERT(global_variableConstInfo);
  VariableId varId;
  bool isVar=FIConstAnalysis::determineVariable(node, varId, *global_variableIdMapping);
  assert(isVar);
  // varId is now VariableId of VarRefExp
  if(global_variableConstInfo->isUniqueConst(varId)) {
    return AbstractValue(global_variableConstInfo->uniqueConst(varId));
  } else {
    return AbstractValue(CodeThorn::Top());
  }
}

bool FIConstAnalysis::isRelationalOperator(SgExpression* node) {
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

EvalValueType FIConstAnalysis::evalSgAndOp(EvalValueType lhsResult,EvalValueType rhsResult) {
  EvalValueType res;
  // short-circuit CPP-AND semantics
  if(lhsResult.isFalse()) {
    res=lhsResult;
  } else {
    res=(lhsResult.operatorAnd(rhsResult));
  }
  return res;
}

EvalValueType FIConstAnalysis::evalSgOrOp(EvalValueType lhsResult,EvalValueType rhsResult) {
  EvalValueType res;
  // short-circuit CPP-OR semantics
  if(lhsResult.isTrue()) {
    res=lhsResult;
  } else {
    res=(lhsResult.operatorOr(rhsResult));
  }
  return res;
}

EvalValueType FIConstAnalysis::evalWithMultiConst(SgNode* op, SgVarRefExp* lhsVar, SgVarRefExp* rhsVar) {
  VariableId lhsVarId;
  bool lhsIsVar=determineVariable(lhsVar, lhsVarId, *global_variableIdMapping);
  assert(lhsIsVar);
  bool lhsIsMultiConst=global_variableConstInfo->isMultiConst(lhsVarId);
  VariableId rhsVarId;
  bool rhsIsVar=determineVariable(rhsVar, rhsVarId, *global_variableIdMapping);
  assert(rhsIsVar);
  bool rhsIsMultiConst=global_variableConstInfo->isMultiConst(rhsVarId);
  ROSE_ASSERT(lhsIsMultiConst && rhsIsMultiConst);

  int lhsMinConst=global_variableConstInfo->minConst(lhsVarId);
  int lhsMaxConst=global_variableConstInfo->maxConst(lhsVarId);
  int rhsMinConst=global_variableConstInfo->minConst(rhsVarId);
  int rhsMaxConst=global_variableConstInfo->maxConst(rhsVarId);
  
  EvalValueType res=CodeThorn::Top(); 
  bool haveEmptyIntersect=global_variableConstInfo->haveEmptyIntersection(lhsVarId,rhsVarId);
  switch(op->variantT()) {
  case V_SgEqualityOp:
    if(haveEmptyIntersect) res=EvalValueType(false);
    else res=CodeThorn::Top();
    break;
  case V_SgNotEqualOp: 
    if(haveEmptyIntersect) res=EvalValueType(true);
    else res=CodeThorn::Top();
    break;
  case V_SgGreaterOrEqualOp:
    if(lhsMinConst>=rhsMaxConst) res=EvalValueType(true);
    else if(lhsMaxConst<rhsMinConst) res=EvalValueType(false);
    else res=CodeThorn::Top();
    break;
  case V_SgGreaterThanOp:
    if(lhsMinConst>rhsMaxConst) res=EvalValueType(true);
    else if(lhsMaxConst<=rhsMinConst) res=EvalValueType(false);
    else res=CodeThorn::Top();
    break;
  case V_SgLessOrEqualOp:
    if(lhsMaxConst<=rhsMinConst) res=EvalValueType(true);
    else if(lhsMinConst>rhsMaxConst) res=EvalValueType(false);
    else res=CodeThorn::Top();
    break;
  case V_SgLessThanOp:
    if(lhsMaxConst<rhsMinConst) res=EvalValueType(true);
    else if(lhsMinConst>=rhsMaxConst) res=EvalValueType(false);
    else res=CodeThorn::Top();
    break;
  default:
    cerr<<"Error: evalWithMultiConst: unknown operator."<<endl;
    assert(0);
  }
  
  if(detailedOutput) cout<<" Result: "<<res.toString()<<endl;
  return res;
}

EvalValueType FIConstAnalysis::evalWithMultiConst(SgNode* op, SgVarRefExp* var, EvalValueType constVal0) {
  ROSE_ASSERT(op);
  ROSE_ASSERT(var);

  ROSE_ASSERT(global_variableIdMapping);
  ROSE_ASSERT(global_variableConstInfo);

  assert(!(constVal0.isTop()||constVal0.isBot()));

  EvalValueType res=CodeThorn::Top(); // default if no more precise result can be determined

  if(detailedOutput) cout<<"evalWithMultiConst:"<<op->unparseToString();

  int constVal=constVal0.getIntValue();
  VariableId varId;
  bool isVar=determineVariable(var, varId, *global_variableIdMapping);
  assert(isVar);
  bool myIsMultiConst=global_variableConstInfo->isMultiConst(varId);
  if(myIsMultiConst) {
    bool constValIsInVarMultiConstSet=global_variableConstInfo->isInConstSet(varId,constVal);
    int myMinConst=global_variableConstInfo->minConst(varId);
    int myMaxConst=global_variableConstInfo->maxConst(varId);
    if(detailedOutput) {
      cout<<" isMC:"<<myIsMultiConst;
      cout<<" isInConstSet:"<<constValIsInVarMultiConstSet;
      cout<<" min:"<<myMinConst;
      cout<<" max:"<<myMaxConst;
    }
    //cout<<endl;
    
    // it holds here: val *is* a multi const! (more than one const value)
    switch(op->variantT()) {
    case V_SgEqualityOp:
      // case of one value is handled by const-analysis
      if(!constValIsInVarMultiConstSet) res=EvalValueType(false);
      else res=CodeThorn::Top();
      break;
    case V_SgNotEqualOp: 
      // case of one value is handled by const-analysis
      if(!constValIsInVarMultiConstSet) res=EvalValueType(true);
      else res=CodeThorn::Top();
      break;
    case V_SgGreaterOrEqualOp:
      if(myMinConst>=constVal) res=EvalValueType(true);
      else if(myMaxConst<constVal) res=EvalValueType(false);
      else res=CodeThorn::Top();
      break;
    case V_SgGreaterThanOp:
      if(myMinConst>constVal) res=EvalValueType(true);
      else if(myMaxConst<=constVal) res=EvalValueType(false);
      else res=CodeThorn::Top();
      break;
    case V_SgLessOrEqualOp:
      if(myMaxConst<=constVal) res=EvalValueType(true);
      else if(myMinConst>constVal) res=EvalValueType(false);
      else res=CodeThorn::Top();
      break;
    case V_SgLessThanOp:
      if(myMaxConst<constVal) res=EvalValueType(true);
      else if(myMinConst>=constVal) res=EvalValueType(false);
      else res=CodeThorn::Top();
      break;
    default:
      cerr<<"Error: evalWithMultiConst: unknown operator."<<endl;
      assert(0);
    }
  } else {
    if(detailedOutput) cout<<" not multi-const.";
  }

  if(detailedOutput) cout<<" Result: "<<res.toString()<<endl;
  return res;
}

bool FIConstAnalysis::isConstVal(SgExpression* node) {
  return isSgBoolValExp(node)||isSgIntVal(node);
}

EvalValueType FIConstAnalysis::eval(SgExpression* node) {
  EvalValueType res;

  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(node));
    assert(lhs);
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(node));
    assert(rhs);

    if(option_multiconstanalysis) {
      // refinement for special cases handled by multi-const analysis
      if(isRelationalOperator(node)) {
        EvalValueType res2=CodeThorn::Top();
        if(isSgVarRefExp(lhs) && isConstVal(rhs))
          res2=evalWithMultiConst(node,isSgVarRefExp(lhs),eval(rhs));
        if(isConstVal(lhs) && isSgVarRefExp(rhs))
          res2=evalWithMultiConst(node,isSgVarRefExp(rhs),eval(lhs));
        if(isSgVarRefExp(lhs) && isSgVarRefExp(rhs)) {
          EvalValueType resLhs=evalSgVarRefExp(lhs);
          EvalValueType resRhs=evalSgVarRefExp(rhs);
          if(resRhs.isConstInt()) {
            res2=evalWithMultiConst(node,isSgVarRefExp(lhs),resRhs);
          } else if(resLhs.isConstInt()) {
            res2=evalWithMultiConst(node,isSgVarRefExp(rhs),resLhs);            
          } else {
            res2=evalWithMultiConst(node,isSgVarRefExp(lhs),isSgVarRefExp(rhs));
          }
        }

        if(!res2.isTop()) {
          // found a more precise result with multi-const analysis results
          ROSE_ASSERT(!res2.isBot());
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
    case V_SgEqualityOp: res=(lhsResult.operatorEq(rhsResult));break;
    case V_SgNotEqualOp: res=(lhsResult.operatorNotEq(rhsResult));break;
    case V_SgGreaterOrEqualOp: res=(lhsResult.operatorMoreOrEq(rhsResult));break;
    case V_SgGreaterThanOp: res=(lhsResult.operatorMore(rhsResult));break;
    case V_SgLessThanOp: res=(lhsResult.operatorLess(rhsResult));break;
    case V_SgLessOrEqualOp: res=(lhsResult.operatorLessOrEq(rhsResult));break;
    case V_SgPntrArrRefExp: res=CodeThorn::Top();break;
    default:cerr<<"EvalValueType:unknown binary operator:"<<node->class_name()<<"::"<<node->unparseToString()<<" using top as default."<<endl; res=CodeThorn::Top();break;
    }
  } else if(dynamic_cast<SgUnaryOp*>(node)) {
    SgExpression* child=isSgExpression(SgNodeHelper::getFirstChild(node));
    assert(child);
    EvalValueType childVal=eval(child);
    switch(node->variantT()) {
    case V_SgNotOp: res=childVal.operatorNot();break;
    case V_SgCastExp: res=childVal;break; // requires refinement for different types
    case V_SgMinusOp: res=childVal.operatorUnaryMinus(); break;
    case V_SgPointerDerefExp: res=CodeThorn::Top();break;
    default:cerr<<"EvalValueType:unknown unary operator:"<<node->class_name()<<"::"<<node->unparseToString()<<endl; res=CodeThorn::Top();break;
    }
  } else {
    // ALL REMAINING CASES ARE EXPRESSION LEAF NODES
    switch(node->variantT()) {
    case V_SgBoolValExp: res=evalSgBoolValExp(node);break;
    case V_SgIntVal: res=evalSgIntVal(node);break;
    case V_SgVarRefExp: res=evalSgVarRefExp(node);break;
    default: cerr<<"EvalValueType:unknown operator:"<<node->class_name()<<"::"<<node->unparseToString()<<endl; res=CodeThorn::Top();break;
    }
  }
  return res;
}

void FIConstAnalysis::setVariableConstInfo(VariableConstInfo* varConstInfo) {
  global_variableConstInfo=varConstInfo;
}
void FIConstAnalysis::setOptionMultiConstAnalysis(bool opt) {
  option_multiconstanalysis=opt;
}

void FIConstAnalysis::attachAstAttributes(Labeler* labeler, string attributeName) {
  ROSE_ASSERT(global_variableConstInfo);
  for(Labeler::iterator i=labeler->begin();i!=labeler->end();++i) {
    SgNode* node=labeler->getNode(*i);
    ROSE_ASSERT(node);
    node->setAttribute(attributeName, new CPAstAttribute(global_variableConstInfo,node,global_variableIdMapping));
  }
}

void FIConstAnalysis::filterVariables(VariableIdSet& variableIdSet) {
  VariableIdSet toBeRemoved;
  for(VarConstSetMap::iterator i=_varConstSetMap.begin();i!=_varConstSetMap.end();++i) {
    VariableId varId=(*i).first;
    if(variableIdSet.find(varId)==variableIdSet.end()) {
      toBeRemoved.insert(varId);
    }
  }
  for(VariableIdSet::iterator i=toBeRemoved.begin();i!=toBeRemoved.end();++i) {
    _varConstSetMap.erase(*i);
  }
}

/* format: varname, isAny, isUniqueconst, isMultiConst, width(>=1 or 0 or -1 (for any)), min, max, numBits, "{...}"
*/
void FIConstAnalysis::writeCvsConstResult(VariableIdMapping& variableIdMapping, const char* filename) {
  ofstream myfile;
  cout<<"FIConstAnalysis: writing to file "<<filename<<endl;
  myfile.open(filename);
  if (!myfile.is_open()) {
    cout << "Error: could not open file: " << filename << endl;
    exit(1);
  }
  cout<<"DEBUG: FIConstAnalysis: Result:"<<endl;
  //VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=_varConstSetMap.begin();i!=_varConstSetMap.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
    cout<<"DEBUG: writing data for variable "<<variableName<<endl;
    myfile<<variableName;
    myfile<<",";
    myfile<<global_variableConstInfo->isAny(varId);
    myfile<<",";
    myfile<<global_variableConstInfo->isUniqueConst(varId);
    myfile<<",";
    myfile<<global_variableConstInfo->isMultiConst(varId);
    myfile<<",";
    if(global_variableConstInfo->isUniqueConst(varId)||global_variableConstInfo->isMultiConst(varId)) {
      myfile<<global_variableConstInfo->minConst(varId);
      myfile<<",";
      myfile<<global_variableConstInfo->maxConst(varId);
      size_t mywidth=global_variableConstInfo->width(varId);
      assert(mywidth==(size_t)global_variableConstInfo->maxConst(varId)-global_variableConstInfo->minConst(varId)+1);
      int mylog2=log2(mywidth);
      // compute upper whole number
      int bits=-1;

      // casts on pow args to ensure that the correct overloaded C++ cmath function is selected
      if(mywidth==static_cast<size_t>(pow(2.0,(double)mylog2))) {
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
    //myfile<<arraySize<<",";
#if 1
    set<AbstractValue> valueSet=(*i).second;
    stringstream setstr;
    myfile<<"{";
    for(set<AbstractValue>::iterator i=valueSet.begin();i!=valueSet.end();++i) {
      if(i!=valueSet.begin())
        myfile<<",";
      myfile<<(*i).toString();
    }
    myfile<<"}";
#endif
    myfile<<endl;
  }
  myfile.close();
}

int FIConstAnalysis::performConditionConstAnalysis(Labeler* labeler) {
  // temporary global var
  //global_variableIdMappingPtr=variableIdMapping;
  //global_variableConstInfo=&vci;
  //FIConstAnalysis fiConstAnalysis(variableIdMapping);
  //fiConstAnalysis.setVariableConstInfo(&vci);
  //RoseAst ast1(root);

  //cout<<"STATUS: Dead code elimination phase 2: Eliminating sub expressions."<<endl;
  //list<pair<SgExpression*,SgExpression*> > toReplaceExpressions;
  //  for(RoseAst::iterator i=ast1.begin();i!=ast1.end();++i) {
  for(Labeler::iterator i=labeler->begin();i!=labeler->end();++i) {
    if(labeler->isConditionLabel(*i)) {
      SgNode* origNode=labeler->getNode(*i);
      SgNode* node=origNode;
      if(isSgExprStatement(node)) {
        node=SgNodeHelper::getExprStmtChild(node);
      }
      //cout<<node->class_name()<<";";
      SgExpression* exp=isSgExpression(node);
      if(exp) {
        AbstractValue res=eval(exp);
        if(res.isTrue()) {
          trueConditions.insert(labeler->getLabel(origNode));
        }
        if(res.isFalse()) {
          falseConditions.insert(labeler->getLabel(origNode));
        }
        if(res.isTop()) {
          nonConstConditions.insert(labeler->getLabel(origNode));
        }
      } else {
        // report all other cases of conditions as non-const (conservative)
        nonConstConditions.insert(labeler->getLabel(origNode));
      }
    }
  }
  return 0;
}

LabelSet FIConstAnalysis::getTrueConditions() {
  return trueConditions;
}
LabelSet FIConstAnalysis::getFalseConditions() {
  return falseConditions;
}
LabelSet FIConstAnalysis::getNonConstConditions() {
  return nonConstConditions;
}

void FIConstAnalysis::setDetailedOutput(bool flag) {
  detailedOutput=flag;
}

void FIConstAnalysis::printResult(VariableIdMapping& variableIdMapping, VarConstSetMap& map) {
  cout<<"Result:"<<endl;
  VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=map.begin();i!=map.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
    set<AbstractValue> valueSet=(*i).second;
    stringstream setstr;
    setstr<<"{";
    for(set<AbstractValue>::iterator i=valueSet.begin();i!=valueSet.end();++i) {
      if(i!=valueSet.begin())
        setstr<<",";
      setstr<<(*i).toString();
    }
    setstr<<"}";
    cout<<variableName<<"="<<setstr.str()<<";";
    cout<<"Range:"<<VariableConstInfo::createVariableValueRangeInfo(varId,map).toString();
    cout<<" width: "<<VariableConstInfo::createVariableValueRangeInfo(varId,map).width().toString();
    cout<<" top: "<<VariableConstInfo::createVariableValueRangeInfo(varId,map).isTop();
    cout<<endl;
    cout<<" isAny:"<<vci.isAny(varId)
        <<" isUniqueConst:"<<vci.isUniqueConst(varId)
        <<" isMultiConst:"<<vci.isMultiConst(varId);
    if(vci.isUniqueConst(varId)||vci.isMultiConst(varId)) {
      cout<<" width:"<<vci.width(varId);
    } else {
      cout<<" width:unknown";
    }
    cout<<endl;
  }
  cout<<"---------------------"<<endl;
}
