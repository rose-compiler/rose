#ifndef PATRANSFERFUNCTIONS_CPP
#define PATRANSFERFUNCTIONS_CPP

#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

using namespace CodeThorn;

DFTransferFunctions::DFTransferFunctions():_programAbstractionLayer(0){}

void DFTransferFunctions::transfer(Edge edge, Lattice& element) {
    Label lab0=edge.source();
    //SgNode* node=getLabeler()->getNode(lab0);
    //cout<<"DEBUG: transfer @"<<lab0<<":"<<node->unparseToString()<<endl;

    // switch statement has its own transfer functions which are selected in transfer function
    if(getLabeler()->isConditionLabel(lab0)&&!getLabeler()->isSwitchExprLabel(lab0)) {
      transferCondition(edge,element);
    } else {
      transfer(lab0,element);
    }
}

void DFTransferFunctions::transferCondition(Edge edge, Lattice& element) {
  Label lab0=edge.source();
  transfer(lab0,element);
}

void DFTransferFunctions::transfer(Label lab, Lattice& element) {
  ROSE_ASSERT(getLabeler());
  //cout<<"transfer @label:"<<lab<<endl;
  SgNode* node=getLabeler()->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;
  //cout<<"DEBUG: transfer: @"<<lab<<": "<<node->class_name()<<":"<<node->unparseToString()<<endl;

  if(element.isBot()) {
    // transfer function applied to the bottom element is the bottom element (non-reachable state)
    // the extremal value must be different to the bottom element.
    return;
  }
  if(getLabeler()->isFunctionCallLabel(lab)) {
    // 1) f(x), 2) y=f(x) (but not y+=f(x))
    if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(node)) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      transferFunctionCall(lab, funCall, arguments, element);
      return;
    } else {
      cerr<<"Error: DFTransferFunctions::callexp: no function call on rhs of assignment found. Only found "<<funCall->class_name()<<endl;
      exit(1);
    }
  }

  if(getLabeler()->isFunctionCallReturnLabel(lab)) {
    if(isSgExprStatement(node)) {
      node=SgNodeHelper::getExprStmtChild(node);
    }
    SgVarRefExp* lhsVar=0;
    // case x=f(y);
    if(isSgAssignOp(node)||isSgCompoundAssignOp(node)) {
      if(SgVarRefExp* lhs=isSgVarRefExp(SgNodeHelper::getLhs(node))) {
        lhsVar=lhs;
      } else {
        cerr<<"Transfer: unknown lhs of function call result assignment."<<endl;
        cerr<<node->unparseToString()<<endl;
        exit(1);
      }
      SgNode* rhs=SgNodeHelper::getRhs(node);
      while(isSgCastExp(rhs))
        rhs=SgNodeHelper::getFirstChild(rhs);
      SgFunctionCallExp* funCall=isSgFunctionCallExp(rhs);
      if(!funCall) {
        cerr<<"Transfer: no function call on rhs of assignment."<<endl;
        cerr<<node->unparseToString()<<endl;
        exit(1);
      }
      transferFunctionCallReturn(lab, lhsVar, funCall, element);
      return;
    } else if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
      SgVarRefExp* lhsVar=0;
      transferFunctionCallReturn(lab, lhsVar, funCall, element);
      return;
    } else if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(node)) {
      // handle special case of function call in variable declaration: type var=f();
      SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(node);
      VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
      ROSE_ASSERT(lhsVarId.isValid());
      transferFunctionCallReturn(lab, lhsVarId, funCall, element);
    } else if(isSgReturnStmt(node)) {
      // special case of return f(...);
      node=SgNodeHelper::getFirstChild(node);
      if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
        transferFunctionCallReturn(lab, 0, funCall, element);
        return;
      }
    } else {
      if(getSkipSelectedFunctionCalls()) {
        // ignore unknown function call (requires command line option --ignore-unknown-functions)
      } else {
        cerr<<"Error: function-call-return unhandled function call."<<endl;
        cerr<<node->unparseToString()<<endl;
        exit(1);
      }
    }
  }

  if(getLabeler()->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) obtain formal parameters
      assert(funDef);
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      transferFunctionEntry(lab, funDef, formalParameters, element);
      return;
    } else {
      ROSE_ASSERT(0);
    }
  }

  if(getLabeler()->isFunctionExitLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state

      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      VariableIdMapping::VariableIdSet localVars=getVariableIdMapping()->determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=getVariableIdMapping()->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      transferFunctionExit(lab,funDef,vars,element); // TEST ONLY
      return;
    } else {
      ROSE_ASSERT(0);
    }
  }
  if(getLabeler()->isEmptyStmtLabel(lab)||getLabeler()->isBlockBeginLabel(lab)) {
    SgStatement* stmt=isSgStatement(node);
    ROSE_ASSERT(stmt);
    transferEmptyStmt(lab,stmt,element);
    return;
  }

  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
  }

  // detect SgReturn statement expressions
  if(isSgReturnStmt(node)) {
    node=SgNodeHelper::getFirstChild(node);
    SgExpression* expr=isSgExpression(node);
    ROSE_ASSERT(expr);
    transferReturnStmtExpr(lab,expr,element);
    return;
  }

  // default identity functions
  if(isSgBreakStmt(node)
     || isSgContinueStmt(node)
     || isSgLabelStatement(node)
     || isSgGotoStatement(node)) {
    return;
  }

  if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(node)) {
    SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    transferSwitchCase(lab,condStmt, caseStmt,element);
    return;
  }
  if(SgDefaultOptionStmt* defaultStmt=isSgDefaultOptionStmt(node)) {
    SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    transferSwitchDefault(lab,condStmt, defaultStmt,element);
    return;
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
    return;
  }
  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
    return;
  }

  if(isSgSwitchStatement(node)) {
    cout<<"WARNING: transfer at SgSwitchStatement node."<<endl;
    return;
  }

