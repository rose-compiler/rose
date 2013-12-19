// analysis

#include "FIConstAnalysis.h"

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
