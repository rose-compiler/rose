#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "RDTransferFunctions.hh"

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transfer(Label lab, LatticeType element) {
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

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transferExpression(Label lab, SgExpression* node, RDLattice& element) {
  return element;
}
  
template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transferDeclaration(Label label, SgVariableDeclaration* decl, RDLattice& element) {
  return element;
}

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>:: transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, RDLattice& element) {
  return element;
}

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, RDLattice& element) {
  return element;
}

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, RDLattice& element) {
  return element;
}

template<LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, RDLattice& element) {
  return element;
}
