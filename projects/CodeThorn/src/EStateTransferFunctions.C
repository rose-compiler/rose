#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "CTAnalysis.h"
#include "AstUtility.h"
#include "Miscellaneous2.h"

using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility CodeThorn::EStateTransferFunctions::logger;

namespace CodeThorn {

  EStateTransferFunctions::EStateTransferFunctions () {
    initDiagnostics();
  }
void EStateTransferFunctions::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::EStateTransferFunctions", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}
  CTIOLabeler* EStateTransferFunctions::getLabeler() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getLabeler();
  }
  VariableIdMappingExtended* EStateTransferFunctions::getVariableIdMapping() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getVariableIdMapping();
  }
  bool EStateTransferFunctions::getOptionOutputWarnings() {
    return _analyzer->getOptionOutputWarnings();
  }

  void EStateTransferFunctions::setAnalyzer(CTAnalysis* analyzer) {
    _analyzer=analyzer;
  }

  CTAnalysis* EStateTransferFunctions::getAnalyzer() {
    return _analyzer;
  }

  ExprAnalyzer* EStateTransferFunctions::getExprAnalyzer() {
    ROSE_ASSERT(_analyzer);
    return getAnalyzer()->getExprAnalyzer();
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset) {
    return getAnalyzer()->createEState(label,cs,pstate,cset);
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io) {
    return getAnalyzer()->createEState(label,cs,pstate,cset,io);
  }

  std::list<EState> EStateTransferFunctions::elistify() {
    std::list<EState> resList;
    return resList;
  }

  std::list<EState> EStateTransferFunctions::elistify(EState res) {
    std::list<EState> resList;
    resList.push_back(res);
    return resList;
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
    Label lab=estate->label();
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
        ROSE_ASSERT(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName==_rersHybridOutputFunctionName) {
          // RERS global vars binary handling
          PState _pstate=*estate->pstate();
          RERS_Problem::rersGlobalVarsCallInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
#if 0
          //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
          int rers_result=RERS_Problem::calculate_outputFP(argument);
          (void) RERS_Problem::calculate_outputFP(argument);
#else
          (void) RERS_Problem::calculate_outputFP( omp_get_thread_num() );
          int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
          //logger[DEBUG]<< "Called calculate_outputFP("<<argument<<")"<<" :: result="<<rers_result<<endl;
          if(rers_result<=-100) {
            // we found a failing assert
            // = rers_result*(-1)-100 : rers error-number
            // = -160 : rers globalError (2012 only)
            int index=((rers_result+100)*(-1));
            ROSE_ASSERT(index>=0 && index <=99);
            getAnalyzer()->binaryBindingAssert[index]=true;
            //reachabilityResults.reachable(index); //previous location in code
            //logger[DEBUG]<<"found assert Error "<<index<<endl;
            ConstraintSet _cset=*estate->constraints();
            InputOutput _io;
            _io.recordFailedAssert();
            // error label encoded in the output value, storing it in the new failing assertion EState
            PState newPstate  = _pstate;
            //newPstate[globalVarIdByName("output")]=CodeThorn::AbstractValue(rers_result);
            getExprAnalyzer()->writeToMemoryLocation(lab,&newPstate,
                                                     AbstractValue::createAddressOfVariable(globalVarIdByName("output")),
                                                     CodeThorn::AbstractValue(rers_result));
            EState _eState=createEState(edge.target(),estate->callString,newPstate,_cset,_io);
            return getAnalyzer()->elistify(_eState);
          }
          RERS_Problem::rersGlobalVarsCallReturnInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
          InputOutput newio;
          if (rers_result == -2) {
            newio.recordVariable(InputOutput::STDERR_VAR,globalVarIdByName("input"));
          }
          // TODO: _pstate[VariableId(output)]=rers_result;
          // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
          if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
            SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
            VariableId lhsVarId;
            bool isLhsVar=getExprAnalyzer()->checkIfVariableAndDetermineVarId(lhs,lhsVarId);
            ROSE_ASSERT(isLhsVar); // must hold
            // logger[DEBUG]<< "lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
            //_pstate[lhsVarId]=AbstractValue(rers_result);
            getExprAnalyzer()->writeToMemoryLocation(lab,&_pstate,AbstractValue::createAddressOfVariable(lhsVarId),AbstractValue(rers_result));
            ConstraintSet _cset=*estate->constraints();
            //_cset.removeAllConstraintsOfVar(lhsVarId);
            EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
            return getAnalyzer()->elistify(_eState);
          } else {
            ConstraintSet _cset=*estate->constraints();
            EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
            return getAnalyzer()->elistify(_eState);
          }
          cout <<"PState:"<< _pstate<<endl;
          logger[ERROR] <<"RERS-MODE: call of unknown function."<<endl;
          exit(1);
          // _pstate now contains the current state obtained from the binary
        }
        // logger[DEBUG]<< "@LOCAL_EDGE: function call:"<<SgNodeHelper::nodeToString(funCall)<<endl;
      }
    }
    return getAnalyzer()->elistify();
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionCall(Edge edge, const EState* estate) {
    // 1) obtain actual parameters from source
    // 2) obtain formal parameters from target
    // 3) eval each actual parameter and assign result to formal parameter in state
    // 4) create new estate and update callstring (context sensitive analysis)
    SAWYER_MESG(logger[TRACE])<<"transferFunctionCall: "<<getLabeler()->getNode(edge.source())->unparseToString()<<endl;
    Label currentLabel=estate->label();
    EState currentEState=*estate;
    PState currentPState=*currentEState.pstate();
    ConstraintSet cset=*currentEState.constraints();

    // ad 1)
    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source()));
    ROSE_ASSERT(funCall);
    //FUNCTION CALL:funcPtr(data) L:53:SgFunctionCallExp(SgVarRefExp:funcPtr,SgExprListExp(SgVarRefExp:data))
    //cout<<"DEBUG: FUNCTION CALL:"<<funCall->unparseToString()<<" L:"<<edge.target().toString()<<":"<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName==_rersHybridOutputFunctionName) {
        // logger[DEBUG]<< "rers-binary mode: skipped static-analysis call."<<endl;
        return elistify();
      }
    }

    // cases: get_function() :
    // C function call        : FunctionRefExp
    // C function pointer call: VarRefExp
    // member function call (stack)      : SgDotExp(SgVarRefExp:a,SgMemberFunctionRefExp:m)
    // member function call (reference)  : SgDotExp(SgVarRefExp:r,SgMemberFunctionRefExp:m)
    // member functino call (pointer)    : SgArrowExp(SgVarRefExp:p,SgMemberFunctionRefExp:m)

    // check if function pointer call (function=VarRefExp) [otherwise it is a direct function call: function=FunctionRefExp]
    if(SgExpression* funExp=funCall->get_function()) {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(funExp)) {
        //cout<<"Function pointer call: VarRefExp: "<<varRefExp->unparseToString()<<endl;
        // determine if the edge leads to the called function
        Label targetLabel=edge.target();
        VariableId varId=getVariableIdMapping()->variableId(varRefExp);
        //cout<<"DEBUG: function pointer var id: "<<varId.toString(getVariableIdMapping())<<endl;
        AbstractValue varAddress=AbstractValue::createAddressOfVariable(varId);
        //cout<<"DEBUG: function pointer var address: "<<varAddress.toString(getVariableIdMapping())<<endl;
        AbstractValue funcPtrVal=estate->pstate()->readFromMemoryLocation(varAddress);
        //cout<<"DEBUG: function pointer value: "<<funcPtrVal.toString(getVariableIdMapping())<<": isFunPtr:"<<funcPtrVal.isFunctionPtr()<<endl;
        if(funcPtrVal.isFunctionPtr()&&!funcPtrVal.isTop()&&!funcPtrVal.isBot()) {
          Label  funTargetLabel=funcPtrVal.getLabel();
          if(funTargetLabel!=targetLabel) {
            //infeasable path
            //cout<<"DEBUG: infeasable path: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
            return elistify();
          } else {
            // continue on this path, function pointer is referring to the target label's function entry
            //cout<<"Resolved function pointer"<<endl;
          }
        } else {
          //cerr<<"INFO: function pointer is top or bot. Not supported: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
          // continue but pass the information now to *all* outgoing static edges (maximum imprecision)
        }
      }
    }
  
    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    // ad 2)
    // check for function pointer label

    SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target()));
    SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    ROSE_ASSERT(funDef);
    // ad 3)
    PState newPState=currentPState;
    SgInitializedNamePtrList::iterator i=formalParameters.begin();
    SgExpressionPtrList::iterator j=actualParameters.begin();
    while(i!=formalParameters.end() || j!=actualParameters.end()) {
      SgInitializedName* formalParameterName=*i;
      ROSE_ASSERT(formalParameterName);
      // test formal parameter (instead of argument type) to allow for expressions in arguments
      VariableId formalParameterVarId=_analyzer->getVariableIdMapping()->variableId(formalParameterName);
      AbstractValue evalResultValue;
      if(_analyzer->getVariableIdMapping()->isOfClassType(formalParameterVarId)) {
        if(getOptionOutputWarnings()) {
          cout<<"Warning: imprecision: "<<SgNodeHelper::sourceLineColumnToString(funCall)<< ": passing of class/Struct/Union types per value as function parameters (assuming top)."<<endl;
        }
        evalResultValue=AbstractValue::createTop();
      } else {
        // VariableName varNameString=name->get_name();
        SgExpression* actualParameterExpr=*j;
        ROSE_ASSERT(actualParameterExpr);
        // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
        // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
        VariableId actualParameterVarId;
        ROSE_ASSERT(actualParameterExpr);
#if 0
        if(getExprAnalyzer.checkIfVariableAndDetermineVarId(actualParameterExpr,actualParameterVarId)) {
          // propagate constraint from actualParamterVarId to formalParameterVarId
          cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
        }
#endif
        // general case: the actual argument is an arbitrary expression (including a single variable)
        list<SingleEvalResultConstInt> evalResultList=getExprAnalyzer()->evaluateExpression(actualParameterExpr,currentEState);
        if(evalResultList.size()==0) {
          SAWYER_MESG(logger[FATAL])<<"Internal error: no state computed for argument evaluation at: "<<SgNodeHelper::sourceLineColumnToString(getLabeler()->getNode(edge.source()))<<endl;
          SAWYER_MESG(logger[FATAL])<<"Argument expression: "<<actualParameterExpr->unparseToString()<<endl;
          SAWYER_MESG(logger[FATAL])<<"EState: "<<currentEState.toString(getVariableIdMapping())<<endl;
          exit(1);
        }
        list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
        SingleEvalResultConstInt evalResult=*resultListIter;
        if(evalResultList.size()>1) {
          SAWYER_MESG(logger[ERROR]) <<"multi-state generating operators in function call parameters not supported."<<endl;
          exit(1);
        }
        evalResultValue=evalResult.value();
      }
      // above evaluateExpression does not use constraints (par3==false). Therefore top vars remain top vars
      getExprAnalyzer()->initializeMemoryLocation(currentLabel,&newPState,AbstractValue::createAddressOfVariable(formalParameterVarId),evalResultValue);
      ++i;++j;
    }
    // assert must hold if #formal-params==#actual-params (TODO: default values)
    ROSE_ASSERT(i==formalParameters.end() && j==actualParameters.end());

    // ad 4
    CallString cs=currentEState.callString;
    if(_analyzer->getOptionContextSensitiveAnalysis()) {
      cs=transferFunctionCallContext(cs, currentEState.label());
    }
    EState newEState=createEState(edge.target(),cs,newPState,cset);
    return elistify(newEState);
  }

