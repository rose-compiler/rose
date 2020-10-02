/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "sageGeneric.h"
#include "sageInterface.h"

#include "CFAnalysis.h"
#include "Labeler.h"
#include "AstTerm.h"
#include <boost/foreach.hpp>
#include "CodeThornException.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

namespace si = SageInterface;

CFAnalysis::FunctionResolutionMode CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_CALL_MAPPING;

Sawyer::Message::Facility CFAnalysis::logger;
void CFAnalysis::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CFAnalysis", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

CFAnalysis::CFAnalysis(Labeler* l):labeler(l),_createLocalEdge(false){
  initDiagnostics();
}
CFAnalysis::CFAnalysis(Labeler* l, bool createLocalEdge):labeler(l),_createLocalEdge(createLocalEdge){
  initDiagnostics();
}
void CFAnalysis::setCreateLocalEdge(bool createLocalEdge) {
  _createLocalEdge=createLocalEdge;
}
bool CFAnalysis::getCreateLocalEdge() {
  return _createLocalEdge;
}
size_t CFAnalysis::deleteFunctionCallLocalEdges(Flow& flow) {
  cerr<<"Internal error: deleteFunctionCallLocalEdges called."<<endl;
  ROSE_ASSERT(false);
  // TODO: investigate why this is not deleting edges
  return flow.deleteEdges(EDGE_LOCAL);
}

// MS: TODO: refactor thse two functions  (1/2)
LabelSet CFAnalysis::functionCallLabels(Flow& flow) {
  LabelSet resultSet;
  LabelSet nodeLabels;
#ifdef ALTERNATIVE_LOCAL_EDGE_HANDLING
  nodeLabels=flow.nodeLabels();
  for(LabelSet::iterator i=nodeLabels.begin();i!=nodeLabels.end();++i) {
    if(labeler->isFunctionCallLabel(*i))
      resultSet.insert(*i);
  }
#else
  // workaround: iterate over all labels to find also non-connected call nodes
  for(Labeler::iterator i=getLabeler()->begin();i!=getLabeler()->end();++i) {
    if(labeler->isFunctionCallLabel(*i))
      resultSet.insert(*i);
  }
#endif

  return resultSet;
}

// MS: TODO: refactor thse two functions  (2/2)
LabelSet CFAnalysis::conditionLabels(Flow& flow) {
  LabelSet resultSet;
  LabelSet nodeLabels;
  nodeLabels=flow.nodeLabels();
  for(LabelSet::iterator i=nodeLabels.begin();i!=nodeLabels.end();++i) {
    if(labeler->isConditionLabel(*i))
      resultSet.insert(*i);
  }
  return resultSet;
}

LabelSet CFAnalysis::functionEntryLabels(Flow& flow) {
  LabelSet resultSet;
  LabelSet nodeLabels;
  nodeLabels=flow.nodeLabels();
  for(LabelSet::iterator i=nodeLabels.begin();i!=nodeLabels.end();++i) {
    if(labeler->isFunctionEntryLabel(*i))
      resultSet.insert(*i);
  }
  return resultSet;
}

Label CFAnalysis::correspondingFunctionExitLabel(Label entryLabel) {
  ROSE_ASSERT(getLabeler()->isFunctionEntryLabel(entryLabel));
  SgNode* fdefnode=getNode(entryLabel);
  ROSE_ASSERT(fdefnode);
  return getLabeler()->functionExitLabel(fdefnode);
}

int CFAnalysis::numberOfFunctionParameters(Label entryLabel) {
  ROSE_ASSERT(getLabeler()->isFunctionEntryLabel(entryLabel));
  SgNode* fdefnode=getNode(entryLabel);
  ROSE_ASSERT(fdefnode);
  SgInitializedNamePtrList& params=SgNodeHelper::getFunctionDefinitionFormalParameterList(fdefnode);
  return params.size();
}

bool CFAnalysis::isVoidFunction(Label entryLabel) {
  ROSE_ASSERT(getLabeler()->isFunctionEntryLabel(entryLabel));
  SgNode* fdefnode=getNode(entryLabel);
  ROSE_ASSERT(fdefnode);
  return isSgTypeVoid(SgNodeHelper::getFunctionReturnType(fdefnode));
}

LabelSetSet CFAnalysis::functionLabelSetSets(Flow& flow) {
  LabelSetSet result;
  LabelSet feLabels=functionEntryLabels(flow);
  for(LabelSet::iterator i=feLabels.begin();i!=feLabels.end();++i) {
    Label entryLabel=*i;
    LabelSet fLabels=functionLabelSet(entryLabel, flow);
    result.insert(fLabels);
  }
  return result;
}

// TODO?: move this function to Flow
InterFlow::LabelToFunctionMap CFAnalysis::labelToFunctionMap(Flow& flow) {
  InterFlow::LabelToFunctionMap map;
  LabelSet entryLabels=functionEntryLabels(flow);
  for(auto entryLabel : entryLabels) {
    LabelSet insideFunctionLabels=functionLabelSet(entryLabel,flow);
    for(auto funLocLabel : insideFunctionLabels) {
      // need all labels for verification of functions
      map[funLocLabel]=entryLabel;
    }
  }
  return map;
}

LabelSet CFAnalysis::functionLabelSet(Label entryLabel, Flow& flow) {
  LabelSet fLabels;
  SgNode* functionDef=getLabeler()->getNode(entryLabel);
  RoseAst ast(functionDef);
  for(auto node : ast) {
    if(!isSgBasicBlock(node)) {
      Label lab=getLabeler()->getLabel(node);
      if(lab.isValid()) {
        fLabels.insert(lab);
      }
    }
  }
  return fLabels;
}

template <class SageNode>
SageNode*
definingDecl(SageNode* decl)
{
  return sg::assert_sage_type<SageNode>(decl->get_definingDeclaration());
}

InterFlow CFAnalysis::interFlow2(Flow& flow) {
  ROSE_ASSERT(functionResolutionMode == FRM_FUNCTION_CALL_MAPPING);
  // 1) for each call use AST information to find its corresponding called function
  // 2) create a set of <call,entry,exit,callreturn> edges
  SAWYER_MESG(logger[INFO])<<"establishing inter-flow 2 ..."<<endl;
  InterFlow interFlow;
  LabelSet callLabs=functionCallLabels(flow);
  int callLabsNum=callLabs.size();
  SAWYER_MESG(logger[INFO])<<"number of function call labels: "<<callLabsNum<<endl;
  
  for(LabelSet::iterator i=callLabs.begin();i!=callLabs.end();++i) {
    SgNode* callNode=getNode(*i);
    
    // filtering for templated code is not strictly necessary
    //   but it avoids misleading logging output. 
    if (insideTemplatedCode(callNode))
      continue;

    SgNodeHelper::ExtendedCallInfo callInfo = SgNodeHelper::matchExtendedNormalizedCall(callNode);
    if(!callInfo)
    {
      SAWYER_MESG(logger[ERROR]) << callNode->unparseToString() << std::endl;
      throw CodeThorn::Exception("interFlow2: unknown call expression");
    }

    switch(functionResolutionMode) {
    case FRM_FUNCTION_CALL_MAPPING: {
      FunctionCallTargetSet funCallTargetSet=determineFunctionDefinition5(*i, callInfo.representativeNode());
      Label callLabel,entryLabel,exitLabel,callReturnLabel;
      if(funCallTargetSet.size()==0) {
        SAWYER_MESG(logger[WARN]) << "undefined call target: " << callNode->unparseToString() 
                     << " <" << typeid(*callNode).name() << ">"
                     << std::endl;
        callLabel=*i;
        entryLabel=Labeler::NO_LABEL;
        exitLabel=Labeler::NO_LABEL;
        callReturnLabel=labeler->functionCallReturnLabel(callNode);
        interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
      } else {
        SAWYER_MESG(logger[TRACE]) << "defined call target: " << callNode->unparseToString() 
                      << " <" << typeid(*callNode).name() << ">"
                      << std::endl;
        for(auto fct : funCallTargetSet) {
          callLabel=*i;
          SgFunctionDefinition* funDef=fct.getDefinition();
          if(funDef) {
            entryLabel=labeler->functionEntryLabel(funDef);
            exitLabel=labeler->functionExitLabel(funDef);
          } else {
            entryLabel=Labeler::NO_LABEL;
            exitLabel=Labeler::NO_LABEL;
          }
          callReturnLabel=labeler->functionCallReturnLabel(callNode);
          interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
          
          //~ SAWYER_MESG(logger[TRACE]) << "iflow2b " 
                        //~ << callLabel << ", " << entryLabel << ", " << exitLabel << ", " << callReturnLabel
                        //~ << std::endl;
        }
      }
      break;
    }
    default:
      SAWYER_MESG(logger[ERROR])<<endl<<"Unsupported function resolution mode."<<endl;
      exit(1);
    }
  }
  return interFlow;
}


