// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
using namespace std;
using namespace CodeThorn;

#include "CollectionOperators.h"

#include "RDTransferFunctions.hpp"

RDAnalyzer::RDAnalyzer() {
  _transferFunctions=new RDTransferFunctions();
}

RDAnalyzer::~RDAnalyzer() {
  delete _transferFunctions;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void RDAnalyzer::attachInInfoToAst(string attributeName) {
  if(!_preInfoIsValid)
    computeAllPreInfo();
  attachInfoToAst(attributeName,true);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void RDAnalyzer::attachOutInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,false);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
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

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
#if 1
RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  ROSE_ASSERT(_transferFunctions);
  return _transferFunctions->transfer(lab,element);
}
#else 
RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  if(element.isBot())
    element.setEmptySet();
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;

  if(_labeler->isFunctionCallLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      transferFunctionCall(lab, funCall, arguments, element);
      return element;
    }
  }
  if(_labeler->isFunctionCallReturnLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      transferFunctionCallReturn(lab, funCall, element);
      return element;
    }
  }
  if(_labeler->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) obtain formal parameters
      assert(funDef);
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      transferFunctionEntry(lab, funDef, formalParameters, element);
      return element;
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
      transferFunctionExit(lab,funDef,vars,element); // TEST ONLY
      return element;
    } else {
      ROSE_ASSERT(0);
    }
  }
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
  }
#if 0
  cout << "RDAnalyzer: called transfer function. result: ";
  element.toStream(cout,&_variableIdMapping);
  cout<<endl;
#endif
  return element;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDAnalyzer::transferExpression(Label lab, SgExpression* node, RDLattice& element) {
  // update analysis information
  // this is only correct for RERS12-C programs
  // 1) remove all pairs with lhs-variableid
  // 2) add (lab,lhs.varid)
  
  // (for programs with pointers we require a set here)
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,_variableIdMapping);  
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

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
//NOTE: missing: UD must take uses in initializers into account
void RDAnalyzer::transferDeclaration(Label lab, SgVariableDeclaration* declnode, RDLattice& element) {
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(node);
  // same as in transferExpression ... needs to be refined
  VariableIdSet defVarIds=AnalysisAbstractionLayer::defVariables(node,_variableIdMapping);  
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

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDAnalyzer::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,RDLattice& element) {
  // uses and defs in argument-expressions
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    transferExpression(lab,*i,element);
  }
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDAnalyzer::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, RDLattice& element) {
  //TODO: def in x=f(...) (not seen as assignment)
}
//NOTE: UD analysis must take uses of function-call arguments into account
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDAnalyzer::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, RDLattice& element) {
  // generate RDs for each parameter variable
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=_variableIdMapping.variableId(formalParameterName);
    // it must hold that this VarId does not exist in the RD-element
    //assert
    element.insertPair(lab,formalParameterVarId);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDAnalyzer::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, RDLattice& element) {
  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    element.eraseAllPairsWithVariableId(varId);
  }
  // TODO:: return variable $r
}
#endif