std::list<EState> EStateTransferFunctions::transferReturnStmt(Edge edge, const EState* estate) {
  Label lab=estate->label();
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  ROSE_ASSERT(_analyzer);
  ROSE_ASSERT(_analyzer->getCFAnalyzer());
  SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);
  SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);

  if(isSgNullExpression(expr)) {
      // null expr is a no-op
    return elistify(createEState(edge.target(),cs,currentPState,cset));
  } else {
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
    }

    // MS 04/09/2020: removed all old code here and function analyzeAssignRhs. Replaced it with function call 'evaluateExpression'
    // old version available in commit: 9645ce260b0bd44207d342a760afc27620380140
    list<SingleEvalResultConstInt> rhsResList=getExprAnalyzer()->evaluateExpression(expr,currentEState, CodeThorn::ExprAnalyzer::MODE_VALUE);
    AbstractValue rhsResultValue;
    if(rhsResList.size()==0) {
      // analyzed program execution ends, return empty set to indicate that execution does not continue
      // TODO: may also return an error state here
      std::list<EState> emptyList;
      return emptyList;
    } else if(rhsResList.size()>1) {
      cerr<<"Error: return expression gives more than one result. Not normalized."<<endl;
      exit(1);
    } else {
      ROSE_ASSERT(rhsResList.size()==1);
      rhsResultValue=(*rhsResList.begin()).value();
    }
    PState newPState=currentPState;
    getExprAnalyzer()->initializeMemoryLocation(lab,&newPState,returnVarId,rhsResultValue);
    return elistify(createEState(edge.target(),cs,newPState,cset));
  }
}

std::list<EState> EStateTransferFunctions::transferFunctionCallReturn(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // determine functionCallLabel corresponding to functioncallReturnLabel.
  Label functionCallReturnLabel=edge.source();
  SgNode* node=getLabeler()->getNode(functionCallReturnLabel);
  Label functionCallLabel=getLabeler()->functionCallLabel(node);
  SAWYER_MESG(logger[TRACE])<<"FunctionCallReturnTransfer: "<<functionCallLabel.toString()<<":"<<functionCallReturnLabel.toString()<<" cs: "<<estate->callString.toString()<<endl;

  CallString cs=currentEState.callString;
  if(_analyzer->getOptionContextSensitiveAnalysis()) {
    if(getLabeler()->isExternalFunctionCallLabel(functionCallLabel)) {
      // nothing to do for external function call (label is not added
      // to callstring by external function call)
    } else {
      if(isFeasiblePathContext(cs,functionCallLabel)) {
        cs.removeLastLabel();
      } else {
        if(cs.isEmpty()) {
          SAWYER_MESG(logger[WARN])<<"Empty context on non-feasable path at label "<<functionCallLabel.toString()<<endl;
        }
        // definitely not feasible path, do not return a state
        SAWYER_MESG(logger[TRACE])<<"definitly on non-feasable path at label (no next state)"<<functionCallLabel.toString()<<endl;
        std::list<EState> emptyList;
        return emptyList;
      }
    }
  }

  // 1. handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 1: return f(); pass estate through
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  } else if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
    // case 2a: x=f(); bind variable x to value of $return
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName==_rersHybridOutputFunctionName) {
          EState newEState=currentEState;
          newEState.setLabel(edge.target());
          newEState.callString=cs;
          return elistify(newEState);
        }
      }
    }
    SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
    VariableId lhsVarId;
    bool isLhsVar=getExprAnalyzer()->checkIfVariableAndDetermineVarId(lhs,lhsVarId);
    ROSE_ASSERT(isLhsVar); // must hold
    PState newPState=*currentEState.pstate();
    // we only create this variable here to be able to find an existing $return variable!
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
    }

    if(newPState.varExists(returnVarId)) {
      AbstractValue evalResult=getExprAnalyzer()->readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
      getExprAnalyzer()->initializeMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
      newPState.deleteVar(returnVarId); // remove $return from state
      return elistify(createEState(edge.target(),cs,newPState,cset));
    } else {
      // no $return variable found in state. This can be the case for an extern function.
      // alternatively a $return variable could be added in the external function call to
      // make this handling here uniform
      // for external functions no constraints are generated in the call-return node
      return elistify(createEState(edge.target(),cs,newPState,cset));
    }
  } else if(SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(nextNodeToAnalyze1)) {
    // case 2b: Type x=f(); bind variable x to value of $return for function call in declaration
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName==_rersHybridOutputFunctionName) {
          EState newEState=currentEState;
          newEState.setLabel(edge.target());
          newEState.callString=cs;
          return elistify(newEState);
        }
      }
    }

    // these two lines are different to x=f();
    SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(nextNodeToAnalyze1);
    VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
    ROSE_ASSERT(lhsVarId.isValid());

    PState newPState=*currentEState.pstate();
    // this variable is created here only to be able to find an existing $return variable in the state
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
    }

    if(newPState.varExists(returnVarId)) {
      AbstractValue evalResult=getExprAnalyzer()->readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
      getExprAnalyzer()->initializeMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
      newPState.deleteVar(returnVarId); // remove $return from state
      SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn(initialization): LHSVAR:"<<getVariableIdMapping()->variableName(lhsVarId)<<" value: "<<evalResult.toString()<<endl;
      return elistify(createEState(edge.target(),cs,newPState,cset));
    } else {
      // no $return variable found in state. This can be the case for an extern function.
      // alternatively a $return variable could be added in the external function call to
      // make this handling here uniform
      // for external functions no constraints are generated in the call-return node
      SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
      SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn: Variable declaration with function call: no function-return variable $return found! @ "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze1)<<":"<<nextNodeToAnalyze1->unparseToString()<<endl;
      SAWYER_MESG(logger[TRACE])<<estate->toString(getVariableIdMapping())<<endl;
      SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
      return elistify(createEState(edge.target(),cs,newPState,cset));
    }
  } else  if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 3: f(); remove $return from state (discard value)
    PState newPState=*currentEState.pstate();
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
    }
    // no effect if $return does not exist
    newPState.deleteVar(returnVarId);
    return elistify(createEState(edge.target(),cs,newPState,cset));
  } else if (SgNodeHelper::matchExtendedNormalizedCall(nextNodeToAnalyze1)) {
    // Handles Constructor Calls
    // \pp TEMPORARY SOLUTION TO MAKE CODE PASS THROUGH
    // \todo THIS IS WRONG 
    // (1) matchExtendedNormalizedCall is too coarse grain, as it covers
    //     both constructor and function calls
    // (2) distinction between constructor call of a base class and an
    //     variable initialization needs to be made. 
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  } else {
    logger[FATAL] << "function call-return from unsupported call type:"<<nextNodeToAnalyze1->unparseToString()<<endl;
    exit(1);
  }
}

  std::list<EState> EStateTransferFunctions::transferAsmStmt(Edge edge, const EState* estate) {
    // ignore AsmStmt
    return transferIdentity(edge,estate);
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionEntry(Edge edge, const EState* estate) {
    Label lab=estate->label();
    SgNode* node=_analyzer->getLabeler()->getNode(lab);
    SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
    if(funDef) {
      string functionName=SgNodeHelper::getFunctionName(node);
      string fileName=SgNodeHelper::sourceFilenameToString(node);
      SAWYER_MESG(logger[INFO])<<"Analyzing Function:"<<fileName<<" : "<<functionName<<endl;
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      SAWYER_MESG(logger[TRACE])<<"Function:"<<functionName<<" Parameters: ";
      for(auto fParam : formalParameters) {
        SAWYER_MESG(logger[TRACE])<<fParam->unparseToString()<<" sym:"<<fParam->search_for_symbol_from_symbol_table()<<endl;
      }
      SAWYER_MESG(logger[TRACE])<<endl;
    }
    return transferIdentity(edge,estate);
  }
  
  std::list<EState> EStateTransferFunctions::transferFunctionExit(Edge edge, const EState* estate) {
    EState currentEState=*estate;
  if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source()))) {
    // 1) determine all local variables (including formal parameters) of function
    // 2) delete all local variables from state
    // 2a) remove variable from state
    // 2b) remove all constraints concerning this variable
    // 3) create new EState and return

    // ad 1)
    set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
    // ad 2)
    ConstraintSet cset=*currentEState.constraints();
    PState newPState=*(currentEState.pstate());
    VariableIdMapping::VariableIdSet localVars=_analyzer->getVariableIdMapping()->determineVariableIdsOfVariableDeclarations(varDecls);
    SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    VariableIdMapping::VariableIdSet formalParams=_analyzer->getVariableIdMapping()->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
    VariableIdMapping::VariableIdSet vars=localVars+formalParams;
    set<string> names=_analyzer->variableIdsToVariableNames(vars);

    for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
      VariableId varId=*i;
      newPState.deleteVar(varId);
      //cset.removeAllConstraintsOfVar(varId);
    }
    // ad 3)
    return elistify(createEState(edge.target(),estate->callString,newPState,cset));
  } else {
    logger[FATAL] << "no function definition associated with function exit label."<<endl;
    exit(1);
  }
}