InterFlow CFAnalysis::interFlow(Flow& flow) {
  if (SgNodeHelper::WITH_EXTENDED_NORMALIZED_CALL)
    return interFlow2(flow);

  // 1) for each call use AST information to find its corresponding called function
  // 2) create a set of <call,entry,exit,callreturn> edges
  SAWYER_MESG(logger[INFO])<<"establishing inter-flow ..."<<endl;
  InterFlow interFlow;
  LabelSet callLabs=functionCallLabels(flow);
  int callLabsNum=callLabs.size();
  SAWYER_MESG(logger[INFO])<<"number of function call labels: "<<callLabsNum<<endl;
  int callLabNr=0;
  int externalFunCalls=0;
  int externalFunCallsWithoutDecl=0;
  int functionsFound=0;

  for(LabelSet::iterator i=callLabs.begin();i!=callLabs.end();++i) {
    //cout<<"INFO: resolving function call "<<callLabNr<<" of "<<callLabs.size()<<endl;
    SgNode* callNode=getNode(*i);
    //cout<<"INFO: Functioncall: creating inter-flow for "<<callNode->unparseToString()<<endl;
    //info: callNode->get_args()
    SgFunctionCallExp *funCall=SgNodeHelper::Pattern::matchFunctionCall(callNode);
    if(!funCall)
      throw CodeThorn::Exception("interFlow: unknown call exp (not a SgFunctionCallExp)");
#if 0
    //SgFunctionDeclaration* funDecl=funCall->getAssociatedFunctionDeclaration();
    //SgFunctionSymbol* funSym=funCall->getAssociatedFunctionSymbol();
    //SgType* funCallType=funCall->get_type(); // return type
    SgExpression* funExp=funDecl->get_function();
    ROSE_ASSERT(funExp);
    SgType* funExpType=funExp->get_type();
    ROSE_ASSERT(funExpType);
    SgFunctionType* funType=isSgFunctionType(funExpType);
    if(funType) {
      SgFunctionParameterTypeList* funParamTypeList=funType->get_argument_list();
    }
#endif
    if(functionResolutionMode!=FRM_FUNCTION_CALL_MAPPING)
      SAWYER_MESG(logger[TRACE])<<"Resolving function call: "<<funCall<<": "<<funCall->unparseToString()<<": ";
    SgFunctionDefinition* funDef=nullptr;
    FunctionCallTargetSet funCallTargetSet;
    switch(functionResolutionMode) {
    case FRM_TRANSLATION_UNIT: funDef=SgNodeHelper::determineFunctionDefinition(funCall);break;
    case FRM_WHOLE_AST_LOOKUP: funDef=determineFunctionDefinition2(funCall);break;
    case FRM_FUNCTION_ID_MAPPING: funDef=determineFunctionDefinition3(funCall);break;
    case FRM_FUNCTION_CALL_MAPPING: {
      funCallTargetSet=determineFunctionDefinition4(funCall);
      Label callLabel,entryLabel,exitLabel,callReturnLabel;
      if(funCallTargetSet.size()==0) {
        callLabel=*i;
        entryLabel=Labeler::NO_LABEL;
        exitLabel=Labeler::NO_LABEL;
        callReturnLabel=labeler->functionCallReturnLabel(callNode);
        interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
      } else {
        for(auto fct : funCallTargetSet) {
          callLabel=*i;
          SgFunctionDefinition* funDef=fct.getDefinition();
          if(funDef) {
            entryLabel=labeler->functionEntryLabel(funDef);
            exitLabel=labeler->functionExitLabel(funDef);
          } else {
            entryLabel=Labeler::NO_LABEL;
            exitLabel=Labeler::NO_LABEL;
          }
          callReturnLabel=labeler->functionCallReturnLabel(callNode);
          interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
        }
      }
      break;
    }
    default:
      SAWYER_MESG(logger[ERROR])<<endl<<"Unsupported function resolution mode."<<endl;
      exit(1);
    }

    if(functionResolutionMode!=FRM_FUNCTION_CALL_MAPPING) {
      Label callLabel,entryLabel,exitLabel,callReturnLabel;
      if(funDef==0) {
        //cout<<" [no definition found]"<<endl;
        // we were not able to find the funDef in the AST
        //cout << "STATUS: External function ";
        SgFunctionDeclaration* funDecl=funCall->getAssociatedFunctionDeclaration();
        if(funDecl) {
          //cout << "External function: "<<SgNodeHelper::getFunctionName(funDecl)<<"."<<endl;
          externalFunCalls++;
        } else {
          //cout << "No function declaration found (call:"<<funCall->unparseToString()<<endl;
          externalFunCallsWithoutDecl++;
        }
        callLabel=*i;
        entryLabel=Labeler::NO_LABEL;
        exitLabel=Labeler::NO_LABEL;
        callReturnLabel=labeler->functionCallReturnLabel(callNode);
        //cout <<"No function definition found for call: "<<funCall->unparseToString()<<endl;
      } else {
        //cout<<"Found function: "<<SgNodeHelper::getFunctionName(funDef)<<endl;
        callLabel=*i;
        entryLabel=labeler->functionEntryLabel(funDef);
        exitLabel=labeler->functionExitLabel(funDef);
        callReturnLabel=labeler->functionCallReturnLabel(callNode);
        functionsFound++;
      }
      interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
      callLabNr++;
      //cout<<"STATUS: inter-flow established."<<endl;
      //cout<<"INFO: Call labels: "<<callLabNr<<endl;
      //cout<<"INFO: externalFunCalls: "<<externalFunCalls<<endl;
      //cout<<"INFO: externalFunCallWitoutDecl: "<<externalFunCallsWithoutDecl<<endl;
      //cout<<"INFO: functions found: "<<functionsFound<<endl;
    }
  }
  return interFlow;
}


Label CFAnalysis::getLabel(SgNode* node) {
  ROSE_ASSERT(labeler);
  return labeler->getLabel(node);
}

SgNode* CFAnalysis::getNode(Label label) {
  ROSE_ASSERT(labeler);
  return labeler->getNode(label);
}

Labeler* CFAnalysis::getLabeler() {
  return labeler;
}

// returns 0 if no statement (other than SgBasicBlock) exists in block.
SgStatement* CFAnalysis::getFirstStmtInBlock(SgBasicBlock* block) {
  ROSE_ASSERT(block);
  const SgStatementPtrList& stmtList=block->get_statements();
  if(stmtList.size()>=1) {
    return stmtList.front();
  } else {
    return 0;
  }
}

// returns 0 if no statement (other than SgBasicBlock) exists in block.
SgStatement* CFAnalysis::getLastStmtInBlock(SgBasicBlock* block) {
  ROSE_ASSERT(block);
  const SgStatementPtrList& stmtList=block->get_statements();
  if(stmtList.size()>=1) {
    return stmtList.back();
  } else {
    return 0;
  }
}

Label CFAnalysis::initialLabel(SgNode* node) {
  ROSE_ASSERT(node);

  // special case of incExpr in SgForStatement
  if(SgNodeHelper::isForIncExpr(node))
    return labeler->getLabel(node);

  // special case of function call
  if(SgNodeHelper::matchExtendedNormalizedCall(node) || SgNodeHelper::Pattern::matchFunctionCall(node))
    return labeler->getLabel(node);

  if(!labeler->numberOfAssociatedLabels(node)) {
    throw std::logic_error("Error: icfg construction: not label relevant node: "+node->class_name());
  }
  ROSE_ASSERT(labeler->numberOfAssociatedLabels(node));
  switch (node->variantT()) {
  case V_SgFunctionDeclaration:
    cerr<<"Error: icfg construction: function declarations are not associated with a label."<<endl;
    exit(1);
  case V_SgTryStmt: // PP (09/04/20)
  case V_SgNullStatement:
  case V_SgPragmaDeclaration:
  case V_SgLabelStatement:
    return labeler->getLabel(node);
  case V_SgFunctionDefinition:
  case V_SgBreakStmt:
  case V_SgContinueStmt:
  case V_SgReturnStmt:
  case V_SgCaseOptionStmt:
  case V_SgDefaultOptionStmt:
      return labeler->getLabel(node);

  case V_SgVariableDeclaration:
  case V_SgClassDeclaration:
  case V_SgEnumDeclaration:
      return labeler->getLabel(node);

  case V_SgExprStatement: {
    return labeler->getLabel(node);
  }
  case V_SgInitializedName:
    return labeler->getLabel(node);
  case V_SgIfStmt:
  case V_SgWhileStmt:
    node=SgNodeHelper::getCond(node);
    return labeler->getLabel(node);
  case V_SgDoWhileStmt:
    node=SgNodeHelper::getLoopBody(node);
    return labeler->getLabel(node);
  case V_SgBasicBlock:
   return labeler->blockBeginLabel(node);

   // TODO: for(emptyInitList;S;S) {}
  case V_SgForStatement: {
    SgStatementPtrList& stmtPtrList=SgNodeHelper::getForInitList(node);
    if(stmtPtrList.size()==0) {
      // empty initializer list (hence, an initialization stmt cannot be initial stmt of for)
      throw CodeThorn::Exception("Error: for-stmt: initializer-list is empty. Not supported.");
    }
    ROSE_ASSERT(stmtPtrList.size()>0);
    node=*stmtPtrList.begin();
    return labeler->getLabel(node);
  }
  case V_SgGotoStatement: {
    return labeler->getLabel(node);
  }
  case V_SgAsmStmt: {
    return labeler->getLabel(node);
  }
  case V_SgSwitchStatement: {
    node=SgNodeHelper::getCond(node);
    ROSE_ASSERT(node);
    return labeler->getLabel(node);
  }

    // all omp statements
  case V_SgOmpParallelStatement: {
    return labeler->forkLabel(node);
  }
  case V_SgOmpForSimdStatement:
  case V_SgOmpSimdStatement:
  case V_SgOmpSectionsStatement:
  case V_SgOmpForStatement: {
    return labeler->workshareLabel(node);
  }
  case V_SgOmpBarrierStatement: {
    return labeler->barrierLabel(node);
  }
  case V_SgOmpTargetStatement:
  case V_SgOmpSectionStatement:
  case V_SgOmpAtomicStatement:
  case V_SgOmpCriticalStatement:
  case V_SgOmpDoStatement:
  case V_SgOmpFlushStatement:
  case V_SgOmpMasterStatement:
  case V_SgOmpOrderedStatement:
  case V_SgOmpSingleStatement:
  case V_SgOmpTargetDataStatement:
  case V_SgOmpTaskStatement:
  case V_SgOmpTaskwaitStatement:
  case V_SgOmpThreadprivateStatement:
  case V_SgOmpWorkshareStatement:
    return labeler->getLabel(node);

    // special case
  case V_SgTypedefDeclaration:
    return labeler->getLabel(node);

  case V_SgFunctionCallExp:
    // the first label of a function call is the CALL label.
    return labeler->getLabel(node);

  default:
    cerr << "Error: Unknown xnode in CodeThorn::CFAnalysis::initialLabel: "<<node->sage_class_name()<<endl;
    exit(1);
  }
}

