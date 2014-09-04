#include "sage3basic.h"
#include "GeneralAnalyzer.h"
using namespace std;
using namespace CodeThorn;

#include "CollectionOperators.h"


GeneralAnalyzer::GeneralAnalyzer() {
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */

PropertyState* GeneralAnalyzer::transfer(Label lab, PropertyState* element) {
  //Labeler* _labeler=getLabeler();
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;

  if(_labeler->isFunctionCallLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(_labeler->getNode(lab))) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      return transferFunctionCall(lab, funCall, arguments, element);
    }
  }
  if(_labeler->isFunctionCallReturnLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(_labeler->getNode(lab))) {
      return transferFunctionCallReturn(lab, funCall, element);
    }
  }
  if(_labeler->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(_labeler->getNode(lab))) {
      // 1) obtain formal parameters
      assert(funDef);
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      return transferFunctionEntry(lab, funDef, formalParameters, element);
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
      VariableIdMapping::VariableIdSet localVars=GeneralAnalyzer::_variableIdMapping.determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=GeneralAnalyzer::_variableIdMapping.determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      return transferFunctionExit(lab,funDef,vars,element); // TEST ONLY
    } else {
      ROSE_ASSERT(0);
    }
  }
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgExpression* expr=isSgExpression(node)) {
    return transferExpression(lab,expr,element);
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    return transferDeclaration(lab,vardecl,element);
  }
#if 0
  cout << "Analyzer: called transfer function. result: ";
  element.toStream(cout,&GeneralAnalyzer::_variableIdMapping);
  cout<<endl;
#endif
  cerr<<"Error: interval analysis: unknown language construct."<<endl;
  cerr<<"Node: "<<node->unparseToString()<<endl;
  exit(1);
}


PropertyState* GeneralAnalyzer::transferCondition(Label sourceLabel, EdgeType edgeType, Label targetLabel, SgExpression* expr, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferExpression(Label label, SgExpression* expr, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferDeclaration(Label label, SgVariableDeclaration* decl, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, PropertyState* element) {
  return element;
}


PropertyState* GeneralAnalyzer::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, PropertyState* element) {
  return element;
}
