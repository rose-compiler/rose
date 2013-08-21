// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
using namespace std;
using namespace CodeThorn;

#include "CollectionOperators.h"

RDAnalyzer::RDAnalyzer() {
}

void RDAnalyzer::attachInInfoToAst(string attributeName) {
  if(!_preInfoIsValid)
	computeAllPreInfo();
  attachInfoToAst(attributeName,true);
}

void RDAnalyzer::attachOutInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,false);
}

void RDAnalyzer::attachInfoToAst(string attributeName,bool inInfo) {
#if 0
  size_t lab=0;
  for(std::vector<RDLattice>::iterator i=_analyzerData.begin();
      i!=_analyzerData.end();
      ++i) {
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. functio call)
    _labeler->getNode(lab)->setAttribute(attributeName,new RDAstAttribute(&(*i)));
    lab++;
  }
#else
  if(inInfo && !_preInfoIsValid)
	computeAllPreInfo();
  LabelSet labelSet=_flow.nodeLabels();
  for(LabelSet::iterator i=labelSet.begin();
      i!=labelSet.end();
      ++i) {
	ROSE_ASSERT(*i<_analyzerData.size());
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. function call)
	if(!_labeler->isFunctionExitLabel(*i) /* && !_labeler->isCallReturnLabel(lab)*/)
	  if(*i >=0 ) {
		if(inInfo)
		  _labeler->getNode(*i)->setAttribute(attributeName,new RDAstAttribute(&_analyzerDataPreInfo[*i]));
		else
		  _labeler->getNode(*i)->setAttribute(attributeName,new RDAstAttribute(&_analyzerData[*i]));
	  }
	
  }

#endif
}

RDAnalyzer::iterator RDAnalyzer::begin() {
  return _analyzerData.begin();
}
  
RDAnalyzer::iterator RDAnalyzer::end() {
  return _analyzerData.end();
}
size_t RDAnalyzer::size() {
  return _analyzerData.size();
}

// TODO: refactor in separate functions
RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  if(element.isBot())
    element.setEmptySet();
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;

  ///////////////////////////////////////////
  // remove undeclared variable at function exit
  if(_labeler->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
	  // 1) obtain formal parameters
	  // 2) generate RDs for each parameter variable
	  SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	  assert(funDef);
	  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
		  i!=formalParameters.end();
		  ++i) {
		SgInitializedName* formalParameterName=*i;
		assert(formalParameterName);
		VariableId formalParameterVarId=_variableIdMapping.variableId(formalParameterName);
		// it must hold that this VarId does not exist in the RD-element
		// TODO: when local variables go out of scope in a function call (transfer-call can clear those)
		//assert
		element.insertPair(lab,formalParameterVarId);
	  }
	} else {
	  ROSE_ASSERT(0);
	}
  }

  if(_labeler->isFunctionExitLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(_labeler->getNode(lab))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      
      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      VariableIdMapping::VariableIdSet localVars=_variableIdMapping.determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=_variableIdMapping.determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
        VariableId varId=*i;
        element.eraseAllPairsWithVariableId(varId);
      }
      return element;
	} else {
	  ROSE_ASSERT(0);
	}
  }
  ///////////////////////////////////////////
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(expr,lab,element);
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
	transferDeclaration(vardecl,lab,element);
  }
#if 0
  cout << "RDAnalyzer: called transfer function. result: ";
  element.toStream(cout,&_variableIdMapping);
  cout<<endl;
#endif
  return element;
}

void RDAnalyzer::transferExpression(SgExpression* node, Label& lab, RDLattice& element) {
  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)
  
  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariablesInExpression(node,_variableIdMapping);  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
	// since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
	for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
	  element.insertPair(lab,*i);
	}
	assert(0);
  } else if(defVarIds.size()==1) {
	// one unique memory location (variable). We can remove all pairs with this variable
	VariableId var=*defVarIds.begin();
	element.eraseAllPairsWithVariableId(var);
	element.insertPair(lab,var);
  }
}

void RDAnalyzer::transferDeclaration(SgVariableDeclaration* declnode, Label& lab, RDLattice& element) {
  cout<<"Calling transferDeclaration1."<<endl;
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariablesInExpression(node,_variableIdMapping);  
  if(defVarIds.size()>1 /* TODO: || existsArrayVarId(defVarIds)*/ ) {
	// since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
	for(VariableIdMapping::VariableIdSet::iterator i=defVarIds.begin();i!=defVarIds.end();++i) {
	  element.insertPair(lab,*i);
	}
	assert(0);
  } else if(defVarIds.size()==1) {
	// one unique memory location (variable). We can remove all pairs with this variable
	cout<<"Calling transferDeclaration2."<<endl;
	VariableId var=*defVarIds.begin();
	element.eraseAllPairsWithVariableId(var);
	element.insertPair(lab,var);
  }
}