SgStatement* CFAnalysis::getCaseOrDefaultBodyStmt(SgNode* node) {
  SgStatement* body=0;
  if(SgCaseOptionStmt* stmt=isSgCaseOptionStmt(node)) {
    body=stmt->get_body();
  } else if(SgDefaultOptionStmt* stmt=isSgDefaultOptionStmt(node)) {
    body=stmt->get_body();
  } else {
    cerr<<"Error: requesting body of switch case or default, but node is not a default or option stmt."<<endl;
    exit(1);
  }
  ROSE_ASSERT(body);
  return body;
}

LabelSet CFAnalysis::finalLabels(SgNode* node) {
  ROSE_ASSERT(node);
  ROSE_ASSERT(labeler->numberOfAssociatedLabels(node));
  LabelSet finalSet;

  // special case of incExpr in SgForStatement
  if(SgNodeHelper::isForIncExpr(node)) {
    finalSet.insert(labeler->getLabel(node));
    return finalSet;
  }

  // special case of function call
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
      finalSet.insert(labeler->functionCallReturnLabel(node)+1); // special case of function call in return-stmt
    } else {
      finalSet.insert(labeler->functionCallReturnLabel(node));
    }
    return finalSet;
  }
  
  if (SgNodeHelper::matchExtendedNormalizedCall(node))
  {
    finalSet.insert(labeler->functionCallReturnLabel(node));
    return finalSet;
  }

  switch (node->variantT()) {
  // function declarations inside basic block
  case V_SgFunctionDeclaration:
    cerr<<"Error: icfg construction: function declarations are not associated with a label."<<endl;
    exit(1);
    //finalSet.insert(labeler->getLabel(node));
    //return finalSet;
  case V_SgFunctionDefinition: {
    SgBasicBlock* body=isSgFunctionDefinition(node)->get_body();
    return finalLabels(body);
  }
  case V_SgTryStmt: { // PP
    SgStatement* body=isSgTryStmt(node)->get_body(); 
    return finalLabels(body);
  }
  case V_SgBreakStmt:
  case V_SgContinueStmt:
    return finalSet;
  case V_SgReturnStmt:
    return finalSet;
  case V_SgLabelStatement: {
    // MS 2/15/2018: added support for new AST structure in ROSE: SgLabelStatement(child).
    SgStatement* child=isSgLabelStatement(node)->get_statement();
    if(child) {
      LabelSet s=finalLabels(child);
      finalSet+=s;
    } else {
      finalSet.insert(labeler->getLabel(node));
    }
    return finalSet;
  }
  case V_SgNullStatement:
  case V_SgPragmaDeclaration:
  case V_SgInitializedName:
    finalSet.insert(labeler->getLabel(node));
    return finalSet;
    // declarations
  case V_SgVariableDeclaration:
  case V_SgClassDeclaration:
  case V_SgEnumDeclaration:
    finalSet.insert(labeler->getLabel(node));
    return finalSet;
  case V_SgDefaultOptionStmt:
  case V_SgCaseOptionStmt: {
    // MS 2/15/2018: added support for new AST structure in ROSE
    SgStatement* child=getCaseOrDefaultBodyStmt(node);
    if(child) {
      LabelSet s=finalLabels(child);
      finalSet+=s;
    } else {
      finalSet.insert(labeler->getLabel(node));
    }
    return finalSet;
  }
  case V_SgExprStatement: {
    finalSet.insert(labeler->getLabel(node));
    return finalSet;
  }
  case V_SgIfStmt: {
    SgNode* nodeTB=SgNodeHelper::getTrueBranch(node);
    LabelSet finalSetTB=finalLabels(nodeTB);
    finalSet+=finalSetTB;
    if(SgNode* nodeFB=SgNodeHelper::getFalseBranch(node)) {
      LabelSet finalSetFB=finalLabels(nodeFB);
      finalSet+=finalSetFB;
    } else {
      // in case of an empty else branch the cond node becomes the final node
      SgNode* condNode=SgNodeHelper::getCond(node);
      finalSet.insert(labeler->getLabel(condNode));
    }
    return finalSet;
  }
  case V_SgForStatement:
  case V_SgDoWhileStmt:
  case V_SgWhileStmt: {
    SgNode* condNode=SgNodeHelper::getCond(node);
    finalSet.insert(labeler->getLabel(condNode));
    set<SgNode*> breakNodes=SgNodeHelper::loopRelevantBreakStmtNodes(node);
    LabelSet lset=labeler->getLabelSet(breakNodes);
    finalSet+=lset;
    return finalSet;
  }
  case V_SgBasicBlock: {
    if(SgNodeHelper::numChildren(node)>0) {
      SgNode* lastNode=SgNodeHelper::getLastOfBlock(node);
      LabelSet s=finalLabels(lastNode);
      finalSet+=s;
    } else {
      // empty basic block
      finalSet.insert(initialLabel(node));
    }
    return finalSet;
  }
  case V_SgFunctionCallExp:
    finalSet.insert(labeler->functionCallReturnLabel(node));
    return finalSet;
  case V_SgGotoStatement: {
    // for the goto statement (as special case) the final set is empty. This allows all other functions
    // operate correctly even in the presence of gotos. The edge for 'goto label' is created as part
    // of the semantics of goto (and does not *require* the final labels).
    return finalSet;
  }
  case V_SgSwitchStatement: {
    // 1) add all break statements, 2) add final label of last stmt (emulating a break)
    set<SgNode*> breakNodes=SgNodeHelper::loopRelevantBreakStmtNodes(node);
    LabelSet lset=labeler->getLabelSet(breakNodes);
    finalSet+=lset;
    // very last case in switch (not necessarily default), if it does not contain a break has still a final label.
    // if it is a break it will still be the last label. If it is a goto it will not have a final label (which is correct).
    SgSwitchStatement* switchStmt=isSgSwitchStatement(node);
    SgStatement* body=switchStmt->get_body();
    SgBasicBlock* block=isSgBasicBlock(body);

    // TODO: revisit, finalLabels(block) might be sufficient
    if(!block && !isSgStatement(node)) {
      cerr<<"Error: CFAnalysis::finalLabels: body of switch is not a basic block or stmt. Unknown structure."<<endl;
      exit(1);
    } else if(!block && isSgStatement(body)) {
      SgStatement* singleStmt=isSgStatement(body);
      LabelSet singleStatementLabels=finalLabels(singleStmt);
      finalSet+=singleStatementLabels;
      return finalSet;
    }

    const SgStatementPtrList& stmtList=block->get_statements();
    if(stmtList.size()>0) {
      SgNode* lastStmt=stmtList.back();
      LabelSet lsetLastStmt=finalLabels(lastStmt);
      finalSet+=lsetLastStmt;
    } else {
      cerr<<"Error: CFAnalysis::finalLabels: body of switch is empty."<<endl;
      exit(1);
    }
    return finalSet;
  }

  case V_SgOmpParallelStatement: {
    finalSet.insert(labeler->joinLabel(node));
    return finalSet;
  }
  case V_SgOmpSectionsStatement: {
    ROSE_ASSERT(isSgOmpClauseBodyStatement(node));
    // If sections are marked nowait, we need to connect all final labels of sections to successive / join node
    if (SgNodeHelper::hasOmpNoWait(isSgOmpClauseBodyStatement(node))) {
      auto sections = SgNodeHelper::getOmpSectionList(isSgOmpSectionsStatement(node));
      for (auto s : sections) {
        auto finals = finalLabels(s);
        finalSet += finals;
      }
    } else {
      // normally introduce barrier node and return its label as final label
      finalSet.insert(labeler->barrierLabel(node));
    }
    return finalSet;
  }

  case V_SgOmpSimdStatement:
  case V_SgOmpForSimdStatement:
  case V_SgOmpForStatement: {
    ROSE_ASSERT(isSgOmpClauseBodyStatement(node));
    // In case the workshare is marked with nowait ommit barrier
    if (SgNodeHelper::hasOmpNoWait(isSgOmpClauseBodyStatement(node))) {
      auto lbls = finalLabels(node->get_traversalSuccessorByIndex(0));
      finalSet += lbls;
    } else {
      // normally introduce barrier node and return its label as final label
      finalSet.insert(labeler->barrierLabel(node));
    }
    return finalSet;
  }

    // all omp statements
  case V_SgOmpSectionStatement:{
    auto body = node->get_traversalSuccessorByIndex(0);
    auto bodyFinals = finalLabels(body);
    finalSet += bodyFinals;
    return finalSet;
  }
  case V_SgOmpBarrierStatement: {
    finalSet.insert(labeler->barrierLabel(node));
    return finalSet;
  }

  case V_SgOmpTargetStatement:
  case V_SgOmpAtomicStatement: {
    auto atomicStmt = node->get_traversalSuccessorByIndex(0);
    auto atomicFinals = finalLabels(atomicStmt);
    finalSet += atomicFinals;
    return finalSet;
  }

  case V_SgOmpCriticalStatement:
  case V_SgOmpDoStatement:
  case V_SgOmpFlushStatement:
  case V_SgOmpMasterStatement:
  case V_SgOmpOrderedStatement:
  case V_SgOmpSingleStatement:
  case V_SgOmpTargetDataStatement:
  case V_SgOmpTaskStatement:
  case V_SgOmpTaskwaitStatement:
  case V_SgOmpThreadprivateStatement:
  case V_SgOmpWorkshareStatement:
    finalSet.insert(labeler->getLabel(node));
    return finalSet;

    // special case
  case V_SgTypedefDeclaration:
    return finalSet;

  case V_SgAsmStmt: {
    return finalSet;
  }

  default:
    cerr << "Error: Unknown node in CFAnalysis::finalLabels: "<<node->sage_class_name()<<endl; exit(1);
  }
}