// called from transferForkFunction
std::list<EState> EStateTransferFunctions::transferForkFunctionWithExternalTargetFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall) {
  //_analyzer->recordExternalFunctionCall(funCall); funcall would be forkFunction
  // arg5 is expression with functino pointer to external function
  list<EState> estateList;
  EState estate1=*estate;
  estate1.setLabel(edge.target());
  // no forked state
  estateList.push_back(estate1);
  return estateList;
}

std::list<EState> EStateTransferFunctions::transferForkFunction(Edge edge, const EState* estate, SgFunctionCallExp* funCall) {
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  SAWYER_MESG(logger[TRACE])<<getAnalyzer()->_ctOpt.forkFunctionName<<" #args:"<<actualParameters.size()<<endl;
  // get 5th argument
  SgExpressionPtrList::iterator pIter=actualParameters.begin();
  for(int j=1;j<5;j++) {
    ++pIter;
  }        
  SgExpression* actualParameterExpr=*pIter;
  // general case: the actual argument is an arbitrary expression (including a single variable)
  list<SingleEvalResultConstInt> evalResultList=getExprAnalyzer()->evaluateExpression(actualParameterExpr,currentEState);
  if(evalResultList.size()==0) {
    SAWYER_MESG(logger[FATAL])<<"Internal error: no state computed for argument evaluation at: "<<SgNodeHelper::sourceLineColumnToString(getLabeler()->getNode(edge.source()))<<endl;
    SAWYER_MESG(logger[FATAL])<<"Argument expression: "<<actualParameterExpr->unparseToString()<<endl;
    SAWYER_MESG(logger[FATAL])<<"EState: "<<currentEState.toString(getVariableIdMapping())<<endl;
    exit(1);
  }
  list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
  SingleEvalResultConstInt evalResult=*resultListIter;
  if(evalResultList.size()>1) {
    SAWYER_MESG(logger[ERROR]) <<"multi-state generating operators in function call parameters not supported."<<endl;
    exit(1);
  }
  AbstractValue arg5Value=evalResult.value();
  // this result value has to be a function pointer value, create a state (representing the fork), and continue with current state
  if(!arg5Value.isFunctionPtr()) {
    // case where no source exists for function pointer
    // handle like any other external function call
    return transferForkFunctionWithExternalTargetFunction(edge,estate,funCall);
  }
  // TODO: create state with this function label as start state
  EState forkedEState=*estate;
  // set target label in new state to function pointer label
  forkedEState.setLabel(arg5Value.getLabel());
        
  // allow both formats x=f(...) and f(...)
  SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
  if(assignOp) {
    list<EState> estateList1=transferAssignOp(assignOp,edge,estate); // use current estate, do not mix with forked state
    ROSE_ASSERT(estateList1.size()==1);
    EState estate1=*estateList1.begin();
    estate1.setLabel(edge.target());
    list<EState> estateList2;
    estateList2.push_back(estate1);
    estateList2.push_back(forkedEState);
    return estateList2;
  } else {
    list<EState> estateList;
    EState estate1=*estate;
    estate1.setLabel(edge.target());
    estateList.push_back(estate1);
    estateList.push_back(forkedEState);
    return estateList;
  }
}

std::list<EState> EStateTransferFunctions::transferFunctionCallExternal(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  SAWYER_MESG(logger[TRACE]) << "transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

  //cout<<"DEBUG: transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

  InputOutput newio;
  Label lab=_analyzer->getLabeler()->getLabel(nextNodeToAnalyze1);

  VariableId varId;
  bool isFunctionCallWithAssignmentFlag=_analyzer->isFunctionCallWithAssignment(lab,&varId);

  SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1);
  _analyzer->recordExternalFunctionCall(funCall);
  _analyzer->evaluateFunctionCallArguments(edge,funCall,*estate,false);

  // TODO: check whether the following test is superfluous meanwhile, since isStdInLabel does take NonDetX functions into account
  bool isExternalNonDetXFunction=false;
  if(_analyzer->svCompFunctionSemantics()) {
    if(funCall) {
      string externalFunctionName=SgNodeHelper::getFunctionName(funCall);
      if(externalFunctionName==_analyzer->_externalNonDetIntFunctionName||externalFunctionName==_analyzer->_externalNonDetLongFunctionName) {
        isExternalNonDetXFunction=true;
      }
    }
  }
  if(isExternalNonDetXFunction || _analyzer->getLabeler()->isStdInLabel(lab,&varId)) {
    if(_analyzer->_inputSequence.size()>0) {
      PState newPState=*currentEState.pstate();
      ConstraintSet newCSet=*currentEState.constraints();
      //newCSet.removeAllConstraintsOfVar(varId);
      list<EState> resList;
      int newValue;
      if(_analyzer->_inputSequenceIterator!=_analyzer->_inputSequence.end()) {
        newValue=*_analyzer->_inputSequenceIterator;
        ++_analyzer->_inputSequenceIterator;
        //cout<<"INFO: input sequence value: "<<newValue<<endl;
      } else {
        return resList; // return no state (this ends the analysis)
      }
      if(_analyzer->getOptionsRef().inputValuesAsConstraints) {
        SAWYER_MESG(logger[FATAL])<<"Option input-values-as-constraints no longer supported."<<endl;
        exit(1);
        //newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=CodeThorn::Top();
        //newPState.writeTopToMemoryLocation(varId);
        //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(newValue)));
        //ROSE_ASSERT(newCSet.size()>0);
      } else {
        //newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=AbstractValue(newValue);
        getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(newValue));
      }
      newio.recordVariable(InputOutput::STDIN_VAR,varId);
      EState newEState=createEState(edge.target(),cs,newPState,newCSet,newio);
      resList.push_back(newEState);
      return resList;
    } else {
      if(_analyzer->_inputVarValues.size()>0) {
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        //newCSet.removeAllConstraintsOfVar(varId);
        list<EState> resList;
        for(set<int>::iterator i=_analyzer->_inputVarValues.begin();i!=_analyzer->_inputVarValues.end();++i) {
          PState newPState=*currentEState.pstate();
          if(_analyzer->getOptionsRef().inputValuesAsConstraints) {
            SAWYER_MESG(logger[FATAL])<<"Option input-values-as-constraints no longer supported."<<endl;
            exit(1);
            //newCSet.removeAllConstraintsOfVar(varId);
            //newPState[varId]=CodeThorn::Top();
            //newPState.writeTopToMemoryLocation(varId);
            //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(*i)));
            //assert(newCSet.size()>0);
          } else {
            //newCSet.removeAllConstraintsOfVar(varId);
            // new input value must be const (otherwise constraints must be used)
            //newPState[varId]=AbstractValue(*i);
            getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(*i));
          }
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          EState newEState=createEState(edge.target(),estate->callString,newPState,newCSet,newio);
          resList.push_back(newEState);
        }
        return resList;
      } else {
        // without specified input values (default mode: analysis performed for all possible input values)
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        // update input var
        //newCSet.removeAllConstraintsOfVar(varId);
        newPState.writeTopToMemoryLocation(varId);
        newio.recordVariable(InputOutput::STDIN_VAR,varId);

        // external call context
        // call string is reused from input-estate. An external function call does not change the call string
        // callReturn node must check for being an external call
        return elistify(createEState(edge.target(),cs,newPState,newCSet,newio));
      }
    }
  }

  if(_analyzer->getInterpreterMode()!=IM_ENABLED) {
    int constvalue=0;
    if(_analyzer->getLabeler()->isStdOutVarLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDOUT_VAR,varId);
      ROSE_ASSERT(newio.var==varId);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    } else if(_analyzer->getLabeler()->isStdOutConstLabel(lab,&constvalue)) {
      newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    } else if(_analyzer->getLabeler()->isStdErrLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDERR_VAR,varId);
      ROSE_ASSERT(newio.var==varId);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    }
  }

  /* handling of specific semantics for external function */
  if(funCall) {
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(_analyzer->svCompFunctionSemantics()) {
      if(funName==_analyzer->_externalErrorFunctionName) {
        //cout<<"DETECTED error function: "<<_externalErrorFunctionName<<endl;
        return elistify(_analyzer->createVerificationErrorEState(currentEState,edge.target()));
      } else if(funName==_analyzer->_externalExitFunctionName) {
        /* the exit function is modeled to terminate the program
           (therefore no successor state is generated)
        */
        return elistify();
      } else {
        // dispatch all other external function calls to the other
        // transferFunctions where the external function call is handled as an expression
        if(isFunctionCallWithAssignmentFlag) {
          // here only the specific format x=f(...) can exist
          SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
          ROSE_ASSERT(assignOp);
          return transferAssignOp(assignOp,edge,estate);
        } else {
          // special case: void function call f(...);
          list<SingleEvalResultConstInt> res=getExprAnalyzer()->evalFunctionCall(funCall,currentEState);
          // build new estate(s) from single eval result list
          list<EState> estateList;
          for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
            EState estate=(*i).estate;
            PState newPState=*estate.pstate();
            ConstraintSet cset=*estate.constraints();
            estateList.push_back(createEState(edge.target(),cs,newPState,cset));
          }
          return estateList;
        }
      }
    }

    if(getAnalyzer()->_ctOpt.forkFunctionEnabled) {
      if(funName==getAnalyzer()->_ctOpt.forkFunctionName) {
        return transferForkFunction(edge,estate,funCall);
      }
    }

    if(isFunctionCallWithAssignmentFlag) {
      // here only the specific format x=f(...) can exist
      SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
      ROSE_ASSERT(assignOp);
      return transferAssignOp(assignOp,edge,estate);
    } else {
      // all other cases, evaluate function call as expression
      //cout<<"DEBUG: external function call: "<<funCall->unparseToString()<<"; evaluating as expression."<<endl;
      list<SingleEvalResultConstInt> res2=getExprAnalyzer()->evaluateExpression(funCall,currentEState);
      ROSE_ASSERT(res2.size()==1);
      SingleEvalResultConstInt evalResult2=*res2.begin();
      SAWYER_MESG(logger[TRACE])<<"EXTERNAL FUNCTION: "<<SgNodeHelper::getFunctionName(funCall)<<" result(added to state):"<<evalResult2.result.toString()<<endl;

      //EState estate2=evalResult2.estate;
      // create new estate with added return variable (for inter-procedural analysis)
      CallString cs=evalResult2.estate.callString;
      PState newPState=*evalResult2.estate.pstate();
      ConstraintSet cset=*evalResult2.estate.constraints();
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=_analyzer->getVariableIdMapping()->createUniqueTemporaryVariableId(string("$return"));
      }
      // added function call result value to state. The returnVarId does not correspond to a declaration, and therefore it ise
      // treated as being initialized (and declared).
      getExprAnalyzer()->initializeMemoryLocation(currentEState.label(),&newPState,returnVarId,evalResult2.result);
      //estate2.setLabel(edge.target());
      return elistify(createEState(edge.target(),cs,newPState,cset,evalResult2.estate.io));
    }
  }
  //cout<<"DEBUG: identity: "<<funCall->unparseToString()<<endl; // fflush is an example in the test cases
  // for all other external functions we use identity as transfer function
  EState newEState=currentEState;
  newEState.io=newio;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}

