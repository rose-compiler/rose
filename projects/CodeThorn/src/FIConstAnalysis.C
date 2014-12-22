// analysis
#include <rose.h>
#include "FIConstAnalysis.h"
#include "AType.h"
#include "CPAstAttribute.h"
#include "AnalysisAbstractionLayer.h"

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

ConstIntLattice FIConstAnalysis::analyzeAssignRhs(SgNode* rhs) {
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


bool FIConstAnalysis::analyzeAssignment(SgAssignOp* assignOp,VariableIdMapping& varIdMapping, VariableValuePair* result) {
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

void FIConstAnalysis::determineVarConstValueSet(SgNode* node, VariableIdMapping& varIdMapping, VarConstSetMap& map) {
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


void
VariableValueRangeInfo::setArraySize(int asize) {
  _asize=asize;
}

int
VariableValueRangeInfo::arraySize() {
  return _asize;
}

VariableValueRangeInfo::VariableValueRangeInfo(ConstIntLattice min0, ConstIntLattice max0) {
  assert(min0.isConstInt() && max0.isConstInt());
  _width=max0-min0;
  _min=min0;
  _max=max0;
  if((_width<0).isTrue())
    _width=ConstIntLattice(0);
  ConstIntLattice one=AType::ConstIntLattice(1);
  _width=(VariableValueRangeInfo::_width+one);
  _asize=1;
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

VariableValueRangeInfo VariableConstInfo::createVariableValueRangeInfo(VariableId varId, VarConstSetMap& map) {
  ROSE_ASSERT(map.size()>0);
  ROSE_ASSERT(varId.isValid());
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
ConstIntLattice VariableConstInfo::isConstInSet(ConstIntLattice val, set<CppCapsuleConstIntLattice> valSet) {
  if(valSet.find(CppCapsuleConstIntLattice(ConstIntLattice(AType::Top())))!=valSet.end()) {
    return ConstIntLattice(AType::Top());
  }
  if(valSet.find(CppCapsuleConstIntLattice(val))!=valSet.end()) {  
    return ConstIntLattice(true);
  }
  return ConstIntLattice(false);
}

VariableConstInfo::VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map):_variableIdMapping(variableIdMapping),_map(map) {
}

int VariableConstInfo::arraySize(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return vri.arraySize();
}

bool VariableConstInfo::isAny(VariableId varId) {
  return createVariableValueRangeInfo(varId,*_map).isTop();
}
bool VariableConstInfo::isUniqueConst(VariableId varId) {
  ROSE_ASSERT(_map);
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && !vri.width().isBot() && !vri.width().isTop() && (vri.width()==ConstIntLattice(1)).isTrue();
}
bool VariableConstInfo::isMultiConst(VariableId varId) {
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return !vri.isTop() && !vri.width().isBot() && !vri.width().isTop() && (vri.width()>ConstIntLattice(1)).isTrue();
}
size_t VariableConstInfo::width(VariableId varId) {
  ConstIntLattice width=createVariableValueRangeInfo(varId,*_map).width();
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
  VariableValueRangeInfo vri=createVariableValueRangeInfo(varId,*_map);
  return isConstInSet(ConstIntLattice(varVal),(*_map)[varId]).isTrue();
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
    set<CppCapsuleConstIntLattice> emptySet;
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
    return ConstIntLattice(global_variableConstInfo->uniqueConst(varId));
  } else {
    return ConstIntLattice(AType::Top());
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
    res=(lhsResult&&rhsResult);
  }
  return res;
}

EvalValueType FIConstAnalysis::evalSgOrOp(EvalValueType lhsResult,EvalValueType rhsResult) {
  EvalValueType res;
  // short-circuit CPP-OR semantics
  if(lhsResult.isTrue()) {
    res=lhsResult;
  } else {
    res=(lhsResult||rhsResult);
  }
  return res;
}

EvalValueType FIConstAnalysis::evalWithMultiConst(SgNode* op, SgVarRefExp* var, EvalValueType val) {
  ROSE_ASSERT(op);
  ROSE_ASSERT(var);

  ROSE_ASSERT(global_variableIdMapping);
  ROSE_ASSERT(global_variableConstInfo);

  assert(!(val.isTop()||val.isBot()));

    EvalValueType res=AType::Top(); // default if no more precise result can be determined

  int constVal=val.getIntValue();
  VariableId varId;
  bool isVar=determineVariable(var, varId, *global_variableIdMapping);
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

bool FIConstAnalysis::isConstVal(SgExpression* node) {
  return isSgBoolValExp(node)||isSgIntVal(node);
}

EvalValueType FIConstAnalysis::eval(SgExpression* node) {
  EvalValueType res;
  stringstream watch;

  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(node));
    assert(lhs);
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(node));
    assert(rhs);

    if(option_multiconstanalysis) {
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
    default:cerr<<"EvalValueType:unknown binary operator:"<<node->class_name()<<"::"<<node->unparseToString()<<" using top as default."<<endl; res=AType::Top();break;
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

/* format: varname, isAny, isUniqueconst, isMultiConst, width(>=1 or 0 or -1 (for any)), min, max, numBits, "{...}"
*/
void FIConstAnalysis::writeCvsConstResult(VariableIdMapping& variableIdMapping, string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());

  //  cout<<"Result:"<<endl;
  //VariableConstInfo vci(&variableIdMapping, &map);
  for(VarConstSetMap::iterator i=_varConstSetMap.begin();i!=_varConstSetMap.end();++i) {
    VariableId varId=(*i).first;
    //string variableName=variableIdMapping.uniqueShortVariableName(varId);
    string variableName=variableIdMapping.variableName(varId);
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
        ConstIntLattice res=eval(exp);
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