Flow CFAnalysis::flow(SgNode* s1, SgNode* s2) {
  ROSE_ASSERT(s1);
  ROSE_ASSERT(s2);
  Flow flow12;
  Flow flow1=flow(s1);
  Flow flow2=flow(s2);
  flow12+=flow1;
  flow12+=flow2;
  LabelSet finalSets1=finalLabels(s1);
  Label initLabel2=initialLabel(s2);
  for(LabelSet::iterator i=finalSets1.begin();i!=finalSets1.end();++i) {

    // special case: case-blocks of switch: an edge between case-labels never goes
    // directly from case to case, but instead to the other case's following statement (to
    // model switch-case fallthrough). The edge directly to each case label is
    // created by the CFG creation for the switch node.
    SgNode* node=getNode(initLabel2);
    if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(node)) {
      // adjust init label to stmt following the case label (the child of the SgCaseStmt node).
      SgNode* body=caseStmt->get_body();
      ROSE_ASSERT(body);
      initLabel2=initialLabel(body);
    } else if(SgDefaultOptionStmt* caseStmt=isSgDefaultOptionStmt(node)) {
      SgNode* body=caseStmt->get_body();
      ROSE_ASSERT(body);
      initLabel2=initialLabel(body);
    }

    Edge e(*i,initLabel2);

    // special case FALSE edges of conditions (all TRUE edges are created by the respective CFG construction)
    if(SgNodeHelper::isCond(labeler->getNode(*i))) {
      e.addType(EDGE_FALSE);
    }
    e.addType(EDGE_FORWARD);
    flow12.insert(e);
  }
  return flow12;
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
int CFAnalysis::inlineTrivialFunctions(Flow& flow) {
  // 1) compute all functions that are called exactly once (i.e. number of pred in ICFG is 1)
  //    AND have the number of formal parameters is 0 AND have void return type.
  // 2) inline function
  // more advanced version will also clone function-CFGs, but this makes the mapping label<->code loose the 1-1 mapping property.
  int numInlined=0;
  LabelSet lnLabs=functionEntryLabels(flow);
  for(LabelSet::iterator i=lnLabs.begin();i!=lnLabs.end();++i) {
    LabelSet pred=flow.pred(*i);
    if(pred.size()==1) {
      Label lc=*pred.begin();
      ROSE_ASSERT(getLabeler()->isFunctionCallLabel(lc));
      // check the number of formal parameters of ln
      if(numberOfFunctionParameters(*i)==0 && isVoidFunction(*i)) {

        // reduce all four nodes: lc,ln,lx,lr (this also reduces a possibly existing local edge)
        Label ln=*i;
        Label lx=correspondingFunctionExitLabel(ln);
        LabelSet succ=flow.succ(lx);
        // since we have exactly one call there must be exactly one return edge
        ROSE_ASSERT(succ.size()==1);
        Label lr=*succ.begin();
        // reduce all four nodes now
        reduceNode(flow,lc);
        reduceNode(flow,ln);
        reduceNode(flow,lx);
        reduceNode(flow,lr);
        numInlined++;
      }
    }
  }
  return numInlined;
}

int CFAnalysis::reduceEmptyConditionNodes(Flow& flow) {
  LabelSet labs=conditionLabels(flow);
  int cnt=0;
  for(LabelSet::iterator i=labs.begin();i!=labs.end();++i) {
    if(flow.succ(*i).size()==1) {
      cnt+=reduceNode(flow,*i);
    }
  }
  return cnt;
}

int CFAnalysis::reduceNode(Flow& flow, Label lab) {
  Flow inFlow=flow.inEdges(lab);
  Flow outFlow=flow.outEdges(lab);
  // edge type cleanup
  // if true and false edge exist, remove both (merging true and false branches to a single branch)
  // if forward and backward exist, remove forward (we are not removing the cycle)

  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedge: (b,n2)
   *   remove(n_i,b)
   *   remove(b,n2)
   *   insert(n1,n2)
   */
  if(inFlow.size()==0 && outFlow.size()==0) {
    return 0;
  } else if(inFlow.size()>0 && outFlow.size()>0) {
    set<Edge> toErase;
    set<Edge> toInsert;
    for(Flow::iterator initer=inFlow.begin();initer!=inFlow.end();++initer) {
      for(Flow::iterator outiter=outFlow.begin();outiter!=outFlow.end();++outiter) {
        Edge e1=*initer;
        Edge e2=*outiter;
        // preserve edge annotations of ingoing and outgoing edges
        EdgeTypeSet unionEdgeTypeSet;
        EdgeTypeSet edgeTypeSet1=(*initer).types();
        unionEdgeTypeSet.insert(edgeTypeSet1.begin(),edgeTypeSet1.end());
        EdgeTypeSet edgeTypeSet2=(*outiter).types();
        // only copy an edge annotation in the outgoing edge if it is
        // not a true-annotation or a false-annotation
        for(EdgeTypeSet::iterator i=edgeTypeSet2.begin();i!=edgeTypeSet2.end();++i) {
          if(*i!=EDGE_TRUE && *i!=EDGE_FALSE) {
            unionEdgeTypeSet.insert(*i);
          }
        }
        if(unionEdgeTypeSet.find(EDGE_TRUE)!=unionEdgeTypeSet.end()
           && unionEdgeTypeSet.find(EDGE_FALSE)!=unionEdgeTypeSet.end()) {
          unionEdgeTypeSet.erase(EDGE_TRUE);
          unionEdgeTypeSet.erase(EDGE_FALSE);
        }
        if(unionEdgeTypeSet.find(EDGE_FORWARD)!=unionEdgeTypeSet.end()
           && unionEdgeTypeSet.find(EDGE_BACKWARD)!=unionEdgeTypeSet.end()) {
          unionEdgeTypeSet.erase(EDGE_FORWARD);
          // keep backward edge annotation
        }

        Edge newEdge=Edge(e1.source(),unionEdgeTypeSet,e2.target());
        toErase.insert(e1);
        toErase.insert(e2);
        if(e1.source()!=e2.target()) {
          toInsert.insert(newEdge);
        }
      }
    }
    for(set<Edge>::iterator i=toErase.begin();i!=toErase.end();++i) {
      flow.erase(*i);
    }
    for(set<Edge>::iterator i=toInsert.begin();i!=toInsert.end();++i) {
      flow.insert(*i);
    }
    return 1;
  } else if(inFlow.size()>0) {
    for(Flow::iterator initer=inFlow.begin();initer!=inFlow.end();++initer) {
      Edge e1=*initer;
      flow.erase(e1);
    }
    return 1;
  } else if(outFlow.size()>0) {
    for(Flow::iterator outiter=outFlow.begin();outiter!=outFlow.end();++outiter) {
      Edge e2=*outiter;
      flow.erase(e2);
    }
    return 1;
  }
  return 0;
}

int CFAnalysis::optimizeFlow(Flow& flow) {
  int n=0;
  // TODO: reduce: SgBreakStmt, SgContinueStmt, SgLabelStatement, SgGotoStatement
  n+=reduceBlockBeginNodes(flow);
  //n+=reduceEmptyConditionNodes(flow);
  return n;
}

int CFAnalysis::reduceBlockBeginEndNodes(Flow& flow) {
  int cnt=0;
  cnt+=reduceBlockBeginNodes(flow);
  cnt+=reduceBlockEndNodes(flow);
  return cnt;
}

int CFAnalysis::reduceBlockBeginNodes(Flow& flow) {
  LabelSet labs=flow.nodeLabels();
  int cnt=0;
  for(LabelSet::iterator i=labs.begin();i!=labs.end();++i) {
    if(labeler->isBlockBeginLabel(*i)) {
      cnt+=reduceNode(flow,*i);
    }
  }
  return cnt;
}

int CFAnalysis::reduceBlockEndNodes(Flow& flow) {
  LabelSet labs=flow.nodeLabels();
  int cnt=0;
  for(LabelSet::iterator i=labs.begin();i!=labs.end();++i) {
    if(labeler->isBlockEndLabel(*i)) {
      cnt+=reduceNode(flow,*i);
    }
  }
  return cnt;
}

void CFAnalysis::setInterProcedural(bool flag) {
  _interProcedural=flag;
}

bool CFAnalysis::getInterProcedural() {
  return _interProcedural;
}

int CFAnalysis::reduceToFunctionEntryNodes(Flow& flow) {
  LabelSet labs=flow.nodeLabels();
  int cnt=0;
  for(auto lab:labs) {
    if(!labeler->isFunctionEntryLabel(lab)) {
      cnt+=reduceNode(flow,lab);
    }
  }
  return cnt;
}

void CFAnalysis::createInterProceduralCallEdges(Flow& flow, InterFlow& interFlow) {
  for(InterFlow::iterator i=interFlow.begin();i!=interFlow.end();++i) {
    if((*i).entry==Labeler::NO_LABEL && (*i).exit==Labeler::NO_LABEL) {
      Edge externalEdge=Edge((*i).call,EDGE_EXTERNAL,(*i).callReturn);
      // register in Labeler as external function call
      getLabeler()->setExternalFunctionCallLabel((*i).call);
      flow.insert(externalEdge);
    } else {
      Edge callEdge=Edge((*i).call,EDGE_CALL,(*i).entry);
      flow.insert(callEdge);
      Edge callReturnEdge=Edge((*i).exit,EDGE_CALLRETURN,(*i).callReturn);
      flow.insert(callReturnEdge);

      if(_createLocalEdge) {
        Edge localEdge=Edge((*i).call,EDGE_LOCAL,(*i).callReturn);
        flow.insert(localEdge);
      }
    }
  }
}

void CFAnalysis::createIntraProceduralCallEdges(Flow& flow, InterFlow& interFlow) {
  // in case intra-procedural analysis is requested, model every function call as external
  for(InterFlow::iterator i=interFlow.begin();i!=interFlow.end();++i) {
    Edge externalEdge=Edge((*i).call,EDGE_EXTERNAL,(*i).callReturn);
    // register in Labeler as external function call
    getLabeler()->setExternalFunctionCallLabel((*i).call);
    flow.insert(externalEdge);
  }
}