std::list<EState> EStateTransferFunctions::transferDefaultOptionStmt(SgDefaultOptionStmt* defaultStmt,Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: DEFAULTSTMT: "<<defaultStmt->unparseToString()<<endl;

  Label targetLabel=edge.target();
  CallString cs=estate->callString;
  PState newPState=*estate->pstate();
  ConstraintSet cset=*estate->constraints();
  SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
  ROSE_ASSERT(blockStmt);
  SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
  ROSE_ASSERT(switchStmt);
  SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
  ROSE_ASSERT(condStmt);
  SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

  EState currentEState=*estate;

  // value of switch expression
  AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);

  // if there is at least one case that is definitely reachable, then
  // the default (label) is non-reachable (fall-through still
  // applies), otherwise default is may-reachable
  bool defaultReachable=true;

  // create filter for all case labels (TODO: precompute this set)
  set<SgCaseOptionStmt*> caseStmtSet=SgNodeHelper::switchRelevantCaseStmtNodes(blockStmt); // argument must be the blockStmt of swithStmt
  SAWYER_MESG(logger[TRACE])<<"castStmtSet.size(): "<<caseStmtSet.size()<<endl;
  for(set<SgCaseOptionStmt*>::iterator i=caseStmtSet.begin();i!=caseStmtSet.end();++i) {
    SAWYER_MESG(logger[TRACE])<<"switch-stmt (@default): analyzing case "<<(*i)->unparseToString()<<endl;
    SgCaseOptionStmt* caseStmt=*i;
    SgExpression* caseExpr=caseStmt->get_key();
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd) {
      AbstractValue caseValRangeBegin=singleValevaluateExpression(caseExpr,currentEState);
      AbstractValue caseValRangeEnd=singleValevaluateExpression(caseExprOptionalRangeEnd,currentEState);
      AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
      AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
      if(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue()) {
        SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
        defaultReachable=false;
        break;
      }
    }
    // value of constant case value
    AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
    // compare case constant with switch expression value
    SAWYER_MESG(logger[TRACE])<<"switch-default filter cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
    // check that not any case label may be equal to the switch-expr value (exact for concrete values)
    AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
    // top is *not* considered here, because only a definitive case makes default non-reachable
    if(comparisonVal.isTrue()) {
      // determined that at least one case may be reachable
      SAWYER_MESG(logger[TRACE])<<"switch-default: continuing."<<endl;
      //return elistify(createEState(targetLabel,cs,newPState,cset));
      SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
      defaultReachable=false;
      break;
    }
  }
  if(defaultReachable) {
    SAWYER_MESG(logger[TRACE])<<"switch-default: reachable."<<endl;
    return elistify(createEState(targetLabel,cs,newPState,cset));
  } else {
    // detected infeasable path (default is not reachable)
    SAWYER_MESG(logger[TRACE])<<"switch-default: infeasable path."<<endl;
    list<EState> emptyList;
    return emptyList;
  }
}

std::list<EState> EStateTransferFunctions::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE])<<"CASESTMT: "<<caseStmt->unparseToString()<<endl;
  Label targetLabel=edge.target();
  CallString cs=estate->callString;
  PState newPState=*estate->pstate();
  ConstraintSet cset=*estate->constraints();
  SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
  ROSE_ASSERT(blockStmt);
  SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
  ROSE_ASSERT(switchStmt);
  SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
  ROSE_ASSERT(condStmt);
  SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

  EState currentEState=*estate;

  // value of switch expression
  AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);

  SgExpression* caseExpr=caseStmt->get_key();
  SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
  if(caseExprOptionalRangeEnd) {
    AbstractValue caseValRangeBegin=singleValevaluateExpression(caseExpr,currentEState);
    AbstractValue caseValRangeEnd=singleValevaluateExpression(caseExprOptionalRangeEnd,currentEState);
    AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
    AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
    if(comparisonValBegin.isTop()||comparisonValEnd.isTop()||(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue())) {
      SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: continuing."<<endl;
      return elistify(createEState(targetLabel,cs,newPState,cset));
    } else {
      SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: infeasable path."<<endl;
      // infeasable path
      list<EState> emptyList;
      return emptyList;
    }
  }
  // value of constant case value
  AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
  // compare case constant with switch expression value
  SAWYER_MESG(logger[TRACE])<<"switch cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
  AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
  if(comparisonVal.isTop()||comparisonVal.isTrue()) {
    SAWYER_MESG(logger[TRACE])<<"switch-case: continuing."<<endl;
    return elistify(createEState(targetLabel,cs,newPState,cset));
  } else {
    // infeasable path
    SAWYER_MESG(logger[TRACE])<<"switch-case: infeasable path."<<endl;
    list<EState> emptyList;
    return emptyList;
  }
}

  std::list<EState> EStateTransferFunctions::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
    return elistify(analyzeVariableDeclaration(decl,*estate, edge.target()));
  }

  std::list<EState> EStateTransferFunctions::transferGnuExtensionStmtExpr(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
    //cout<<"WARNING: ignoring GNU extension StmtExpr (EStateTransferFunctions::transferGnuExtensionStmtExpr)"<<endl;
    return elistify(*estate);
  }

std::list<EState> EStateTransferFunctions::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
  SgNode* nextNodeToAnalyze2=0;
  if(isSgExprStatement(nextNodeToAnalyze1))
    nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
  if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    nextNodeToAnalyze2=nextNodeToAnalyze1;
  }
  ROSE_ASSERT(nextNodeToAnalyze2);

  //Label newLabel;
  //PState newPState;
  //ConstraintSet newCSet;
  if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
    return transferTrueFalseEdge(nextNodeToAnalyze2, edge, estate);
  } else if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2) || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
    return transferIncDecOp(nextNodeToAnalyze2,edge,estate);
  } else if(SgAssignOp* assignOp=isSgAssignOp(nextNodeToAnalyze2)) {
    return transferAssignOp(assignOp, edge, estate);
  } else if(isSgCompoundAssignOp(nextNodeToAnalyze2)) {
    logger[ERROR] <<"compound assignment operators not supported. Use normalization to eliminate these operators."<<endl;
    logger[ERROR] <<"expr: "<<nextNodeToAnalyze2->unparseToString()<<endl;
    exit(1);
  } else if(isSgConditionalExp(nextNodeToAnalyze2)) {
    // this is meanwhile modeled in the ExprAnalyzer - TODO: utilize as expr-stmt.
    logger[ERROR] <<"found conditional expression outside expression (should have been dispatched to ExprAnalyzer)."<<endl;
    exit(1);
  } else {
    return transferIdentity(edge,estate);
  }
}