#if 0
  /*
     ensure there is no fall through as this would mean that not the correct transferfunction is invoked.
  */
  cerr<<"Error: unknown language construct in transfer function."<<endl;
  cerr<<"  Label: "<<lab<<endl;
  cerr<<"  NodeType: "<<node->class_name()<<endl;
  cerr<<"  Source Code: "<<node->unparseToString()<<endl;
  exit(1);
#else
  /* fall trough for all other stmts */
  return;
#endif
}

void DFTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* expr, Lattice& element) {
  transferExpression(lab,expr,element);
}



void DFTransferFunctions::transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferEmptyStmt(Label label, SgStatement* stmt, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferSwitchCase(Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate) {
  // default identity function
}

void DFTransferFunctions::transferSwitchDefault(Label lab,SgStatement* condStmt, SgDefaultOptionStmt* caseStmt,Lattice& pstate) {
  // default identity function
}

void DFTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferExternalFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element) {
  // default function implementation
  VariableId varId;
  if(lhsVar) {
    varId=getVariableIdMapping()->variableId(lhsVar);
  }
  // for void functions, varId remains invalid
  transferFunctionCallReturn(lab,varId,callExp,element);
}

void DFTransferFunctions::transferFunctionCallReturn(Label lab, VariableId varId, SgFunctionCallExp* callExp, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::addParameterPassingVariables() {
  std::string nameprefix="$p";
  /* this variable is necessary to know the id-range where parameter
     passing variable-ids are starting in the id-range.
  */
  parameter0VariableId=getVariableIdMapping()->createUniqueTemporaryVariableId(nameprefix+"0");
  for(int i=1;i<20;i++) {
    std::stringstream ss;
    ss<<nameprefix<<i;
    string varName=ss.str();
    getVariableIdMapping()->createUniqueTemporaryVariableId(varName);
  }
  resultVariableId=getVariableIdMapping()->createUniqueTemporaryVariableId("$r");
}

VariableId DFTransferFunctions::getParameterVariableId(int paramNr) {
  if(paramNr>=20) {
    cerr<<"Error: only 20 formal parameters are supported."<<endl;
    exit(1);
  }
  int idCode = parameter0VariableId.getIdCode();
  int elemIdCode = idCode + paramNr;
  VariableId elemVarId;
  elemVarId.setIdCode(elemIdCode);
  return elemVarId;
}

VariableId DFTransferFunctions::getResultVariableId() {
  return resultVariableId;
}

void DFTransferFunctions::setSkipSelectedFunctionCalls(bool flag) {
  _skipSelectedFunctionCalls=flag;
}

bool DFTransferFunctions::getSkipSelectedFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

bool DFTransferFunctions::isExternalFunctionCall(Label lab) {
  return _programAbstractionLayer->getLabeler()->isExternalFunctionCallLabel(lab);
}

void DFTransferFunctions::setInitialElementFactory(PropertyStateFactory* pf) {
  _initialElementFactory=pf;
}

PropertyStateFactory* DFTransferFunctions::getInitialElementFactory() {
  return _initialElementFactory;
}

void DFTransferFunctions::initializeExtremalValue(Lattice& element) {
  // default empty function
}

Lattice* DFTransferFunctions::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(root);
  cout << "INFO: Initializing property state with global variables."<<endl;
  VariableIdSet globalVars=AnalysisAbstractionLayer::globalVariables(root,getVariableIdMapping());
  VariableIdSet usedVarsInFuncs=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,getVariableIdMapping());
  VariableIdSet usedVarsInGlobalVarsInitializers=AnalysisAbstractionLayer::usedVariablesInGlobalVariableInitializers(root,getVariableIdMapping());
  VariableIdSet usedGlobalVarIds=globalVars; //*usedVarsInFuncs; //+usedVarsInGlobalVarsInitializers;;
  //  usedGlobalVarIds.insert(usedVarsInGlobalVarsInitializers.begin(),
  //        usedVarsInGlobalVarsInitializers.end());
  cout <<"INFO: number of global variables: "<<globalVars.size()<<endl;
  //  cout <<"INFO: used variables in functions: "<<usedVarsInFuncs.size()<<endl;
  //cout <<"INFO: used global vars: "<<usedGlobalVarIds.size()<<endl;
  Lattice* elem=_initialElementFactory->create();
  this->initializeExtremalValue(*elem);
  //cout << "INIT: initial element: ";elem->toStream(cout,getVariableIdMapping());
  list<SgVariableDeclaration*> globalVarDecls=SgNodeHelper::listOfGlobalVars(root);
  for(list<SgVariableDeclaration*>::iterator i=globalVarDecls.begin();i!=globalVarDecls.end();++i) {
    if(usedGlobalVarIds.find(getVariableIdMapping()->variableId(*i))!=usedGlobalVarIds.end()) {
      //cout<<"DEBUG: transfer for global var @"<<_labeler->getLabel(*i)<<" : "<<(*i)->unparseToString()<<endl;
      transfer(getLabeler()->getLabel(*i),*elem);
    } else {
      cout<<"INFO: filtered from initial state: "<<(*i)->unparseToString()<<endl;
    }
  }
  //cout << "INIT: initial state: ";
  //elem->toStream(cout,getVariableIdMapping());
  //cout<<endl;
  return elem;
}

#endif