void CFAnalysis::intraInterFlow(Flow& flow, InterFlow& interFlow) {
  if(getInterProcedural()) {
    SAWYER_MESG(logger[INFO])<<"Creating inter-procedural analysis."<<endl;
    createInterProceduralCallEdges(flow,interFlow);
  } else {
    SAWYER_MESG(logger[INFO])<<"Creating intra-procedural analysis."<<endl;
    createIntraProceduralCallEdges(flow,interFlow);
  }
}

bool CFAnalysis::isLoopConstructRootNode(SgNode* node) {
  return isSgWhileStmt(node)||isSgDoWhileStmt(node)||isSgForStatement(node);
}

// used to determine loop consttuct of  SgContinueStmt
// returns 0 if error (only possible in illformed AST)
SgNode* CFAnalysis::correspondingLoopConstruct(SgNode* node) {
  // find sourrounding loop construct (this is better implemented by
  // an init routine using an inherited attribute, but it's a very
  // short search on success (=number of nested blocks + nested branch constructs +1)
  while(!isLoopConstructRootNode(node)) {
    if(isSgFile(node))
      return 0;
    node=node->get_parent();
    if(node==0)
      return 0;
    ROSE_ASSERT(node);
  }
  ROSE_ASSERT(isLoopConstructRootNode(node));
  return node;
}

LabelSet CFAnalysis::labelsOfInterestSet() {
  LabelSet ls;
  Labeler& labeler=*getLabeler();
  for(auto l : labeler) {
    if(!(labeler.isBlockBeginLabel(l)||labeler.isBlockEndLabel(l))) {
      ls.insert(l);
    }
  }
  return ls;
}

LabelSet CFAnalysis::initialLabelsOfStmtsInBlockSet(SgNode* node) {
  LabelSet ls;
  if(node==0)
    return ls;
  if(!isSgStatement(node)) {
    //cerr<<"ERROR: "<<node->class_name()<<endl;
  }
  size_t len=node->get_numberOfTraversalSuccessors();
  for(size_t i=0;i<len;++i) {
    SgNode* childNode=node->get_traversalSuccessorByIndex(i);
    ls.insert(initialLabel(childNode));
  }
  return ls;
}

Flow CFAnalysis::controlDependenceGraph(Flow& controlFlow) {
  LabelSet condLabels=conditionLabels(controlFlow);
  LabelSet targetLabels;
  Flow controlDependenceEdges;
  for(LabelSet::iterator i=condLabels.begin();i!=condLabels.end();++i) {
    SgNode* condition=getLabeler()->getNode(*i);
    //cerr<<"DEBUG: cond:"<<condition->class_name()<<endl;
    SgNode* stmt=SgNodeHelper::getParent(condition);
    //cerr<<"DEBUG: stmt:"<<stmt->class_name()<<endl;
    // while/dowhile/for
    if(SgNodeHelper::isLoopCond(condition)) {
      SgNode* loopBody=SgNodeHelper::getLoopBody(stmt);
      //cerr<<"DEBUG: loopBody:"<<loopBody->class_name()<<endl;
      LabelSet loopBodyInitLabels=initialLabelsOfStmtsInBlockSet(loopBody);
      targetLabels=loopBodyInitLabels;
    }
    // if
    if(isSgIfStmt(stmt)) {
      SgNode* trueBranch=SgNodeHelper::getTrueBranch(stmt);
      LabelSet trueBranchInitLabels=initialLabelsOfStmtsInBlockSet(trueBranch);
      SgNode* falseBranch=SgNodeHelper::getFalseBranch(stmt);
      LabelSet falseBranchInitLabels=initialLabelsOfStmtsInBlockSet(falseBranch);
      targetLabels=trueBranchInitLabels+falseBranchInitLabels;
    }
    for(LabelSet::iterator j=targetLabels.begin();j!=targetLabels.end();++j) {
      controlDependenceEdges.insert(Edge(*i,EDGE_FORWARD,*j));
    }
  }
  return controlDependenceEdges;
}

Flow CFAnalysis::WhileAndDoWhileLoopFlow(SgNode* node,
                                         Flow edgeSet,
                                         EdgeType edgeTypeParam1,
                                         EdgeType edgeTypeParam2) {
  if(!(isSgWhileStmt(node) || isSgDoWhileStmt(node))) {
    throw CodeThorn::Exception("Error: WhileAndDoWhileLoopFlow: unsupported loop construct.");
  }
  SgNode* condNode=SgNodeHelper::getCond(node);
  Label condLabel=getLabel(condNode);
  SgNode* bodyNode=SgNodeHelper::getLoopBody(node);
  ROSE_ASSERT(bodyNode);
  Edge edge=Edge(condLabel,EDGE_TRUE,initialLabel(bodyNode));
  edge.addType(edgeTypeParam1);
  Flow flowB=flow(bodyNode);
  LabelSet finalSetB=finalLabels(bodyNode);
  edgeSet+=flowB;
  edgeSet.insert(edge);
  // back edges in while (forward edges in do-while)
  for(LabelSet::iterator i=finalSetB.begin();i!=finalSetB.end();++i) {
    Edge e;
    if(SgNodeHelper::isCond(labeler->getNode(*i))) {
      e=Edge(*i,EDGE_FALSE,condLabel);
      e.addType(edgeTypeParam2);
    } else {
      e=Edge(*i,edgeTypeParam2,condLabel);
    }
    edgeSet.insert(e);
  }
  return edgeSet;
}

namespace
{
  struct ExcludeFromCfg : sg::DispatchHandler<bool>
  {
    typedef sg::DispatchHandler<bool> base;
    
    ExcludeFromCfg()
    : base(false /* include in CFG */)
    {}
    
    void handle(SgNode&)                             { /* default = false */ }
    void handle(SgUsingDeclarationStatement&)        { res = true; }
    void handle(SgUsingDirectiveStatement&)          { res = true; }
    void handle(SgC_PreprocessorDirectiveStatement&) { res = true; }
    void handle(SgTypedefDeclaration&)               { res = true; /* includes subclasses */ }    
  };
}


Flow CFAnalysis::flow(SgNode* n) {
  SgNode* const node = n;
  ROSE_ASSERT(node);

  Flow edgeSet;
  if(node==0)
    return edgeSet;
  if(isSgFunctionDeclaration(node)) {
    return edgeSet;
  }

  if(isSgProject(node)||isSgFileList(node)||isSgGlobal(node)||isSgSourceFile(node)) {
    RoseAst ast(node);
    Flow tmpEdgeSet;
    // search for all functions and union flow for all functions
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      // schroder3 (2016-07-29): Added " && !isSgTemplateFunctionDefinition(*i)" to prevent
      //  SgTemplateFunctionDefinition nodes from being added to the ICFG. SgTemplateFunctionDefinitions
      //  are never called because they are only used in template declarations (SgTemplateDeclaration)
      //  (and not in template instantiation or specialization declarations (SgTemplateInstantiationFunctionDecl)).
      //  Template instantiation/ specialization declarations have "normal" SgFunctionDefinition nodes as
      //  corresponding definition. Even in case of an implicit instantiation of an implicit specialization ROSE
      //  creates a SgTemplateInstantiationFunctionDecl and copies the body of the SgTemplateFunctionDefinition
      //  to a new SgFunctionDefinition and uses the SgFunctionDefinition as definition.
      if(isSgFunctionDefinition(*i) && !isSgTemplateFunctionDefinition(*i)) {
        //cout << "STATUS: Generating flow for function "<<SgNodeHelper::getFunctionName(*i)<<endl;
        tmpEdgeSet=flow(*i);
        edgeSet+=tmpEdgeSet;
        // schroder3 (2016-07-12): We can not skip the children of a function definition
        //  because there might be a member function definition inside the function definition.
        //  Example:
        //   int main() {
        //     class A {
        //      public:
        //       void mf() {
        //         int i = 2;
        //       }
        //     };
        //   }
        //
        // MS 2018-04-05: we can skip children here, because flow does handle the body of function definitions
        i.skipChildrenOnForward();
      }
    }
    return edgeSet;
  }

  // special case of function call pattern
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
#ifdef ALTERNATIVE_LOCAL_EDGE_HANDLING
    // local edge for function call: call -> callReturn is added
    Label callLabel=labeler->functionCallLabel(node);
    Label callReturnLabel=labeler->functionCallReturnLabel(node);
    edgeSet.insert(Edge(callLabel,EDGE_LOCAL,callReturnLabel));
#else
    // 'local' edge is added when intraInter flow is computed