list<EState> EStateTransferFunctions::transferIdentity(Edge edge, const EState* estate) {
  // nothing to analyze, just create new estate (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=*estate;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}


std::list<EState> EStateTransferFunctions::transferAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE]) << "transferAssignOp:"<<nextNodeToAnalyze2->unparseToString()<<endl;
  auto pList=evalAssignOp(nextNodeToAnalyze2, edge, estate);
  std::list<EState> estateList;
  for (auto p : pList) {
    EState estate=p.first;
    AbstractValue lhsAddress=p.second.first;
    AbstractValue rhsValue=p.second.second;
    Label label=estate.label();
    PState newPState=*estate.pstate();
    ConstraintSet cset=*estate.constraints();
    if(lhsAddress.isReferenceVariableAddress())
      getExprAnalyzer()->writeToReferenceMemoryLocation(label,&newPState,lhsAddress,rhsValue);
    else
      getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAddress,rhsValue);
    CallString cs=estate.callString;
    estateList.push_back(createEState(edge.target(),cs,newPState,cset));
  }
  SAWYER_MESG(logger[TRACE]) << "transferAssignOp: finished"<<endl;
  return estateList;
}

  list<EState> EStateTransferFunctions::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
    EState currentEState=*estate;
    CallString cs=estate->callString;

  SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
  VariableId var;
  if(getExprAnalyzer()->checkIfVariableAndDetermineVarId(nextNodeToAnalyze3,var)) {
    list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateExpression(nextNodeToAnalyze3,currentEState);
    ROSE_ASSERT(res.size()==1); // must hold for currently supported limited form of ++,--
    list<SingleEvalResultConstInt>::iterator i=res.begin();
    EState estate=(*i).estate;
    PState newPState=*estate.pstate();
    ConstraintSet cset=*estate.constraints();

    AbstractValue oldVarVal=getExprAnalyzer()->readFromMemoryLocation(estate.label(),&newPState,var);
    AbstractValue newVarVal;
    AbstractValue const1=1;
    switch(nextNodeToAnalyze2->variantT()) {
    case V_SgPlusPlusOp:
      newVarVal=AbstractValue::operatorAdd(oldVarVal,const1); // overloaded binary + operator
      break;
    case V_SgMinusMinusOp:
      newVarVal=AbstractValue::operatorSub(oldVarVal,const1); // overloaded binary - operator
      break;
    default:
      logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
      logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
      logger[ERROR] << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
      logger[ERROR] <<"programmatic error in handling of inc/dec operators."<<endl;
      exit(1);
    }
    //newPState[var]=varVal;
    // TODOREF
    getExprAnalyzer()->writeToMemoryLocation(estate.label(),&newPState,AbstractValue::createAddressOfVariable(var),newVarVal);

#if 0
    if(!(*i).result.isTop())
      cset.removeAllConstraintsOfVar(var);
#endif
    list<EState> estateList;
    estateList.push_back(createEState(edge.target(),cs,newPState,cset));
    return estateList;
  } else {
    throw CodeThorn::Exception("Error: currently inc/dec operators are only supported for variables.");
  }
}

  EState EStateTransferFunctions::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {

  /*
    1) declaration of variable or array
    - AggregateInitializer (e.g. T a[]={1,2,3};)
    - AggregateInitializer (e.g. T a[5]={1,2,3};)
    - AssignInitializer (e.g. T x=1+2;)
    2) if array, determine size of array (from VariableIdMapping)
    3) if no size is provided, determine it from the initializer list (and add this information to the variableIdMapping - or update the variableIdMapping).
   */

  SAWYER_MESG(logger[TRACE])<<"analyzeVariableDeclaration:"<<decl->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
  CallString cs=currentEState.callString;
  Label label=currentEState.label();

  const SgInitializedNamePtrList& initNameList=decl->get_variables();
  SgNode* initName0;
  if(initNameList.size()>1) {
    SAWYER_MESG(logger[ERROR])<<"Error: variable declaration contains more than one variable. Normalization required."<<endl;
    SAWYER_MESG(logger[ERROR])<<"Error: "<<decl->unparseToString()<<endl;

    exit(1);
  } else {
    //SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initNameList.size()==1);
    initName0=*initNameList.begin();
  }
  if(initName0!=nullptr) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
      // not possible to support yet. getIntValue must succeed on declarations.
      /*
      if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
        // DEAD CODE
        PState newPState=*currentEState.pstate();
        newPState.reserveMemoryLocation(initDeclVarId);
        ConstraintSet cset=*currentEState.constraints();
        InputOutput io;
        io.recordNone();
        return createEState(targetLabel,cs,newPState,cset);
      }
      */
      // deactivated 05/20/2020
      //      if(getVariableIdMapping()->isOfArrayType(initDeclVarId) && _ctOpt.explicitArrays==false) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        // logger[DEBUG] <<"not adding array to PState."<<endl;
      //  PState newPState=*currentEState.pstate();
      //  ConstraintSet cset=*currentEState.constraints();
      //  return createEState(targetLabel,cs,newPState,cset);
      //}

      ConstraintSet cset=*currentEState.constraints();
      SgInitializer* initializer=initName->get_initializer();
      if(initializer) {
        //cout<<"DEBUG: decl-init: "<<decl->unparseToString()<<":AST:"<<AstTerm::astTermWithNullValuesToString(initializer)<<endl;
        // special case that initializer is an assignment (reusing transferAssignOp)
        if(SgAssignInitializer* assignInit=isSgAssignInitializer(initializer)) {
          SgExpression* assignInitOperand=assignInit->get_operand_i();
          ROSE_ASSERT(assignInitOperand);
          if(SgAssignOp* assignOp=isSgAssignOp(assignInitOperand)) {
            SAWYER_MESG(logger[TRACE])<<"assignment in initializer: "<<decl->unparseToString()<<endl;
            //cout<<"DEBUG: assignment in initializer: "<<decl->unparseToString()<<endl;
            Edge dummyEdge(targetLabel,EDGE_FORWARD,targetLabel); // only target label is used in transferAssignOp
            //std::list<EState> estateList=transferAssignOp(assignOp, dummyEdge, &currentEState);
            CodeThorn::EStateTransferFunctions::MemoryUpdateList memUpdList=evalAssignOp(assignOp,dummyEdge,&currentEState);
            std::list<EState> estateList;
            ROSE_ASSERT(memUpdList.size()==1);
            auto memUpd=*memUpdList.begin();
            // code is normalized, lhs must be a variable : tmpVar= var=val;
            // store result of assignment in declaration variable
            EState estate=memUpd.first;
            PState newPState=*estate.pstate();
            AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
            AbstractValue lhsAddr=memUpd.second.first;
            AbstractValue rhsValue=memUpd.second.second;
            getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAddr,rhsValue); // assignment in initializer
            getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,rhsValue); // initialization of declared var
            return createEState(targetLabel,cs,newPState,cset);
          } else if(SgFunctionRefExp* funRefExp=isSgFunctionRefExp(assignInitOperand)) {
            //cout<<"DEBUG: DETECTED isSgFunctionRefExp: evaluating expression ..."<<endl;
            list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateExpression(funRefExp,currentEState);
            if(res.size()!=1) {
              if(res.size()>1) {
                SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
                SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
                exit(1);
              } else {
                ROSE_ASSERT(res.size()==0);
                SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
              }
            } else {
              ROSE_ASSERT(res.size()==1);
              SingleEvalResultConstInt evalResult=*res.begin();
              SAWYER_MESG(logger[TRACE])<<"rhs eval result 2: "<<evalResult.result.toString()<<endl;
              
              EState estate=evalResult.estate;
              PState newPState=*estate.pstate();
              AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
              getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
              ConstraintSet cset=*estate.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
        }
        if(getVariableIdMapping()->isOfClassType(initDeclVarId)) {
          SAWYER_MESG(logger[WARN])<<"initialization of structs not supported yet (not added to state) "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
          // TODO: for(offset(membervar) : membervars {initialize(address(initDeclVarId)+offset,eval(initializer+));}
          //AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          // TODO: STRUCT VARIABLE DECLARATION
          //getExprAnalyzer()->reserveMemoryLocation(label,&newPState,pointerVal);
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
        if(getVariableIdMapping()->isOfReferenceType(initDeclVarId)) {
          SAWYER_MESG(logger[TRACE])<<"initialization of reference:"<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
          SgAssignInitializer* assignInit=isSgAssignInitializer(initializer);
          ROSE_ASSERT(assignInit);
          SgExpression* assignInitOperand=assignInit->get_operand_i();
          ROSE_ASSERT(assignInitOperand);
          list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateLExpression(assignInitOperand,currentEState);

          SAWYER_MESG(logger[TRACE])<<"initialization of reference:"<<AstTerm::astTermWithNullValuesToString(assignInitOperand)<<endl;
          if(res.size()>1) {
            SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
            SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
            exit(1);
          } else if(res.size()==0) {
            // TODO: remove this case once initialization is fully supported
            SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
            AbstractValue result=AbstractValue::createTop();
            EState estate=currentEState;
            PState newPState=*estate.pstate();
            AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
            //initDeclVarAddr.setRefType(); // known to be ref from isOfReferenceType above
            // creates a memory cell in state that contains the address of the referred memory cell
            getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,result);
            ConstraintSet cset=*estate.constraints();
            return createEState(targetLabel,cs,newPState,cset);
          }
          SingleEvalResultConstInt evalResult=*res.begin();
          SAWYER_MESG(logger[TRACE])<<"rhs (reference init) result: "<<evalResult.result.toString()<<endl;
            
          EState estate=evalResult.estate;
          PState newPState=*estate.pstate();
          AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
          //initDeclVarAddr.setRefType(); // known to be ref from isOfReferenceType above
          // creates a memory cell in state that contains the address of the referred memory cell
          getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
          ConstraintSet cset=*estate.constraints();
          return createEState(targetLabel,cs,newPState,cset);
        }
        // has aggregate initializer
        if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
          if(SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type())) {
            SgType* arrayElementType=arrayType->get_base_type();
            setElementSize(initDeclVarId,arrayElementType);
            // only set size from aggregate initializer if not known from type
            if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==0) {
               getVariableIdMapping()->setNumberOfElements(initDeclVarId, _analyzer->computeNumberOfElements(decl));
            }
            PState newPState=*currentEState.pstate();
            newPState=analyzeSgAggregateInitializer(initDeclVarId, aggregateInitializer,newPState, currentEState);
            return createEState(targetLabel,cs,newPState,cset);
          } else {
            // type not supported yet
            SAWYER_MESG(logger[WARN])<<"aggregate initializer: unsupported type at: "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<" : "<<aggregateInitializer->get_type()->unparseToString()<<endl;
            // do not modify state. Value remains top.
            PState newPState=*currentEState.pstate();
            ConstraintSet cset=*currentEState.constraints();
            return createEState(targetLabel,cs,newPState,cset);
          }
        } else if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
          SgExpression* rhs=assignInitializer->get_operand_i();
          ROSE_ASSERT(rhs);
          SAWYER_MESG(logger[TRACE])<<"declaration with assign initializer:"<<" lhs:"<<initDeclVarId.toString(getVariableIdMapping())<<" rhs:"<<assignInitializer->unparseToString()<<" decl-term:"<<AstTerm::astTermWithNullValuesToString(initName)<<endl;

          // only create string in state with variable as pointer-address if it is an array (not for the case it is a char* pointer)
          // in the case of char* it is handled as a pointer initializer (and the string-pointer is already available in state)
          if(SgStringVal* stringValNode=isSgStringVal(assignInitializer->get_operand())) {
            if(isSgArrayType(initName->get_type())) {
              // handle special cases of: char a[]="abc"; char a[4]="abc";
              // TODO: a[5]="ab";
              SAWYER_MESG(logger[TRACE])<<"Initalizing (array) with string: "<<stringValNode->unparseToString()<<endl;
              //size_t stringLen=stringValNode->get_value().size();
              if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==0) {
                VariableId stringLiteralId=getVariableIdMapping()->getStringLiteralVariableId(stringValNode);
                size_t stringLiteralMemoryRegionSize=getVariableIdMapping()->getNumberOfElements(stringLiteralId);
                getVariableIdMapping()->setNumberOfElements(initDeclVarId,stringLiteralMemoryRegionSize);
                SAWYER_MESG(logger[TRACE])<<"Determined size of array from literal string memory region size: "<<stringLiteralMemoryRegionSize<<endl;
              } else {
                SAWYER_MESG(logger[TRACE])<<"Determined size of array from array variable (containing string memory region) size: "<<getVariableIdMapping()->getNumberOfElements(initDeclVarId)<<endl;
              }
              SgType* variableType=initializer->get_type(); // for char and wchar
              setElementSize(initDeclVarId,variableType); // this must be a pointer, if it's not an array
              PState newPState=*currentEState.pstate();
              _analyzer->initializeStringLiteralInState(newPState,stringValNode,initDeclVarId);
              ConstraintSet cset=*currentEState.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          // set type info for initDeclVarId
          getVariableIdMapping()->setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initializer->get_type();
          setElementSize(initDeclVarId,variableType);

          // build lhs-value dependent on type of declared variable
          //AbstractValue lhsAbstractAddress=AbstractValue(initDeclVarId); // creates a pointer to initDeclVar
          AbstractValue lhsAbstractAddress=AbstractValue::createAddressOfVariable(initDeclVarId); // creates a pointer to initDeclVar
          list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateExpression(rhs,currentEState);
          if(res.size()!=1) {
            if(res.size()>1) {
              SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
              SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
              exit(1);
            } else {
              ROSE_ASSERT(res.size()==0);
              SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<rhs->unparseToString()<<endl;
              EState estate=currentEState;
              PState newPState=*estate.pstate();
              // getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAbstractAddress,CodeThorn::Top());
              getExprAnalyzer()->reserveMemoryLocation(label,&newPState,lhsAbstractAddress);
              ConstraintSet cset=*estate.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          ROSE_ASSERT(res.size()==1);
          SingleEvalResultConstInt evalResult=*res.begin();
          SAWYER_MESG(logger[TRACE])<<"rhs eval result 1: "<<evalResult.result.toString()<<endl;

          EState estate=evalResult.estate;
          PState newPState=*estate.pstate();
          getExprAnalyzer()->initializeMemoryLocation(label,&newPState,lhsAbstractAddress,evalResult.value());
          ConstraintSet cset=*estate.constraints();
          return createEState(targetLabel,cs,newPState,cset);
        } else {
          SAWYER_MESG(logger[WARN]) << "unsupported initializer in declaration: "<<decl->unparseToString()<<" not adding to state (assuming arbitrary value)"<<endl;
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
      } else {
        // no initializer (model default cases)
        ROSE_ASSERT(initName!=nullptr);
        SgArrayType* arrayType=isSgArrayType(initName->get_type());
        if(arrayType) {
          SgType* arrayElementType=arrayType->get_base_type();
          setElementSize(initDeclVarId,arrayElementType);
          int numElements=getVariableIdMapping()->getArrayElementCount(arrayType);
          if(numElements==0) {
            SAWYER_MESG(logger[TRACE])<<"Number of elements in array is 0 (from variableIdMapping) - evaluating expression"<<endl;
            std::vector<SgExpression*> arrayDimExps=SageInterface::get_C_array_dimensions(*arrayType);
            if(arrayDimExps.size()>1) {
              if(_analyzer->getAbstractionMode()==3) {
                CodeThorn::Exception("multi-dimensional arrays not supported yet.");
              } else {
                SAWYER_MESG(logger[WARN])<<"multi-dimensional arrays not supported yet. Only linear arrays are supported. Not added to state (assuming arbitrary value)."<<endl;
              }
              // not adding it to state. Will be used as unknown.
              PState newPState=*currentEState.pstate();
              return createEState(targetLabel,cs,newPState,cset);
            }
            ROSE_ASSERT(arrayDimExps.size()==1);
            SgExpression* arrayDimExp=*arrayDimExps.begin();
            SAWYER_MESG(logger[TRACE])<<"Array dimension expression: "<<arrayDimExp->unparseToString()<<endl;
            list<SingleEvalResultConstInt> evalResultList=getExprAnalyzer()->evaluateExpression(arrayDimExp,currentEState);
            ROSE_ASSERT(evalResultList.size()==1);
            SingleEvalResultConstInt evalRes=*evalResultList.begin();
            AbstractValue arrayDimAVal=evalRes.result;
            SAWYER_MESG(logger[TRACE])<<"Computed array dimension: "<<arrayDimAVal.toString()<<endl;
            if(arrayDimAVal.isConstInt()) {
              numElements=arrayDimAVal.getIntValue();
              getVariableIdMapping()->setNumberOfElements(initDeclVarId,numElements);
            } else {
              if(_analyzer->getAbstractionMode()==3) {
                CodeThorn::Exception("Could not determine size of array (non-const size).");
              }
              // TODO: size of array remains 1?
            }
          } else {
            getVariableIdMapping()->setNumberOfElements(initDeclVarId,numElements);
          }
        } else {
          // set type info for initDeclVarId
          getVariableIdMapping()->setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initName->get_type();
          setElementSize(initDeclVarId,variableType);
        }

	SAWYER_MESG(logger[TRACE])<<"Creating new PState"<<endl;
        PState newPState=*currentEState.pstate();
        if(getVariableIdMapping()->isOfArrayType(initDeclVarId)) {
	  SAWYER_MESG(logger[TRACE])<<"PState: upd: array"<<endl;
          // add default array elements to PState
          auto length=getVariableIdMapping()->getNumberOfElements(initDeclVarId);
	  if(length>0) {
	    SAWYER_MESG(logger[TRACE])<<"DECLARING ARRAY of size: "<<decl->unparseToString()<<":"<<length<<endl;
	    for(CodeThorn::TypeSize elemIndex=0;elemIndex<length;elemIndex++) {
	      AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
	      // set default init value
	      getExprAnalyzer()->reserveMemoryLocation(label,&newPState,newArrayElementAddr);
	    }
	  } else {
	    SAWYER_MESG(logger[TRACE])<<"DECLARING ARRAY of unknown size: "<<decl->unparseToString()<<":"<<length<<endl;
	    AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(0)); // use elem index 0
	      // set default init value
	      getExprAnalyzer()->reserveMemoryLocation(label,&newPState,newArrayElementAddr); // TODO: reserve summary memory location	    
	  }

        } else if(getVariableIdMapping()->isOfClassType(initDeclVarId)) {
	  SAWYER_MESG(logger[TRACE])<<"PState: upd: class"<<endl;
          // create only address start address of struct (on the
          // stack) alternatively addresses for all member variables
          // can be created; however, a member var can only be
          // assigned by denoting an element relative to the start of
          // the struct, similar only a pointer can be created. the
          // value is actually uninitialized and therefore is
          // implicitly bot.
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          SAWYER_MESG(logger[TRACE])<<"declaration of struct: "<<getVariableIdMapping()->getVariableDeclaration(initDeclVarId)->unparseToString()<<" : "<<pointerVal.toString(getVariableIdMapping())<<endl;
          // TODO: STRUCT VARIABLE DECLARATION
          getExprAnalyzer()->reserveMemoryLocation(label,&newPState,pointerVal);
        } else if(getVariableIdMapping()->isOfPointerType(initDeclVarId)) {
	  SAWYER_MESG(logger[TRACE])<<"PState: upd: pointer"<<endl;
          // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,pointerVal);
        } else {
          // set it to top (=any value possible (uninitialized)) for
          // all remaining cases. It will become an error-path once
          // all cases are addressed explicitly above.
          SAWYER_MESG(logger[TRACE])<<"declaration of variable (other): "<<getVariableIdMapping()->getVariableDeclaration(initDeclVarId)->unparseToString()<<endl;
          getExprAnalyzer()->reserveMemoryLocation(label,&newPState,AbstractValue::createAddressOfVariable(initDeclVarId));
        }
	SAWYER_MESG(logger[TRACE])<<"Creating new EState"<<endl;
        return createEState(targetLabel,cs,newPState,cset);
      }
    } else {
      logger[ERROR] << "in declaration (@initializedName) no variable found ... bailing out."<<endl;
      exit(1);
    }
  } else {
    logger[ERROR] << "in declaration: no variable found ... bailing out."<<endl;
    exit(1);
  }
  ROSE_ASSERT(false); // non-reachable
}

  // set the size of an element determined by this type
