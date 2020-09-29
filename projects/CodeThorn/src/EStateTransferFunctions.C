#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "Analyzer.h"
#include "AstUtility.h"

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

  void EStateTransferFunctions::setAnalyzer(Analyzer* analyzer) {
    _analyzer=analyzer;
  }

  Analyzer* EStateTransferFunctions::getAnalyzer() {
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
      // logger[DEBUG]<<"ESTATE: "<<estate->toString(&variableIdMapping)<<endl;
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
                                                     AbstractValue::createAddressOfVariable(getAnalyzer()->globalVarIdByName("output")),
                                                     CodeThorn::AbstractValue(rers_result));
            EState _eState=createEState(edge.target(),estate->callString,newPstate,_cset,_io);
            return getAnalyzer()->elistify(_eState);
          }
          RERS_Problem::rersGlobalVarsCallReturnInitFP(getAnalyzer(),_pstate, omp_get_thread_num());
          InputOutput newio;
          if (rers_result == -2) {
            newio.recordVariable(InputOutput::STDERR_VAR,getAnalyzer()->globalVarIdByName("input"));
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
    _analyzer->recordAnalyzedFunction(funDef);
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
      if(_analyzer->getVariableIdMapping()->hasClassType(formalParameterVarId)) {
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
      cs=_analyzer->transferFunctionCallContext(cs, currentEState.label());
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

  SgNode* nextNodeToAnalyze1=_analyzer->cfanalyzer->getNode(edge.source());
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
      if(_analyzer->isFeasiblePathContext(cs,functionCallLabel)) {
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
  SgNode* nextNodeToAnalyze1=_analyzer->cfanalyzer->getNode(edge.source());
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

std::list<EState> EStateTransferFunctions::transferFunctionCallExternal(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=_analyzer->cfanalyzer->getNode(edge.source());
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
      // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
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
        // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
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
  AbstractValue switchCondVal=_analyzer->singleValevaluateExpression(condExpr,currentEState);

  // create filter for all case labels (TODO: precompute this set)
  set<SgCaseOptionStmt*> caseStmtSet=SgNodeHelper::switchRelevantCaseStmtNodes(switchStmt);
  for(set<SgCaseOptionStmt*>::iterator i=caseStmtSet.begin();i!=caseStmtSet.end();++i) {
    SgCaseOptionStmt* caseStmt=*i;
    SgExpression* caseExpr=caseStmt->get_key();
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd) {
      SAWYER_MESG(logger[ERROR])<<"Error: GNU extension range in case statement not supported."<<endl;
      exit(1);
    }
    // value of constant case value
    AbstractValue caseVal=_analyzer->singleValevaluateExpression(caseExpr,currentEState);
    // compare case constant with switch expression value
    SAWYER_MESG(logger[TRACE])<<"switch-default filter cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
    // check that not any case label may be equal to the switch-expr value (exact for concrete values)
    AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
    if(comparisonVal.isTop()||comparisonVal.isTrue()) {
      // determined that at least one case may be reachable
      SAWYER_MESG(logger[TRACE])<<"switch-default: continuing."<<endl;
      return elistify(createEState(targetLabel,cs,newPState,cset));
    }
  }
  // detected infeasable path (default is not reachable)
  SAWYER_MESG(logger[TRACE])<<"switch-default: infeasable path."<<endl;
  list<EState> emptyList;
  return emptyList;
}

std::list<EState> EStateTransferFunctions::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: CASESTMT: "<<caseStmt->unparseToString()<<endl;
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
  AbstractValue switchCondVal=_analyzer->singleValevaluateExpression(condExpr,currentEState);

  SgExpression* caseExpr=caseStmt->get_key();
  SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
  if(caseExprOptionalRangeEnd) {
    SAWYER_MESG(logger[ERROR])<<"Error: GNU extension range in case statement not supported."<<endl;
    exit(1);
  }
  // value of constant case value
  AbstractValue caseVal=_analyzer->singleValevaluateExpression(caseExpr,currentEState);
  // compare case constant with switch expression value
  SAWYER_MESG(logger[TRACE])<<"switch cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
  AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
  if(comparisonVal.isTop()||comparisonVal.isTrue()) {
    SAWYER_MESG(logger[TRACE])<<"switch-case: continuing."<<endl;
    return elistify(createEState(targetLabel,cs,newPState,cset));
  } else {
    // detected infeasable path
    SAWYER_MESG(logger[TRACE])<<"switch-case: infeasable path."<<endl;
    list<EState> emptyList;
    return emptyList;
  }
}

std::list<EState> EStateTransferFunctions::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
  return elistify(_analyzer->analyzeVariableDeclaration(decl,*estate, edge.target()));
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
    return _analyzer->transferTrueFalseEdge(nextNodeToAnalyze2, edge, estate);
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
  logger[TRACE] << "transferAssignOp:"<<nextNodeToAnalyze2->unparseToString()<<endl;
  auto pList=_analyzer->evalAssignOp(nextNodeToAnalyze2, edge, estate);
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
  logger[TRACE] << "transferAssignOp: finished"<<endl;
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
      newVarVal=oldVarVal+const1; // overloaded binary + operator
      break;
    case V_SgMinusMinusOp:
      newVarVal=oldVarVal-const1; // overloaded binary - operator
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

}