#endif
    // add special case edge for callReturn to returnNode SgReturnStmt(SgFunctionCallExp)
    // edge: SgFunctionCallExp.callReturn->init(SgReturnStmt)
    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
      Label callReturnLabel=labeler->functionCallReturnLabel(node);
      Label returnStmtLabel=labeler->functionCallReturnLabel(node)+1;
      edgeSet.insert(Edge(callReturnLabel,EDGE_FORWARD,returnStmtLabel));
    }
    return edgeSet;
  }
  
  if(SgNodeHelper::matchExtendedNormalizedCall(node)) {
    Label callLabel=labeler->functionCallLabel(node);
    Label callReturnLabel=labeler->functionCallReturnLabel(node);
    edgeSet.insert(Edge(callLabel,EDGE_FORWARD,callReturnLabel));
    return edgeSet;
  }

  switch (node->variantT()) {
  case V_SgFunctionDefinition: {
    Sg_File_Info* fi = node->get_file_info();
    SAWYER_MESG(logger[INFO])<<"Building CFG for function: "<<SgNodeHelper::getFunctionName(node)<< endl;
    // PP (04/09/20)
    // do nothing for function definitions that did not receive a label
    // e.g., templated functions
    Label entryLabel = labeler->functionEntryLabel(node);
    if (!entryLabel.isValid())
      return edgeSet;
    
    SgBasicBlock* body=isSgFunctionDefinition(node)->get_body();
    Edge edge=Edge(entryLabel,EDGE_FORWARD,initialLabel(body));
    edgeSet.insert(edge);
    Flow bodyFlow=flow(body);
    edgeSet+=bodyFlow;
    /* add explicitly edge from last stmt of function to exit-node
       not that a return at the end of a function is *not* represented in a ROSE AST.
       if return does not exist, we need to add an explicit edge from end of function to exit
       since we create this edge with the computed final node a branch with returns in all branches is
       maintained properly.
       this edge is identical if we have some other branches. Because we maintain the edges as an edge-set
       this operation is always OK.
     */
#if 0
    LabelSet funFinalLabels=finalLabels(node);
#else
    LabelSet funFinalLabels=finalLabels(body);
#endif
    for(LabelSet::iterator i=funFinalLabels.begin();i!=funFinalLabels.end();++i) {
      Edge explicitEdge=Edge(*i,EDGE_FORWARD,labeler->functionExitLabel(node));
      if(SgNodeHelper::isLoopCond(labeler->getNode(*i))) {
        explicitEdge.addType(EDGE_FALSE);
      }
      if(SgNodeHelper::isCond(labeler->getNode(*i))) {
        if(SgIfStmt* ifStmt=isSgIfStmt(labeler->getNode(*i)->get_parent())) {
          if(!SgNodeHelper::getFalseBranch(ifStmt)) {
            // MS: 07/02/2014: in case of empty if-false branch (at end of function), FALSE must be added to explicit node (only if-false can be empty)
            explicitEdge.addType(EDGE_FALSE);
          }
        }
      }
      edgeSet.insert(explicitEdge);
    }
    return edgeSet;
  }
  case V_SgReturnStmt: {
    SgNode* funcDef=SgNodeHelper::correspondingSgFunctionDefinition(node);
    if(!funcDef)
      cerr << "Error: No corresponding function for ReturnStmt found."<<endl;
    ROSE_ASSERT(isSgFunctionDefinition(funcDef));
    Edge edge=Edge(getLabel(node),EDGE_FORWARD,labeler->functionExitLabel(funcDef));
    edgeSet.insert(edge);
    return edgeSet;
  }
  case V_SgLabelStatement: {
    // MS 2/15/2018: added support for new AST structure in ROSE: SgLabelStatement(child).
    SgStatement* child=isSgLabelStatement(node)->get_statement();
    if(child) {
      Edge edge=Edge(getLabel(node),EDGE_FORWARD,initialLabel(child));
      edgeSet.insert(edge);
      Flow flowSgLabelChild=flow(child);
      edgeSet+=flowSgLabelChild;
    }
    return edgeSet;
  }
  case V_SgUsingDeclarationStatement: // PP
  case V_SgBreakStmt:
  case V_SgInitializedName:
  case V_SgNullStatement:
  case V_SgPragmaDeclaration:
  case V_SgExprStatement:
    return edgeSet;

    // declarations
  case V_SgVariableDeclaration:
  case V_SgClassDeclaration:
  case V_SgEnumDeclaration:
    return edgeSet;

  // Code duplication only for easy distinction between OMP parallel and OMP for (could be combined easily)
  case V_SgOmpParallelStatement: {
    SgNode *nextNestedStmt = node->get_traversalSuccessorByIndex(0);
    auto nextFlow = flow(nextNestedStmt);
    edgeSet += nextFlow;

    // Forward edge to connect nested body
    auto lab = labeler->forkLabel(node);
    auto e = Edge(lab, EDGE_FORWARD, initialLabel(nextNestedStmt));
    edgeSet.insert(e);

    // Edges connecting inner final labels with join node for proper indication of synchonization
    auto finals = finalLabels(nextNestedStmt);
    auto join = labeler->joinLabel(node);
    for (auto l : finals) {
      auto e = Edge(l, EDGE_FORWARD, join);
      edgeSet.insert(e);
    }
    return edgeSet;
  }

  case V_SgOmpForSimdStatement:
  case V_SgOmpSimdStatement:
  case V_SgOmpForStatement: {
    SgNode *nextNestedStmt = node->get_traversalSuccessorByIndex(0);
    auto nextFlow = flow(nextNestedStmt);
    edgeSet += nextFlow;

    // Forward edge to connect nested body
    auto lab = labeler->workshareLabel(node);
    auto e = Edge(lab, EDGE_FORWARD, initialLabel(nextNestedStmt));
    edgeSet.insert(e);

    // Edges connecting inner final labels with barrier node for proper indication of synchonization
    auto finals = finalLabels(nextNestedStmt);
    // omit edge to barrier node when nowait clause is given
    if (SgNodeHelper::hasOmpNoWait(isSgOmpClauseBodyStatement(node))) {
      return edgeSet;
    }
    // Introduce the edges to the implicit barrier
    auto barrier = labeler->barrierLabel(node);
    for (auto l : finals) {
      auto e = Edge(l, EDGE_FORWARD, barrier);
      if (SgNodeHelper::isCond(labeler->getNode(l))) {
        e.addType(EDGE_FALSE);
      }
      edgeSet.insert(e);
    }
    return edgeSet;
  }

  case V_SgOmpSectionsStatement: {
    // every statement in the basic block needs to be a SgOmpSectionStatement
    // don't construct the control flow for the basic block, because OMP semantics is different here
    auto lab = labeler->workshareLabel(node);
    auto sections = SgNodeHelper::getOmpSectionList(isSgOmpSectionsStatement(node));
    for (auto s : sections) {
      ROSE_ASSERT(isSgOmpSectionStatement(s));
      auto bodyFlow = flow(s);
      edgeSet += bodyFlow;
      auto e = Edge(lab, EDGE_FORWARD, initialLabel(s));
      edgeSet.insert(e);
    }

    // Omit the introduction of additional final->barrier edges when nowait is given
    if (SgNodeHelper::hasOmpNoWait(isSgOmpClauseBodyStatement(node))) {
      return edgeSet;
    }
    // Add the edge to the barrier node
    ROSE_ASSERT(isSgOmpSectionsStatement(node));
    auto bb = isSgBasicBlock(node->get_traversalSuccessorByIndex(0));
    auto barrier = labeler->barrierLabel(node);
    for (auto stmt : bb->get_statements()) {
      auto finals = finalLabels(stmt);
      for (auto l : finals) {
        auto e = Edge(l, EDGE_FORWARD, barrier);
        edgeSet.insert(e);
      }
    }
    return edgeSet;
  }

  case V_SgOmpTargetStatement:
  case V_SgOmpAtomicStatement:
  case V_SgOmpSectionStatement: {
    auto nextStmt = node->get_traversalSuccessorByIndex(0);
    auto bodyFlow = flow(nextStmt);
    edgeSet += bodyFlow;
    auto e = Edge(labeler->getLabel(node), EDGE_FORWARD, initialLabel(nextStmt));
    edgeSet.insert(e);
    return edgeSet;
  }


    // omp statements that introduce some sort of synchronization (no all implemented yet?)
  case V_SgOmpBarrierStatement:
  case V_SgOmpCriticalStatement:
  case V_SgOmpFlushStatement:
  case V_SgOmpMasterStatement:
  case V_SgOmpSingleStatement:
  // these omp statements do not generate edges in addition to the ingoing and outgoing edge
  case V_SgOmpDoStatement:
  case V_SgOmpOrderedStatement:
  case V_SgOmpTargetDataStatement:
  case V_SgOmpTaskStatement:
  case V_SgOmpTaskwaitStatement:
  case V_SgOmpThreadprivateStatement:
  case V_SgOmpWorkshareStatement:
    return edgeSet;

    // special case
  case V_SgTypedefDeclaration:
    return edgeSet;

  case V_SgAsmStmt: {
    // content of asm stmt is ignored
    return edgeSet;
  }

  case V_SgContinueStmt: {
    SgNode* loopStmt=correspondingLoopConstruct(node);
    if(isSgWhileStmt(loopStmt)) {
      // target is condition node
      SgNode* targetNode=SgNodeHelper::getCond(loopStmt);
      ROSE_ASSERT(targetNode);
      Edge edge=Edge(getLabel(node),EDGE_BACKWARD,getLabel(targetNode));
      edgeSet.insert(edge);
    } else if(isSgDoWhileStmt(loopStmt)) {
      // target is condition node
      SgNode* targetNode=SgNodeHelper::getCond(loopStmt);
      ROSE_ASSERT(targetNode);
      Edge edge=Edge(getLabel(node),EDGE_FORWARD,getLabel(targetNode));
      edgeSet.insert(edge);
    } else if(isSgForStatement(loopStmt)) {
      // target is increment expr
      SgExpression* incExp=SgNodeHelper::getForIncExpr(loopStmt);
      if(!incExp)
        throw CodeThorn::Exception("CFAnalysis: for-loop: empty incExpr not supported.");
      SgNode* targetNode=incExp;
      ROSE_ASSERT(targetNode);
      Edge edge=Edge(getLabel(node),EDGE_FORWARD,getLabel(targetNode));
      edgeSet.insert(edge);
    } else {
      throw CodeThorn::Exception("CFAnalysis: continue in unknown loop construct (not while,do-while, or for).");
    }
    return edgeSet;
  }
  case V_SgIfStmt: {
    SgNode* nodeC=SgNodeHelper::getCond(node);
    Label condLabel=getLabel(nodeC);
    SgNode* nodeTB=SgNodeHelper::getTrueBranch(node);
    Edge edgeTB=Edge(condLabel,EDGE_TRUE,initialLabel(nodeTB));
    edgeTB.addType(EDGE_FORWARD);
    edgeSet.insert(edgeTB);
    Flow flowTB=flow(nodeTB);
    edgeSet+=flowTB;
    if(SgNode* nodeFB=SgNodeHelper::getFalseBranch(node)) {
      Flow flowFB=flow(nodeFB);
      Edge edgeFB=Edge(condLabel,EDGE_FALSE,initialLabel(nodeFB));
      edgeFB.addType(EDGE_FORWARD);
      edgeSet.insert(edgeFB);
      edgeSet+=flowFB;
    }
    return edgeSet;
  }
  case V_SgGotoStatement: {
    SgGotoStatement* gotoStmt=isSgGotoStatement(node);
    SgLabelStatement* targetSgLabelStmt=gotoStmt->get_label();
    ROSE_ASSERT(targetSgLabelStmt);
    // note that the target label is not an element of the finalLabels set.
    Label targetLabel=labeler->getLabel(targetSgLabelStmt);
    edgeSet.insert(Edge(initialLabel(node),EDGE_FORWARD,targetLabel));
    return edgeSet;
  }
 
  case V_SgCaseOptionStmt:
  case V_SgDefaultOptionStmt: {
    Label caseStmtLab=labeler->getLabel(node);
    SgStatement* caseBody=getCaseOrDefaultBodyStmt(node);
    if(caseBody) {
      edgeSet.insert(Edge(caseStmtLab,EDGE_FORWARD,initialLabel(caseBody)));
      Flow flowStmt=flow(caseBody);
      edgeSet+=flowStmt;
    } else {
      // single case/default without body: no edges inside this construct, intentionally empty.
    }
    return edgeSet;
    break;
  }
  case V_SgSwitchStatement: {
    // create edges for body of switch
    SgSwitchStatement* switchStmt=isSgSwitchStatement(node);
    SgStatement* block=switchStmt->get_body();
    Flow blockFlow=flow(block);
    edgeSet+=blockFlow;
    // create edges from condition to case (if they exist)
    SgNode* nodeC=SgNodeHelper::getCond(node);
    Label condLabel=getLabel(nodeC);
    std::set<SgCaseOptionStmt*> caseNodes=SgNodeHelper::switchRelevantCaseStmtNodes(block);
    for (auto caseNode : caseNodes) {
      edgeSet.insert(Edge(condLabel,EDGE_FORWARD,initialLabel(caseNode)));
    }
    // create edge from condition to default label (if it exists)
    SgDefaultOptionStmt* defaultNode=SgNodeHelper::switchRelevantDefaultStmtNode(block);
    if(defaultNode) {
      edgeSet.insert(Edge(condLabel,EDGE_FORWARD,initialLabel(defaultNode)));
    }
    // special case: if there are no case and no default labels, create an edge to the block or stmt
    if(caseNodes.size()==0 && !defaultNode) {
      edgeSet.insert(Edge(condLabel,EDGE_FORWARD,initialLabel(block)));
    }
    return edgeSet;
  }
  case V_SgWhileStmt:
    return WhileAndDoWhileLoopFlow(node,edgeSet,EDGE_FORWARD,EDGE_BACKWARD);
  case V_SgDoWhileStmt:
    return WhileAndDoWhileLoopFlow(node,edgeSet,EDGE_BACKWARD,EDGE_FORWARD);
  
  case V_SgBasicBlock: {
    std::vector<SgNode*> succ = node->get_traversalSuccessorContainer();
    auto pos = std::remove_if( succ.begin(), succ.end(), 
                               [=](SgNode* el) -> bool
                               {
                                 return sg::dispatch(ExcludeFromCfg(), el); // for better debugging 
                                 //~ return labeler->numberOfAssociatedLabels(el) == 0;
                               }
                             );
    size_t len=std::distance(succ.begin(), pos);
    if(len==0) {
      return edgeSet;
    } 
    
    if(len==1) {
      SgNode* onlyStmt=succ.at(0);
      Flow onlyFlow=flow(onlyStmt);
      edgeSet+=onlyFlow;
    } else {
      for(size_t i=0;i<len-1;++i) {
        SgNode* childNode1=succ.at(i);
        ROSE_ASSERT(!isSgTemplateTypedefDeclaration(childNode1));
        SgNode* childNode2=succ.at(i+1);
        ROSE_ASSERT(!isSgTemplateTypedefDeclaration(childNode2));
        Flow flow12=flow(childNode1,childNode2);
        edgeSet+=flow12;
      }
    }
    SgNode* firstStmt=succ.at(0);
    Edge edge1=Edge(labeler->blockBeginLabel(node),EDGE_FORWARD,initialLabel(firstStmt));
    edgeSet.insert(edge1);
    ROSE_ASSERT(len>=1);
    // do not generate edges to blockEndLabel
#if 0
    SgNode* lastStmt=node->get_traversalSuccessorByIndex(len-1);
    ROSE_ASSERT(isSgStatement(lastStmt));
    LabelSet lastStmtFinalLabels=finalLabels(lastStmt);
    for(LabelSet::iterator i=lastStmtFinalLabels.begin();i!=lastStmtFinalLabels.end();++i) {
      Edge edge2=Edge(*i,EDGE_FORWARD,labeler->blockEndLabel(node));
      edgeSet.insert(edge2);
    }
#endif
    return edgeSet;
  }
  
  case V_SgTryStmt: {
    // PP
    SgNode* childStmt=node->get_traversalSuccessorByIndex(0);
    Edge    edge1=Edge(labeler->getLabel(node),EDGE_FORWARD,initialLabel(childStmt));
    edgeSet.insert(edge1);
    Flow    childFlow=flow(childStmt);
    edgeSet+=childFlow;
      
    return edgeSet;    
  }
  
  case V_SgForStatement: {
    SgStatementPtrList& stmtPtrList=SgNodeHelper::getForInitList(node);
    int len=stmtPtrList.size();
    if(len==0) {
      // empty initializer list (hence, an initialization stmt cannot be initial stmt of for)
      cout << "INFO: for-stmt: initializer-list is empty."<<endl;
      cerr << "Error: empty for-stmt initializer (should be an empty statement node)."<<endl;
      exit(1);
    }
    ROSE_ASSERT(len>0);
    SgNode* lastNode=0;
    if(len==1) {
      SgNode* onlyStmt=*stmtPtrList.begin();
      Flow onlyFlow=flow(onlyStmt);
      edgeSet+=onlyFlow;
      lastNode=onlyStmt;
    } else {
      ROSE_ASSERT(stmtPtrList.size()>=2);
      for(SgStatementPtrList::iterator i=stmtPtrList.begin();
          i!=stmtPtrList.end();
          ++i) {
        SgNode* node1=*i;
        SgStatementPtrList::iterator i2=i;
        i2++;
        SgNode* node2=*i2;
        Flow flow12=flow(node1,node2);
        edgeSet+=flow12;
        i2++;
        if(i2==stmtPtrList.end()) {
          lastNode=node2;
          break;
        }
      }
    }
    SgNode* condNode=SgNodeHelper::getCond(node);
    if(!condNode)
      throw CodeThorn::Exception("Error: for-loop: empty condition not supported. Normalization required.");
    Flow flowInitToCond=flow(lastNode,condNode);
    edgeSet+=flowInitToCond;
    Label condLabel=getLabel(condNode);
    SgNode* bodyNode=SgNodeHelper::getLoopBody(node);
    ROSE_ASSERT(bodyNode);
    Edge edge=Edge(condLabel,EDGE_TRUE,initialLabel(bodyNode));
    edge.addType(EDGE_FORWARD);
    Flow flowB=flow(bodyNode);
    edgeSet+=flowB;
    edgeSet.insert(edge);

    // Increment Expression:
    SgExpression* incExp=SgNodeHelper::getForIncExpr(node);
    if(!incExp)
      throw CodeThorn::Exception("Error: for-loop: empty incExpr not supported. Normalization required.");
    ROSE_ASSERT(incExp);
    Label incExpLabel=getLabel(incExp);
    ROSE_ASSERT(incExpLabel!=Labeler::NO_LABEL);

    // Edges from final labels of for body to the increment expression:
    LabelSet finalSetB=finalLabels(bodyNode);
    for(LabelSet::iterator i=finalSetB.begin();i!=finalSetB.end();++i) {
      Edge edgeToIncExpr=Edge(*i,EDGE_FORWARD,incExpLabel);
      if(SgNodeHelper::isCond(labeler->getNode(*i))) {
        edgeToIncExpr.addType(EDGE_FALSE);
      }
      edgeSet.insert(edgeToIncExpr);
    }

    // Edge from increment expression back to condition:
    Edge backwardEdge = Edge(incExpLabel,EDGE_BACKWARD,condLabel);
    edgeSet.insert(backwardEdge);
    return edgeSet;
  }
  default:
    throw CodeThorn::Exception("Unknown node in CFAnalysis::flow: Problemnode "+node->class_name()+" Input file: "+SgNodeHelper::sourceLineColumnToString(node)+": "+node->unparseToString());
  }
}