void EStateTransferFunctions::setElementSize(VariableId variableId, SgType* elementType) {
  unsigned int typeSize=getVariableIdMapping()->getTypeSize(elementType);
  if(getVariableIdMapping()->getElementSize(variableId)!=0
     && getVariableIdMapping()->getElementSize(variableId)!=typeSize) {
    SAWYER_MESG(logger[WARN])<<"Element type size mismatch: "
                <<"EStateTransferFunctions::setElementSize: variableId name: "<<getVariableIdMapping()->variableName(variableId)
                <<"typesize(from VID):"<<getVariableIdMapping()->getElementSize(variableId)
                <<" typeSize(fromtype): "<<typeSize<<" of "<<elementType->unparseToString()
                <<endl;
    // keep existing size from variableIdMapping
    return;
  }
  getVariableIdMapping()->setElementSize(variableId,typeSize);
}

void EStateTransferFunctions::initializeGlobalVariablesNew(SgProject* root, EState& estate) {
  if(SgProject* project=isSgProject(root)) {
    ROSE_ASSERT(getVariableIdMapping());
    CodeThorn::VariableIdSet setOfGlobalVars=getVariableIdMapping()->getSetOfGlobalVarIds();
    CodeThorn::VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,getVariableIdMapping());
    std::set<VariableId> setOfFilteredGlobalVars=CodeThorn::setIntersect(setOfGlobalVars, setOfUsedVars);
    uint32_t numFilteredVars=setOfGlobalVars.size()-setOfFilteredGlobalVars.size();
    if(getAnalyzer()->_ctOpt.status) {
      cout<< "STATUS: Number of global variables: "<<setOfGlobalVars.size()<<endl;
      cout<< "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;
    }
    std::list<SgVariableDeclaration*> relevantGlobalVariableDecls=(getAnalyzer()->_ctOpt.initialStateFilterUnusedVariables)?
      getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfFilteredGlobalVars)
      : getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfGlobalVars)
      ;
    uint32_t declaredInGlobalState=0;
    for(auto decl : relevantGlobalVariableDecls) {
      estate=analyzeVariableDeclaration(decl,estate,estate.label());
      declaredInGlobalState++;
      // this data is only used by globalVarIdByName to determine rers 'output' variable name in binary mode
      globalVarName2VarIdMapping[getVariableIdMapping()->variableName(getVariableIdMapping()->variableId(decl))]=getVariableIdMapping()->variableId(decl);
    }
    if(getAnalyzer()->_ctOpt.status) {
      cout<< "STATUS: Number of unused variables filtered in initial state: "<<numFilteredVars<<endl;
      cout<< "STATUS: Number of global variables declared in initial state: "<<declaredInGlobalState<<endl;
    }
  } else {
    cout<< "STATUS: no global scope. Global state remains without entries.";
  }
}
  
  VariableId EStateTransferFunctions::globalVarIdByName(std::string varName) {
    return globalVarName2VarIdMapping[varName];
  }