SgFunctionDefinition* CFAnalysis::determineFunctionDefinition3(SgFunctionCallExp* funCall) {
  //cout<<"DEBUG: CFAnalysis::determineFunctionDefinition3:"<<funCall->unparseToString()<<endl;
  SgFunctionDefinition* funDef=nullptr;
  // TODO (use function id mapping)
  ROSE_ASSERT(getFunctionIdMapping());
  SgExpression* node=funCall->get_function();
  if(node) {
    if(SgFunctionRefExp* funRef=isSgFunctionRefExp(node)) {
      funDef=getFunctionIdMapping()->resolveFunctionRef(funRef);
      if(funDef) SAWYER_MESG(logger[TRACE])<<"Resolved to "<<funDef;
      else SAWYER_MESG(logger[TRACE])<<"NOT resolved.";
    }
  }
  SAWYER_MESG(logger[TRACE])<<" FunDef: "<<funDef<<endl;
  return funDef;
}


FunctionCallTargetSet CFAnalysis::determineFunctionDefinition4(SgFunctionCallExp* funCall) {
  SAWYER_MESG(logger[TRACE])<<"CFAnalysis::determineFunctionDefinition4:"<<funCall->unparseToString()<<": ";
  ROSE_ASSERT(getFunctionCallMapping());
  FunctionCallTargetSet res=getFunctionCallMapping()->resolveFunctionCall(funCall);
  if(res.size()>0) {
    if(res.size()==1) {
      SAWYER_MESG(logger[TRACE]) << "RESOLVED to "<<(*res.begin()).getDefinition()<<endl;
    } else {
      SAWYER_MESG(logger[TRACE])<< "RESOLVED to "<<res.size()<<" targets"<<endl;
    }
  } else {
    SAWYER_MESG(logger[TRACE]) << "NOT RESOLVED."<<endl;
  }
  return res;
}


FunctionCallTargetSet CFAnalysis::determineFunctionDefinition5(Label lbl, SgLocatedNode* astnode) {
  SAWYER_MESG(logger[TRACE])<<"CFAnalysis::determineFunctionDefinition5:"<<astnode->unparseToString()<<": ";
  ROSE_ASSERT(getFunctionCallMapping2());
  FunctionCallTargetSet res=getFunctionCallMapping2()->resolveFunctionCall(lbl);
#if 1
  if(res.size()>0) {
    if(res.size()==1) {
      SAWYER_MESG(logger[TRACE]) << "RESOLVED to "<<(*res.begin()).getDefinition()<<endl;
    } else {
      SAWYER_MESG(logger[TRACE])<< "RESOLVED to "<<res.size()<<" targets"<<endl;
    }
  } else {
    SAWYER_MESG(logger[TRACE]) << "NOT RESOLVED."<<endl;
  }
#endif
  return res;
}


SgFunctionDefinition* CFAnalysis::determineFunctionDefinition2(SgFunctionCallExp* funCall) {

  SgExpression* funExp=funCall->get_function();
  ROSE_ASSERT(funExp);
  SgType* funExpType=funExp->get_type();
  ROSE_ASSERT(funExpType);
  SgFunctionType* callFunType=isSgFunctionType(funExpType);
  SAWYER_MESG(logger[TRACE])<<"FD2: funExp: "<<funExp->unparseToString()<<" callFunType: "<<callFunType->unparseToString()<<endl;

  SgFunctionSymbol* funCallFunctionSymbol=0;
  SgName funCallName;
  if(SgFunctionRefExp* funRef=isSgFunctionRefExp(funExp)) {
    funCallFunctionSymbol=funRef->get_symbol();
    funCallName=funCallFunctionSymbol->get_name();
  }
  SAWYER_MESG(logger[TRACE])<<"FD2: funCallFunctionSymbol: "<<funCallFunctionSymbol<<endl;

  if(callFunType) {
    SgNode* rootNode=funCall;
    while(!isSgProject(rootNode)) {
      rootNode=rootNode->get_parent();
    }
    //SgFunctionParameterTypeList* funParamTypeList=funType->get_argument_list();
    // search for this function type in the AST.
    RoseAst ast(rootNode);
    for(auto node : ast) {
      // check every function definition
      if(SgFunctionDefinition* funDef=isSgFunctionDefinition(node)) {
        SgFunctionDeclaration* funDecl=funDef->get_declaration();
        // SgName funDecl->get_mangled_name()
        //const SgInitializedNamePtrList & 	get_args () const

        SgName funName=funDecl->get_name();
        if(funName.get_length()>0 && funName==funCallName) {
          SAWYER_MESG(logger[TRACE])<<"Names match: "<<funName<<endl;
        } else {
          SAWYER_MESG(logger[TRACE])<<"Names do NOT match: "<<funCallName<<" : "<<funName<<endl;
        }

        SgFunctionType* funType=funDecl->get_type();
        if(callFunType==funType) {
          SAWYER_MESG(logger[TRACE])<<"Found function type: "<<funType->unparseToString()<<endl;
          SgName qualifiedFunName=funDecl->get_qualified_name();
          SAWYER_MESG(logger[TRACE])<<"Found (qual) function name: "<<qualifiedFunName<<endl;
          SgName FunName=funDecl->get_name();
          SAWYER_MESG(logger[TRACE])<<"Found (bare) function name: "<<qualifiedFunName<<endl;
          return funDef;
        }
      }
    }
  }
  return 0;
}

void CFAnalysis::setFunctionIdMapping(FunctionIdMapping* fim) {
  _functionIdMapping=fim;
}

FunctionIdMapping* CFAnalysis::getFunctionIdMapping() {
  return _functionIdMapping;
}

void CFAnalysis::setFunctionCallMapping(FunctionCallMapping* fcm) {
  _functionCallMapping=fcm;
}

void CFAnalysis::setFunctionCallMapping2(FunctionCallMapping2* fcm) {
  _functionCallMapping2=fcm;
}


FunctionCallMapping* CFAnalysis::getFunctionCallMapping() {
  return _functionCallMapping;
}

FunctionCallMapping2* CFAnalysis::getFunctionCallMapping2() {
  return _functionCallMapping2;
}

bool CFAnalysis::forkJoinConsistencyChecks(Flow &flow) const {
  SAWYER_MESG(logger[INFO]) << "Running fork/join consistency tests." << endl;
  const auto flowLabels = flow.nodeLabels();
  int forks, joins, workshares, barriers;
  forks = joins = workshares = barriers = 0;
  for (const auto l : flowLabels) {
    if (labeler->isForkLabel(l)) {
      auto node = isSgOmpParallelStatement(labeler->getNode(l));
      assert(node && "Node for fork label is SgOmpParallelNode");
      forks++;
    }
    if (labeler->isJoinLabel(l)) {
      auto node = isSgOmpParallelStatement(labeler->getNode(l));
      assert(node && "Node for join label is SgOmpParallelNode");
      joins++;
    }
    if (labeler->isWorkshareLabel(l)) {
      auto node = labeler->getNode(l);
      bool correctNodeType = isSgOmpForStatement(node) || isSgOmpSectionsStatement(node) || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node);
      assert(correctNodeType && "Node for workshare label is one of for / sections / simd / simd for");
      workshares++;
    }
    if (labeler->isBarrierLabel(l)) {
      auto node = labeler->getNode(l);
      bool correctNodeType = isSgOmpForStatement(node) || isSgOmpSectionsStatement(node) || isSgOmpBarrierStatement(node) || isSgOmpSimdStatement(node) || isSgOmpForSimdStatement(node);
      assert(correctNodeType && "Node for barrier label is one of for / sections / barrier");
      barriers++;
    }
  }
  // At that point we cannot really make any more assumptions
  SAWYER_MESG(logger[TRACE]) << "Forks: " << forks << " | " << joins << " :Joins" << endl;
  bool forksEqualJoins(forks == joins);
  assert(forksEqualJoins);
  SAWYER_MESG(logger[INFO]) << "Passed fork/join consistency checks 1/2" << endl;

  /* Lambda function to collext all function definitions */
  const auto collectFunctionDefs = [&] (const LabelSet &ls) {
    std::vector<SgFunctionDefinition *> nodes;
    for (auto l : ls) {
      if (labeler->isFunctionEntryLabel(l)) {
        auto n = isSgFunctionDefinition(labeler->getNode(l));
        assert(n);
        nodes.push_back(n);
      }
    }
    return nodes;
  };

  auto startNodes = collectFunctionDefs(flowLabels);
  assert(startNodes.size() > 0 && "Target AST has no function definition. No control flow.");

  /*
   * We sum all parallel cfg labels and check whether the numbers make sense when compared to all AST nodes.
   * Every omp parallel construct introduces one fork and one join node.
   * Every omp for / omp sections / omp simd / omp for simd introduces a workshare and a barrier node.
   * Every omp nowait removed a barrier node.
   * Every omp barrier introduces a barrier node.
   */
  int parallelNodes, forNodes, sectionsNodes, simdNodes, forSimdNodes, barrierNodes;
  parallelNodes = forNodes = sectionsNodes = simdNodes = forSimdNodes = barrierNodes = 0;

  /* This might be somewhat time consuming for large programs. */
  for (auto rootNode : startNodes) {
  RoseAst ast(rootNode);

    for(auto n : ast) {
      switch (n->variantT()) {
        case V_SgOmpParallelStatement: {
          parallelNodes++;
          continue;
        }
        case V_SgOmpForStatement: {
          forNodes++;
          continue;
        }
        case V_SgOmpSectionsStatement: {
          sectionsNodes++;
          continue;
        }
        case V_SgOmpSimdStatement: {
          simdNodes++;
          continue;
        }
        case V_SgOmpForSimdStatement: {
          forSimdNodes++;
          continue;
        }
        case V_SgOmpBarrierStatement: {
          barrierNodes++;
          continue;
        }
        default: { }
      }
    }
  }
  int workshareAstNodes = forNodes + sectionsNodes + simdNodes + forSimdNodes;
  int barrierAstNodes = forNodes + sectionsNodes + simdNodes + forSimdNodes + barrierNodes;
  SAWYER_MESG(logger[TRACE]) << "Type | CFG nodes | AST nodes\nFork | " << forks << " | " << parallelNodes << "\n";
  SAWYER_MESG(logger[TRACE]) << "Join | " << joins << " | " << parallelNodes << "\n";
  SAWYER_MESG(logger[TRACE]) << "Workshare | " << workshares << " | " << workshareAstNodes << "\n";
  SAWYER_MESG(logger[TRACE]) << "Barrier | " << barriers << " | " << barrierAstNodes << "\n";
  assert(forks == parallelNodes);
  assert(joins == parallelNodes);
  assert(workshares == workshareAstNodes);
  assert(barriers <= barrierAstNodes);
  SAWYER_MESG(logger[INFO]) << "Passed fork/join consistency checks 2/2" << endl;

  return forksEqualJoins;
}