// sets all elements in PState according to aggregate
// initializer. Also models default values of Integers (floats not
// supported yet). EState is only used for lookup (modified is only
// the PState object).
PState EStateTransferFunctions::analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PState pstate, /* for evaluation only  */ EState currentEState) {
  //cout<<"DEBUG: AST:"<<AstTerm::astTermWithNullValuesToString(aggregateInitializer)<<endl;
  ROSE_ASSERT(aggregateInitializer);
  SAWYER_MESG(logger[TRACE])<<"analyzeSgAggregateInitializer::array-initializer:"<<aggregateInitializer->unparseToString()<<endl;
  Label label=currentEState.label();
  PState newPState=pstate;
  int elemIndex=0;
  SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
  SgExpressionPtrList& initList=initListObjPtr->get_expressions();

  for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
    AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
    SgExpression* exp=*i;
    SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
    if(assignInit==nullptr) {
      SAWYER_MESG(logger[WARN])<<"expected assign initializer but found "<<exp->unparseToString();
      SAWYER_MESG(logger[WARN])<<"AST: "<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
      AbstractValue newVal=AbstractValue::createTop();
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
    } else {
      // initialize element of array initializer in state
      SgExpression* assignInitExpr=assignInit->get_operand();
      // currentEState from above, newPState must be the same as in currentEState.
      AbstractValue newVal=singleValevaluateExpression(assignInitExpr,currentEState);
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
    }
    elemIndex++;
  }
  // initialize remaining elements (if there are any) with default value
  int aggregateSize=(int)getVariableIdMapping()->getNumberOfElements(initDeclVarId);
  // if array size is not 0 then it was determined from the type and remaining elements are initialized
  if(aggregateSize==0) {
    // update aggregate size if it was not set yet
    aggregateSize=initList.size();
    getVariableIdMapping()->setNumberOfElements(initDeclVarId,aggregateSize);
  }

  // otherwise the size is determined from the aggregate initializer itself (done above)
  if(aggregateSize!=0) {
    for(int i=elemIndex;i<aggregateSize;i++) {
      AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i));
      SAWYER_MESG(logger[TRACE])<<"Init aggregate default value: "<<arrayElemAddr.toString()<<endl;
      // there must be a default value because an aggregate initializer exists (it is never undefined in this case)
      // note: int a[3]={}; also forces the array to be initialized with {0,0,0}, the list can be empty.
      // whereas with int a[3]; all 3 are undefined
      AbstractValue defaultValue=AbstractValue(0); // TODO: cases where default value is not 0.
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,defaultValue);
    }
  } else {
    // if aggregate size is 0 there is nothing to do
  }
  return newPState;
}

AbstractValue EStateTransferFunctions::singleValevaluateExpression(SgExpression* expr,EState currentEState) {
  list<SingleEvalResultConstInt> resultList=getExprAnalyzer()->evaluateExpression(expr,currentEState);
  ROSE_ASSERT(resultList.size()==1);
  SingleEvalResultConstInt valueResult=*resultList.begin();
  AbstractValue val=valueResult.result;
  return val;
}

list<EState> EStateTransferFunctions::transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=estate->callString;
  Label newLabel;
  PState newPState;
  ConstraintSet newCSet;
  list<SingleEvalResultConstInt> evalResultList=getExprAnalyzer()->evaluateExpression(nextNodeToAnalyze2,currentEState);
  list<EState> newEStateList;
  for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
      i!=evalResultList.end();
      ++i) {
    SingleEvalResultConstInt evalResult=*i;
    if(evalResult.isBot()) {
      SAWYER_MESG(logger[WARN])<<"PSTATE: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
      SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT : "<<nextNodeToAnalyze2->unparseToString()<<endl;
      SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT at: "
                               <<ProgramLocationsReport::programLocation(getLabeler(),estate->label())
                               <<endl;
      newLabel=edge.target();
      newPState=*evalResult.estate.pstate();
      ROSE_ASSERT(getExprAnalyzer());
      EState newEstate=createEState(newLabel,cs,newPState,newCSet);
      newEStateList.push_back(newEstate);
    } else if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
      // pass on EState
      newLabel=edge.target();
      newPState=*evalResult.estate.pstate();
      ROSE_ASSERT(getExprAnalyzer());
      if(ReadWriteListener* readWriteListener=getExprAnalyzer()->getReadWriteListener()) {
        readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
      }
      // use new empty cset instead of computed cset
      ROSE_ASSERT(newCSet.size()==0);
      EState newEstate=createEState(newLabel,cs,newPState,newCSet);
      newEStateList.push_back(newEstate);
    } else if((evalResult.isFalse() && edge.isType(EDGE_TRUE)) || (evalResult.isTrue() && edge.isType(EDGE_FALSE))) {
      // we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
      //cout<<"DEBUG: not on feasable execution path. skipping."<<endl;
    } else {
      // all other cases (assume evaluating to true or false, sane as top)
      // pass on EState
      newLabel=edge.target();
      newPState=*evalResult.estate.pstate();
      ROSE_ASSERT(getExprAnalyzer());
      if(ReadWriteListener* readWriteListener=getExprAnalyzer()->getReadWriteListener()) {
        readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
      }
      // use new empty cset instead of computed cset
      ROSE_ASSERT(newCSet.size()==0);
      EState newEstate=createEState(newLabel,cs,newPState,newCSet);
      newEStateList.push_back(newEstate);
    }
  }
  return newEStateList;
}

CodeThorn::EStateTransferFunctions::MemoryUpdateList
CodeThorn::EStateTransferFunctions::evalAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estatePtr) {
  MemoryUpdateList memoryUpdateList;
  CallString cs=estatePtr->callString;
  EState currentEState=*estatePtr;
  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
  list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateExpression(rhs,currentEState, CodeThorn::ExprAnalyzer::MODE_VALUE);
  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
    VariableId lhsVar;
    bool isLhsVar=getExprAnalyzer()->checkIfVariableAndDetermineVarId(lhs,lhsVar);
    if(isLhsVar) {
      EState estate=(*i).estate;
      if(getVariableIdMapping()->isOfClassType(lhsVar)) {
        // assignments to struct variables are not supported yet (this test does not detect s1.s2 (where s2 is a struct, see below)).
        SAWYER_MESG(logger[WARN])<<"assignment of structs (copy constructor) is not supported yet. Target update ignored! (unsound)"<<endl;
      } else if(getVariableIdMapping()->isOfCharType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfIntegerType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfEnumType(lhsVar)) /* PP */ {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfFloatingPointType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfBoolType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfPointerType(lhsVar)) {
        // assume here that only arrays (pointers to arrays) are assigned
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(SgTypeString* lhsTypeTypeString=isSgTypeString(getVariableIdMapping()->getType(lhsVar))) {
        // assume here that only arrays (pointers to arrays) are assigned
        SAWYER_MESG(logger[WARN])<<"DEBUG: LHS assignment: typestring band aid"<<endl;
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(getVariableIdMapping()->isOfReferenceType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else {
        // other types (e.g. function pointer type)
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
#if 0
        SAWYER_MESG(logger[ERROR])<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(nextNodeToAnalyze2)<<endl;
        SAWYER_MESG(logger[ERROR])<<"Unsupported type on LHS side of assignment: "
                                  <<"type lhs: '"<<getVariableIdMapping()->getType(lhsVar)->unparseToString()<<"'"
                                  <<", "
                                  <<"AST type node: "<<getVariableIdMapping()->getType(lhsVar)->class_name()
                                  <<", Expr:"<<nextNodeToAnalyze2->unparseToString()
                                  <<endl;
        exit(1);
#endif
      }
    } else if(isSgDotExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=getExprAnalyzer()->evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
      }
    } else if(isSgArrowExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=getExprAnalyzer()->evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
      }
    } else if(isSgPntrArrRefExp(lhs)) {
      EState estate=(*i).estate;
      PState oldPState=*estate.pstate();
      if(_analyzer->getSkipArrayAccesses()) {
        // estateList.push_back(createEState(edge.target(),cs,oldPState,oldcset));
        // nothing to do (no memory access to add to list)
      } else {
        SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(lhs));
        SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(lhs));
        if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
          PState pstate2=oldPState;
          VariableId arrayVarId=getVariableIdMapping()->variableId(varRefExp);
          AbstractValue arrayPtrValue;
          // two cases
          if(getVariableIdMapping()->isOfArrayType(arrayVarId)) {
            // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
            arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
          } else if(getVariableIdMapping()->isOfPointerType(arrayVarId)) {
            // in case it is a pointer retrieve pointer value
            AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
            if(pstate2.varExists(ptr)) {
              arrayPtrValue=getExprAnalyzer()->readFromMemoryLocation(estate.label(),&pstate2,ptr);
              //cout<<"DEBUG: arrayPtrValue: "<<arrayPtrValue.toString(getVariableIdMapping())<<endl;
              // convert integer to VariableId
              if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
                if(getOptionOutputWarnings())
                  cout<<"Warning: "<<nextNodeToAnalyze2->unparseToString()<<arrayPtrValue.toString(getVariableIdMapping())<<" array index is top or bot. Not supported yet."<<endl;
              }
              // logger[DEBUG]<<"defering pointer-to-array: ptr:"<<getVariableIdMapping()->variableName(arrayVarId);
            } else {
              if(getOptionOutputWarnings())
                cout<<"Warning: lhs array access: pointer variable does not exis2t in PState:"<<ptr.toString()<<endl;
              arrayPtrValue=AbstractValue::createTop();
            }
          } else if(getVariableIdMapping()->isOfReferenceType(arrayVarId)) {
            AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
            if(pstate2.varExists(ptr)) {
              arrayPtrValue=getExprAnalyzer()->readFromReferenceMemoryLocation(estate.label(),&pstate2,ptr);
            } else {
              arrayPtrValue=AbstractValue::createTop();
            }
          } else {
            SAWYER_MESG(logger[ERROR]) <<"lhs array access: unknown type of array or pointer."<<endl;
            exit(1);
          }

          list<SingleEvalResultConstInt> res=getExprAnalyzer()->evaluateExpression(indexExp,currentEState);
          ROSE_ASSERT(res.size()==1); // this should always hold for normalized ASTs
          AbstractValue indexValue=(*(res.begin())).value();
          AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
          AbstractValue arrayElementAddr=arrayPtrPlusIndexValue;
          //cout<<"DEBUG: arrayElementId: "<<arrayElementId.toString(getVariableIdMapping())<<endl;
          //SAWYER_MESG(logger[TRACE])<<"arrayElementVarId:"<<arrayElementId.toString()<<":"<<getVariableIdMapping()->variableName(arrayVarId)<<" Index:"<<index<<endl;
          if(arrayElementAddr.isBot()) {
            // inaccessible memory location, return empty estate list
            return memoryUpdateList;
          }
          // read value of variable var id (same as for VarRefExp - TODO: reuse)
          // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
          //TODO: getExprAnalyzer()->writeToMemoryLocation(label,&pstate2,arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
          memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,(*i).result)));
        } else if(SgAddressOfOp* addressOfOp=isSgAddressOfOp(lhs)) {
          // address of op, need to compute an l-value and use as address
          
        } else {
          list<SingleEvalResultConstInt> arrExpRes=getExprAnalyzer()->evaluateExpression(arrExp,currentEState);
          ROSE_ASSERT(arrExpRes.size()==1); // this should always hold for normalized ASTs
          AbstractValue arrPtrValue=(*(arrExpRes.begin())).value();
          list<SingleEvalResultConstInt> indexExpRes=getExprAnalyzer()->evaluateExpression(indexExp,currentEState);
          ROSE_ASSERT(indexExpRes.size()==1); // this should always hold for normalized ASTs
          AbstractValue indexValue=(*(indexExpRes.begin())).value();
          AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrPtrValue,indexValue);
          AbstractValue arrayElementAddr=arrayPtrPlusIndexValue;
          if(arrayElementAddr.isBot()) {
            return memoryUpdateList; // inaccessible memory location, return empty estate list
          }
          memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,(*i).result)));
          //above evaluation covers all cases now
          //SAWYER_MESG(logger[ERROR]) <<"array-access uses expr for denoting the array. Normalization missing."<<endl;
          //SAWYER_MESG(logger[ERROR]) <<"lhs AST: "<<AstTerm::astTermWithNullValuesToString(lhs)<<endl;
          //SAWYER_MESG(logger[ERROR]) <<"expr: "<<lhs->unparseToString()<<endl;
          //SAWYER_MESG(logger[ERROR]) <<"arraySkip: "<<_analyzer->getSkipArrayAccesses()<<endl;
          //exit(1);
        }
      }
    } else if(SgPointerDerefExp* lhsDerefExp=isSgPointerDerefExp(lhs)) {
      SgExpression* lhsOperand=lhsDerefExp->get_operand();
      SAWYER_MESG(logger[TRACE])<<"lhsOperand: "<<lhsOperand->unparseToString()<<endl;
      list<SingleEvalResultConstInt> resLhs=getExprAnalyzer()->evaluateExpression(lhsOperand,currentEState);
      if(resLhs.size()>1) {
        throw CodeThorn::Exception("more than 1 execution path (probably due to abstraction) in operand's expression of pointer dereference operator on lhs of "+nextNodeToAnalyze2->unparseToString());
      }
      ROSE_ASSERT(resLhs.size()==1);
      AbstractValue lhsPointerValue=(*resLhs.begin()).result;
      SAWYER_MESG(logger[TRACE])<<"lhsPointerValue: "<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
      if(lhsPointerValue.isNullPtr()) {
        getExprAnalyzer()->recordDefinitiveNullPointerDereferenceLocation(estatePtr->label());
        //return estateList;
        if(_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
          // no state can follow, but as requested keep going without effect
        } else {
          // no state can follow
          return memoryUpdateList;
        }
      } else if(lhsPointerValue.isTop()) {
        getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estatePtr->label());
        // specific case a pointer expr evaluates to top. Dereference operation
        // potentially modifies any memory location in the state.
        AbstractValue lhsAddress=lhsPointerValue;
        memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsAddress,(*i).result))); // lhsAddress is TOP!!!
      } else if(!(lhsPointerValue.isPtr())) {
        // changed isUndefined to isTop (2/17/20)
        if(lhsPointerValue.isTop() && _analyzer->getIgnoreUndefinedDereference()) {
          //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
          // skip write access, just create new state (no effect)
          // nothing to do, as it request to not update the state in this case
        } else {
          SAWYER_MESG(logger[WARN])<<"not a pointer value (or top) in dereference operator: lhs-value:"<<lhsPointerValue.toLhsString(getVariableIdMapping())<<" lhs: "<<lhs->unparseToString()<<" : assuming change of any memory location."<<endl;
          getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estatePtr->label());
          // specific case a pointer expr evaluates to top. Dereference operation
          // potentially modifies any memory location in the state.
          // iterate over all elements of the state and merge with rhs value
          memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result))); // lhsPointerValue is TOP!!!
        }
      } else {
        //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
        memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result)));
      }
    } else {
      //cout<<"DEBUG: else (no var, no ptr) ... "<<endl;
      if(_analyzer->getSkipArrayAccesses()&&isSgPointerDerefExp(lhs)) {
        SAWYER_MESG(logger[WARN])<<"skipping pointer dereference: "<<lhs->unparseToString()<<endl;
      } else {
        SAWYER_MESG(logger[ERROR]) << "transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
        SAWYER_MESG(logger[ERROR]) << "expr: "<< lhs->unparseToString()<<endl;
        SAWYER_MESG(logger[ERROR]) << "type: "<<lhs->class_name()<<endl;
        //cerr << "performing no update of state!"<<endl;
        exit(1);
      }
    }
  }
  return memoryUpdateList;
}

  // value semantics for upates
CallString EStateTransferFunctions::transferFunctionCallContext(CallString cs, Label lab) {
  SAWYER_MESG(logger[TRACE])<<"FunctionCallTransfer: adding "<<lab.toString()<<" to cs: "<<cs.toString()<<endl;
  cs.addLabel(lab);
  return cs;
}
bool EStateTransferFunctions::isFeasiblePathContext(CallString& cs,Label lab) {
  return cs.getLength()==0||cs.isLastLabel(lab);
}

  
}
