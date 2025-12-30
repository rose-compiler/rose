#include "sage3basic.h"
#include "EStateTransferFunctions.h"
#include "CTAnalysis.h"
#include "AstUtility.h"
#include "Miscellaneous2.h"

#include "CodeThornException.h"
#include "CppStdUtilities.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include "PredefinedSemanticFunctions.h"
#include "AstTerm.h"
#include <map>
#include <iomanip>

// context sensitive transfer functions for function calls are only provided by Solver 18. This is explicitly checked.
#include "Solver18.h"
#include "ROSE_FALLTHROUGH.h"

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

Sawyer::Message::Facility CodeThorn::EStateTransferFunctions::logger;

namespace CodeThorn {

  AbstractValue::Operator EStateTransferFunctions::sgNodeToAbstractValueOperator(SgNode* node) {
    VariantT variant=node->variantT();
    static std::map<VariantT,AbstractValue::Operator>
      mapping={
               {V_SgMinusOp,AbstractValue::Operator::UnaryMinus},
               {V_SgNotOp,AbstractValue::Operator::Not},
               {V_SgBitComplementOp,AbstractValue::Operator::BitwiseComplement},
               {V_SgAddOp,AbstractValue::Operator::Add},
               {V_SgSubtractOp,AbstractValue::Operator::Sub},
               {V_SgMultiplyOp,AbstractValue::Operator::Mul},
               {V_SgDivideOp,AbstractValue::Operator::Div},
               {V_SgModOp,AbstractValue::Operator::Mod},
               {V_SgBitAndOp,AbstractValue::Operator::BitwiseAnd},
               {V_SgBitOrOp,AbstractValue::Operator::BitwiseOr},
               {V_SgBitXorOp,AbstractValue::Operator::BitwiseXor},
               {V_SgLshiftOp,AbstractValue::Operator::BitwiseShiftLeft},
               {V_SgRshiftOp,AbstractValue::Operator::BitwiseShiftRight},
               {V_SgEqualityOp,AbstractValue::Operator::Eq},
               {V_SgNotEqualOp,AbstractValue::Operator::NotEq},
               {V_SgLessThanOp,AbstractValue::Operator::Less},
               {V_SgLessOrEqualOp,AbstractValue::Operator::LessOrEq},
               {V_SgGreaterOrEqualOp,AbstractValue::Operator::MoreOrEq},
               {V_SgGreaterThanOp,AbstractValue::Operator::More},

    };
    return mapping[variant];

  }

  EStateTransferFunctions::EStateTransferFunctions () {
    initDiagnostics();
    initViolatingLocations();
  }
  void EStateTransferFunctions::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
      initialized = true;
      EStateTransferFunctions::logger = Sawyer::Message::Facility("CodeThorn::EStateTransferFunctions", Rose::Diagnostics::destination);
      Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
    }
  }
  Labeler* EStateTransferFunctions::getLabeler() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getLabeler();
  }
  VariableIdMappingExtended* EStateTransferFunctions::getVariableIdMapping() {
    ROSE_ASSERT(_analyzer);
    return _analyzer->getVariableIdMapping();
  }

  void EStateTransferFunctions::setOptionOutputWarnings(bool flag) {
    _optionOutputWarnings=flag;
  }

  bool EStateTransferFunctions::getOptionOutputWarnings() {
    return _optionOutputWarnings;
  }

  void EStateTransferFunctions::setAnalyzer(CTAnalysis* analyzer) {
    _analyzer=analyzer;
  }

  CTAnalysis* EStateTransferFunctions::getAnalyzer() {
    return _analyzer;
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate) {
    EState estate=createEStateInternal(label,pstate);
    estate.setCallString(cs);
    estate.io.recordNone(); // create NONE not bot by default
    return estate;
  }

  EState EStateTransferFunctions::createEState(Label label, CallString cs, PState pstate, InputOutput io) {
    EState estate=createEStateInternal(label,pstate);
    estate.setCallString(cs);
    estate.io=io;
    return estate;
  }

  EStatePtr EStateTransferFunctions::reInitEState(EStatePtr estate, Label label, CallString cs, PStatePtr pstate, InputOutput io) {
    estate->setLabel(label);
    estate->setCallString(cs);
    estate->setPState(pstate);
    estate->io=io;
    return estate;
  }
  EStatePtr EStateTransferFunctions::reInitEState(EStatePtr estate, Label label, CallString cs, PStatePtr pstate) {
    estate->setLabel(label);
    estate->setCallString(cs);
    estate->setPState(pstate);
    estate->io.recordNone(); // create NONE not bot by default
    if(_analyzer->isActiveGlobalTopify()) {
#if 1
      AbstractValueSet varSet=estate->pstate()->getVariableIds();
      for(AbstractValueSet::iterator i=varSet.begin();i!=varSet.end();++i) {
        if(_analyzer->getVariableValueMonitor()->isHotVariable(_analyzer,*i)) {
          estate->pstate()->writeTopToMemoryLocation(*i);
        }
      }
#endif
    }
    return estate;
  }

  // does not use a context
  EState EStateTransferFunctions::createEStateInternal(Label label, PState pstate) {
    // here is the best location to adapt the analysis results to certain global restrictions
    if(_analyzer->isActiveGlobalTopify()) {
#if 1
      AbstractValueSet varSet=pstate.getVariableIds();
      for(AbstractValueSet::iterator i=varSet.begin();i!=varSet.end();++i) {
        if(_analyzer->getVariableValueMonitor()->isHotVariable(_analyzer,*i)) {
          //ROSE_ASSERT(false); // this branch is live
          _analyzer->topifyVariable(pstate, *i);
        }
      }
#else
      //pstate.topifyState();
#endif
    }
    PStatePtr newPStatePtr=_analyzer->processNewOrExisting(pstate); // CHANGING PSTATE-ESTATE
    EState estate=EState(label,newPStatePtr);
    estate.io.recordNone();
    return estate;
  }

  bool EStateTransferFunctions::isApproximatedBy(EStatePtr es1, EStatePtr es2) {
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      cout<<"transfer: isApproximatedBy("<<es1->label().toString()<<","<<es1->label().toString()<<") ==> "<<es1->isApproximatedBy(es2)<<endl;
    }
    return es1->isApproximatedBy(es2);
  }

  // merges es2 into es1
  void EStateTransferFunctions::combineInPlace1st(EStatePtr es1, EStatePtr es2) {
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      cout<<"transfer: combineInPlace1st("<<es1->label().toString()<<","<<es1->label().toString()<<")"<<endl;
    }
    ROSE_ASSERT(es1->label()==es2->label());
    if(es1->getCallString()!=es2->getCallString()) {
      if(_analyzer->getOptionOutputWarnings()) {
        SAWYER_MESG(logger[WARN])<<"combining estates with different callstrings at label:"<<es1->label().toString()<<endl;
        SAWYER_MESG(logger[WARN])<<"cs1: "<<es1->getCallString().toString()<<endl;
        SAWYER_MESG(logger[WARN])<<"cs2: "<<es2->getCallString().toString()<<endl;
      }
    }
    InputOutput io;
    if(es1->io.isBot()) {
      es1->copy(es1,es2,false);
      return;
    } else if(es2->io.isBot()) {
      // no update of es1 necessary
      return;
    } else {
      ROSE_ASSERT(es1->io==es2->io);
    }
    PState::combineInPlace1st(es1->pstate(),es2->pstate());
  }

  EState EStateTransferFunctions::combine(EStatePtr es1, EStatePtr es2) {
    ROSE_ASSERT(es1->label()==es2->label());
    //ROSE_ASSERT(es1->constraints()==es2->constraints()); // pointer equality
    if(es1->getCallString()!=es2->getCallString()) {
      if(_analyzer->getOptionOutputWarnings()) {
        SAWYER_MESG(logger[WARN])<<"combining estates with different callstrings at label:"<<es1->label().toString()<<endl;
        SAWYER_MESG(logger[WARN])<<"cs1: "<<es1->getCallString().toString()<<endl;
        SAWYER_MESG(logger[WARN])<<"cs2: "<<es2->getCallString().toString()<<endl;
      }
    }
    InputOutput io;
    if(es1->io.isBot()) {
      io=es2->io;
    } else if(es2->io.isBot()) {
      io=es1->io;
    } else {
      ROSE_ASSERT(es1->io==es2->io);
      io=es1->io;
    }

    return createEState(es1->label(),es1->getCallString(),PState::combine(es1->pstate(),es2->pstate()),io);
  }

  std::list<EStatePtr> EStateTransferFunctions::elistify() {
    std::list<EStatePtr> resList;
    return resList;
  }

  std::list<EStatePtr> EStateTransferFunctions::elistify(EState estate) {
    std::list<EStatePtr> resList;
    EStatePtr estatePtr=new EState(estate);
    resList.push_back(estatePtr);
    return resList;
  }

  std::list<EStatePtr> EStateTransferFunctions::elistify(EStatePtr estate) {
    std::list<EStatePtr> resList;
    resList.push_back(estate);
    return resList;
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionCallLocalEdgeRersBinaryMode(Edge edge, EStatePtr estate) {
    SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
    if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
      ROSE_ASSERT(funCall);
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName==_rersHybridOutputFunctionName) {
        // RERS global vars binary handling
        PStatePtr pstate=estate->pstate();
        RERS_Problem::rersGlobalVarsCallInitFP(getAnalyzer(),*pstate, omp_get_thread_num());
#if 0
        //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
        int rers_result=RERS_Problem::calculate_outputFP(argument);
        (void) RERS_Problem::calculate_outputFP(argument);
#else
        (void) RERS_Problem::calculate_outputFP( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
        //logger[DEBUG]<< "Called calculate_outputFP("<<argument<<")"<<" :: result="<<rers_result<<endl;
        Label lab=estate->label();
        if(rers_result<=-100) {
          // we found a failing assert
          // = rers_result*(-1)-100 : rers error-number
          // = -160 : rers globalError (2012 only)
          int index=((rers_result+100)*(-1));
          ROSE_ASSERT(index>=0 && index <=99);
          getAnalyzer()->binaryBindingAssert[index]=true;
          //reachabilityResults.reachable(index); //previous location in code
          //logger[DEBUG]<<"found assert Error "<<index<<endl;
          InputOutput _io;
          _io.recordFailedAssert();
          // error label encoded in the output value, storing it in the new failing assertion EState
          PStatePtr newPstate  = pstate;
          writeToMemoryLocation(lab,newPstate,
                                AbstractValue::createAddressOfVariable(globalVarIdByName("output")),
                                CodeThorn::AbstractValue(rers_result));
          EStatePtr _eState=reInitEState(estate,edge.target(),estate->getCallString(),newPstate,_io);
          ROSE_ASSERT(_eState->io.isFailedAssertIO());
          return elistify(_eState);
        }
        RERS_Problem::rersGlobalVarsCallReturnInitFP(getAnalyzer(),*pstate, omp_get_thread_num());
        InputOutput newio;
        if (rers_result == -2) {
          newio.recordVariable(InputOutput::STDERR_VAR,globalVarIdByName("input"));
        }
        // TODO: pstate[VariableId(output)]=rers_result;
        // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
        if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
          SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
          VariableId lhsVarId;
          bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVarId);
          ROSE_ASSERT(isLhsVar); // must hold
          writeToMemoryLocation(lab,pstate,AbstractValue::createAddressOfVariable(lhsVarId),AbstractValue(rers_result));
          EStatePtr _eState=reInitEState(estate,edge.target(),estate->getCallString(),pstate,newio);
          return elistify(_eState);
        } else {
          EStatePtr _eState=reInitEState(estate,edge.target(),estate->getCallString(),pstate,newio);
          return elistify(_eState);
        }
        SAWYER_MESG(logger[ERROR]) <<"PState:"<< pstate<<endl;
        SAWYER_MESG(logger[ERROR]) <<"RERS-MODE: call of unknown function."<<endl;
        exit(1);
        // pstate now contains the current state obtained from the binary
      }
    }
    return elistify(); // did not match RERS function
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionCallLocalEdge(Edge edge, EStatePtr estate) {
    if(_analyzer->getOptionsRef().rers.rersBinary) {
      return transferFunctionCallLocalEdgeRersBinaryMode(edge, estate);
    }
    // default behavior, empty list of states, no information propagated on local edge
    return getAnalyzer()->elistify();
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionCall(Edge edge, EStatePtr estate) {
    //cout<<"DEBUG: transferFunctionCall:edge:"<<edge.toString()<<" estate label:"<<estate->label().toString()<<endl;
    // 1) obtain actual parameters from source
    // 2) obtain formal parameters from target
    // 3) eval each actual parameter and assign result to formal parameter in state
    // 4) create new estate and update callstring (context sensitive analysis)
    SAWYER_MESG(logger[TRACE])<<"transferFunctionCall: "<<getLabeler()->getNode(edge.source())->unparseToString()<<endl;
    Label currentLabel=estate->label();
    EStatePtr currentEState=estate;
    PStatePtr currentPState=currentEState->pstate();

    // ad 1)
    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source()));
    ROSE_ASSERT(funCall);
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
    //cout<<"DEBUG: funexp: "<<funCall->get_function()->unparseToString()<< " : AST:"<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
    if(SgExpression* funExp=funCall->get_function()) {
      if(SgPointerDerefExp* ptrDerefExp=isSgPointerDerefExp(funExp)) {
        // ensure funExp is the actual function pointer (makes (*fp)(args) equivalent to: fp(args))
        funExp=ptrDerefExp->get_operand();
      }
      if(SgVarRefExp* varRefExp=isSgVarRefExp(funExp)) {
        // determine if the edge leads to the called function
        Label targetLabel=edge.target();
        VariableId varId=getVariableIdMapping()->variableId(varRefExp);
        AbstractValue varAddress=AbstractValue::createAddressOfVariable(varId);
        AbstractValue funcPtrVal=readFromMemoryLocation(currentLabel,estate->pstate(),varAddress);
        //cout<<"DEBUG: function pointer value: "<<funcPtrVal.toString(getVariableIdMapping())<<": isFunPtr:"<<funcPtrVal.isFunctionPtr()<<endl;
        if(funcPtrVal.isFunctionPtr()&&!funcPtrVal.isTop()&&!funcPtrVal.isPointerToArbitraryMemory()&&!funcPtrVal.isBot()) {
          Label  funTargetLabel=funcPtrVal.getLabel();
          if(funTargetLabel!=targetLabel) {
            //infeasable path
            //cout<<"DEBUG: infeasable path: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
            return elistify();
          } else {
            // continue on this path, function pointer is referring to the target label's function entry
            //cout<<"DEBUG: Resolved function pointer"<<endl;
          }
        } else {
          // must call listener for potential null pointer dereference
          notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),funcPtrVal);
          // continue but pass the information now to *all* outgoing static edges (maximum imprecision)
          //cerr<<"DEBUG: function pointer is top or bot. Continue: "<<funCall->unparseToString()<<":"<<funcPtrVal.toString(getVariableIdMapping())<<endl;
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
    PStatePtr newPState=currentPState;
    SgInitializedNamePtrList::iterator i=formalParameters.begin();
    SgExpressionPtrList::iterator j=actualParameters.begin();
    while(i!=formalParameters.end() && j!=actualParameters.end()) {
      SgInitializedName* formalParameterName=*i;
      ROSE_ASSERT(formalParameterName);
      // test formal parameter (instead of argument type) to allow for expressions in arguments
      VariableId formalParameterVarId=_analyzer->getVariableIdMapping()->variableId(formalParameterName);

      if(_analyzer->getVariableIdMapping()->isOfClassType(formalParameterVarId)) {
        SgExpression* actualParameterExpr=*j;
        VariableId actualParameterVarId;
        // general case: the actual argument is an arbitrary expression (including a single variable)
        SingleEvalResult evalResult=evaluateLExpression(actualParameterExpr,currentEState);

        AbstractValue actualParamAddress=evalResult.value();
        AbstractValue formalParamAddress=AbstractValue::createAddressOfVariable(formalParameterVarId);
        SAWYER_MESG(logger[TRACE])<<SgNodeHelper::sourceLineColumnToString(funCall)<< ": passing of class/Struct/Union types per value as function parameter: ";
        SgType* ctype=_analyzer->getVariableIdMapping()->getType(formalParameterVarId);
        auto membersList=_analyzer->getVariableIdMapping()->getClassMembers(ctype);
        SAWYER_MESG(logger[TRACE])<<" #classmembers: "<<membersList.size()<<endl;
        for(auto mvarId : membersList) {
          CodeThorn::TypeSize offset=_analyzer->getVariableIdMapping()->getOffset(mvarId);
          SAWYER_MESG(logger[TRACE])<<" formal param    : "<<formalParamAddress.toString(_analyzer->getVariableIdMapping())<<endl;
          SAWYER_MESG(logger[TRACE])<<" actual param (L): "<<actualParamAddress.toString(_analyzer->getVariableIdMapping())<<endl;
          SAWYER_MESG(logger[TRACE])<<" actual param-vid: "<<mvarId.toString()<<endl;
          SAWYER_MESG(logger[TRACE])<<" actual param-vid: "<<mvarId.toString(_analyzer->getVariableIdMapping())<<endl;
          SAWYER_MESG(logger[TRACE])<<" offset          : "<<offset<<endl;
          AbstractValue offsetAV(offset);
          AbstractValue formalParameterStructMemberAddress=AbstractValue::operatorAdd(formalParamAddress,offsetAV);
          AbstractValue actualParameterStructMemberAddress=AbstractValue::operatorAdd(actualParamAddress,offsetAV);
          // read from evalResultValue+offset and write to formal-param-address+offset
          AbstractValue newVal=readFromAnyMemoryLocation(currentLabel,newPState,actualParameterStructMemberAddress);
          initializeMemoryLocation(currentLabel,newPState,formalParameterStructMemberAddress,newVal);
        }
      } else {
        // VariableName varNameString=name->get_name();
        SgExpression* actualParameterExpr=*j;
        ROSE_ASSERT(actualParameterExpr);
        VariableId actualParameterVarId;

        // general case: the actual argument is an arbitrary expression (including a single variable)
        SingleEvalResult evalResult=evaluateExpression(actualParameterExpr,currentEState);
        AbstractValue evalResultValue=evalResult.value();
        initializeMemoryLocation(currentLabel,newPState,AbstractValue::createAddressOfVariable(formalParameterVarId),evalResultValue);
      }
      ++i;++j;
    }
    // assert must hold if #formal-params==#actual-params (TODO: default values)
    if(!(i==formalParameters.end() && j==actualParameters.end())) {
      logger[WARN]<<"Number of function call arguments not matching formal function parameters:"<<endl;
      logger[WARN]<<"fcall   : "<<SgNodeHelper::sourceLineColumnToString(funCall)<<endl;
      logger[WARN]<<"fdef    : "<<SgNodeHelper::sourceLineColumnToString(funDef)<<endl;
    }

    // ad 4 call context (call string)
    CallString cs=currentEState->getCallString();
    if(_analyzer->getOptionContextSensitiveAnalysis()) {
       SAWYER_MESG(logger[TRACE])<<"transfer functioncall:"<<currentEState->label().toString()<<":"<<cs.toString()<<":"<<SgNodeHelper::sourceLocationAndNodeToString(funCall)<<endl;
       if(transferFunctionCallContextInPlace(cs, currentEState->label())) {
         EStatePtr newEState=reInitEState(estate,edge.target(),cs,newPState);
         return elistify(newEState);
       } else {
         return elistify(); // do not continue
       }
    }
    // default for non-context sensitive analysis
    EStatePtr newEState=reInitEState(estate,edge.target(),cs,newPState);
    return elistify(newEState);
  }

  std::list<EStatePtr> EStateTransferFunctions::transferReturnStmt(Edge edge, EStatePtr estate) {
    Label lab=estate->label();
    EStatePtr currentEState=estate;
    CallString cs=currentEState->getCallString();
    PStatePtr currentPState=currentEState->pstate();
    ROSE_ASSERT(_analyzer);
    ROSE_ASSERT(_analyzer->getCFAnalyzer());
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);
    SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);

    if(isSgNullExpression(expr)) {
      // null expr is a no-op
      return elistify(reInitEState(currentEState,edge.target(),cs,currentPState));
    } else {
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=_analyzer->getVariableIdMapping()->getReturnVariableId();
      }
      SingleEvalResult rhsRes=evaluateExpression(expr,currentEState, CodeThorn::EStateTransferFunctions::MODE_VALUE);
      AbstractValue rhsResultValue=rhsRes.value();
      PStatePtr newPState=currentPState;
      initializeMemoryLocation(lab,newPState,returnVarId,rhsResultValue);
      return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
    }
  }

  std::pair<bool,CallString> EStateTransferFunctions::determinePathFeasibilityAndContext(CallString cs, Label functionCallLabel) {
    if(_analyzer->getSolver()->getId()==18) {
      Solver18* solver18=dynamic_cast<Solver18*>(_analyzer->getSolver());
      ROSE_ASSERT(solver18);
      size_t k=CallString::getMaxLength();
      if(cs.getLength()==0) {
        // always return every context
        if(solver18->callStringExistsAtLabel(cs,functionCallLabel)) {
          return make_pair(true,cs);
        } else {
          return make_pair(false,CallString());
        }
      }
      if(cs.getLength()<k) {
        // callstring is exact
        if(cs.isLastLabel(functionCallLabel)) {
          // exact
          CallString shortenedCs=cs.withoutLastLabel();
          if(solver18->callStringExistsAtLabel(shortenedCs,functionCallLabel)) {
            return make_pair(true,shortenedCs);
          }
        }
        // infeasible
        return make_pair(false,CallString());
      } else {
        // callstring is possibly approximate (=k can represent exact length k OR truncated)
        ROSE_ASSERT(cs.getLength()==k);
        /* the callstring can be shortened if the last label is the
         * callstring label AND the shortened cs has been computed
         * before at the callsite
         */
        if(cs.isLastLabel(functionCallLabel)) {
          CallString shortenedCs=cs.withoutLastLabel();
          if(solver18->callStringExistsAtLabel(shortenedCs,functionCallLabel)) {
            // becomes exact again
            return make_pair(true,shortenedCs);
          }
          // intentional fall through to the following two cases
        }
        if(solver18->callStringExistsAtLabel(cs,functionCallLabel)) {
          // remains aproximate
          return make_pair(true,cs);
        } else {
          // infeasible
          return make_pair(false,CallString());
        }
      }
    } else {
      // old default behavior for older solvers, to be updated to above
      if(isFeasiblePathContextOld(cs,functionCallLabel)) {
        transferFunctionCallReturnContextInPlaceOld(cs,functionCallLabel);
        SAWYER_MESG(logger[TRACE])<<"new context: cs: "<<cs.toString()<<endl;
        return make_pair(true,cs);
      } else {
        return make_pair(false,CallString());
      }
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionCallReturn(Edge edge, EStatePtr estate) {
    EStatePtr currentEState=estate;

    // determine functionCallLabel corresponding to functioncallReturnLabel.
#if 0
    Label functionCallReturnLabel=edge.source();
    SgNode* node=getLabeler()->getNode(functionCallReturnLabel);
    Label functionCallLabel=getLabeler()->functionCallLabel(node);
#else
    Label functionCallReturnLabel=estate->label();
    Label functionCallLabel=getLabeler()->getFunctionCallLabelFromReturnLabel(functionCallReturnLabel);
#endif
    SAWYER_MESG(logger[TRACE])<<"FunctionCallReturnTransfer: call: L"<<functionCallLabel.toString()<<": callret: L"<<functionCallReturnLabel.toString()<<" cs: "<<estate->getCallString().toString()<<endl;
    CallString cs=currentEState->getCallString();
    if(_analyzer->getOptionContextSensitiveAnalysis()) {
      if(getLabeler()->isExternalFunctionCallLabel(functionCallLabel)) {
        // nothing to do for external function call (label is not added
        // to callstring by external function call)
        SAWYER_MESG(logger[TRACE])<<": external edge"<<endl;
      } else {
        std::pair<bool,CallString> resultPair=determinePathFeasibilityAndContext(cs,functionCallLabel);
        SAWYER_MESG(logger[TRACE])<<"transfer functioncallreturn: call: L"<<functionCallLabel.toString()<<": callret: L"<<functionCallReturnLabel.toString()<<" cs: "<<estate->getCallString().toString()<<" : pathfeasibility:"<<resultPair.first<<" : ";
        if(resultPair.first) SAWYER_MESG(logger[TRACE])<<resultPair.second.toString()<<endl;
        else SAWYER_MESG(logger[TRACE])<<"infeasible"<<endl;

        if(resultPair.first) {
          // set new, possibly shorter, callstring
          cs=resultPair.second;
        } else {
          // definitely not feasible path, do not return a state
          std::list<EStatePtr> emptyList;
          return emptyList;
        }
      }
    }

    // 1. handle the edge as outgoing edge
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);

    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
      // case 1: return f(); pass estate through
      EStatePtr newEState=currentEState;
      newEState->setLabel(edge.target());
      return elistify(newEState);
    } else if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
      // case 2a: x=f(); bind variable x to value of returnVariableId
      if(_analyzer->getOptionsRef().rers.rersBinary) {
        if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
          string funName=SgNodeHelper::getFunctionName(funCall);
          if(funName==_rersHybridOutputFunctionName) {
            EStatePtr newEState=currentEState;
            newEState->setLabel(edge.target());
            newEState->setCallString(cs);
            return elistify(newEState);
          }
        }
      }
      SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
      VariableId lhsVarId;
      bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVarId);
      ROSE_ASSERT(isLhsVar); // must hold
      PStatePtr newPState=currentEState->pstate();

      // determine the temporary return variable's varid to remove it from state after the value has been copied
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=_analyzer->getVariableIdMapping()->getReturnVariableId();
      }

      if(newPState->varExists(returnVarId)) {
        AbstractValue evalResult=readFromMemoryLocation(currentEState->label(),newPState,returnVarId);
        initializeMemoryLocation(currentEState->label(),newPState,lhsVarId,evalResult);
        newPState->deleteVar(returnVarId); // remove temporary return variable from state
        return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
      } else {
        // no return-variable found in state. This can be the case for an extern function.
        // alternatively a return-variable could be added in the external function call to
        // make this handling here uniform
        return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
      }
    } else if(SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(nextNodeToAnalyze1)) {
      // case 2b: Type x=f(); bind variable x to value of the return-variable for function call in declaration
      if(_analyzer->getOptionsRef().rers.rersBinary) {
        if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
          string funName=SgNodeHelper::getFunctionName(funCall);
          if(funName==_rersHybridOutputFunctionName) {
            EStatePtr newEState=currentEState;
            newEState->setLabel(edge.target());
            newEState->setCallString(cs);
            return elistify(newEState);
          }
        }
      }

      // these two lines are different to x=f();
      SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(nextNodeToAnalyze1);
      VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
      ROSE_ASSERT(lhsVarId.isValid());

      PStatePtr newPState=currentEState->pstate();

      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=_analyzer->getVariableIdMapping()->getReturnVariableId();
      }

      if(newPState->varExists(returnVarId)) {
        AbstractValue evalResult=readFromMemoryLocation(currentEState->label(),newPState,returnVarId);
        initializeMemoryLocation(currentEState->label(),newPState,lhsVarId,evalResult);
        newPState->deleteVar(returnVarId); // remove return-variable from state
        SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn(initialization): LHSVAR:"<<getVariableIdMapping()->variableName(lhsVarId)<<" value: "<<evalResult.toString()<<endl;
        return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
      } else {
        // no return-variable found in state. This can be the case for an extern function.
        // alternatively a return-variable could be added in the external function call to
        // make this handling here uniform
        SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
        SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn: Variable declaration with function call: no function-return variable found! @ "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze1)<<":"<<nextNodeToAnalyze1->unparseToString()<<endl;
        SAWYER_MESG(logger[TRACE])<<estate->toString(getVariableIdMapping())<<endl;
        SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
        return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
      }
    } else  if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
      // case 3: f(); remove return-variable from state (discard value)
      PStatePtr newPState=currentEState->pstate();
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=_analyzer->getVariableIdMapping()->getReturnVariableId();
      }
      // no effect if return-variable does not exist
      newPState->deleteVar(returnVarId);
      return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
    } else if (SgNodeHelper::matchExtendedNormalizedCall(nextNodeToAnalyze1)) {
      // Handles Constructor Calls
      // \pp TEMPORARY SOLUTION TO MAKE CODE PASS THROUGH
      // \todo THIS IS WRONG
      // (1) matchExtendedNormalizedCall is too coarse grain, as it covers
      //     both constructor and function calls
      // (2) distinction between constructor call of a base class and an
      //     variable initialization needs to be made.
      EStatePtr newEState=currentEState;
      newEState->setLabel(edge.target());
      return elistify(newEState);
    } else {
      logger[FATAL] << "function call-return from unsupported call type:"<<nextNodeToAnalyze1->unparseToString()<<endl;
      exit(1);
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferAsmStmt(Edge edge, EStatePtr estate) {
    // ignore AsmStmt
    return transferIdentity(edge,estate);
  }

  void EStateTransferFunctions::updateMaxStateSize(size_t newVal) {
    _maxStateSize=std::max(_maxStateSize,newVal);
  }

  size_t EStateTransferFunctions::getMaxStateSize() {
    return _maxStateSize;
  }

  void EStateTransferFunctions::updateMaxCSLength(size_t newVal) {
    _maxCSLength=std::max(_maxCSLength,newVal);
  }

  size_t EStateTransferFunctions::getMaxCSLength() {
    return _maxCSLength;
  }

  bool EStateTransferFunctions::isRegisterAddress(AbstractValue memLoc) {
    ROSE_ASSERT(_analyzer);
    return _analyzer->isRegisterAddress(memLoc);
  }

  void EStateTransferFunctions::transferFunctionEntryPrintStatus(Edge /*edge*/, EStatePtr estate, std::string fileName, std::string functionName) {
    if(_analyzer->getOptionsRef().status) {
      if(_analyzer->getOptionsRef().precisionLevel==1) {
        size_t numFunctions=_analyzer->getTotalNumberOfFunctions();
        _analyzer->printStatusMessage("Analyzing function #"+std::to_string(_functionsAnalyzedNum)+" of "+std::to_string(numFunctions)+": ");
      } else if(_analyzer->getOptionsRef().precisionLevel>=2) {
        _analyzer->printStatusMessage(std::to_string(_functionsAnalyzedNum)+". ");
        auto stateSize=estate->pstate()->stateSize();
        _analyzer->printStatusMessage("Analyzing function (cslen="+std::to_string(estate->getCallStringLength())+") (state size:"+to_string(stateSize)+" max:"+std::to_string(_maxStateSize)+"): ");
      }
      if(_analyzer->getOptionsRef().precisionLevel>0)
        _analyzer->printStatusMessageLine(": "+fileName+" : "+functionName);
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionEntry(Edge edge, EStatePtr estate) {
    Label lab=estate->label();
    SgNode* node=_analyzer->getLabeler()->getNode(lab);
    SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
    if(funDef) {
      string functionName=SgNodeHelper::getFunctionName(node);
      string fileName=SgNodeHelper::sourceFilenameToString(node);

      // update counters and stats used in status messages
      _functionsAnalyzedNum++;
      auto stateSize=estate->pstate()->stateSize();
      updateMaxStateSize(stateSize);
      updateMaxCSLength(estate->getCallStringLength());
      transferFunctionEntryPrintStatus(edge,estate,functionName,fileName);

      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      SAWYER_MESG(logger[TRACE])<<"Function:"<<functionName<<" Parameters: "<<formalParameters.size()<<endl;
      for(auto fParam : formalParameters) {
        SAWYER_MESG(logger[TRACE])<<SAWYER_MESG(logger[TRACE])<<"Function: "<<fParam->unparseToString()<<" sym:"<<fParam->search_for_symbol_from_symbol_table()<<endl;
      }
      SAWYER_MESG(logger[TRACE])<<endl;
      SAWYER_MESG(logger[TRACE])<<"Function:"<<functionName<<" Parameters: end"<<endl;
    }
    auto resultList=transferIdentity(edge,estate);
    return resultList;
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionExit(Edge edge, EStatePtr estate) {
    EStatePtr currentEState=estate;
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source()))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      // 3) create new EState and return

      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      PStatePtr newPState=currentEState->pstate();
      VariableIdMapping::VariableIdSet localVars=_analyzer->getVariableIdMapping()->determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=_analyzer->getVariableIdMapping()->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      set<string> names=_analyzer->variableIdsToVariableNames(vars);

      std::list<PState::iterator> toBeDeleted;
      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
        VariableId varId=*i;
        // delete all entries with varid as part of address (e.g. struct members, etc.)
        for(auto psIter=newPState->begin();psIter!=newPState->end();++psIter) {
          if((*psIter).first==varId)
            toBeDeleted.push_back(psIter);
          //newPState->deleteVar(varId);
        }
      }
      for(auto mapElIter : toBeDeleted) {
        newPState->erase(mapElIter);
      }
      // ad 3)
      return elistify(reInitEState(currentEState,edge.target(),estate->getCallString(),newPState));
    } else {
      logger[FATAL] << "no function definition associated with function exit label."<<endl;
      exit(1);
    }
  }

  // called from transferForkFunction
  std::list<EStatePtr> EStateTransferFunctions::transferForkFunctionWithExternalTargetFunction(Edge edge, EStatePtr estate, SgFunctionCallExp*) {
    //_analyzer->recordExternalFunctionCall(funCall); funcall would be forkFunction
    // arg5 is expression with functino pointer to external function
    list<EStatePtr> estateList;
    estate->setLabel(edge.target());
    // no forked state
    estateList.push_back(estate);
    return estateList;
  }

  std::list<EStatePtr> EStateTransferFunctions::transferForkFunction(Edge edge, EStatePtr estate, SgFunctionCallExp* funCall) {
    //cout<<"DEBUG: transfer fork function:"<<funCall->unparseToString()<<endl;
    EStatePtr currentEState=estate;
    CallString cs=currentEState->getCallString();

    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    SAWYER_MESG(logger[TRACE])<<getAnalyzer()->getOptionsRef().forkFunctionName<<" #args:"<<actualParameters.size()<<endl;
    // get 5th argument
    SgExpressionPtrList::iterator pIter=actualParameters.begin();
    for(int j=1;j<5;j++) {
      ++pIter;
    }
    SgExpression* actualParameterExpr=*pIter;
    //cout<<"DEBUG: transfer fork function: ESTATE1:"<<currentEState->toString()<<endl;
    // general case: the argument is an arbitrary expression (including a single variable)
    SingleEvalResult evalResult=evaluateExpression(actualParameterExpr,currentEState);
    //cout<<"DEBUG: transfer fork function: ESTATE2:"<<currentEState->toString()<<endl;
    AbstractValue arg5Value=evalResult.value();
    //cout<<"DEBUG: transfer fork function: arg5: "<<arg5Value.toString()<<endl;
    // this result value has to be a function pointer value, create a state (representing the fork), and continue with current state
    if(!arg5Value.isFunctionPtr()) {
      // case where no source exists for function pointer
      // handle like any other external function call
      return transferForkFunctionWithExternalTargetFunction(edge,estate,funCall);
    }
    // create state with this function label as start state
    EStatePtr forkedEState=estate->cloneWithoutIO();
    // set target label in new state to function pointer label
    forkedEState->setLabel(arg5Value.getLabel());

    // allow both formats x=f(...) and f(...)
    SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
    if(assignOp) {
      list<EStatePtr> estateList1=transferAssignOp(assignOp,edge,estate); // use current estate, do not mix with forked state
      ROSE_ASSERT(estateList1.size()==1);
      EStatePtr estate=*estateList1.begin();
      estate->setLabel(edge.target());
      list<EStatePtr> estateList2;
      estateList2.push_back(estate);
      estateList2.push_back(forkedEState);
      return estateList2;
    } else {
      list<EStatePtr> estateList;
      estate->setLabel(edge.target());
      estateList.push_back(estate);
      estateList.push_back(forkedEState);
      return estateList;
    }
  }

  void EStateTransferFunctions::modelOptionalExternalSoundness(PStatePtr pstate) {
    if(_analyzer->getOptionsRef().externalSoundness) {
      pstate->writeTopToAllPotentialMemoryLocations();
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferFunctionCallExternal(Edge edge, EStatePtr estate) {
    EStatePtr currentEState=estate;
    CallString cs=currentEState->getCallString();

    // handle special case for readwrite listener
#pragma omp critical(VIOLATIONRECORDING)
    {
      if(numberOfReadWriteListeners()>0) {
        for(auto p : _readWriteListenerMap) {
          ReadWriteListener* readWriteListener=p.second;
          readWriteListener->functionCallExternal(edge,currentEState);
        }
      }
    }

    // handle the edge as outgoing edge
    SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze1);

    SAWYER_MESG(logger[TRACE]) << "transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

    InputOutput newio;
    Label lab=_analyzer->getLabeler()->getLabel(nextNodeToAnalyze1);

    VariableId varId;
    bool isFunctionCallWithAssignmentFlag=isFunctionCallWithAssignment(lab,&varId);

    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1);
    _analyzer->recordExternalFunctionCall(funCall);
    //evaluateFunctionCallArguments(edge,funCall,currentEState,false);

    CTIOLabeler* ctioLabeler=dynamic_cast<CTIOLabeler*>(_analyzer->getLabeler());
    ROSE_ASSERT(ctioLabeler);

    if(ctioLabeler->isStdInLabel(lab,&varId)) {
      if(_analyzer->_inputSequence.size()>0) {
        PStatePtr newPState=currentEState->pstate();
        list<EStatePtr> resList;
        int newValue;
        if(_analyzer->_inputSequenceIterator!=_analyzer->_inputSequence.end()) {
          newValue=*_analyzer->_inputSequenceIterator;
          ++_analyzer->_inputSequenceIterator;
        } else {
          return resList; // return no state (this terminates the input sequence)
        }
        writeToMemoryLocation(currentEState->label(),newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(newValue));
        newio.recordVariable(InputOutput::STDIN_VAR,varId);
        // a value of the input sequence can be added in-place to the state (in contrast to the input set below)
        EStatePtr newEState=reInitEState(currentEState,edge.target(),cs,newPState,newio);
        resList.push_back(newEState);
        return resList;
      } else {
        if(_analyzer->_inputVarValues.size()>0) {
          //PStatePtr newPState=currentEState->pstate();
          list<EStatePtr> resList;
          int numState=1;
          for(set<int>::iterator i=_analyzer->_inputVarValues.begin();i!=_analyzer->_inputVarValues.end();++i) {
            if(true || numState!=1) {
              // clone state if creating more than one value (state splitting)
              // currentEState has been stored in resList in previous iteration, can be re-assigned without creating a leak
              currentEState=currentEState->cloneWithoutIO(); // reuse state and write next io value from set
            }
            PStatePtr newPState=currentEState->pstate();
            writeToMemoryLocation(currentEState->label(),newPState,AbstractValue::createAddressOfVariable(varId),AbstractValue(*i));
            newio.recordVariable(InputOutput::STDIN_VAR,varId);
            EStatePtr newEState=reInitEState(currentEState,edge.target(),estate->getCallString(),newPState,newio);
            resList.push_back(newEState);
            numState++;
          }
          return resList;
        } else {
          // without specified input values (default mode: analysis performed for all possible input values)
          // update state (remove all existing constraint on that variable and set it to top)
          PStatePtr newPState=currentEState->pstate();
          // update input var
          newPState->writeTopToMemoryLocation(varId);
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          modelOptionalExternalSoundness(newPState);
          // external call context
          // call string is reused from input-estate. An external function call does not change the call string
          // callReturn node must check for being an external call
          return elistify(reInitEState(currentEState,edge.target(),cs,newPState,newio));
        }
      }
    }

    if(_analyzer->getInterpreterMode()!=IM_ENABLED) {
      int constvalue=0;
      if(ctioLabeler->isStdOutVarLabel(lab,&varId)) {
        newio.recordVariable(InputOutput::STDOUT_VAR,varId);
        ROSE_ASSERT(newio.var==varId);
        currentEState=reInitEState(currentEState,edge.target(),cs,currentEState->pstate(), newio);
        ROSE_ASSERT(currentEState->io.isStdOutIO());
        return elistify(currentEState);
      } else if(ctioLabeler->isStdOutConstLabel(lab,&constvalue)) {
        newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
        currentEState=reInitEState(currentEState,edge.target(),cs,currentEState->pstate(), newio);
        ROSE_ASSERT(currentEState->io.isStdOutIO());
        return elistify(currentEState);
      } else if(ctioLabeler->isStdErrLabel(lab,&varId)) {
        newio.recordVariable(InputOutput::STDERR_VAR,varId);
        ROSE_ASSERT(newio.var==varId);
        return elistify(reInitEState(currentEState,edge.target(),cs,currentEState->pstate(), newio));
      }
    }

    /* handling of specific semantics for external function */
    if(funCall) {
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(getAnalyzer()->getOptionsRef().forkFunctionEnabled) {
        if(funName==getAnalyzer()->getOptionsRef().forkFunctionName) {
          return transferForkFunction(edge,currentEState,funCall); // evaluates arguments by itself
        }
      }

      //evaluateFunctionCallArguments(edge,funCall,currentEState,false); // evaluate for side-effects. transferForkFunction
      if(isFunctionCallWithAssignmentFlag) {
        // here only the specific format x=f(...) can exist
        SgAssignOp* assignOp=isSgAssignOp(AstUtility::findExprNodeInAstUpwards(V_SgAssignOp,funCall));
        ROSE_ASSERT(assignOp);
        modelOptionalExternalSoundness(estate->pstate());
        return evalAssignOp3(assignOp,edge.target(),estate);
      } else {
        // all other cases, evaluate function call as expression
        SingleEvalResult evalResult2=evaluateExpression(funCall,currentEState); // eventually calls evalFunctionCall
        SAWYER_MESG(logger[TRACE])<<"EXTERNAL FUNCTION: "<<SgNodeHelper::getFunctionName(funCall)<<" result(added to state):"<<evalResult2.result.toString()<<endl;

        // create new estate with added return variable (for inter-procedural analysis)
        CallString cs=evalResult2.estate->getCallString();
        PStatePtr newPState=evalResult2.estate->pstate(); // same pstate object as currentEState, but possibly modified
        VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
        {
          returnVarId=_analyzer->getVariableIdMapping()->getReturnVariableId();
        }
        // added function call result value to state. The returnVarId does not correspond to a declaration, and therefore it is
        // treated as being initialized (and declared).
        initializeMemoryLocation(currentEState->label(),newPState,returnVarId,evalResult2.result);
        modelOptionalExternalSoundness(newPState);
        return elistify(reInitEState(currentEState,edge.target(),cs,newPState,evalResult2.estate->io)); // pstate from evalResult2
      }
    }
    // for all other external functions use identity as transfer function
    EStatePtr newEState=currentEState;
    modelOptionalExternalSoundness(newEState->pstate());
    newEState->io=newio;
    newEState->setLabel(edge.target());
    return elistify(newEState);
  }

  std::list<EStatePtr> EStateTransferFunctions::transferDefaultOptionStmt(SgDefaultOptionStmt* defaultStmt,Edge edge, EStatePtr estate) {
    SAWYER_MESG(logger[TRACE])<<"SWITCH CASE DEFAULT: "<<defaultStmt->unparseToString()<<endl;

    Label targetLabel=edge.target();
    CallString cs=estate->getCallString();
    PStatePtr newPState=estate->pstate();
    SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

    EStatePtr currentEState=estate;

    // value of switch expression
    AbstractValue switchCondVal=evaluateExpressionAV(condExpr,currentEState);

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
        AbstractValue caseValRangeBegin=evaluateExpressionAV(caseExpr,currentEState);
        AbstractValue caseValRangeEnd=evaluateExpressionAV(caseExprOptionalRangeEnd,currentEState);
        AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
        AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
        if(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue()) {
          SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
          defaultReachable=false;
          break;
        }
      }
      // value of constant case value
      AbstractValue caseVal=evaluateExpressionAV(caseExpr,currentEState);
      // compare case constant with switch expression value
      SAWYER_MESG(logger[TRACE])<<"switch-default filter cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
      // check that not any case label may be equal to the switch-expr value (exact for concrete values)
      AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
      // top is *not* considered here, because only a definitive case makes default non-reachable
      if(comparisonVal.isTrue()) {
        // determined that at least one case may be reachable
        SAWYER_MESG(logger[TRACE])<<"switch-default: continuing."<<endl;
        //return elistify(createEState(targetLabel,cs,newPState));
        SAWYER_MESG(logger[TRACE])<<"switch-default: non-reachable=false."<<endl;
        defaultReachable=false;
        break;
      }
    }
    if(defaultReachable) {
      SAWYER_MESG(logger[TRACE])<<"switch-default: reachable."<<endl;
      return elistify(reInitEState(currentEState,targetLabel,cs,newPState));
    } else {
      // detected infeasable path (default is not reachable)
      SAWYER_MESG(logger[TRACE])<<"switch-default: infeasable path."<<endl;
      list<EStatePtr> emptyList;
      return emptyList;
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, EStatePtr estate) {
    SAWYER_MESG(logger[TRACE])<<"CASESTMT: "<<caseStmt->unparseToString()<<endl;
    Label targetLabel=edge.target();
    CallString cs=estate->getCallString();
    PStatePtr newPState=estate->pstate();
    SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

    EStatePtr currentEState=estate;

    // value of switch expression
    AbstractValue switchCondVal=evaluateExpressionAV(condExpr,currentEState);

    SgExpression* caseExpr=caseStmt->get_key();
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd) {
      AbstractValue caseValRangeBegin=evaluateExpressionAV(caseExpr,currentEState);
      AbstractValue caseValRangeEnd=evaluateExpressionAV(caseExprOptionalRangeEnd,currentEState);
      AbstractValue comparisonValBegin=caseValRangeBegin.operatorLessOrEq(switchCondVal);
      AbstractValue comparisonValEnd=caseValRangeEnd.operatorMoreOrEq(switchCondVal);
      if(comparisonValBegin.isTop()||comparisonValEnd.isTop()||(comparisonValBegin.isTrue()&&comparisonValEnd.isTrue())) {
        SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: continuing."<<endl;
        return elistify(reInitEState(currentEState,targetLabel,cs,newPState));
      } else {
        SAWYER_MESG(logger[TRACE])<<"switch-case GNU Range: infeasable path."<<endl;
        // infeasable path
        list<EStatePtr> emptyList;
        return emptyList;
      }
    }
    // value of constant case value
    AbstractValue caseVal=evaluateExpressionAV(caseExpr,currentEState);
    // compare case constant with switch expression value
    SAWYER_MESG(logger[TRACE])<<"switch cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
    AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
    if(comparisonVal.isTop()||comparisonVal.isTrue()) {
      SAWYER_MESG(logger[TRACE])<<"switch-case: continuing."<<endl;
      return elistify(reInitEState(currentEState,targetLabel,cs,newPState));
    } else {
      // infeasable path
      SAWYER_MESG(logger[TRACE])<<"switch-case: infeasable path."<<endl;
      list<EStatePtr> emptyList;
      return emptyList;
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, EStatePtr estate) {
    return elistify(transferVariableDeclarationEState(decl,estate, edge.target()));
  }

  std::list<EStatePtr> EStateTransferFunctions::transferGnuExtensionStmtExpr(SgNode* /*nextNodeToAnalyze1*/, Edge /*edge*/, EStatePtr estate) {
    //cout<<"WARNING: ignoring GNU extension StmtExpr (EStateTransferFunctions::transferGnuExtensionStmtExpr)"<<endl;
    return elistify(estate);
  }

  std::list<EStatePtr> EStateTransferFunctions::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, EStatePtr estate) {
    SgNode* nextNodeToAnalyze2=0;
    if(isSgExprStatement(nextNodeToAnalyze1)) {
      nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
    }
    if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
      nextNodeToAnalyze2=nextNodeToAnalyze1;
    }
    ROSE_ASSERT(nextNodeToAnalyze2);

    if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
      return transferTrueFalseEdge(nextNodeToAnalyze2, edge, estate);
    } else if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2) || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
      return transferIncDecOp(nextNodeToAnalyze2,edge,estate);
    } else if(SgAssignOp* assignOp=isSgAssignOp(nextNodeToAnalyze2)) {
      return transferAssignOp(assignOp, edge, estate);
    } else if(isSgCompoundAssignOp(nextNodeToAnalyze2)) {
      logger[ERROR] <<"compound assignment operators not supported. Normalization required."<<endl;
      logger[ERROR] <<"expr: "<<nextNodeToAnalyze2->unparseToString()<<endl;
      exit(1);
    } else if(isSgConditionalExp(nextNodeToAnalyze2)) {
      // this is meanwhile modeled in the ExprAnalyzer - TODO: utilize as expr-stmt.
      logger[ERROR] <<"found conditional expression outside expression. Normalization required."<<endl;
      exit(1);
    } else if(SgExpression* exp=isSgExpression(nextNodeToAnalyze2)) {
      SingleEvalResult res=evaluateExpression(exp,estate,MODE_VALUE);
      // this builds a new estate (sets new target label)
      EStatePtr estate2=res.estate; // use return state from ++/-- TODO: investigate (otherwise use estate->)
      PStatePtr newPState=estate2->pstate();
      CallString cs=estate->getCallString();
      return elistify(reInitEState(estate2,edge.target(),cs,newPState));
    } else {
      return transferIdentity(edge,estate);
    }
  }

  list<EStatePtr> EStateTransferFunctions::transferIdentity(Edge edge, EStatePtr estate) {
    // nothing to analyze, just create new estate (from same State) with target label of edge
    // can be same state if edge is a backedge to same cfg node
    ROSE_ASSERT(estate);
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      _analyzer->printStatusMessageLine("subtransfer     : transferIdentity      : -");
    }

    EStatePtr newEState=estate;
    newEState->setLabel(edge.target());
    return elistify(newEState);
  }


  std::list<EStatePtr> EStateTransferFunctions::transferAssignOp(SgAssignOp* node, Edge edge, EStatePtr estate) {

    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      //printTransferFunctionInfo(TransferFunctionCode::Assign,nextNodeToAnalyze2,edge,estate);
      _analyzer->printStatusMessageLine("subtransfer     : transferAssignOp      : "+node->unparseToString());
    }
    return evalAssignOp3(node,edge.target(),estate);
  }


  EStatePtr EStateTransferFunctions::reInitFailedAssertEState(EStatePtr estate, Label target) {
    EStatePtr newEState=estate;
    newEState->io.recordFailedAssert();
    newEState->setLabel(target);
    return newEState;
  }


  list<EStatePtr> EStateTransferFunctions::transferFailedAssert(Edge edge, EStatePtr estate) {
    return elistify(reInitFailedAssertEState(estate,edge.target()));
  }

  AbstractValue EStateTransferFunctions::getMemoryRegionAbstractElementSize(AbstractValue memLoc) {
    if(memLoc.isTop()||memLoc.isPointerToArbitraryMemory()||memLoc.isBot()||memLoc.isNullPtr())
      return AbstractValue::createTop();
    VariableId memId=memLoc.getVariableId();
    CodeThorn::TypeSize elementSize=_variableIdMapping->getElementSize(memId);
    if (VariableIdMapping::isUnknownSizeValue(elementSize)) {
      return AbstractValue::createTop();
    } else {
      return AbstractValue(elementSize);
    }
  }

  AbstractValue EStateTransferFunctions::getMemoryRegionAbstractNumElements(AbstractValue memLoc) {
    VariableId memId=memLoc.getVariableId();
    CodeThorn::TypeSize numElements=_variableIdMapping->getNumberOfElements(memId);
    if(VariableIdMapping::isUnknownSizeValue(numElements)) {
      return AbstractValue::createTop();
    } else {
      return AbstractValue(numElements);
    }
  }

  // wrapper function for calling eval functions for inc/dec ops at top-level AST
  // THIS FUNCTION IS NOT USED YET because evaluateExpression is still using transferIncDecOp (which calls this function)
  list<EStatePtr> EStateTransferFunctions::transferIncDecOpEvalWrapper(SgNode* node, Edge edge, EStatePtr estate) {
    SingleEvalResult res;
    switch(node->variantT()) {
    case V_SgPlusPlusOp:
    case V_SgMinusMinusOp:
      res=evaluateExpression(node,estate,MODE_VALUE);
      break;
    default:
      logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(node,2)<<endl;
      logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(node)<<endl;
      logger[ERROR] << "transferIncDecOp (top level): programmatic error in handling of inc/dec operators."<<endl;
      exit(1);
    }
    // this builds a new estate (sets new target label)
    EStatePtr estate2=res.estate; // use return state from ++/-- TODO: investigate (otherwise use estate->)
    PStatePtr newPState=estate2->pstate();
    CallString cs=estate->getCallString();
    return elistify(reInitEState(estate2,edge.target(),cs,newPState));
  }

  // used for top-level evaluation
  list<EStatePtr> EStateTransferFunctions::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, EStatePtr estate) {
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      printTransferFunctionInfo(TransferFunctionCode::IncDec,nextNodeToAnalyze2,edge,estate);
    }
    EStatePtr currentEState=estate;
    CallString cs=estate->getCallString();
    SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
    VariableId var;
    if(checkIfVariableAndDetermineVarId(nextNodeToAnalyze3,var)) {
      SingleEvalResult res=evaluateExpression(nextNodeToAnalyze3,currentEState);
      EStatePtr estate=res.estate;
      PStatePtr newPState=estate->pstate();

      AbstractValue oldVarVal=readFromAnyMemoryLocation(estate->label(),newPState,var);
      AbstractValue newVarVal;
      AbstractValue const1=1;
      AbstractValue elemSize=(oldVarVal.isPtr()?getMemoryRegionAbstractElementSize(oldVarVal):AbstractValue(1));
      switch(nextNodeToAnalyze2->variantT()) {
      case V_SgPlusPlusOp:
        newVarVal=AbstractValue::operatorAdd(oldVarVal,const1,elemSize);
        break;
      case V_SgMinusMinusOp:
        newVarVal=AbstractValue::operatorSub(oldVarVal,const1,elemSize);
        break;
      default:
        logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
        logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
        logger[ERROR] << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
        logger[ERROR] <<"programmatic error in handling of inc/dec operators."<<endl;
        exit(1);
      }
      writeToAnyMemoryLocation(estate->label(),newPState,AbstractValue::createAddressOfVariable(var),newVarVal);

      return elistify(reInitEState(estate,edge.target(),cs,newPState));
    } else {
      throw CodeThorn::Exception("Error: Normalization required. Inc/dec operators are only supported for variables: "+SgNodeHelper::sourceFilenameLineColumnToString(nextNodeToAnalyze2));
    }
  }

  // struct address is already determined, but no memory is reserved
  void EStateTransferFunctions::declareUninitializedStruct(Label lab,PState* pstate,AbstractValue structAddress, VariableId structVarId) {
    ROSE_ASSERT(structVarId.isValid());
    SgType* structType=getVariableIdMapping()->getType(structVarId);
    ROSE_ASSERT(structType);
    auto structMemberList=getVariableIdMapping()->getRegisteredClassMemberVars(structType);
    for(auto varId : structMemberList) {
      AbstractValue dmAddress=createStructDataMemberAddress(structAddress,varId);
      SgType* dmType=getVariableIdMapping()->getType(varId);
      if(isSgClassType(dmType)) {
        // nested struct type
        declareUninitializedStruct(lab,pstate,dmAddress,varId);
      } else {
        reserveMemoryLocation(lab,pstate,dmAddress);
      }
    }
  }

  AbstractValue EStateTransferFunctions::createStructDataMemberAddress(AbstractValue structAddress,VariableId varId) {
    ROSE_ASSERT(getVariableIdMapping()->isMemberVariable(varId));
    TypeSize offset=getVariableIdMapping()->getOffset(varId);
    AbstractValue offsetAV(offset);
    AbstractValue structDataMemberAddr=AbstractValue::operatorAdd(structAddress,offsetAV);
    return structDataMemberAddr;
  }

  EStatePtr EStateTransferFunctions::transferVariableDeclarationWithInitializerEState(SgVariableDeclaration* decl, SgInitializedName* initName, SgInitializer* initializer, VariableId initDeclVarId, EStatePtr currentEState, Label targetLabel) {
    CallString cs=currentEState->getCallString();
    Label label=currentEState->label();
    ROSE_ASSERT(currentEState->pstate());
    //cout<<"DEBUG: variable decl with initializer: "<<decl->unparseToString()<<":AST:"<<AstTerm::astTermWithNullValuesToString(initializer)<<endl;
    if(SgAssignInitializer* assignInit=isSgAssignInitializer(initializer)) {
      SgExpression* assignInitOperand=assignInit->get_operand_i();
      ROSE_ASSERT(assignInitOperand);
      if(SgAssignOp* assignOp=isSgAssignOp(assignInitOperand)) {
        CodeThorn::EStateTransferFunctions::MemoryUpdateList memUpdList=evalAssignOpMemUpdates(assignOp,currentEState);
        std::list<EStatePtr> estateList;
        ROSE_ASSERT(memUpdList.size()==1);
        auto memUpd=*memUpdList.begin();
        // code is normalized, lhs must be a variable : tmpVar= var=val;
        // store result of assignment in declaration variable
        EStatePtr estate=memUpd.first;
        PStatePtr newPState=estate->pstate();
        AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
        AbstractValue lhsAddr=memUpd.second.first;
        AbstractValue rhsValue=memUpd.second.second;
        writeToMemoryLocation(label,newPState,lhsAddr,rhsValue); // assignment in initializer
        initializeMemoryLocation(label,newPState,initDeclVarAddr,rhsValue); // initialization of declared var
        return reInitEState(estate,targetLabel,cs,newPState);
      } else if(SgFunctionRefExp* funRefExp=isSgFunctionRefExp(assignInitOperand)) {
        SingleEvalResult evalResult=evaluateExpression(funRefExp,currentEState);
        SAWYER_MESG(logger[TRACE])<<"rhs eval result 2: "<<evalResult.result.toString()<<endl;
        EStatePtr estate=evalResult.estate;
        PStatePtr newPState=estate->pstate();
        AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
        initializeMemoryLocation(label,newPState,initDeclVarAddr,evalResult.value());
        return reInitEState(estate,targetLabel,cs,newPState);
      }
    }
    if(getVariableIdMapping()->isOfClassType(initDeclVarId)) {
      SAWYER_MESG(logger[WARN])<<"initialization of structs not supported yet (not added to state) "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
      // for(offset(membervar) : membervars {initialize(address(initDeclVarId)+offset,eval(initializer+));}
      AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
      // STRUCT VARIABLE DECLARATION (reservative: revserve one abstract memory location)
      PStatePtr pstate=currentEState->pstate();
      reserveMemoryLocation(label,pstate,pointerVal);
      return reInitEState(currentEState,targetLabel,cs,pstate);
    }
    if(getVariableIdMapping()->isOfReferenceType(initDeclVarId)) {
      SAWYER_MESG(logger[TRACE])<<"initialization of reference 1:"<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
      SgAssignInitializer* assignInit=isSgAssignInitializer(initializer);
      ROSE_ASSERT(assignInit);
      SgExpression* assignInitOperand=assignInit->get_operand_i();
      ROSE_ASSERT(assignInitOperand);
      SAWYER_MESG(logger[TRACE])<<"initialization of reference 2:"<<AstTerm::astTermWithNullValuesToString(assignInitOperand)<<endl;
      SingleEvalResult evalResult=evaluateLExpression(assignInitOperand,currentEState);
      EStatePtr estate=evalResult.estate;
      PStatePtr newPState=estate->pstate();
      AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
      //initDeclVarAddr.setRefType(); // known to be ref from isOfReferenceType above
      // creates a memory cell in state that contains the address of the referred memory cell
      initializeMemoryLocation(label,newPState,initDeclVarAddr,evalResult.value());
      return reInitEState(estate,targetLabel,cs,newPState);
    }
    // has aggregate initializer
    if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
      if(isSgArrayType(aggregateInitializer->get_type())) {
        // only set size from aggregate initializer if not already determined
        if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==getVariableIdMapping()->unknownSizeValue()) {
          SAWYER_MESG(logger[TRACE])<<"Obtaining number of array elements from initializer in analyze declaration."<<endl;
          SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
          SgExpressionPtrList& initList=initListObjPtr->get_expressions();
          // TODO: nested initializers, currently only outermost elements: {{1,2,3},{1,2,3}} evaluates to 2.
          getVariableIdMapping()->setNumberOfElements(initDeclVarId, initList.size());
        }
        PStatePtr newPState=currentEState->pstate();
        newPState=analyzeSgAggregateInitializer(initDeclVarId, aggregateInitializer,newPState, currentEState);
        return reInitEState(currentEState,targetLabel,cs,newPState);
      } else {
        // type not supported yet
        SAWYER_MESG(logger[WARN])<<"aggregate initializer: unsupported type at: "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<" : "<<aggregateInitializer->get_type()->unparseToString()<<endl;
        // do not modify state. Value remains top.
        PStatePtr newPState=currentEState->pstate();
        return reInitEState(currentEState,targetLabel,cs,newPState);
      }
    } else if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
      SgExpression* rhs=assignInitializer->get_operand_i();
      ROSE_ASSERT(rhs);
      SAWYER_MESG(logger[DEBUG])<<"declaration with assign initializer:"<<" lhs:"<<initDeclVarId.toString(getVariableIdMapping())<<" rhs:"<<assignInitializer->unparseToString()<<" decl-term:"<<AstTerm::astTermWithNullValuesToString(decl)<<endl;

      // only create string in state with variable as pointer-address if it is an array (not for the case it is a char* pointer)
      // in the case of char* it is handled as a pointer initializer (and the string-pointer is already available in state)
      if(SgStringVal* stringValNode=isSgStringVal(assignInitializer->get_operand())) {
  if(isSgArrayType(initName->get_type())) {
    // handle special cases such as: char a[5]="abc";
    SAWYER_MESG(logger[TRACE])<<"Initalizing (array) with string: "<<stringValNode->unparseToString()<<endl;
    if(getVariableIdMapping()->getNumberOfElements(initDeclVarId)==-1) {
      VariableId stringLiteralId=getVariableIdMapping()->getStringLiteralVariableId(stringValNode);
      size_t stringLiteralMemoryRegionSize=getVariableIdMapping()->getNumberOfElements(stringLiteralId);
      getVariableIdMapping()->setNumberOfElements(initDeclVarId,stringLiteralMemoryRegionSize);
      SAWYER_MESG(logger[TRACE])<<"Determined size of array from literal string memory region size: "<<stringLiteralMemoryRegionSize<<endl;
    } else {
      SAWYER_MESG(logger[TRACE])<<"Determined size of array from array variable (containing string memory region) size: "<<getVariableIdMapping()->getNumberOfElements(initDeclVarId)<<endl;
    }
    //setElementSize(initDeclVarId,variableType); // set when VIM is created
    CodeThorn::TypeSize stringLen=stringValNode->get_value().size();
          string stringVal=stringValNode->get_value();
          CodeThorn::TypeSize memRegionNumElements=getVariableIdMapping()->getNumberOfElements(initDeclVarId);
          PStatePtr newPState=currentEState->pstate();

          // initialize array elements from string
          // C: stringLen <= memRegionNumElements : C++:stringLen+1 <= memRegionNumElements (checked by frontend)
          ROSE_ASSERT((!getVariableIdMapping()->isUnknownSizeValue(memRegionNumElements)) || (stringLen<=memRegionNumElements)); // isValidTypeSize implies StringLen<memRegionElements
          for(CodeThorn::TypeSize  i=0;i<stringLen;i++) {
            AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i),AbstractValue(1) /* elemensize */);
            // set default init value for past string elements of reserved array
            //cout<<"DEBUG: stringval at pos "<<i<<":"<<stringVal[i]<<" len:"<<stringVal.size()<<" ADDR:"<<newArrayElementAddr.toString()<<endl;
            ROSE_ASSERT(((size_t)i)<stringVal.size());
            initializeMemoryLocation(label,newPState,newArrayElementAddr,stringVal[i]);
          }

    // handle case that string is shorter than allocated memory
    if(stringLen<=memRegionNumElements) {
      CodeThorn::TypeSize numDefaultValuesToAdd=memRegionNumElements-stringLen;
      for(CodeThorn::TypeSize  i=0;i<numDefaultValuesToAdd;i++) {
        AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(stringLen+i),AbstractValue(1) /* element size */);
        // set default init value for past string elements of reserved array
        initializeMemoryLocation(label,newPState,newArrayElementAddr,AbstractValue(0));
      }

    }
    return reInitEState(currentEState,targetLabel,cs,newPState);
  }
      }
      // set type info for initDeclVarId
      //getVariableIdMapping()->setNumberOfElements(initDeclVarId,1); // single variable
      //SgType* variableType=initializer->get_type();
      //setElementSize(initDeclVarId,variableType);

      // build lhs-value dependent on type of declared variable
      AbstractValue lhsAbstractAddress=AbstractValue::createAddressOfVariable(initDeclVarId); // creates a pointer to initDeclVar
      ROSE_ASSERT(currentEState->pstate());
      SingleEvalResult evalResult=evaluateExpression(rhs,currentEState);

      EStatePtr estate=evalResult.estate;
      ROSE_ASSERT(estate->pstate());
      PStatePtr newPState=estate->pstate();
      initializeMemoryLocation(label,newPState,lhsAbstractAddress,evalResult.value());
      return reInitEState(estate,targetLabel,cs,newPState);
    } else {
      SAWYER_MESG(logger[WARN]) << "unsupported initializer in declaration: "<<decl->unparseToString()<<" (assuming arbitrary value)"<<endl;
      PStatePtr newPState=currentEState->pstate();
      return reInitEState(currentEState,targetLabel,cs,newPState);
    }
  }

  void EStateTransferFunctions::createAbstractArrayInPlace(Label label,PStatePtr pstate, VariableId initDeclVarId) {
    int arrayAbstractionIndex=getVariableIdMapping()->getArrayAbstractionIndex();
    int abstractionMode=getAnalyzer()->getOptionsRef().abstractionMode;
    if(arrayAbstractionIndex>=0 && getAnalyzer()->getOptionsRef().abstractionMode==1) {
      if(arrayAbstractionIndex>=1) {
        // only abstraction index 0 is currently supported
        cerr<<"Error: Array Abstraction index "<<arrayAbstractionIndex<<" not supported."<<endl;
        exit(1);
      }
      // declare abstract array, since only a coarse abstraction is supported, size>0 is irrelevant
      int elemIndex=0;
      int elemSize=1;
      AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex),AbstractValue(elemSize));
      // set default init value
      reserveMemoryLocation(label,pstate,newArrayElementAddr);
      newArrayElementAddr.setAbstractFlag(true);
    } else {
      cout<<"Error: array abstraction requested, but abstraction mode = "<<abstractionMode<<endl;
      cout<<"Analysis is misconfigured, exiting."<<endl;
      exit(1);
    }
  }

  EStatePtr EStateTransferFunctions::transferVariableDeclarationWithoutInitializerEState(SgVariableDeclaration*, SgInitializedName* initName, VariableId initDeclVarId, EStatePtr currentEState, Label targetLabel) {
    CallString cs=currentEState->getCallString();
    Label label=currentEState->label();
    PStatePtr newPState=currentEState->pstate();
    ROSE_ASSERT(newPState);
    //VariableId initDeclVarId=getVariableIdMapping()->variableId(decl);
    if(SgArrayType* arrayType=isSgArrayType(initName->get_type())) {
      //SgType* arrayElementType=arrayType->get_base_type();
      int numElements=getVariableIdMapping()->getNumberOfElements(initDeclVarId);
      SAWYER_MESG(logger[TRACE])<<"Number of elements in array is 0 (from variableIdMapping) - evaluating expression"<<endl;
      std::vector<SgExpression*> arrayDimExps=SageInterface::get_C_array_dimensions(*arrayType);
      // check if numElements is known, if not, try to determine dynamic value from state
      if(numElements<0) {
        // unknown number of elements, try to compute by evaluating expressions and compute size
        int computedNumElements=-1;
        for(auto dimIter=arrayDimExps.begin();dimIter!=arrayDimExps.end();++dimIter) {
          SgExpression* arrayDimExp=*dimIter;
          SAWYER_MESG(logger[TRACE])<<"DEBUG: Array dimension expression: "<<arrayDimExp->unparseToString()<<endl;
          SingleEvalResult evalRes=evaluateExpression(arrayDimExp,currentEState);
          AbstractValue arrayDimAVal=evalRes.result;
          SAWYER_MESG(logger[TRACE])<<"DEBUG: Computed array dimension: "<<arrayDimAVal.toString()<<endl;
          if(dimIter==arrayDimExps.begin()) {
            if(arrayDimAVal.isConstInt()) {
              computedNumElements=arrayDimAVal.getIntValue();
            } else {
              // computedNumElements remains -1 (=unknown)
              break;
            }
          } else {
            if(arrayDimAVal.isConstInt()) {
              numElements*=arrayDimAVal.getIntValue();
            } else {
              numElements=-1; // reset - any unknown dimension means size is unknown
              break;
            }
          }
        }
        numElements=computedNumElements; // determined by evaluating expressions (allows to handle variable length arrays)
      }
      auto elemSize=getVariableIdMapping()->getElementSize(initDeclVarId);
      if(numElements>0) {
        getVariableIdMapping()->setNumberOfElements(initDeclVarId,numElements);
        if(getAnalyzer()->getOptionsRef().abstractionMode>=1 || getVariableIdMapping()->isUnknownSizeValue(elemSize)) {
          createAbstractArrayInPlace(label,newPState,initDeclVarId);
        } else {
          // size is known, element size is known, and abstractionMode==0 (=concrete)
          // initialize all elements of contiguous possibliy multi-dim array of known size
          for(CodeThorn::TypeSize elemIndex=0;elemIndex<numElements;elemIndex++) {
            AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex),AbstractValue(elemSize));
            // set default init value
            reserveMemoryLocation(label,newPState,newArrayElementAddr);
          } // end of concrete init loop
        }
      } else {
        createAbstractArrayInPlace(label,newPState,initDeclVarId); // unknown size of array
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
      declareUninitializedStruct(label,newPState,pointerVal,initDeclVarId);
    } else if(getVariableIdMapping()->isOfPointerType(initDeclVarId)) {
      SAWYER_MESG(logger[TRACE])<<"decl of pointer var (undef)"<<endl;
      // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
      AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
      writeUndefToMemoryLocation(label, newPState,pointerVal);
    } else {
      // set it to top (=any value possible (uninitialized)) for
      // all remaining cases. It will become an error-path once
      // all cases are addressed explicitly above.
      SAWYER_MESG(logger[TRACE])<<"declaration of variable (other): "<<getVariableIdMapping()->getVariableDeclaration(initDeclVarId)->unparseToString()<<endl;
      reserveMemoryLocation(label,newPState,AbstractValue::createAddressOfVariable(initDeclVarId));
    }
    SAWYER_MESG(logger[TRACE])<<"Creating new EState"<<endl;
    return reInitEState(currentEState,targetLabel,cs,newPState);
  }

  EStatePtr EStateTransferFunctions::transferVariableDeclarationEState(SgVariableDeclaration* decl, EStatePtr currentEState, Label targetLabel) {

    /*
      1) declaration of variable or array
      - AggregateInitializer (e.g. T a[]={1,2,3};)
      - AggregateInitializer (e.g. T a[5]={1,2,3};)
      - AssignInitializer (e.g. T x=1+2;)
      2) if array, determine size of array (from VariableIdMapping)
      3) if no size is provided, determine it from the initializer list (and add this information to the variableIdMapping - or update the variableIdMapping).
    */

    SAWYER_MESG(logger[TRACE])<<"transferVariableDeclarationEState:"<<decl->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
    const SgInitializedNamePtrList& initNameList=decl->get_variables();
    if(initNameList.size()>1) {
      SAWYER_MESG(logger[ERROR])<<"Error: variable declaration contains more than one variable. Normalization required."<<endl;
      SAWYER_MESG(logger[ERROR])<<"Error: "<<decl->unparseToString()<<endl;
      exit(1);
    }
    ROSE_ASSERT(initNameList.size()==1);
    SgNode* initName0=*initNameList.begin();
    if(initName0!=nullptr) {
      if(SgInitializedName* initName=isSgInitializedName(initName0)) {
        VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
        SgInitializer* initializer=initName->get_initializer();
        if(initializer) {
          return transferVariableDeclarationWithInitializerEState(decl, initName, initializer, initDeclVarId, currentEState, targetLabel);
        } else {
          // no initializer (model default cases)
          ROSE_ASSERT(initName!=nullptr);
          return transferVariableDeclarationWithoutInitializerEState(decl, initName, initDeclVarId, currentEState, targetLabel);
        }
      } else {
        fatalErrorExit(decl,"in declaration (@initializedName) no variable found ... bailing out");
      }
    } else {
      fatalErrorExit(decl,"in declaration: no variable found ... bailing out");
    }
    ROSE_ASSERT(false); // non-reachable
  }

  // set the size of an element determined by this type
  void EStateTransferFunctions::setElementSize(VariableId variableId, SgType* elementType) {
    unsigned int typeSize=getVariableIdMapping()->getTypeSize(elementType);
    if(getVariableIdMapping()->getElementSize(variableId)!=getVariableIdMapping()->unknownSizeValue()
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

  void EStateTransferFunctions::fatalErrorExit(SgNode* node, string errorMessage) {
    SAWYER_MESG(logger[ERROR])<<errorMessage<<": "<<SgNodeHelper::locationAndSourceCodeToString(node)<<endl;
    exit(1);
  }
  void EStateTransferFunctions::warning(SgNode* node, string errorMessage) {
    SAWYER_MESG(logger[WARN])<<errorMessage<<": "<<SgNodeHelper::locationAndSourceCodeToString(node)<<endl;
  }

  CodeThorn::VariableIdSet EStateTransferFunctions::determineUsedGlobalVars(SgProject* project, CodeThorn::VariableIdSet& setOfGlobalVars) {
    CodeThorn::VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,getVariableIdMapping());
    CodeThorn::VariableIdSet setOfUsedGlobalVars;
    int32_t numUsedVars=0;
    int32_t numStringLiterals=0;
    for(auto var : setOfGlobalVars) {
      bool isUsed=(setOfUsedVars.find(var)!=setOfUsedVars.end());
      bool isStringLiteral=getVariableIdMapping()->isStringLiteralAddress(var);
      if(isUsed)
        numUsedVars++;
      if(isStringLiteral)
        numStringLiterals++;
      if(isUsed||isStringLiteral) {
        setOfUsedGlobalVars.insert(var);
      }
    }
    if(false && getAnalyzer()->getOptionsRef().status) {
      SAWYER_MESG(logger[INFO])<< "STATUS: Number of global variables     : "<<setOfGlobalVars.size()<<endl;
      SAWYER_MESG(logger[INFO])<< "STATUS: Number of used variables       : "<<setOfUsedVars.size()<<endl;
      SAWYER_MESG(logger[INFO])<< "STATUS: Number of string literals      : "<<numStringLiterals<<endl;
      SAWYER_MESG(logger[INFO])<< "STATUS: Number of used global variables: "<<numUsedVars<<endl;
    }
    return setOfUsedGlobalVars;
  }

  void EStateTransferFunctions::initializeGlobalVariables(SgProject* root, EStatePtr estate) {
    if(isSgProject(root)) {
      ROSE_ASSERT(getVariableIdMapping());
      CodeThorn::VariableIdSet setOfGlobalVars=getVariableIdMapping()->getSetOfGlobalVarIds();
      std::set<VariableId> setOfUsedGlobalVars=determineUsedGlobalVars(root,setOfGlobalVars);
      std::list<SgVariableDeclaration*> relevantGlobalVariableDecls=(getAnalyzer()->getOptionsRef().initialStateFilterUnusedVariables)?
        getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfUsedGlobalVars)
        : getVariableIdMapping()->getVariableDeclarationsOfVariableIdSet(setOfGlobalVars)
        ;
      uint32_t declaredInGlobalState=0;
      for(auto decl : relevantGlobalVariableDecls) {
        if(decl) {

          Label declLabel=getLabeler()->getLabel(decl);
          Label originalEstateLabel=estate->label();
          // use decl label when initializing global var to ensure errors in global var init are not associated with current estate
          estate=transferVariableDeclarationEState(decl,estate,declLabel);
          estate->setLabel(originalEstateLabel); // set estate label back to original label (was set to declLabel by transfer function)

          //if(getAnalyzer()->getOptionsRef().status)
          //  cout<<"STATUS: init global decl: "<<SgNodeHelper::locationAndSourceCodeToString(decl)<<": entries: "<<numNewEntries<<endl;
          declaredInGlobalState++;
          // this data is only used by globalVarIdByName to determine rers 'output' variable name in binary mode
          globalVarName2VarIdMapping[getVariableIdMapping()->variableName(getVariableIdMapping()->variableId(decl))]=getVariableIdMapping()->variableId(decl);
        } else {
          //cout<<"DEBUG: init global decl: 0"<<endl;
        }
      }
      if(getAnalyzer()->getOptionsRef().status) {
        //uint32_t numFilteredVars=setOfGlobalVars.size()-setOfUsedGlobalVars.size();
        //cout<< "STATUS: Number of unused variables filtered in initial state: "<<numFilteredVars<<endl;
        _analyzer->printStatusMessageLine("STATUS: Number of global variables declared in initial state: "+std::to_string(declaredInGlobalState));
      }
    } else {
      _analyzer->printStatusMessageLine("STATUS: no global scope. Global state remains without entries.");
    }
  }

  void EStateTransferFunctions::initializeArbitraryMemory(EStatePtr estate) {
    ROSE_ASSERT(getVariableIdMapping());
    AbstractValue memAddr=createNewMemoryRegionIdAndPointerToIt("$arbitraryMemory",-1); // arbitrary memory is of arbitrary size
    AbstractValue::setPointerToArbitraryMemory(memAddr);
    estate->pstate()->initializeArbitraryMemory(memAddr,AbstractValue::createUndefined()); // arbitrary memory has arbitrary values
  }

  VariableId EStateTransferFunctions::globalVarIdByName(std::string varName) {
    return globalVarName2VarIdMapping[varName];
  }

  // sets all elements in PState according to aggregate
  // initializer. Also models default values of Integers (floats not
  // supported yet). EState is only used for lookup (modified is only
  // the PState object).
  PStatePtr EStateTransferFunctions::analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PStatePtr pstate, /* for evaluation only  */ EStatePtr currentEState) {
    ROSE_ASSERT(aggregateInitializer);
    SAWYER_MESG(logger[TRACE])<<"analyzeSgAggregateInitializer::array-initializer:"<<aggregateInitializer->unparseToString()<<endl;
    Label label=currentEState->label();
    PStatePtr newPState=pstate;
    int elemIndex=0;
    SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
    SgExpressionPtrList& initList=initListObjPtr->get_expressions();

    AbstractValue elementSize=getMemoryRegionAbstractElementSize(initDeclVarId);
    for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
      AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex),elementSize);
      SgExpression* exp=*i;
      SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
      if(assignInit==nullptr) {
        SAWYER_MESG(logger[WARN])<<"expected assign initializer but found "<<exp->unparseToString();
        SAWYER_MESG(logger[WARN])<<"AST: "<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
        AbstractValue newVal=AbstractValue::createTop(); // was createBot before
        initializeMemoryLocation(label,newPState,arrayElemAddr,newVal);
      } else {
        // initialize element of array initializer in state
        SgExpression* assignInitExpr=assignInit->get_operand();
        // currentEState from above, newPState must be the same as in currentEState.
        AbstractValue newVal=evaluateExpressionAV(assignInitExpr,currentEState);
        initializeMemoryLocation(label,newPState,arrayElemAddr,newVal);
      }
      elemIndex++;
    }

    // initialize remaining elements (if there are any) with default value
    AbstractValue aggregateSizeAV=getMemoryRegionAbstractNumElements(initDeclVarId);

    // otherwise the size is determined from the aggregate initializer itself (done above)
    if(aggregateSizeAV.isConstInt()) {
      int aggregateSize=aggregateSizeAV.getIntValue();
      for(int i=elemIndex;i<aggregateSize;i++) {
        AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i),elementSize);
        SAWYER_MESG(logger[TRACE])<<"Init aggregate default value: "<<arrayElemAddr.toString()<<endl;
        // there must be a default value because an aggregate initializer exists (it is never undefined in this case)
        // note: int a[3]={}; also forces the array to be initialized with {0,0,0}, the list can be empty.
        // whereas with int a[3]; all 3 are undefined
        AbstractValue defaultValue=AbstractValue(0); // TODO: cases where default value is not 0.
        initializeMemoryLocation(label,newPState,arrayElemAddr,defaultValue);
      }
    } else {
      // if aggregate size is 0 there is nothing to do
    }
    return newPState;
  }

  AbstractValue EStateTransferFunctions::evaluateExpressionAV(SgExpression* expr,EStatePtr currentEState) {
    SingleEvalResult valueResult=evaluateExpression(expr,currentEState);
    AbstractValue val=valueResult.result;
    return val;
  }

  list<EStatePtr> EStateTransferFunctions::transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, EStatePtr estate) {
    EStatePtr currentEState=estate;
    CallString cs=estate->getCallString();
    Label newLabel;
    PStatePtr newPState;
    SingleEvalResult evalResult=evaluateExpression(nextNodeToAnalyze2,currentEState);
    list<EStatePtr> newEStateList;
    if(evalResult.isBot()) {
      SAWYER_MESG(logger[WARN])<<"PSTATE: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
      SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT : "<<nextNodeToAnalyze2->unparseToString()<<endl;
      SAWYER_MESG(logger[WARN])<<"CONDITION EVALUATES TO BOT at: "
                               <<ProgramLocationsReport::findOriginalProgramLocationOfLabel(getLabeler(),estate->label())
                               <<endl;
      newLabel=edge.target();
      newPState=evalResult.estate->pstate();
      EStatePtr newEstate=reInitEState(evalResult.estate,newLabel,cs,newPState);
      newEStateList.push_back(newEstate);
    } else if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
      // pass on EState
      newLabel=edge.target();
      newPState=evalResult.estate->pstate();

#pragma omp critical(VIOLATIONRECORDING)
      {
        if(numberOfReadWriteListeners()>0) {
          for(auto p : _readWriteListenerMap) {
            ReadWriteListener* readWriteListener=p.second;
            readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
          }
        }
      }
      newEStateList.push_back(reInitEState(estate,newLabel,cs,newPState));
    } else if((evalResult.isFalse() && edge.isType(EDGE_TRUE)) || (evalResult.isTrue() && edge.isType(EDGE_FALSE))) {
      // determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
      //cout<<"DEBUG: not on feasable execution path. skipping."<<endl;
    } else {
      // all other cases (assume evaluating to true or false, same as top)
      // pass on EState
      newLabel=edge.target();
      newPState=evalResult.estate->pstate();

#pragma omp critical(VIOLATIONRECORDING)
      {
        if(numberOfReadWriteListeners()>0) {
          for(auto p : _readWriteListenerMap) {
            ReadWriteListener* readWriteListener=p.second;
            readWriteListener->trueFalseEdgeEvaluation(edge,evalResult,estate);
          }
        }
      }

      newEStateList.push_back(reInitEState(estate,newLabel,cs,newPState));
    }
    return newEStateList;
  }

  CodeThorn::EStateTransferFunctions::MemoryUpdateList
  CodeThorn::EStateTransferFunctions::evalAssignOpMemUpdates(SgAssignOp* nextNodeToAnalyze2, EStatePtr estatePtr) {
    MemoryUpdateList memoryUpdateList;
    CallString cs=estatePtr->getCallString();
    EStatePtr currentEState=estatePtr;
    SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
    SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
    SAWYER_MESG(logger[TRACE])<<"evalAssignOpMemUpdates: lhs:"<<lhs->unparseToString()<<":"<<lhs->class_name()<<" rhs:"<<rhs->unparseToString()<<endl;
    SingleEvalResult rhsRes=evaluateExpression(rhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_VALUE);
    VariableId lhsVar;
    bool isLhsVar=checkIfVariableAndDetermineVarId(lhs,lhsVar);
    if(isLhsVar) {
      EStatePtr estate=rhsRes.estate;
      if(getVariableIdMapping()->isOfClassType(lhsVar)) {
        // assignments to struct variables are not supported yet (this test does not detect s1.s2 (where s2 is a struct, see below)).
        SAWYER_MESG(logger[WARN])<<"assignment of structs (copy constructor) is not supported yet. Target update ignored! (unsound)"<<endl;
      } else if(getVariableIdMapping()->isOfCharType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfIntegerType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfEnumType(lhsVar)) /* PP */ {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfFloatingPointType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfBoolType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfPointerType(lhsVar)) {
        // assume here that only arrays (pointers to arrays) are assigned
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(isSgTypeString(getVariableIdMapping()->getType(lhsVar))) {
        // assume here that only arrays (pointers to arrays) are assigned
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else if(getVariableIdMapping()->isOfReferenceType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      } else {
        // other types (e.g. function pointer type)
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,rhsRes.result)));
      }
    } else if(isSgDotExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      SingleEvalResult lhsResAddressResult=evaluateExpression(lhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_ADDRESS);
      EStatePtr estate=rhsRes.estate;
      AbstractValue lhsAddress=lhsResAddressResult.result;
      SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
      memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,rhsRes.result))); // TODO: investigate rhsRes.result
    } else if(isSgArrowExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      SingleEvalResult lhsResAddressResult=evaluateExpression(lhs,currentEState, CodeThorn::EStateTransferFunctions::MODE_ADDRESS);
      EStatePtr estate=rhsRes.estate;
      AbstractValue lhsAddress=lhsResAddressResult.result;
      SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
      memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,rhsRes.result))); // TODO: investigate rhsRes.result
    } else if(isSgPntrArrRefExp(lhs)) {
      EStatePtr estate=rhsRes.estate;
      PStatePtr oldPState=estate->pstate();
      if(_analyzer->getSkipArrayAccesses()) {
        // nothing to do (no memory access to add to list)
      } else {
        SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(lhs));
        SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(lhs));
        if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
          PStatePtr pstate2=oldPState;
          VariableId arrayVarId=getVariableIdMapping()->variableId(varRefExp);
          AbstractValue arrayPtrValue;
          // two cases
          if(getVariableIdMapping()->isOfArrayType(arrayVarId)) {
            // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
            arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
          } else if(getVariableIdMapping()->isOfPointerType(arrayVarId)) {
            // in case it is a pointer retrieve pointer value
            AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
            if(pstate2->memLocExists(ptr)) {
              arrayPtrValue=readFromMemoryLocation(estate->label(),pstate2,ptr);
            } else {
              arrayPtrValue=AbstractValue::createTop();
            }
          } else if(getVariableIdMapping()->isOfReferenceType(arrayVarId)) {
            AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
            if(pstate2->memLocExists(ptr)) {
              arrayPtrValue=readFromReferenceMemoryLocation(estate->label(),pstate2,ptr);
            } else {
              arrayPtrValue=AbstractValue::createTop();
            }
          } else {
            fatalErrorExit(nextNodeToAnalyze2,"lhs array access: unknown type of array or pointer.");
          }

          SingleEvalResult res2=evaluateExpression(indexExp,currentEState);
          AbstractValue indexValue=res2.value();
          if(_variableIdMapping->isFunctionParameter(arrayVarId)) {
            arrayPtrValue=readFromMemoryLocation(estate->label(),pstate2,arrayVarId); // pointer value of array function parameter
          }
          AbstractValue elementSize=getMemoryRegionAbstractElementSize(arrayPtrValue);
          AbstractValue arrayElementAddr=AbstractValue::operatorAdd(arrayPtrValue,indexValue,elementSize);
          //cout<<"DEBUG6: arrayPtrValue("<<arrayPtrValue.toString()<<")+indexValue("<<indexValue.toString()<<") => arrayElementAddr:"<<arrayElementAddr.toString()<<endl;
          if(arrayElementAddr.isBot()) {
            // inaccessible memory location, return empty estate list
            //return memoryUpdateList;
          }
          memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,rhsRes.result)));
        } else if(isSgAddressOfOp(lhs)) {
          // address of op, need to compute an l-value and use as address
          ROSE_ASSERT(false);
        } else {
          SingleEvalResult arrExpRes=evaluateExpression(arrExp,currentEState);
          AbstractValue arrPtrValue=arrExpRes.value();
          SingleEvalResult indexExpRes=evaluateExpression(indexExp,currentEState);
          AbstractValue indexValue=indexExpRes.value();
          AbstractValue elementSize=getMemoryRegionAbstractElementSize(arrPtrValue);
          AbstractValue arrayElementAddr=AbstractValue::operatorAdd(arrPtrValue,indexValue,elementSize);
          if(arrayElementAddr.isBot()) {
            //return memoryUpdateList; // inaccessible memory location, return empty estate list
          }
          memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,rhsRes.result)));
        }
      }
    } else if(SgPointerDerefExp* lhsDerefExp=isSgPointerDerefExp(lhs)) {
      SgExpression* lhsOperand=lhsDerefExp->get_operand();
      SingleEvalResult resLhs=evaluateExpression(lhsOperand,currentEState);
      AbstractValue lhsPointerValue=resLhs.result;
      if(lhsPointerValue.isNullPtr()) {
        notifyReadWriteListenersOnReading(estatePtr->label(),estatePtr->pstate(),lhsPointerValue);
        if(_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
          // no state can follow, but as requested keep going without effect
        } else {
          // no state can follow
          return memoryUpdateList;
        }
      } else if(lhsPointerValue.isTop()||lhsPointerValue.isPointerToArbitraryMemory()) {
        //recordPotentialNullPointerDereferenceLocation(estatePtr->label()); // notify?
        // specific case a pointer expr evaluates to top. Dereference operation
        // potentially modifies any memory location in the state.
        AbstractValue lhsAddress=lhsPointerValue;
        memoryUpdateList.push_back(make_pair(estatePtr,make_pair(lhsAddress,rhsRes.result))); // lhsAddress is top (must NOT be notified, because entered in updateList)
      } else if(!(lhsPointerValue.isPtr())) {
        // changed isUndefined to isTop (2/17/20)
        if(lhsPointerValue.isTop() && _analyzer->getIgnoreUndefinedDereference()) {
          // skip write access, just create new state (no effect)
          // nothing to do, as it request to not update the state in this case
        } else {
          // specific case a pointer expr evaluates to top. Dereference operation
          // potentially modifies any memory location in the state.
          // iterate over all elements of the state and merge with rhs value
          memoryUpdateList.push_back(make_pair(estatePtr,make_pair(lhsPointerValue,rhsRes.result)));
        }
      } else {
        memoryUpdateList.push_back(make_pair(estatePtr,make_pair(lhsPointerValue,rhsRes.result)));
      }
    } else {
      if(_analyzer->getSkipArrayAccesses()&&isSgPointerDerefExp(lhs)) {
        SAWYER_MESG(logger[WARN])<<"skipping pointer dereference: "<<lhs->unparseToString()<<endl;
      } else {
        fatalErrorExit(nextNodeToAnalyze2,"transferfunction:SgAssignOp: unrecognized expression on lhs.");
      }
    }
#if 0
    cout<<"DEBUG: EStateTransferFunctions::evalAssignOpMemUpdates:"<<endl;
    int i=0;
    cout<<"--------start---------------"<<endl;
    for(auto e:memoryUpdateList) {
      cout<<"memupdatelist["<<i<<": "<<e.second.first.toString(_variableIdMapping)<<" : "<<e.second.second.toString(_variableIdMapping)<<endl;
    }
    cout<<"--------end-----------------"<<endl;
#endif
    return memoryUpdateList;
  }

  bool EStateTransferFunctions::transferFunctionCallContextInPlace(CallString& cs, Label lab) {
    return cs.addLabel(lab);
  }

  void EStateTransferFunctions::transferFunctionCallReturnContextInPlaceOld(CallString& cs, Label lab) {
    if(cs.isLastLabel(lab)) {
      cs.removeLastLabel();
    }
  }

  bool EStateTransferFunctions::isFeasiblePathContextOld(CallString& cs,Label callLabel) {
    return cs.getLength()==0||cs.isLastLabel(callLabel)||cs.isMaxLength();
    //return cs.getLength()==0||cs.isLastLabel(callLabel);
  }

  std::string EStateTransferFunctions::transferFunctionCodeToString(TransferFunctionCode tfCode) {
    static std::map<TransferFunctionCode,string> tfCodeInfo=
      {
       {TransferFunctionCode::Unknown,"unknown"},
       {TransferFunctionCode::FunctionCall,"FunctionCall"},
       {TransferFunctionCode::FunctionCallLocalEdge,"FunctionCallLocalEdge"},
       {TransferFunctionCode::FunctionCallExternal,"FunctionCallExternal"},
       {TransferFunctionCode::FunctionCallReturn,"FunctionCallReturn"},
       {TransferFunctionCode::FunctionEntry,"FunctionEntry"},
       {TransferFunctionCode::FunctionExit,"FunctionExit"},
       {TransferFunctionCode::ReturnStmt,"ReturnStmt"},
       {TransferFunctionCode::FailedAssert,"FailedAssert"},
       {TransferFunctionCode::AsmStmt,"AsmStmt"},
       {TransferFunctionCode::ExprStmt,"ExprStmt"},
       {TransferFunctionCode::GnuExtensionStmtExpr,"GnuExtensionStmtExpr"},
       {TransferFunctionCode::Identity,"Identity"},
       {TransferFunctionCode::VariableDeclaration,"VariableDeclaration"},
       {TransferFunctionCode::CaseOptionStmt,"CaseOptionStmt"},
       {TransferFunctionCode::DefaultOptionStmt,"DefaultOptionStmt"},
       {TransferFunctionCode::Assign,"Assign"},
       {TransferFunctionCode::IncDec,"IncDec"},
       {TransferFunctionCode::ForkFunction,"ForkFunction"},
       {TransferFunctionCode::ForkFunctionWithExternalTargetFunction,"ForkFunctionWithExternalTargetFunction"},
      };
    ROSE_ASSERT(tfCode<tfCodeInfo.size());
    return tfCodeInfo[tfCode];
  }

  uint32_t EStateTransferFunctions::topSortLabelPos(Label lab) {
    TopologicalSort* topSort=getAnalyzer()->getTopologicalSort();
    auto topSortLabelPos=topSort->getLabelPosition(lab);
    return topSortLabelPos;
  }

  string EStateTransferFunctions::labelPosInfoToString(Label lab) {
    return std::to_string(topSortLabelPos(lab))+":"+"L"+lab.toString();
  }

  void EStateTransferFunctions::printTransferFunctionInfo(TransferFunctionCode tfCode, SgNode* node, Edge edge, EStatePtr estate) {
    stringstream ss;
    ss<<"transfer: "<<labelPosInfoToString(edge.source())<<"=>"<<labelPosInfoToString(edge.target())
      <<std::setw(6)<<"L"+estate->label().toString()
      <<" "<<estate->getCallStringRef().toString()
      <<": "<<std::setw(22)<<std::left<<transferFunctionCodeToString(tfCode)<<": ";
    if(getLabeler()->isFunctionEntryLabel(edge.source())||getLabeler()->isFunctionExitLabel(edge.source())) {
      ss<<SgNodeHelper::locationToString(node)<<": "<<SgNodeHelper::getFunctionName(node);
    } else {
      ss<<SgNodeHelper::locationAndSourceCodeToString(node,40,40);
    }
    #pragma omp critical (STATUS_MESSAGES)
    cout << ss.str()<<endl;
  }

  void EStateTransferFunctions::printEvaluateExpressionInfo(SgNode* node,EStatePtr /*estate*/, EvalMode mode) {
    stringstream ss;
    ss<<"          "<<std::setw(6+2)<<" "<<"evaluateExpression    : "<<node->unparseToString()<<" [evalmode"<<mode<<": "<<AstTerm::astTermWithNullValuesToString(node)<<"]";
    #pragma omp critical (STATUS_MESSAGES)
    cout<<ss.str()<<endl;
  }

  list<EStatePtr> EStateTransferFunctions::transferEdgeEStateDispatch(TransferFunctionCode tfCode, SgNode* node, Edge edge, EStatePtr estate) {
    ROSE_ASSERT(estate);
    ROSE_ASSERT(estate->pstate());
    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      printTransferFunctionInfo(tfCode,node,edge,estate);
    }
    switch(tfCode) {
    case TransferFunctionCode::FunctionCall: return transferFunctionCall(edge,estate);
    case TransferFunctionCode::FunctionCallLocalEdge: return transferFunctionCallLocalEdge(edge,estate);
    case TransferFunctionCode::FunctionCallExternal: return transferFunctionCallExternal(edge,estate);
    case TransferFunctionCode::FunctionCallReturn: return transferFunctionCallReturn(edge,estate);
    case TransferFunctionCode::FunctionEntry: return transferFunctionEntry(edge,estate);
    case TransferFunctionCode::FunctionExit: return transferFunctionExit(edge,estate);
    case TransferFunctionCode::ReturnStmt: return transferReturnStmt(edge,estate);
    case TransferFunctionCode::AsmStmt: return transferAsmStmt(edge,estate);
    case TransferFunctionCode::FailedAssert: return transferFailedAssert(edge,estate);
    case TransferFunctionCode::ExprStmt: return transferExprStmt(node,edge,estate);
    case TransferFunctionCode::GnuExtensionStmtExpr: return transferGnuExtensionStmtExpr(node,edge,estate);
    case TransferFunctionCode::Identity: return transferIdentity(edge,estate);
    case TransferFunctionCode::VariableDeclaration: return transferVariableDeclaration(isSgVariableDeclaration(node),edge,estate);
    case TransferFunctionCode::CaseOptionStmt: return transferCaseOptionStmt(isSgCaseOptionStmt(node),edge,estate);
    case TransferFunctionCode::DefaultOptionStmt: return transferDefaultOptionStmt(isSgDefaultOptionStmt(node),edge,estate);
    case TransferFunctionCode::Assign: return transferAssignOp(isSgAssignOp(node),edge,estate);
    case TransferFunctionCode::IncDec: return transferIncDecOp(node,edge,estate);
    case TransferFunctionCode::ForkFunction: return transferForkFunction(edge,estate,isSgFunctionCallExp(node));
    case TransferFunctionCode::ForkFunctionWithExternalTargetFunction: return transferForkFunctionWithExternalTargetFunction(edge,estate,isSgFunctionCallExp(node));
    default:
      return transferIdentity(edge,estate);
    }
    //unreachable
  }

  list<EStatePtr> EStateTransferFunctions::transferEdgeEStateInPlace(Edge edge, EStatePtr estate) {
    if(AbstractValue::domainAbstractionVariant==2) {
      estate->pstate()->removeAllElements();
    }
    pair<TransferFunctionCode,SgNode*> tfCodeNodePair=determineTransferFunctionCode(edge,estate);
    list<EStatePtr> list=transferEdgeEStateDispatch(tfCodeNodePair.first,tfCodeNodePair.second,edge,estate);
    if(AbstractValue::domainAbstractionVariant==2) {
      for(auto estate : list) {
        estate->pstate()->removeAllElements();
      }
    }
    return list;
  }

  list<EStatePtr> EStateTransferFunctions::transferEdgeEState(Edge edge, EStatePtr estate) {
    EStatePtr estateClone=estate->cloneWithoutIO(); // prepare for in-place modification of estate
    list<EStatePtr> newEStateList=transferEdgeEStateInPlace(edge,estateClone);
    // delete cloned state if another state has been created and it is not used in list
    // the list has either length 0 or 1.
    bool reused=false;
    for(auto es : newEStateList) {
      if(estateClone == es)
        reused=true;
      ROSE_ASSERT(estate != es); // ensure only new states are returned
    }
    if(!reused)
      delete estateClone;
    return newEStateList;
  }

  std::pair<EStateTransferFunctions::TransferFunctionCode,SgNode*> EStateTransferFunctions::determineTransferFunctionCode(Edge edge, EStatePtr estate) {
    ROSE_ASSERT(edge.source()==estate->label());
    ROSE_ASSERT(estate->pstate());
    // handle the edge as outgoing edge
    ROSE_ASSERT(_analyzer->getCFAnalyzer());
    SgNode* nextNodeToAnalyze=_analyzer->getCFAnalyzer()->getNode(edge.source());
    ROSE_ASSERT(nextNodeToAnalyze);
    TransferFunctionCode tfCode=TransferFunctionCode::Unknown;
    if(edge.isType(EDGE_LOCAL)) {
      tfCode=TransferFunctionCode::FunctionCallLocalEdge;
    } else if(SgNodeHelper::Pattern::matchAssertExpr(nextNodeToAnalyze)) {
      // handle assert(0)
      tfCode=TransferFunctionCode::FailedAssert;
    } else if(edge.isType(EDGE_CALL)) {
      tfCode=TransferFunctionCode::FunctionCall;
    } else if(edge.isType(EDGE_EXTERNAL)) {
      tfCode=TransferFunctionCode::FunctionCallExternal;
    } else if(isSgReturnStmt(nextNodeToAnalyze) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze)) {
      // "return x;": adds returnVar=eval() [but not for "return f();"]
      tfCode=TransferFunctionCode::ReturnStmt;
    } else if(isSgAsmStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::AsmStmt;
    } else if(getLabeler()->isFunctionEntryLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionEntry;
    } else if(getLabeler()->isFunctionExitLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionExit;
    } else if(getLabeler()->isFunctionCallReturnLabel(edge.source())) {
      tfCode=TransferFunctionCode::FunctionCallReturn;
    } else if(isSgCaseOptionStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::CaseOptionStmt;
    } else if(isSgDefaultOptionStmt(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::DefaultOptionStmt;
    } else if(isSgVariableDeclaration(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::VariableDeclaration;
    } else if(isSgExprStatement(nextNodeToAnalyze) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze)) {
      tfCode=TransferFunctionCode::ExprStmt;
    } else if(isSgStatementExpression(nextNodeToAnalyze)) {
      // GNU extension
      tfCode=TransferFunctionCode::GnuExtensionStmtExpr;
    } else if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze)) {
      // this case cannot happen for normalized code
      fatalErrorExit(funCall,"Function call detected not represented in ICFG. Normalization required.");
    } else {
      // all other cases identity function
      ROSE_ASSERT(!edge.isType(EDGE_EXTERNAL));
      ROSE_ASSERT(!edge.isType(EDGE_CALLRETURN));
      tfCode=TransferFunctionCode::Identity;
    }
    return std::make_pair(tfCode,nextNodeToAnalyze);
  }

  // ExprAnalyzer functions
  void EStateTransferFunctions::setVariableIdMapping(VariableIdMappingExtended* variableIdMapping) {
    _variableIdMapping=variableIdMapping;
  }
  CodeThorn::InterpreterMode EStateTransferFunctions::getInterpreterMode() {
    return _interpreterMode;
  }
  void EStateTransferFunctions::setInterpreterMode(CodeThorn::InterpreterMode im) {
    _interpreterMode=im;
  }

  string EStateTransferFunctions::getInterpreterModeFileName() {
    return _interpreterModeFileName;
  }
  void EStateTransferFunctions::setInterpreterModeFileName(string imFileName) {
    _interpreterModeFileName=imFileName;
  }

  bool EStateTransferFunctions::getIgnoreUndefinedDereference() {
    return _analyzer->getOptionsRef().ignoreUndefinedDereference;
  }

  bool EStateTransferFunctions::getIgnoreFunctionPointers() {
    return _analyzer->getOptionsRef().ignoreFunctionPointers;
  }

  // TODO: all following options to read from ctOpt (as above)
  void EStateTransferFunctions::setSkipUnknownFunctionCalls(bool skip) {
    _skipSelectedFunctionCalls=skip;
  }

  bool EStateTransferFunctions::getSkipUnknownFunctionCalls() {
    return _skipSelectedFunctionCalls;
  }

  void EStateTransferFunctions::setSkipArrayAccesses(bool skip) {
    _skipArrayAccesses=skip;
  }

  bool EStateTransferFunctions::getSkipArrayAccesses() {
    return _skipArrayAccesses;
  }

  void EStateTransferFunctions::setSVCompFunctionSemantics(bool flag) {
    _svCompFunctionSemantics=flag;
  }

  bool EStateTransferFunctions::getSVCompFunctionSemantics() {
    return _svCompFunctionSemantics;
  }

  bool EStateTransferFunctions::stdFunctionSemantics() {
    return _stdFunctionSemantics;
  }

  bool EStateTransferFunctions::getStdFunctionSemantics() {
    return _stdFunctionSemantics;
  }

  void EStateTransferFunctions::setStdFunctionSemantics(bool flag) {
    _stdFunctionSemantics=flag;
  }

  bool EStateTransferFunctions::checkIfVariableAndDetermineVarId(SgNode* node, VariableId& varId) {
    assert(node);
    if(SgNodeHelper::isArrayAccess(node)) {
      // 1) array variable id
      // 2) eval array-index expr
      // 3) if const then compute variable id otherwise return non-valid var id (would require set)
      return false;
    }
    if(SgVarRefExp* varref=isSgVarRefExp(node)) {
      // found variable
      ROSE_ASSERT(_variableIdMapping);
      varId=_variableIdMapping->variableId(varref);
      return true;
    } else {
      VariableId defaultVarId;
      varId=defaultVarId;
      return false;
    }
  }

  // macro for repeating code pattern to get correctly typed values from value expressions
#define CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SG_VALUE_EXP_TYPE, CPP_TYPE) \
  case V_ ## SG_VALUE_EXP_TYPE: {                                       \
    SG_VALUE_EXP_TYPE* exp=is ## SG_VALUE_EXP_TYPE(valueExp);           \
    ROSE_ASSERT(exp);                                                   \
    CPP_TYPE val=exp->get_value();                                      \
    return AbstractValue(val); }

  AbstractValue EStateTransferFunctions::abstractValueFromSgValueExp(SgValueExp* valueExp, EvalMode mode) {
    ROSE_ASSERT(valueExp);
    switch(valueExp->variantT()) {
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgFloatVal, float);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgDoubleVal, double);
      //    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongDoubleVal, long double);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgShortVal, short int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgIntVal, int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongIntVal, long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongLongIntVal, long long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedCharVal, unsigned char);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedShortVal, unsigned short);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedIntVal, unsigned int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongVal, unsigned long int);
      CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongLongIntVal, unsigned long long int);
      // following are all cases that require specific handling in ROSE AST
    case  V_SgCharVal: {
      SgCharVal* exp=isSgCharVal(valueExp);
      unsigned char val=(unsigned char)(signed char)exp->get_value();
      return AbstractValue(val);
    }
    case V_SgWcharVal: {
      SgWcharVal* exp=isSgWcharVal(valueExp);
      long int val=exp->get_value();
      return AbstractValue(val);
    }
    case V_SgComplexVal: {
      return AbstractValue(CodeThorn::Top());
    }
    case V_SgNullptrValExp: {
      return AbstractValue((int)0); // null pointer value
    }
    case V_SgEnumVal: {
      SgEnumVal* exp=isSgEnumVal(valueExp);
      int val=exp->get_value();
      return AbstractValue(val);
    }
    case V_SgStringVal: {
      SgStringVal* stringVal=isSgStringVal(valueExp);
      if(mode==MODE_EMPTY_STATE) {
        // string val addresses are only available if variable ids are available
        return AbstractValue::createTop();
      }
      // handle string literals
      std::string s=stringVal->get_value();
      VariableId stringValVarId=_variableIdMapping->getStringLiteralVariableId(stringVal);
      AbstractValue val=AbstractValue::createAddressOfVariable(stringValVarId);
      SAWYER_MESG(logger[TRACE])<<"Created abstract string literal value: "<<"\""<<s<<"\""<<": abstract value: "<<val.toString(_variableIdMapping)<<endl;
      return val;
    }
    case V_SgBoolValExp: {
      SgBoolValExp* exp=isSgBoolValExp(valueExp);
      // ROSE uses an integer for a bool
      int val=exp->get_value();
      if(val==0)
        return AbstractValue(false);
      else if(val==1)
        return AbstractValue(true);
      else {
        stringstream ss;
        ss<<"Error: unknown bool value (not 0 or 1): SgBoolExp::get_value()=="<<val;
        fatalErrorExit(valueExp,ss.str());
      }
      ROSE_FALLTHROUGH; // PP (20/11/24)
    }
    default:
      return AbstractValue::createTop();
    }
  }

  //////////////////////////////////////////////////////////////////////
  // EVAL CONSTINT
  //////////////////////////////////////////////////////////////////////

  void SingleEvalResult::init(EStatePtr estate, AbstractValue result) {
    this->estate=estate;
    this->result=result;
  }

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: return EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate,mode);break

#define CASE_EXPR_ANALYZER_EVAL_UNARY_OP(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: return EVALFUNCTIONNAME(is ## ROSENODENAME(node),operandResult,estate,mode);break

  namespace
  {
    SingleEvalResult
    notImplementedPlaceholder(EStatePtr estate) {
      SingleEvalResult res;
      res.init(estate,AbstractValue::createTop());
      return res;
    }
  }

  SingleEvalResult EStateTransferFunctions::evaluateLExpression(SgNode* node,EStatePtr estate) {
    AbstractValue result;
    if(SgVarRefExp* varExp=isSgVarRefExp(node)) {
      return evalLValueVarRefExp(varExp,estate);
    } else if(SgPntrArrRefExp* arrRef=isSgPntrArrRefExp(node)) {
      return evalLValuePntrArrRefExp(arrRef,estate);
    } else if(SgDotExp* dotExp=isSgDotExp(node)) {
      return evalLValueDotOrArrowExp(dotExp,estate);
    } else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
      return evalLValueDotOrArrowExp(arrowExp,estate);
    } else if(SgPointerDerefExp* ptrDerefExp=isSgPointerDerefExp(node)) {
      return evalLValuePointerDerefExp(ptrDerefExp,estate);
    } else if(/*SgAddressOfOp* addrOf=*/isSgAddressOfOp(node)) {
      return notImplementedPlaceholder(estate);
    } else if(/* SgCastExp* castExp=*/isSgCastExp(node)) {
      return notImplementedPlaceholder(estate);
    } else if(/*SgMemberFunctionRefExp* memFnExp=*/isSgMemberFunctionRefExp(node)) {
      return notImplementedPlaceholder(estate);
    } else {
      cerr<<"Error: unsupported lvalue expression2: "<<node->get_parent()->get_parent()->unparseToString()<<endl;
      cerr<<"     : "<<SgNodeHelper::sourceLineColumnToString(node)<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
      exit(1);
    }
    // unreachable
  }

  bool EStateTransferFunctions::isLValueOp(SgNode* node) {
    // assign operators not included yet
    return isSgAddressOfOp(node)
      || SgNodeHelper::isPrefixIncDecOp(node)
      || SgNodeHelper::isPostfixIncDecOp(node)
      ;
  }

  AbstractValue EStateTransferFunctions::evaluateExpressionWithEmptyState(SgExpression* expr) {
    SAWYER_MESG(logger[TRACE])<<"evaluateExpressionWithEmptyState(1):"<<expr->unparseToString()<<endl;
    ROSE_ASSERT(AbstractValue::getVariableIdMapping());
    if(EState::sharedPStates) {
      EStatePtr emptyEState=new EState(); // local temporary state is OK
      PStatePtr emptyPState=new PState(); // local temporary state is OK
      emptyEState->setPState(emptyPState);
      EStateTransferFunctions::EvalMode evalMode=EStateTransferFunctions::MODE_EMPTY_STATE;
      SingleEvalResult res=evaluateExpression(expr,emptyEState,evalMode);
      delete emptyEState; // also deallocates PState
      return res.value();
    } else {
      EStatePtr emptyEState=new EState(); // in non-shared pstate mode an empty pstate is allocated by the estate
      EStateTransferFunctions::EvalMode evalMode=EStateTransferFunctions::MODE_EMPTY_STATE;
      SingleEvalResult res=evaluateExpression(expr,emptyEState,evalMode);
      delete emptyEState;
      return res.value();
    }
  }

  SingleEvalResult EStateTransferFunctions::evalOp(SgNode* node,
                                                                 SingleEvalResult lhsResult,
                                                                 SingleEvalResult rhsResult,
                                                                 EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    AbstractValue::Operator op=sgNodeToAbstractValueOperator(node);
    res.estate=estate;
    res.result=AbstractValue::applyOperator(op,lhsResult.result,rhsResult.result);
    return res;
   }

  SingleEvalResult EStateTransferFunctions::evaluateExpression(SgNode* node, EStatePtr estate, EvalMode mode) {
    ROSE_ASSERT(estate->pstate()); // ensure state exists

    if(_analyzer->getOptionsRef().info.printTransferFunctionInfo) {
      printEvaluateExpressionInfo(node,estate,mode);
    }


    // initialize with default values from argument(s)
    SingleEvalResult res;
    res.estate=estate;
    res.result=AbstractValue::createBot();

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      if(isSgAssignOp(node)||isSgFunctionCallExp(node)||isSgVarRefExp(node)||isSgPlusPlusOp(node)||isSgMinusMinusOp(node)||isSgCompoundAssignOp(node)||isSgConditionalExp(node)) {
        res.result=AbstractValue::createTop();
        return res;
      }
      //cout<<"DEBUG: evalExp at: "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    }

    SAWYER_MESG(logger[TRACE])<<"evalExp at: "<<node->unparseToString()<<" evalmode: "<<mode<<endl;

    if(SgStatementExpression* gnuExtensionStmtExpr=isSgStatementExpression(node)) {
      warning(gnuExtensionStmtExpr,"ignoring GNU extension StmtExpr:");
      res.result=AbstractValue::createTop();
      return res;
    }

    if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
      return evalConditionalExpr(condExp,estate,mode);
    }

    if(dynamic_cast<SgBinaryOp*>(node)) {
      // special handling of short-circuit operators
      if(isSgAndOp(node)||isSgOrOp(node)) {
        return evaluateShortCircuitOperators(node,estate,mode);
      }
      SgNode* lhs=SgNodeHelper::getLhs(node);
      SingleEvalResult lhsResult=evaluateExpression(lhs,estate,mode);
      SgNode* rhs=SgNodeHelper::getRhs(node);
      SingleEvalResult rhsResult=evaluateExpression(rhs,estate,mode);
      // handle binary pointer operators
      switch(node->variantT()) {
        CASE_EXPR_ANALYZER_EVAL(SgArrowExp,evalArrowOp);
        CASE_EXPR_ANALYZER_EVAL(SgDotExp,evalDotOp);
      default:
        // fall through;
        ;
      }
      switch(node->variantT()) {
        CASE_EXPR_ANALYZER_EVAL(SgEqualityOp,evalEqualOp);
        CASE_EXPR_ANALYZER_EVAL(SgNotEqualOp,evalNotEqualOp);
        CASE_EXPR_ANALYZER_EVAL(SgAddOp,evalAddOp);
        CASE_EXPR_ANALYZER_EVAL(SgSubtractOp,evalSubOp);
        CASE_EXPR_ANALYZER_EVAL(SgMultiplyOp,evalMulOp);
        CASE_EXPR_ANALYZER_EVAL(SgDivideOp,evalDivOp);
        CASE_EXPR_ANALYZER_EVAL(SgModOp,evalModOp);
        CASE_EXPR_ANALYZER_EVAL(SgBitAndOp,evalBitwiseAndOp);
        CASE_EXPR_ANALYZER_EVAL(SgBitOrOp,evalBitwiseOrOp);
        CASE_EXPR_ANALYZER_EVAL(SgBitXorOp,evalBitwiseXorOp);
        CASE_EXPR_ANALYZER_EVAL(SgGreaterOrEqualOp,evalGreaterOrEqualOp);
        CASE_EXPR_ANALYZER_EVAL(SgGreaterThanOp,evalGreaterThanOp);
        CASE_EXPR_ANALYZER_EVAL(SgLessThanOp,evalLessThanOp);
        CASE_EXPR_ANALYZER_EVAL(SgLessOrEqualOp,evalLessOrEqualOp);
        CASE_EXPR_ANALYZER_EVAL(SgLshiftOp,evalBitwiseShiftLeftOp);
        CASE_EXPR_ANALYZER_EVAL(SgRshiftOp,evalBitwiseShiftRightOp);

        CASE_EXPR_ANALYZER_EVAL(SgPntrArrRefExp,evalArrayReferenceOp);
        CASE_EXPR_ANALYZER_EVAL(SgCommaOpExp,evalCommaOp);
      case V_SgAssignOp: {
        SingleEvalResult lhsAddress=evaluateLExpression(lhs,estate);
        Label fakeLabel; // ensures it is detected if not properly overwritten
        return evalAssignOp(isSgAssignOp(node),lhsResult /*ignored*/,rhsResult,fakeLabel,estate,mode);
        break;
      }
      default:
        fatalErrorExit(node,"EStateTransferFunctions: unknown binary operator ("+node->class_name()+")");
      }
    }

    // unary ops (1 of 2)
    if(isLValueOp(node)) {
      if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
        // arbitrary value
        res.init(estate,CodeThorn::Top());
        return res;
      }
      SgNode* child=SgNodeHelper::getFirstChild(node);
      SingleEvalResult operandResult=evaluateLExpression(child,estate);
      switch(node->variantT()) {
      case V_SgAddressOfOp: return evalAddressOfOp(isSgAddressOfOp(node),operandResult,estate,MODE_ADDRESS);
      case V_SgPlusPlusOp: return evalPlusPlusOp(isSgPlusPlusOp(node),operandResult,estate,mode);
      case V_SgMinusMinusOp: return evalMinusMinusOp(isSgMinusMinusOp(node),operandResult,estate,mode);
        // SgPointerDerefExp??
      default:
        ; // nothing to do, fall through to next loop on unary ops
      }
    }

    // unary ops (2 of 2)
    if(dynamic_cast<SgUnaryOp*>(node)) {
      SgNode* child=SgNodeHelper::getFirstChild(node);
      SingleEvalResult operandResult=evaluateExpression(child,estate,mode); // operandResult used in macro
      switch(node->variantT()) {
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgNotOp,evalNotOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgCastExp,evalCastOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgBitComplementOp,evalBitwiseComplementOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusOp,evalUnaryMinusOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPointerDerefExp,evalDereferenceOp);
      default:
        fatalErrorExit(node,string("evaluateExpression::unknown unary operation at node ")+node->sage_class_name());
      } // end switch
    }

    ROSE_ASSERT(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));

    // EXPRESSION LEAF NODES

    // special case sizeof operator (operates on types and types of expressions)
    if(SgSizeOfOp* sizeOfOp=isSgSizeOfOp(node)) {
      return evalSizeofOp(sizeOfOp,estate);
    }
    if(SgValueExp* exp=isSgValueExp(node)) {
      ROSE_ASSERT(exp!=nullptr);
      return evalValueExp(exp,estate,mode);
    }
    switch(node->variantT()) {
    case V_SgVarRefExp: {
      //SAWYER_MESG(logger[TRACE])<<"case V_SgVarRefExp: started"<<endl;
      auto ret=evalVarRefExp(isSgVarRefExp(node),estate,mode);
      //SAWYER_MESG(logger[TRACE])<<"case V_SgVarRefExp: done"<<endl;
      return ret;
    }
    case V_SgFunctionCallExp: {
      return evalFunctionCall(isSgFunctionCallExp(node),estate);
    }
    case V_SgNullExpression: {
      res.result=AbstractValue::createTop();
      return res;
    }
    case V_SgFunctionRefExp: {
      if(getIgnoreFunctionPointers()) {
        // just ignore the call (this is unsound and only for testing)
        res.result=AbstractValue::createTop();
        return res;
      } else {
        // use of function addresses as value.
        return evalFunctionRefExp(isSgFunctionRefExp(node),estate,mode);
      }
    }
    case V_SgThisExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return res;
    }
    case V_SgNewExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return res;
    }
    case V_SgDeleteExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return res;
    }
    case V_SgAssignInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return res;
    }
    case V_SgConstructorInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return res;
    }
    case V_SgAggregateInitializer: {
      res.result=AbstractValue::createTop();
      SAWYER_MESG(logger[WARN])<<"AggregateInitializer in expression (only handled at stmt level, returning top)"<<endl;
      return res;
    }
    case V_SgExprListExp: {
      res.result=AbstractValue::createTop();
      SAWYER_MESG(logger[WARN])<<"AggregateInitializer's ExprListExp in expression (only handled at stmt level, returning top)"<<endl;
      return res;
    }
    default:
      throw CodeThorn::Exception("Error: evaluateExpression::unknown node in expression: "+string(node->sage_class_name())+" at "+SgNodeHelper::sourceFilenameToString(node)+" in file "+SgNodeHelper::sourceFilenameToString(node)+string(":")+node->unparseToString());
    } // end of switch
    throw CodeThorn::Exception("Error: evaluateExpression failed.");
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // EVAL FUNCTIONS
  /////////////////////////////////////////////////////////////////////////////////////////////////

  // evaluation functions
  SingleEvalResult EStateTransferFunctions::evalConditionalExpr(SgConditionalExp* condExp, EStatePtr estate, EvalMode /*mode*/) {
    SgExpression* cond=condExp->get_conditional_exp();
    SingleEvalResult condResult=evaluateExpression(cond,estate);
    SingleEvalResult singleResult=condResult;
    if(singleResult.result.isTop()||singleResult.result.isPointerToArbitraryMemory()) {
      EStatePtr temporaryFalseBranchEState=estate->cloneWithoutIO();
      SgExpression* trueBranch=condExp->get_true_exp();
      SingleEvalResult trueBranchResult=evaluateExpression(trueBranch,estate);
      SgExpression* falseBranch=condExp->get_false_exp();
      // clone estate for false branch
      SingleEvalResult falseBranchResult=evaluateExpression(falseBranch,temporaryFalseBranchEState);
      // merge falseBranchResult and trueBranchResult
      SingleEvalResult res;
      // reuse estate for final merged result
      //PState combinedPState=CodeThorn::PState::combine(estate->pstate(),temporaryFalseBranchEState->pstate()); // COMBINE PSTATE (IO missing), PSTATE COPY
      //PStatePtr combinedPStatePtr=new PState(combinedPState);
      //estate->setPState(combinedPStatePtr);
      res.init(estate,AbstractValue::combine(trueBranchResult.result,falseBranchResult.result));
      //delete temporaryFalseBranchEState;
      return res;
    } else if(singleResult.result.isTrue()) {
      SgExpression* trueBranch=condExp->get_true_exp();
      SingleEvalResult trueBranchResult=evaluateExpression(trueBranch,estate);
      return trueBranchResult;
    } else if(singleResult.result.isFalse()) {
      SgExpression* falseBranch=condExp->get_false_exp();
      SingleEvalResult falseBranchResult=evaluateExpression(falseBranch,estate);
      return falseBranchResult;
    } else {
      fatalErrorExit(condExp,"Error: evaluating conditional operator inside expressions - unknown behavior (condition may have evaluated to bot).");
    }
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evaluateShortCircuitOperators(SgNode* node,EStatePtr estate, EvalMode mode) {
    SgNode* lhs=SgNodeHelper::getLhs(node);
    SingleEvalResult lhsResult=evaluateExpression(lhs,estate,mode);
    switch(node->variantT()) {
    case V_SgAndOp: {
      // short circuit semantics
      if(lhsResult.isTrue()
         ||lhsResult.isTop()
         ||lhsResult.isPointerToArbitraryMemory()
         ||lhsResult.isBot()
         ||((lhsResult.result.isPtr()||lhsResult.result.isFunctionPtr())&&!lhsResult.result.isNullPtr())
         ||lhsResult.result.isRef() // this can be refined to access the referenced value and check that it is not 0
         ) {
        SgNode* rhs=SgNodeHelper::getRhs(node);
        SingleEvalResult rhsResult=evaluateExpression(rhs,estate,mode);
        return evalAndOp(isSgAndOp(node),lhsResult,rhsResult,estate,mode);
      } else {
        // rhs not executed
        if(!lhsResult.isFalse()) {
          cerr<<"Error: lhsResult is not false as expected here (of short-circuit AND op): "<<lhsResult.result.toString()<<endl;
        }
        ROSE_ASSERT(lhsResult.isFalse());
        // result must be zero (=false)
        return lhsResult;
      }
      break;
    }
    case V_SgOrOp: {
      if(lhsResult.isFalse()||lhsResult.isTop()||lhsResult.isPointerToArbitraryMemory()||lhsResult.isBot()) {
        SgNode* rhs=SgNodeHelper::getRhs(node);
        SingleEvalResult rhsResult=evaluateExpression(rhs,estate,mode);
        return evalOrOp(isSgOrOp(node),lhsResult,rhsResult,estate,mode);
      } else {
        // rhs not executed
        ROSE_ASSERT(lhsResult.isTrue());
        // bugfix MS 05/06/2020: convert any true value to true/1: (10||x)=>1
        lhsResult.result=AbstractValue(true);
        return lhsResult;
      }
      break;
    }
    default:
      fatalErrorExit(node, "Unknown short circuit binary op, nodetype:"+node->class_name());
    }
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evalEqualOp(SgEqualityOp*,
                                                                      SingleEvalResult lhsResult,
                                                                      SingleEvalResult rhsResult,
                                                                      EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorEq(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalNotEqualOp(SgNotEqualOp*,
                                                                         SingleEvalResult lhsResult,
                                                                         SingleEvalResult rhsResult,
                                                                         EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorNotEq(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalAndOp(SgAndOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorAnd(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalOrOp(SgOrOp*,
                                                                   SingleEvalResult lhsResult,
                                                                   SingleEvalResult rhsResult,
                                                                   EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=lhsResult.result.operatorOr(rhsResult.result);

    // encode short-circuit CPP-OR-semantics // TODO: investigate: should this be moved upwards?
    if(lhsResult.result.isTrue()) {
      res.result=lhsResult.result;
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalAddOp(SgAddOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    if(lhsResult.result.isPtr()) {
      AbstractValue elementSize=getMemoryRegionAbstractElementSize(lhsResult.result);
      res.result=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result,elementSize);
    } else if(rhsResult.result.isPtr()) {
      AbstractValue elementSize=getMemoryRegionAbstractElementSize(rhsResult.result);
      res.result=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result,elementSize);
    } else {
      res.result=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalSubOp(SgSubtractOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    if(lhsResult.result.isPtr() && rhsResult.result.isPtr()) {
      // TODO?: can check here for incompatible pointer type subtraction if lhs and rhs point into different memory regions
      AbstractValue elementSize=getMemoryRegionAbstractElementSize(lhsResult.result);
      res.result=AbstractValue::operatorSub(lhsResult.result,rhsResult.result,elementSize);
    } else if(lhsResult.result.isPtr()) {
      AbstractValue elementSize=getMemoryRegionAbstractElementSize(lhsResult.result);
      res.result=AbstractValue::operatorSub(lhsResult.result,rhsResult.result,elementSize);
    } else if(rhsResult.result.isPtr()) {
      AbstractValue elementSize=getMemoryRegionAbstractElementSize(rhsResult.result);
      res.result=AbstractValue::operatorSub(lhsResult.result,rhsResult.result,elementSize);
    } else {
      res.result=AbstractValue::operatorSub(lhsResult.result,rhsResult.result);
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalMulOp(SgMultiplyOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=AbstractValue::operatorMul(lhsResult.result,rhsResult.result);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalDivOp(SgDivideOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=AbstractValue::operatorDiv(lhsResult.result,rhsResult.result);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalModOp(SgModOp*,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=AbstractValue::operatorMod(lhsResult.result,rhsResult.result);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalBitwiseAndOp(SgBitAndOp*,
                                                                           SingleEvalResult lhsResult,
                                                                           SingleEvalResult rhsResult,
                                                                           EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseAnd(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalBitwiseOrOp(SgBitOrOp*,
                                                                          SingleEvalResult lhsResult,
                                                                          SingleEvalResult rhsResult,
                                                                          EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseOr(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalBitwiseXorOp(SgBitXorOp*,
                                                                           SingleEvalResult lhsResult,
                                                                           SingleEvalResult rhsResult,
                                                                           EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseXor(rhsResult.result));
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalBitwiseComplementOp(SgBitComplementOp*,
                                                                                  SingleEvalResult operandResult,
                                                                                  EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=operandResult.result.operatorBitwiseComplement();
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalGreaterOrEqualOp(SgGreaterOrEqualOp*,
                                                SingleEvalResult lhsResult,
                                                SingleEvalResult rhsResult,
                                                EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorMoreOrEq(rhsResult.result));
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalGreaterThanOp(SgGreaterThanOp*,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorMore(rhsResult.result));
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalLessOrEqualOp(SgLessOrEqualOp*,
                                             SingleEvalResult lhsResult,
                                             SingleEvalResult rhsResult,
                                             EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorLessOrEq(rhsResult.result));
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalLessThanOp(SgLessThanOp*,
                                          SingleEvalResult lhsResult,
                                          SingleEvalResult rhsResult,
                                          EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorLess(rhsResult.result));
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalBitwiseShiftLeftOp(SgLshiftOp*,
                                                  SingleEvalResult lhsResult,
                                                  SingleEvalResult rhsResult,
                                                  EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseShiftLeft(rhsResult.result));
    return res;
  }

  SingleEvalResult
  EStateTransferFunctions::evalBitwiseShiftRightOp(SgRshiftOp*,
                                                   SingleEvalResult lhsResult,
                                                   SingleEvalResult rhsResult,
                                                   EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=(lhsResult.result.operatorBitwiseShiftRight(rhsResult.result));
    return res;
  }

  // array variable NOT in state. Special space optimization case for constant array.
  // option: --arrays-not-in-state=on
  SingleEvalResult EStateTransferFunctions::evalArrayNotInState(SgNode* node, SingleEvalResult& res, VariableId arrayVarId, AbstractValue arrayPtrPlusIndexValue) {
    if(_variableIdMapping->hasAssignInitializer(arrayVarId)) {
      // special case of string initializer: x[2]="";
      SAWYER_MESG(logger[WARN])<<"array assign initializer is not supported yet (assuming any value):"<<node->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(node)<<endl;
      AbstractValue val=AbstractValue::createTop();
      res.result=val;
      return res;
    }
    if(_variableIdMapping->isAggregateWithInitializerList(arrayVarId)) {
      SgExpressionPtrList& initList=_variableIdMapping->getInitializerListOfArrayVariable(arrayVarId);
      int elemIndex=0;
      // TODO: slow linear lookup (TODO: pre-compute all values and provide access function)
      for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
        SgExpression* exp=*i;
        SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
        if(assignInit) {
          SgExpression* initExp=assignInit->get_operand_i();
          ROSE_ASSERT(initExp);
          if(SgIntVal* intValNode=isSgIntVal(initExp)) {
            int intVal=intValNode->get_value();
            int index2=arrayPtrPlusIndexValue.getIndexIntValue();
            if(elemIndex==index2) {
              AbstractValue val=AbstractValue(intVal); // TODO BYTEMODE?
              res.result=val;
              return res;
            }
          } else {
            SAWYER_MESG(logger[WARN])<<"unsupported array initializer value (assuming any value):"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
            AbstractValue val=AbstractValue::createTop();
            res.result=val;
            return res;
          }
        } else {
          SAWYER_MESG(logger[WARN])<<"no assign initialize:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
          AbstractValue val=AbstractValue::createTop();
          res.result=val;
          return res;
        }
        elemIndex++;
      } // for loop
      // index not within array bounds, out-of-bounds access
      res.result=AbstractValue::createTop();
      return res;
    } else {
      // no initializer, initialize with top
      AbstractValue val=AbstractValue::createTop();
      res.result=val;
      return res;
    }
  }

  SingleEvalResult
  EStateTransferFunctions::evalArrayReferenceOp(SgPntrArrRefExp* node,
                                                SingleEvalResult arrayExprResult,
                                                SingleEvalResult indexExprResult,
                                                EStatePtr estate, EvalMode mode) {
    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: "<<node->unparseToString()<<"arrayExprResult:"<<arrayExprResult.result.toString()<<" indexExprResult:"<<indexExprResult.result.toString()<<" mode:"<<mode<<endl;
    SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: AST:"<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    SingleEvalResult res;
    res.estate=estate;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      return res;
    }

    SgNode* arrayExpr=SgNodeHelper::getLhs(node);
    SAWYER_MESG(logger[TRACE])<<"arrayExpr: "<<arrayExpr->unparseToString()<<endl;

    if(indexExprResult.value().isTop()||getSkipArrayAccesses()==true) {
      // set result to top when index is top [imprecision]
      // assume top for array elements if skipped
      // Precision: imprecise
      SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::evalArrayReferenceOp: returns top"<<endl;
      res.result=CodeThorn::Top();
      return res;
    } else {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(arrayExpr)) {
        AbstractValue arrayPtrValue=arrayExprResult.result;
        PStatePtr const_pstate=estate->pstate();
        PStatePtr pstate2=const_pstate;
        VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
        // two cases
        if(_variableIdMapping->isOfArrayType(arrayVarId)) {
          if(_variableIdMapping->isFunctionParameter(arrayVarId)) {
            // function parameter of array type contains a pointer value in C/C++
            arrayPtrValue=readFromMemoryLocation(estate->label(),pstate2,arrayVarId); // pointer value of array function paramter
          } else {
            arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
            SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: created array address (from array type): "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
          }
        } else if(_variableIdMapping->isOfPointerType(arrayVarId)) {
          // in case it is a pointer retrieve pointer value
          if(pstate2->varExists(arrayVarId)) {
            arrayPtrValue=readFromMemoryLocation(estate->label(),pstate2,arrayVarId); // pointer value (without index)
            if(!(arrayPtrValue.isTop()||arrayPtrValue.isPointerToArbitraryMemory()||arrayPtrValue.isBot()||arrayPtrValue.isPtr()||arrayPtrValue.isNullPtr())) {
              logger[ERROR]<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<": value not a pointer value: "<<arrayPtrValue.toString()<<endl;
              logger[ERROR]<<estate->toString(_variableIdMapping)<<endl;
              exit(1);
            }
          } else {
            //cerr<<"Error: pointer variable does not exist in PState: "<<arrayVarId.toString()<<endl  ;
            // TODO PRECISION 2
            // variable may have been not written because abstraction is too coarse (subsummed in write to top)
            // => reading from anywhere, returning any value
            SAWYER_MESG(logger[WARN])<<"pointer variable does not exist in PState: "<<arrayVarId.toString()<<endl  ;
            res.result=CodeThorn::Top();
            return res;
          }
        } else if(_variableIdMapping->isOfReferenceType(arrayVarId)) {
          if(mode==MODE_VALUE) {
            arrayPtrValue=readFromReferenceMemoryLocation(estate->label(),pstate2,arrayVarId);
          }
          if(arrayPtrValue.isBot()) {
            // if referred memory location is not in state
            res.result=CodeThorn::Top();
            return res;
          }
        } else {
          cerr<<"Error: unknown type of array or pointer."<<endl;
          cerr<<node->unparseToString()<<" of type "<<node->get_type()->unparseToString()<<endl;
          exit(1);
        }
        if(arrayPtrValue.isNullPtr()) {
          notifyReadWriteListenersOnReading(estate->label(),const_pstate,arrayPtrValue);
          res.result=CodeThorn::Top();
          res.estate->io.recordVerificationError();
          if(!_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
            return res;
          }
        }
        AbstractValue indexExprResultValue=indexExprResult.value();
        AbstractValue elementSize=getMemoryRegionAbstractElementSize(arrayPtrValue);
        AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexExprResultValue,elementSize);
        if(arrayPtrPlusIndexValue.isNullPtr()) {
          notifyReadWriteListenersOnReading(estate->label(),const_pstate,arrayPtrPlusIndexValue);
          // there is no state following a definitive null pointer
          // dereference. An error-state recording this property is
          // created to allow analysis of errors on the programs
          // transition graph. In addition the property is also recorded in the _nullPointerDereferenceLocations list.
          res.result=CodeThorn::Top();
          // verification error states are detected in the solver and no successor states are computed.
          res.estate->io.recordVerificationError();
          if(!_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
            return res;
          }
        }
        if(pstate2->memLocExists(arrayPtrValue)) {
          // required for the following index computation (nothing to do here)
        } else {
          if(arrayPtrValue.isTop()||arrayPtrValue.isPointerToArbitraryMemory()) {
            res.result=CodeThorn::Top();
            notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),arrayPtrValue);
            return res;
          } else {
            // currently same as above
            notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),arrayPtrValue);
            res.result=CodeThorn::Top();
            return res;
          }
          ROSE_ASSERT(false); // not reachable
        }
        if(pstate2->memLocExists(arrayPtrPlusIndexValue)) {
          // address of denoted memory location
          switch(mode) {
          case MODE_VALUE:
            res.result=readFromMemoryLocation(estate->label(),pstate2,arrayPtrPlusIndexValue);
            SAWYER_MESG(logger[TRACE])<<"retrieved array element value:"<<res.result<<" from "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
            return res;
          case MODE_ADDRESS:
            res.result=arrayPtrPlusIndexValue;
            return res;
          default:
            SAWYER_MESG(logger[FATAL])<<"Internal error: evalArrayReferenceOp: unsupported EvalMode."<<endl;
            exit(1);
          }
        } else {
          SAWYER_MESG(logger[WARN])<<"evalArrayReferenceOp:"<<" array memory location not in state: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          if(mode==MODE_ADDRESS) {
            SAWYER_MESG(logger[WARN])<<"EStateTransferFunctions::evalArrayReferenceOp: address mode not possible for variables not in state."<<endl;
            res.result=CodeThorn::Top();
            return res;
          }

          if(_variableIdMapping->isOfArrayType(arrayVarId)) {
            if(_analyzer->getOptionsRef().arraysNotInState) {
              return evalArrayNotInState(node,res,arrayVarId,arrayPtrPlusIndexValue);
            } else {
              res.result=AbstractValue::createTop();
              return res;
            }
          } else if(_variableIdMapping->isStringLiteralAddress(arrayVarId)) {
            SAWYER_MESG(logger[WARN])<<"Error: Found string literal address, but data not present in state."<<endl;
            AbstractValue val=AbstractValue::createTop();
            res.result=val;
            return res;
          } else {
            // continue after potential out-of-bounds access (assume any value can have been read)
            AbstractValue val=AbstractValue::createTop();
            notifyReadWriteListenersOnReading(estate->label(),const_pstate,val); // this triggers NP+OB violation
            res.result=val;
            return res;
          }
        }
      } else {
        warning(node,"Unsupported array-access of multi-dimensional array (assuming top.");
        AbstractValue val=AbstractValue::createTop();
        res.result=val;
        notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),val); // this triggers NP+OB violation
        return res;
      }
    }
    ROSE_ASSERT(false); // not reachable
  }

  SingleEvalResult EStateTransferFunctions::evalCommaOp(SgCommaOpExp*,
                                                                      SingleEvalResult /*lhsResult*/,
                                                                      SingleEvalResult rhsResult,
                                                                      EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    // lhsResult is ignored in comma op
    // result of the expression is the rhs's value
    res.result=rhsResult.result;
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalNotOp(SgNotOp*,
                                                                    SingleEvalResult operandResult,
                                                                    EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=operandResult.result.operatorNot();
    return res;
  }
  SingleEvalResult EStateTransferFunctions::evalUnaryMinusOp(SgMinusOp*,
                                                                           SingleEvalResult operandResult,
                                                                           EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    res.result=operandResult.result.operatorUnaryMinus();
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalSizeofOp(SgSizeOfOp* node,
                                                                       EStatePtr estate, EvalMode /*mode*/) {
    // two cases: (1) operand is a type, operand is a an expression
    SAWYER_MESG(logger[TRACE])<<"evalSizeofOp(started):"<<node->unparseToString()<<endl;
    SgType* operandType=node->get_operand_type();
    CodeThorn::TypeSize typeSize=-1; // remains -1 if no size can be determined
    AbstractValue sizeValue=AbstractValue::createTop();

    if(operandType) {
      typeSize=_variableIdMapping->getTypeSize(operandType);
      SAWYER_MESG(logger[TRACE])<<"typesize: "<<typeSize<<":"<<operandType->unparseToString()<<endl;
    } else if(SgExpression* exp=node->get_operand_expr()) {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
        typeSize=_variableIdMapping->getTypeSize(_variableIdMapping->variableId(varRefExp));
      } else if(SgType* expType=exp->get_type()) {
        typeSize=_variableIdMapping->getTypeSize(expType);
      } else {
        SAWYER_MESG(logger[WARN])<<"sizeof: could not determine any type of sizeof argument and unsupported argument expression: "<<SgNodeHelper::sourceLineColumnToString(exp)<<": "<<exp->unparseToString()<<endl<<AstTerm::astTermWithNullValuesToDot(exp)<<endl;
      }
    } else {
      SAWYER_MESG(logger[WARN]) <<"sizeof: could not determine any type of sizeof argument and no expression found either: "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
    }

    // determines sizeValue based on typesize
    if(typeSize==-1) {
      SAWYER_MESG(logger[WARN])<<"sizeof: could not determine size (= zero) of argument, assuming top "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
      sizeValue=AbstractValue::createTop();
    } else {
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizeof("<<typeSize<<")"<<endl;
      sizeValue=AbstractValue(typeSize);
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizevalue of sizeof("<<typeSize<<"):"<<sizeValue.toString()<<endl;
    }
    SingleEvalResult res;
    res.init(estate,sizeValue);
    SAWYER_MESG(logger[TRACE])<<"evalSizeofOp(finished):"<<node->unparseToString()<<endl;
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalCastOp(SgCastExp* node,
                                                                     SingleEvalResult operandResult,
                                                                     EStatePtr estate, EvalMode /*mode*/) {
    // TODO: truncation of values
    // TODO: adapt pointer value element size
    SgType* targetType=node->get_type();
    targetType=targetType->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
    AbstractValue operandValue=operandResult.value();
    if(operandValue.isPtr() && !operandValue.isNullPtr()) {
      if(SgPointerType* ptrType=isSgPointerType(targetType)) {
        SgType* elementType=ptrType->get_base_type();
        long int elementTypeSize=_variableIdMapping->getTypeSize(elementType);
        long int elementSizeMemRegion=getMemoryRegionElementSize(operandValue);
        // convert to top if a concrete pointer is cast to a pointer of different element size to avoid pointer arithmetic with different element sizes
        if(elementTypeSize!=elementSizeMemRegion
           && elementTypeSize!=0 /* zero-check: exclude cast to void* because it must be cast to some non-zero pointer to be used */
           && !operandValue.isPointerToArbitraryMemory()
           && !operandValue.isAbstract()
           && !operandValue.isUndefined()
           ) {
          SAWYER_MESG(logger[WARN])<<"evalCastOp: pointer cast: "<<node->unparseToString()<<" : operandResult:"<<operandResult.value().toString()<<": "<<AstTerm::astTermWithNullValuesToString(node)<<"casting pointer to mem region with elemsize "<<elementSizeMemRegion<<" to element type size: "<<elementTypeSize<<":"<<elementType->unparseToString()<<" (assuming arbitrary pointer)"<<endl;
          // convert to arbitrary pointer
          SingleEvalResult res;
          res.init(estate,AbstractValue::createTop()); // create arbitrary memory pointer
          return res;
        }
      }
    }
    SingleEvalResult res;
    res.init(estate,operandResult.result);
    //cout<<"evalCastOp: result: "<<operandResult.result.toString()<<endl;
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalArrowOp(SgArrowExp* node,
                                                                      SingleEvalResult lhsResult,
                                                                      SingleEvalResult rhsResult,
                                                                      EStatePtr estate, EvalMode mode) {
    SingleEvalResult res;
    res.estate=estate;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      return res;
    }
    SAWYER_MESG(logger[TRACE])<<"ArrowOp: "<<node->unparseToString()<<"lhsResult(address): "<<lhsResult.result.toString()<< " rhsResult(offset):"<<rhsResult.result.toString()<<" mode:"<<mode<<endl;
    // L->R : L evaluates to pointer value (address), R evaluates to offset value (a struct member always evaluates to an offset)
    //_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
    AbstractValue referencedAddress=lhsResult.result;
    bool isDefinitelyNullPtr=checkAndRecordNullPointer(referencedAddress, estate);
    SAWYER_MESG(logger[TRACE])<<"ArrowOp: is definitely null pointer:"<<isDefinitelyNullPtr<<endl;
    if(!isDefinitelyNullPtr) {
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: referencedAddress(lhs):"<<referencedAddress.toString(_variableIdMapping)<<endl;
      AbstractValue dereferencedAddress=readFromMemoryLocation(estate->label(),estate->pstate(),referencedAddress);
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: dereferencedAddress(lhs):"<<dereferencedAddress.toString(_variableIdMapping)<<endl;
      AbstractValue offset=rhsResult.result;
      AbstractValue denotedAddress=AbstractValue::operatorAdd(dereferencedAddress,offset); // data member offset
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: denoted Address(lhs):"<<denotedAddress.toString(_variableIdMapping)<<endl;

      switch(mode) {
      case MODE_VALUE:
        SAWYER_MESG(logger[TRACE])<<"Arrow op: reading value from arrowop-struct location."<<denotedAddress.toString(_variableIdMapping)<<endl;
        res.result=readFromMemoryLocation(estate->label(),estate->pstate(),denotedAddress);
        break;
      case MODE_ADDRESS:
        res.result=denotedAddress;
        break;
      case MODE_EMPTY_STATE:
        throw CodeThorn::Exception("Interal error: EStateTransferFunctions::evalArrowOp: empty state mode.");
        // intentionally no default case, all cases must be represented
      }
      return res;
    } else {
      SAWYER_MESG(logger[TRACE])<<"ArrowOp: "<<" null pointer deref."<<endl;
      SingleEvalResult empty;
      notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),referencedAddress);
      if(_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
        empty.init(estate,CodeThorn::Top()); // arbitrary result value, keep going
      } else {
        empty.init(estate,CodeThorn::Bot()); // indicates unreachable
      }
      return res;
    }
    ROSE_ASSERT(false); // unreachable
  }

  SingleEvalResult EStateTransferFunctions::evalDotOp(SgDotExp* node,
                                                                    SingleEvalResult lhsResult,
                                                                    SingleEvalResult rhsResult,
                                                                    EStatePtr estate, EvalMode mode) {
    SingleEvalResult res;
    res.estate=estate;
    // L.R : L evaluates to address, R evaluates to offset value (a struct member always evaluates to an offset)
    SAWYER_MESG(logger[TRACE])<<"evalDotOp: lhs:"<<lhsResult.result.toString(_variableIdMapping)<<" rhs: "<<rhsResult.result.toString(_variableIdMapping)<<endl;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      return res;
    }

    AbstractValue address=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result); // data member offset
    SAWYER_MESG(logger[TRACE])<<"DotOp: address:"<<address.toString(_variableIdMapping)<<endl;
    switch(mode) {
    case MODE_VALUE:
      if(isSgDotExp(node->get_parent())) {
        res.result=address;
      } else {
        res.result=readFromMemoryLocation(estate->label(),estate->pstate(),address);
        SAWYER_MESG(logger[TRACE])<<"DotOp: reading from memory:@"<<address.toString()<<" : "<<res.result.toString()<<endl;
      }
      break;
    case MODE_ADDRESS:
      res.result=address;
      break;
    case MODE_EMPTY_STATE:
      throw CodeThorn::Exception("Interal error: EStateTransferFunctions::evalDotOp: empty state mode.");
      // intentionally no default case, all cases must be represented
    }

    SAWYER_MESG(logger[TRACE])<<"DotOp: result:"<<res.result.toString(_variableIdMapping)<<endl;
    return res;
  }

  // returns true if it is definitely a null pointer
  bool EStateTransferFunctions::checkAndRecordNullPointer(AbstractValue derefOperandValue, EStatePtr estate) {
    if(derefOperandValue.isTop()||derefOperandValue.isPointerToArbitraryMemory()) {
      notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),derefOperandValue);
      return false;
    } else if(derefOperandValue.isNullPtr()) {
      notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),derefOperandValue);
      // definitely null pointer
      return true;
    }
    return false;
  }

  SingleEvalResult EStateTransferFunctions::evalAddressOfOp(SgAddressOfOp* node,
                                                                          SingleEvalResult operandResult,
                                                                          EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    AbstractValue operand=operandResult.result;
    SAWYER_MESG(logger[TRACE])<<"AddressOfOp: "<<node->unparseToString()<<" - operand: "<<operand.toString(_variableIdMapping)<<endl;
    if(operand.isTop()||operand.isBot()) {
      res.result=operand;
    } else {
      res.result=operand;
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalDereferenceOp(SgPointerDerefExp*,
                                                                            SingleEvalResult operandResult,
                                                                            EStatePtr estate, EvalMode mode) {
    return semanticEvalDereferenceOp(operandResult,estate,mode);
  }

  SingleEvalResult EStateTransferFunctions::semanticEvalDereferenceOp(SingleEvalResult operandResult,
                                                                                    EStatePtr estate, EvalMode mode) {
    SingleEvalResult res;
    res.estate=estate;
    AbstractValue derefOperandValue=operandResult.result;
    SAWYER_MESG(logger[TRACE])<<"semanticEvalDereferenceOp: derefOperandValue: "<<derefOperandValue.toRhsString(_variableIdMapping)<<" evalmode: "<<mode<<endl;

    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      // arbitrary value
      res.result=CodeThorn::Top();
      return res;
    }

    // null pointer check
    bool isDefinitelyNullPtr=checkAndRecordNullPointer(derefOperandValue, estate);
    if(isDefinitelyNullPtr) {
      SingleEvalResult empty;
      notifyReadWriteListenersOnReading(estate->label(),estate->pstate(),derefOperandValue);
      if(_analyzer->getOptionsRef().nullPointerDereferenceKeepGoing) {
        empty.init(estate,CodeThorn::Top()); // arbitrary result value, keep going
      } else {
        empty.init(estate,CodeThorn::Bot()); // indicates unreachable
      }
      return empty;
    } else {
      switch(mode) {
      case EStateTransferFunctions::MODE_VALUE :res.result=readFromMemoryLocation(estate->label(),estate->pstate(),derefOperandValue);break;
      case EStateTransferFunctions::MODE_ADDRESS:res.result=derefOperandValue;;break;
      default:
        cerr<<"Error: EStateTransferFunctions::semanticEvalDereferenceOp: unknown evaluation mode: "<<mode<<endl;
        exit(1);
      }
      return res;
    }
  }

  SingleEvalResult
  EStateTransferFunctions::evalAssignOp(SgAssignOp* node,
                                        SingleEvalResult /*lhsResult*/,
                                        SingleEvalResult rhsResult,
                                        Label targetLabel, EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    std::list<EStatePtr> estateList=evalAssignOp3(node, targetLabel, estate);
    ROSE_ASSERT(estateList.size()==1);
    res.result=rhsResult.result; // value result of assignment
    res.estate=*estateList.begin();
    return res;
  }

  bool EStateTransferFunctions::isTemporarySingleLocalVar(VariableId varId) {
    auto vim=getVariableIdMapping();
    if(vim->isTemporaryVariableId(varId)) {
      auto vidInfo=vim->getVariableIdInfoPtr(varId);
      return (vidInfo->aggregateType==VariableIdMapping::AT_SINGLE && vidInfo->variableScope==VariableIdMapping::VS_LOCAL);
    } else {
      return false;
    }
  }

  std::list<EStatePtr> EStateTransferFunctions::evalAssignOp3(SgAssignOp* node, Label targetLabel, EStatePtr estate) {
    auto pList=evalAssignOpMemUpdates(node, estate);
    std::list<EStatePtr> estateList;
    for (auto p : pList) {
      EStatePtr estate=p.first;
      Label label=estate->label();
      CallString cs=estate->getCallString();
      PStatePtr newPState=estate->pstate();
      AbstractValue lhsAddress=p.second.first;
      AbstractValue rhsValue=p.second.second;
      writeToAnyMemoryLocation(label,newPState,lhsAddress,rhsValue);
      estateList.push_back(reInitEState(estate,targetLabel,cs,newPState));
    }
    if(estateList.size()==0) {
      estateList.push_back(reInitEState(estate,targetLabel,estate->getCallString(),estate->pstate()));
    }
    return estateList;
  }

  SingleEvalResult EStateTransferFunctions::evalPreComputationOp(EStatePtr estate, AbstractValue address, AbstractValue change) {
    SingleEvalResult res;
    AbstractValue oldValue=readFromAnyMemoryLocation(estate->label(),estate->pstate(),address);
    AbstractValue elementSize=(oldValue.isPtr()?getMemoryRegionAbstractElementSize(oldValue):AbstractValue(1));
    AbstractValue newValue=AbstractValue::operatorAdd(oldValue,change,elementSize);
    CallString cs=estate->getCallString();
    PStatePtr newPState=estate->pstate();
    writeToAnyMemoryLocation(estate->label(),newPState,address,newValue);
    ROSE_ASSERT(_analyzer);
    res.init(reInitEState(estate,estate->label(),cs,newPState),newValue);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalPostComputationOp(EStatePtr estate, AbstractValue address, AbstractValue change) {
    // TODO?: change from precomp to postcomp (is correct for normalized code)
    SingleEvalResult res;
    AbstractValue oldValue=readFromAnyMemoryLocation(estate->label(),estate->pstate(),address);
    AbstractValue elementSize=(oldValue.isPtr()?getMemoryRegionAbstractElementSize(oldValue):AbstractValue(1));
    AbstractValue newValue=AbstractValue::operatorAdd(oldValue,change,elementSize);
    CallString cs=estate->getCallString();
    PStatePtr newPState=estate->pstate();
    writeToAnyMemoryLocation(estate->label(),newPState,address,newValue);
    ROSE_ASSERT(_analyzer);
    res.init(reInitEState(estate,estate->label(),cs,newPState),oldValue);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalPreIncrementOp(SgPlusPlusOp*,
                                                                             SingleEvalResult operandResult,
                                                                             EStatePtr estate, EvalMode /*mode*/) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=1;
    return evalPreComputationOp(estate,address,change);
  }

  SingleEvalResult EStateTransferFunctions::evalPreDecrementOp(SgMinusMinusOp*,
                                                                             SingleEvalResult operandResult,
                                                                             EStatePtr estate, EvalMode /*mode*/) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=-1;
    return evalPreComputationOp(estate,address,change);
  }

  SingleEvalResult EStateTransferFunctions::evalPostIncrementOp(SgPlusPlusOp*,
                                                                              SingleEvalResult operandResult,
                                                                              EStatePtr estate, EvalMode /*mode*/) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=1;
    return evalPostComputationOp(estate,address,change);
  }


  SingleEvalResult EStateTransferFunctions::evalPostDecrementOp(SgMinusMinusOp*,
                                                                              SingleEvalResult operandResult,
                                                                              EStatePtr estate, EvalMode /*mode*/) {
    AbstractValue address=operandResult.result;
    ROSE_ASSERT(address.isPtr()||address.isTop());
    AbstractValue change=-1;
    return evalPostComputationOp(estate,address,change);
  }

  SingleEvalResult EStateTransferFunctions::evalPlusPlusOp(SgPlusPlusOp* node,
                                                                         SingleEvalResult operandResult,
                                                                         EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    if(SgNodeHelper::isPrefixIncDecOp(node)) {
      // preincrement ++E
      return evalPreIncrementOp(node,operandResult,estate);
    } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
      // postincrement E++
      return evalPostIncrementOp(node,operandResult,estate);
    }
    fatalErrorExit(node, "Interal error: EStateTransferFunctions::evalPlusPlusOp");
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evalMinusMinusOp(SgMinusMinusOp* node,
                                                                           SingleEvalResult operandResult,
                                                                           EStatePtr estate, EvalMode /*mode*/) {
    SingleEvalResult res;
    res.estate=estate;
    if(SgNodeHelper::isPrefixIncDecOp(node)) {
      // predecrement --E
      return evalPreDecrementOp(node,operandResult,estate);
    } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
      // postdecrement E--
      return evalPostDecrementOp(node,operandResult,estate);
    }
    fatalErrorExit(node, "Interal error: EStateTransferFunctions::evalMinusMinusOp");
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evalLValueDotOrArrowExp(SgNode* node, EStatePtr estate, EvalMode /*mode*/) {
    ROSE_ASSERT(isSgDotExp(node)||isSgArrowExp(node));
    SingleEvalResult res;
    res.init(estate,AbstractValue::createBot());

    SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
    SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));

    SingleEvalResult lhsResult=evaluateExpression(arrExp,estate,MODE_ADDRESS);
    SingleEvalResult rhsResult=evaluateExpression(indexExp,estate,MODE_ADDRESS);
    SingleEvalResult intermediateResultList;
    if(SgDotExp* dotExp=isSgDotExp(node)) {
      return evalDotOp(dotExp,lhsResult,rhsResult,estate,MODE_ADDRESS);
    } else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
      return evalArrowOp(arrowExp,lhsResult,rhsResult,estate,MODE_ADDRESS);
    } else {
      fatalErrorExit(node,string("Internal error: EStateTransferFunctions::evalLValueExp: wrong operator node type: ")+node->class_name());
    }
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EStatePtr estate, EvalMode /*mode*/) {
    // for now we ignore array refs on lhs
    // TODO: assignments in index computations of ignored array ref
    // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
    // since nothing can change (because of being ignored) state remains the same
    SAWYER_MESG(logger[TRACE])<<"evalLValuePntrArrRefExp"<<endl;
    SingleEvalResult res;
    res.init(estate,AbstractValue::createBot());
    if(getSkipArrayAccesses()) {
      res.result=CodeThorn::Top();
      return res;
    } else {
      SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
      SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));
      SingleEvalResult lhsResult=evaluateExpression(arrExp,estate,MODE_ADDRESS);
      SingleEvalResult rhsResult=evaluateExpression(indexExp,estate,MODE_VALUE);
      SingleEvalResult res=evalArrayReferenceOp(node,lhsResult,rhsResult,estate,MODE_ADDRESS);
      return res;
    }
    // unreachable
    ROSE_ASSERT(false);
  }


  SingleEvalResult EStateTransferFunctions::evalLValueVarRefExp(SgVarRefExp* node, EStatePtr estate) {
    SAWYER_MESG(logger[TRACE])<<"evalLValueVarRefExp: "<<node->unparseToString()<<" label:"<<estate->label().toString()<<endl;
    SingleEvalResult res;
    res.init(estate,AbstractValue::createBot());
    PStatePtr pstate=estate->pstate();
    VariableId varId=_variableIdMapping->variableId(node);
    ROSE_ASSERT(varId.isValid());
    if(isMemberVariable(varId)) {
      ROSE_ASSERT(_variableIdMapping);
      CodeThorn::TypeSize offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
      SAWYER_MESG(logger[TRACE])<<"evalLValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
      if(VariableIdMapping::isUnknownSizeValue(offset)) {
        res.result=AbstractValue::createTop();
        return res;
      } else {
        res.result=AbstractValue(offset);
        return res;
      }
    }
    if(pstate->varExists(varId)) {
      SAWYER_MESG(logger[TRACE])<<"evalLValueVarRefExp: var exists: "<<_variableIdMapping->variableName(varId)<<endl;
      if(_variableIdMapping->isOfArrayType(varId)) {
        SAWYER_MESG(logger[TRACE])<<"lvalue array address(?): "<<node->unparseToString()<<"EState label:"<<estate->label().toString()<<endl;
        res.result=AbstractValue::createAddressOfArray(varId);
      } else if(_variableIdMapping->isOfReferenceType(varId)) {
        res.result=readFromMemoryLocation(estate->label(),pstate,varId); // address of a reference is the value it contains
      } else {
        res.result=AbstractValue::createAddressOfVariable(varId);
      }
      return res;
    } else {
      // special mode to represent information not stored in the state
      // i) unmodified arrays: data can be stored outside the state
      // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
      if(_variableIdMapping->isOfArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
        // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
        //res.result=AbstractValue(varId.getIdCode());
        SAWYER_MESG(logger[TRACE])<<"lvalue array address (non-existing in state)(?): "<<node->unparseToString()<<endl;
        res.result=AbstractValue::createAddressOfArray(varId);
        return res;
      } else {
        Label lab=estate->label();
        res.result=CodeThorn::Top();
        SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (LValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
        //cerr << "WARNING: estate: "<<estate->toString(_variableIdMapping)<<endl;
        return res;
      }
    }
    // unreachable
  }

  SingleEvalResult EStateTransferFunctions::evalLValuePointerDerefExp(SgPointerDerefExp* node, EStatePtr estate) {
    SAWYER_MESG(logger[TRACE])<<"evalLValuePtrDerefExp: "<<node->unparseToString()<<" label:"<<estate->label().toString()<<endl;
    // abstract_value(*p) = abstract_eval(p) : the value of 'p' is an abstract address stored in p, which is the lvalue of *p
    SgExpression* operand=node->get_operand_i();
    return evaluateExpression(operand,estate,EStateTransferFunctions::MODE_VALUE);
  }


  SingleEvalResult EStateTransferFunctions::evalFunctionRefExp(SgFunctionRefExp* node, EStatePtr estate, EvalMode mode) {
    SAWYER_MESG(logger[TRACE])<<"evalFunctionRefExp:"<<node->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    if(mode==EStateTransferFunctions::MODE_EMPTY_STATE) {
      SingleEvalResult res;
      res.init(estate,AbstractValue::createTop());
      return res;
    }
    // create address of function
    SingleEvalResult res;
    ROSE_ASSERT(_analyzer);

    SgFunctionSymbol* functionSym=node->get_symbol_i();
    ROSE_ASSERT(functionSym);
    SgFunctionDeclaration* funDecl=functionSym->get_declaration();
    //SgFunctionDeclaration* funDecl=SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(functionSym);
    SgDeclarationStatement* defFunDecl=funDecl->get_definingDeclaration();
    if(defFunDecl) {
      // if not resolved, funDef will be 0, and functionEntryLabel will be an invalid label id.
      funDecl=isSgFunctionDeclaration(defFunDecl);
    }
    ROSE_ASSERT(funDecl);
    SgFunctionDefinition* funDef=funDecl->get_definition();
    if(funDef) {
      Label funLab=_analyzer->getLabeler()->functionEntryLabel(funDef);

      // label of corresponding entry label of function of node; if function is external, then label is an invalid label.
      res.init(estate,AbstractValue::createAddressOfFunction(funLab));
      SAWYER_MESG(logger[TRACE])<<"evalFunctionRefExp:"<<node->unparseToString()<<" : done1"<<endl;
      return res;
    } else {
      SAWYER_MESG(logger[WARN])<<"EStateTransferFunctions::evalFunctionRefExp: funRefExp==0 (function pointer? - creating top)"<<endl;
      res.init(estate,AbstractValue::createTop());
      SAWYER_MESG(logger[TRACE])<<"evalFunctionRefExp:"<<node->unparseToString()<<" : done2"<<endl;
      return res;
    }
  }

  bool EStateTransferFunctions::isTmpVar(VariableId varId) {
    string varName=getVariableIdMapping()->variableName(varId);
    return CppStdUtilities::isPrefix("__tmp",varName);
  }

  void EStateTransferFunctions::tmpVarCleanUp(EStatePtr estate, VariableId varId) {
    // only solver18 guarantees that all estates can be modified
    if(_analyzer->getSolver()->getId()==18 && _analyzer->getOptionsRef().temporaryLocalVarOptFlag) {
      if(isTmpVar(varId)) {
        estate->pstate()->deleteVar(varId);
        //cout<<"DEBUG: tmpVarCleanUp: removed "<<varId.toString(getVariableIdMapping())<<" from state."<<endl;
      }
    }
  }

  SingleEvalResult EStateTransferFunctions::evalVarRefExp(SgVarRefExp* node, EStatePtr estate, EvalMode mode) {
    switch(mode) {
    case MODE_ADDRESS:
      return evalLValueVarRefExp(node,estate);
    case MODE_VALUE:
      return evalRValueVarRefExp(node,estate,mode);
    case MODE_EMPTY_STATE: {
      SingleEvalResult res;
      res.init(estate,AbstractValue::createTop());
      return res;
    }
    }
    ROSE_ASSERT(false); // unreachable
  }

  SingleEvalResult EStateTransferFunctions::evalRValueVarRefExp(SgVarRefExp* node, EStatePtr estate, EvalMode mode) {
    //SAWYER_MESG(logger[TRACE])<<"evalRValueVarRefExp: "<<node->unparseToString()<<" id:"<<_variableIdMapping->variableId(isSgVarRefExp(node)).toString()<<" MODE:"<<mode<<" Symbol:"<<node->get_symbol()<<endl;
    SingleEvalResult res;
    res.init(estate,AbstractValue::createTop());

    PStatePtr pstate=estate->pstate();
    ROSE_ASSERT(pstate);
    VariableId varId=_variableIdMapping->variableId(node);
    if(!varId.isValid()) {
      SAWYER_MESG(logger[ERROR])<<"Invalid varId: "<<node->unparseToString()<<" Parent: "<<node->get_parent()->unparseToString()<<" Symbol:"<<node->get_symbol()<<endl;
      ROSE_ASSERT(varId.isValid());
    }
    // check if var is a struct member. if yes return struct-offset.
    //SAWYER_MESG(logger[TRACE])<<"VarRefExp name: "<<_variableIdMapping->variableName(varId)<<endl;
    if(isMemberVariable(varId)) {
      int offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
      ROSE_ASSERT(_variableIdMapping);
      SAWYER_MESG(logger[TRACE])<<"evalRValueVarRefExp: evalRValueVarRefExp STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
      res.result=AbstractValue(offset);
      return res;
    }
    // TODO: as rvalue it represents the entire class
    if(_variableIdMapping->isOfClassType(varId)) {
      res.result=AbstractValue::createAddressOfVariable(varId);
      return res;
    }

    if(pstate->varExists(varId)) {
      if(_variableIdMapping->isOfArrayType(varId)) {
        res.result=AbstractValue::createAddressOfArray(varId);
      } else {
        if(_variableIdMapping->isOfReferenceType(varId)) {
          if(mode==MODE_ADDRESS) {
            res.result=readFromMemoryLocation(estate->label(),pstate,varId); // address of a reference is the value it contains
          } else {
            res.result=readFromReferenceMemoryLocation(estate->label(),pstate,varId);
            tmpVarCleanUp(estate,varId);
          }
        } else {
          res.result=readFromMemoryLocation(estate->label(),pstate,varId);
          tmpVarCleanUp(estate,varId);
        }
      }
      return res;
    } else {

      // special mode to represent information not stored in the state
      // i) unmodified arrays: data can be stored outside the state
      // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
      if(_variableIdMapping->isOfArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
        // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
        //res.result=AbstractValue(varId.getIdCode());
        res.result=AbstractValue::createAddressOfArray(varId);
        return res;
      } else {
        res.result=CodeThorn::Top();
        //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
        Label lab=estate->label();
        SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (RValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;

        return res;
      }
    }
    // unreachable
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::evalValueExp(SgValueExp* node, EStatePtr estate, EvalMode mode) {
    ROSE_ASSERT(node);
    SingleEvalResult res;
    res.init(estate,AbstractValue::createBot());
    res.result=abstractValueFromSgValueExp(node,mode);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalFunctionCallArguments(SgFunctionCallExp* funCall, EStatePtr estate) {
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    for (auto arg : argsList) {
      SAWYER_MESG(logger[TRACE])<<"evaluating function call argument: "<<arg->unparseToString()<<endl;
      // Requirement: code is normalized, does not contain state modifying operations in function arguments
      SingleEvalResult resList=evaluateExpression(arg,estate);
    }
    SingleEvalResult res;
    AbstractValue evalResultValue=CodeThorn::Top();
    res.init(estate,evalResultValue);
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalFunctionCall(SgFunctionCallExp* funCall, EStatePtr estate) {
    SingleEvalResult res;
    res.init(estate,AbstractValue::createTop());
    SAWYER_MESG(logger[TRACE])<<"Evaluating function call: "<<funCall->unparseToString()<<endl;
    SAWYER_MESG(logger[TRACE])<<"AST function call: "<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
    if(getStdFunctionSemantics()) {
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="malloc"||funName=="alloca"||funName=="ct_alloca"||funName=="__builtin_alloca") {
        return evalFunctionCallMalloc(funCall,estate);
      } else if(funName=="free") {
        return evalFunctionCallFree(funCall,estate);
      } else if(funName=="strlen") {
        return PredefinedSemanticFunctions::evalFunctionCallStrLen(this,funCall,estate);
      } else if(funName=="memcpy") {
        return PredefinedSemanticFunctions::evalFunctionCallMemCpy(this,funCall,estate);
      } else if(funName=="fflush") {
        // ignoring fflush
        // res initialized above
        return evalFunctionCallArguments(funCall,estate);
      } else if(funName=="time"||funName=="srand"||funName=="rand") {
        // arguments must already be analyzed (normalized code) : TODO check that it is a single variable
        // result is top (time/srand/rand return any value)
        return res; // return top (initialized above (res.init))
      } else if(funName=="__assert_fail") {
        // TODO: create state
        evalFunctionCallArguments(funCall,estate);
        estate->io.recordVerificationError();
        return res;
      } else if(funName=="printf" && (getInterpreterMode()==IM_ENABLED)) {
        // call fprint function in mode CONCRETE and generate output
        // (1) obtain arguments from estate
        // (2) marshall arguments
        // (3) perform function call (causing side effect on stdout (or written to provided file))
        return execFunctionCallPrintf(funCall,estate);
      } else if(funName=="scanf" && (getInterpreterMode()==IM_ENABLED)) {
        // call scanf function in mode CONCRETE and generate output
        // (1) obtain arguments from estate
        // (2) marshall arguments
        // (3) perform function call (causing side effect on stdin)
        return execFunctionCallScanf(funCall,estate);
      }
    }
    if(getSkipUnknownFunctionCalls()) {
      return evalFunctionCallArguments(funCall,estate);
    } else {
      string s=funCall->unparseToString();
      fatalErrorExit(funCall, "unknown semantics of function call inside expression: "+s);
    }
    ROSE_ASSERT(false);
  }

  SingleEvalResult EStateTransferFunctions::execFunctionCallPrintf(SgFunctionCallExp* funCall, EStatePtr estate) {
    SingleEvalResult res;
    res.init(estate,AbstractValue::createTop()); // default value for void function call
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    auto iter=argsList.begin();
    ROSE_ASSERT(iter!=argsList.end());
    SgNode* arg=*iter++;
    while(SgCastExp* castExp=isSgCastExp(arg)) {
      arg=castExp->get_operand_i();
    }
    string formatString="";
    if(SgStringVal* formatStringVal=isSgStringVal(arg)) {
      ROSE_ASSERT(formatStringVal);
      formatString=formatStringVal->get_value();
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(arg)) {
      // reconstruct string from state
      // determine start address of string
      VariableId varId=getVariableIdMapping()->variableId(varRefExp);
      AbstractValue varAddress=AbstractValue::createAddressOfVariable(varId);
      AbstractValue charAddress=readFromAnyMemoryLocation(estate->label(),estate->pstate(),varAddress);
      // build string char by char until 0 or char not in state
      while(estate->pstate()->memLocExists(charAddress)) {
        AbstractValue abstractChar=readFromAnyMemoryLocation(estate->label(),estate->pstate(),charAddress);
        AbstractValue one(1);
        charAddress=AbstractValue::operatorAdd(charAddress,one,one);
        // convert to char and add to string
        if(abstractChar.isConstInt()) {
          if(abstractChar.getIntValue()==0)
            break; // found end of string
          char a=abstractChar.getIntValue();
          formatString=formatString+a;
        }
      }
    }
    //cout<<"string:"<<formatString<<endl;
    if(formatString.size()>0) {
      vector<string> avStringVector;
      for(size_t i=1;i<argsList.size();i++) {
        SgExpression* arg=*iter++;
        SingleEvalResult argRes=evaluateExpression(arg,estate);
        AbstractValue av=argRes.value();
        avStringVector.push_back(av.toString(_variableIdMapping));
      }
      // replace all uses of %? with respective AVString
      string concAVString;
      size_t j=0;
      for(size_t i=0;i<formatString.size();++i) {
        if(formatString[i]=='%') {
          i++; // go to next character after '%'
          if(formatString.size()>2 && formatString[i]=='l') // for handling %ld, %lf
            i+=1;
          if(j>=avStringVector.size()) {
            // number of arguments and uses of '%' don't match in input
            // program. This could be reported as program error.  For now
            // we just do not produce an output (as the original program
            // does not either)
            // TODO: report input program error
            continue; // continue to print other characters
          }
          concAVString+=avStringVector[j++];
        } else if(formatString[i]=='\\') {
          if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
            i+=1; // process additional character '%'
            concAVString+="\%";
          } else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
            concAVString+='\n'; // this generates a proper newline
            i+=1; // process additional character 'n'
          } else {
            concAVString+='\\';
          }
        } else {
          // any other character
          concAVString+=formatString[i];
        }
      }
      string fileName=getInterpreterModeFileName();
      if(fileName!="") {
        bool ok=CppStdUtilities::appendFile(fileName,concAVString);
        if(!ok) {
          cerr<<"Error: could not open output file "<<fileName<<endl;
          exit(1);
        }
      } else {
        cout<<concAVString; // interpreter mode output
      }
    } else {
      // first argument is not a string, must be some constant
      // since it is only printed, it is not modifying the state
      // TODO: it could be recorded as output state
      SAWYER_MESG(logger[WARN])<<"unsupported printf argument: "<<(arg)->class_name()<<" : "<<(arg)->unparseToString()<<endl;
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::execFunctionCallScanf(SgFunctionCallExp* funCall, EStatePtr estate) {
    SingleEvalResult res;
    res.init(estate,AbstractValue::createTop()); // default value for void function call
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    auto iter=argsList.begin();
    ROSE_ASSERT(iter!=argsList.end());
    SgStringVal* formatStringVal=isSgStringVal(*iter++);
    ROSE_ASSERT(formatStringVal);
    string formatString=formatStringVal->get_value();
    vector<string> avStringVector;
    for(size_t i=1;i<argsList.size();i++) {
      SgExpression* arg=*iter++;
      SingleEvalResult argRes=evaluateExpression(arg,estate);
      AbstractValue av=argRes.value();
      avStringVector.push_back(av.toString(_variableIdMapping));
    }
    // replace all uses of %? with respective AVString
    string concAVString;
    size_t j=0;
    for(size_t i=0;i<formatString.size();++i) {
      if(formatString[i]=='%') {
        char controlChar=formatString[i];
        i++; // skip next character
        if(j>=avStringVector.size()) {
          // number of arguments and uses of '%' don't match in input
          // program. This could be reported as program error.  For now
          // we just do not produce an output (as the original program
          // does not either)
          // TODO: report input program error
          continue; // continue to print other characters
        }
        switch(controlChar) {
        case 'd': {
          int val;
          int numParsed=scanf("%d",&val); // read integer from stdin
          AbstractValue av=((numParsed==1)?argsList[j]:AbstractValue::createTop()); // assume any value in case of error
          // write val into state at address argsList[j]
          if(av.isPtr()) {
            PStatePtr pstate=estate->pstate();
            writeToMemoryLocation(estate->label(),pstate,av,val);
            // TODO: pstate is not used yet, because estate is only read but not returned (hence this is a noop and not an update)
            cout<<"Warning: interpreter mode: scanf: memory location "<<av.toString(_variableIdMapping)<<" not updated (not implemented yet)."<<endl;
          } else {
            cerr<<"Warning: interpreter mode: scanf writing to non-address value (ignored)"<<endl;
          }
          break;
        }
        default:
          cerr<<"Warning: interpreter mode: scanf using unknown type "<<endl;
        }
      } else if(formatString[i]=='\\') {
        if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
          i+=1; // process additional character '%'
          concAVString+="\%";
        } else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
          i+=1; // process additional character 'n'
        } else {
          // nothing to do (do not match)
        }
      } else {
        // any other character
      }
    }
    return res;
  }

  AbstractValue EStateTransferFunctions::createNewMemoryRegionIdAndPointerToIt(string name, int memoryRegionSize) {
    // check if memory region is reused
    VariableId memLocVarId=_variableIdMapping->getMemoryRegionIdByName(name);
    if(!memLocVarId.isValid()) {
      // create new memory region id
      memLocVarId=_variableIdMapping->createAndRegisterNewMemoryRegion(name,memoryRegionSize);
      _variableIdMapping->setElementSize(memLocVarId,1); // malloc default for char (size=1)
    }
    AbstractValue allocatedMemoryPtr=AbstractValue::createAddressOfArray(memLocVarId);
    allocatedMemoryPtr.setAbstractFlag(true);
    return allocatedMemoryPtr;
  }

  SingleEvalResult EStateTransferFunctions::evalFunctionCallMalloc(SgFunctionCallExp* funCall, EStatePtr estate) {
    // create two cases: (i) allocation successful, (ii) allocation fails (null pointer is returned, and no memory is allocated).
    SingleEvalResult res;
    static int memorylocid=0; // to be integrated in VariableIdMapping
    stringstream ss;
    // get label of funCall and use label as name to ensure it's reused for same location in program
    if(_analyzer->getOptionsRef().abstractionMode==1) {
      Label fCallLab=_analyzer->getLabeler()->getLabel(funCall); // for __builtin_alloca no label exists. currently NO_LABEL_ID is used for allocas and considered shared
      ss<<"$MEM"<<fCallLab.toString();
    } else {
      memorylocid++;
      ss<<"$MEM"<<memorylocid;
    }
    ROSE_ASSERT(_variableIdMapping);
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      // (i) create state for successful allocation of memory (do not reserve memory yet, only pointer is reserved and size of memory is recorded)
      // memory is allocated when written to it. Otherwise it is assumed to be uninitialized
      SgExpression* arg1=*argsList.begin();
      SingleEvalResult sres=evaluateExpression(arg1,estate);
      AbstractValue arg1val=sres.result;
      int memoryRegionSize;
      if(arg1val.isConstInt()) {
        memoryRegionSize=arg1val.getIntValue();
      } else {
        // unknown size
        memoryRegionSize=0;
      }

      AbstractValue allocatedMemoryPtr;
      if(_analyzer->getOptionsRef().abstractionMode==1) {
        VariableId memRegionId=_variableIdMapping->getMemoryRegionIdByName(ss.str());
        if(!memRegionId.isValid()) {
          allocatedMemoryPtr=createNewMemoryRegionIdAndPointerToIt(ss.str(),memoryRegionSize);
          writeUndefToMemoryLocation(estate->label(), estate->pstate(),allocatedMemoryPtr); // explitly reserve memory in state (array abstraction is considered)'
        } else {
          // use existing memory region id
          allocatedMemoryPtr=AbstractValue::createAddressOfArray(memRegionId);
          allocatedMemoryPtr.setAbstractFlag(true);
        }
      } else {
        // concrete mode, always alloc new memory (can use same name for different regions, since the name is not used in concrete node)
        allocatedMemoryPtr=createNewMemoryRegionIdAndPointerToIt(ss.str(),memoryRegionSize);
        writeUndefToMemoryLocation(estate->label(), estate->pstate(),allocatedMemoryPtr); // explitly reserve memory in state (array abstraction is considered)'
      }
      SAWYER_MESG(logger[TRACE])<<"function call malloc: allocated at: "<<allocatedMemoryPtr.toString()<<endl;
      res.init(estate,allocatedMemoryPtr);
      ROSE_ASSERT(allocatedMemoryPtr.isPtr());
      return res;
    } else {
      cerr<<"WARNING: unknown malloc/alloc function "<<funCall->unparseToString()<<" (ignored)"<<endl;
    }
    return res;
  }

  SingleEvalResult EStateTransferFunctions::evalFunctionCallFree(SgFunctionCallExp* funCall, EStatePtr estate) {
    SingleEvalResult res;
    SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(argsList.size()==1) {
      SgExpression* arg1=*argsList.begin();
      SingleEvalResult sres=evaluateExpression(arg1,estate);
      AbstractValue arg1val=sres.result;
      if(arg1val.isPtr()) {
        // TODO: mark memory as (possibly) deallocated (currently not supported)
      }
      res.init(estate,AbstractValue::createTop()); // void result (using top here)
    } else {
      cerr<<"WARNING: unknown free function "<<funCall->unparseToString()<<endl;
    }
    return res;
  }

  // returns size, or -1 (=any) in case pointer is top.
  int EStateTransferFunctions::getMemoryRegionNumElements(CodeThorn::AbstractValue ptrToRegion) {
    if(ptrToRegion.isTop()||ptrToRegion.isPointerToArbitraryMemory()) {
      return -1;
    }
    ROSE_ASSERT(ptrToRegion.isPtr());
    VariableId ptrVariableId=ptrToRegion.getVariableId();
    int size=_variableIdMapping->getNumberOfElements(ptrVariableId);
    SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
    return size;
  }

  int EStateTransferFunctions::getMemoryRegionElementSize(CodeThorn::AbstractValue ptrToRegion) {
    if(ptrToRegion.isTop()||ptrToRegion.isPointerToArbitraryMemory()) {
      return -1;
    }
    ROSE_ASSERT(ptrToRegion.isPtr());
    VariableId ptrVariableId=ptrToRegion.getVariableId();
    int size=_variableIdMapping->getElementSize(ptrVariableId);
    SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
    return size;
  }


  enum MemoryAccessBounds EStateTransferFunctions::checkMemoryAccessBounds(AbstractValue address) {
    if(address.isTop()||address.isPointerToArbitraryMemory()) {
      return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
    } if(address.isBot()) {
      return ACCESS_NON_EXISTING;
    } if(address.isNullPtr()) {
      return ACCESS_DEFINITELY_NP;
    } else {
      AbstractValue offset=address.getIndexValue();
      if(offset.isTop()) {
        return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
      } else if(offset.isBot()) {
        return ACCESS_NON_EXISTING;
      } else {
        VariableId memId=address.getVariableId();
        // this must be the only remaining case
        if(offset.isConstInt()) {
          // check array bounds
          int memRegionSize=_variableIdMapping->getNumberOfElements(memId);
          if(memRegionSize==0)
            return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS; // will become ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
          int accessIndex=offset.getIntValue();
          if(!(accessIndex<0||accessIndex>=memRegionSize)) {
            return ACCESS_DEFINITELY_INSIDE_BOUNDS;
          } else {
            return ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
          }
        } else {
          return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
        }
      }
    }
  }

  ProgramLocationsReport EStateTransferFunctions::getProgramLocationsReport(enum AnalysisSelector analysisSelector) {
    ProgramLocationsReport report;
#pragma omp critical(VIOLATIONRECORDING)
    ROSE_ASSERT(analysisSelector<_violatingLocations.size());
    report=_violatingLocations.at(analysisSelector);
    return report;
  }

  void EStateTransferFunctions::initViolatingLocations() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      _violatingLocations.push_back(ProgramLocationsReport());
    }
  }
  std::string EStateTransferFunctions::analysisSelectorToString(AnalysisSelector sel) {
    switch(sel) {
    case ANALYSIS_NULL_POINTER: return "null-pointer";
    case ANALYSIS_OUT_OF_BOUNDS: return "out-of-bounds";
    case ANALYSIS_UNINITIALIZED: return "unitialized";
    case ANALYSIS_DEAD_CODE: return "dead-code";
    case ANALYSIS_OPAQUE_PREDICATE: return "opaque-predicate";
    default:
      SAWYER_MESG(logger[FATAL])<<"EStateTransferFunctions::analysisSelectorToString: unknown selector."<<endl;
      exit(1);
    }
  }

  std::string EStateTransferFunctions::sourceLocationAndNodeToString(Label lab) {
    SgNode* node=_analyzer->getLabeler()->getNode(lab);
    return SgNodeHelper::locationAndSourceCodeToString(node);
  }

  // inactive, replaced by version 2
  void EStateTransferFunctions::recordDefinitiveViolatingLocation(enum AnalysisSelector analysisSelector, Label /*label*/) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      ROSE_ASSERT(analysisSelector<_violatingLocations.size());
      //_violatingLocations.at(analysisSelector).recordDefinitiveLocation(label);
    }
  }

  // inactive, replaced by version 2
  void EStateTransferFunctions::recordPotentialViolatingLocation(enum AnalysisSelector analysisSelector, Label /*label*/) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      ROSE_ASSERT(analysisSelector<_violatingLocations.size());
      //      _violatingLocations.at(analysisSelector).recordPotentialLocation(label);
    }
  }

  void EStateTransferFunctions::recordDefinitiveViolatingLocation2(enum AnalysisSelector analysisSelector, Label label) {
    #pragma omp critical(VIOLATIONRECORDING2)
    {
      ROSE_ASSERT(analysisSelector<_violatingLocations.size());
      _violatingLocations.at(analysisSelector).recordDefinitiveLocation(label);
    }
  }

  void EStateTransferFunctions::recordPotentialViolatingLocation2(enum AnalysisSelector analysisSelector, Label label) {
    #pragma omp critical(VIOLATIONRECORDING2)
    {
      ROSE_ASSERT(analysisSelector<_violatingLocations.size());
      _violatingLocations.at(analysisSelector).recordPotentialLocation(label);
    }
  }

  void EStateTransferFunctions::recordDefinitiveNullPointerDereferenceLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_NULL_POINTER,label);
  }

  void EStateTransferFunctions::recordPotentialNullPointerDereferenceLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_NULL_POINTER,label);
  }

  void EStateTransferFunctions::recordDefinitiveOutOfBoundsAccessLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: definitive out of bounds access at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordPotentialOutOfBoundsAccessLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: potential out of bounds access at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordDefinitiveUninitializedAccessLocation(Label label) {
    recordDefinitiveViolatingLocation(ANALYSIS_UNINITIALIZED,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: definitive uninitialized value read at label "<<label.toString()<<endl;
  }

  void EStateTransferFunctions::recordPotentialUninitializedAccessLocation(Label label) {
    recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,label);
    if(_printDetectedViolations)
      cout<<"Violation detected: potential uninitialized value read at label "<<label.toString()<<endl;
  }

  bool EStateTransferFunctions::getPrintDetectedViolations() {
    return _printDetectedViolations;
  }
  void EStateTransferFunctions::setPrintDetectedViolations(bool flag) {
    _printDetectedViolations=flag;
  }

  bool EStateTransferFunctions::isMemberVariable(CodeThorn::VariableId varId) {
    return AbstractValue::getVariableIdMapping()->isMemberVariable(varId);
  }

  bool EStateTransferFunctions::definitiveErrorDetected() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      if(_violatingLocations.at(i).numDefinitiveLocations()>0)
        return true;
    }
    return false;
  }

  bool EStateTransferFunctions::potentialErrorDetected() {
    for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
      if(_violatingLocations.at(i).numPotentialLocations()>0)
        return true;
    }
    return false;
  }

  bool EStateTransferFunctions::isGlobalAddress(AbstractValue val) {
    VariableId varId=val.getVariableId();
    if(varId.isValid()) {
      return getVariableIdMapping()->getVariableIdInfoPtr(varId)->variableScope==VariableIdMapping::VariableScope::VS_GLOBAL;
    }
    return false;
  }

  AbstractValue EStateTransferFunctions::conditionallyApplyArrayAbstraction(AbstractValue val) {
    return AbstractValue::conditionallyApplyArrayAbstraction(val);
  }

  AbstractValue EStateTransferFunctions::readFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc) {
    memLoc=conditionallyApplyArrayAbstraction(memLoc);
    notifyReadWriteListenersOnReading(lab,pstate,memLoc);

    // inspect memory location here
    if(memLoc.isNullPtr()) {
      //return AbstractValue::createBot();
      return AbstractValue::createTop();
    }

    // optimization of temporary single local vars (guaranteed to be used only once, introduced by normalization)
    if(memLoc.isPtr()) {
      VariableId varId=memLoc.getVariableId();
      if(_analyzer->getOptionsRef().temporaryLocalVarOptFlag && isTemporarySingleLocalVar(varId)) {
       pstate->deleteVar(varId); // optimization
      }
    }

    AbstractValue val=pstate->readFromMemoryLocation(memLoc); // relegating to pstate in estate->readFromMemoryLocation
    return val;
  }

  void EStateTransferFunctions::writeToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    memLoc=conditionallyApplyArrayAbstraction(memLoc);
    notifyReadWriteListenersOnWriting(lab,pstate,memLoc,newValue);

    if(memLoc.isTop()||memLoc.isBot()/*||memLoc.isPointerToArbitraryMemory()*/) {
      pstate->writeTopToAllPotentialMemoryLocations();
      return;
    } else if(!pstate->memLocExists(memLoc)) {
      //SAWYER_MESG(logger[TRACE])<<"EStateTransferFunctions::writeToMemoryLocation: memloc does not exist"<<endl;
      pstate->writeTopToAllPotentialMemoryLocations();
      return;
    }
    if(memLoc.isNullPtr()) {
      // do not write to null pointer
    } else {
      if(memLoc.isPtr()||memLoc.isFunctionPtr()) {
        pstate->writeToMemoryLocation(memLoc,newValue);
      } else {
        // memory location is a constant value
        if(!_analyzer->isRegisterAddress(memLoc)) {
          cerr<<"WARNING: attemping to write to integer value memory location: "<<memLoc.toString(getVariableIdMapping())<<endl;
        }
      }
    }
  }

  void EStateTransferFunctions::notifyReadWriteListenersOnReading(Label lab, PStatePtr pstate, AbstractValue& memLoc) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      if(numberOfReadWriteListeners()>0) {
        for(auto p : _readWriteListenerMap) {
          ReadWriteListener* readWriteListener=p.second;
          readWriteListener->readingFromMemoryLocation(lab,pstate,memLoc);
        }
      }
    }
  }

  void EStateTransferFunctions::notifyReadWriteListenersOnWriting(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) {
#pragma omp critical(VIOLATIONRECORDING)
    {
      if(numberOfReadWriteListeners()>0) {
        for(auto p : _readWriteListenerMap) {
          ReadWriteListener* readWriteListener=p.second;
          readWriteListener->writingToMemoryLocation(lab,pstate,memLoc,newValue);
        }
      }
    }
  }

  AbstractValue EStateTransferFunctions::readFromReferenceMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc) {
    AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
    return readFromMemoryLocation(lab,pstate,referedMemLoc);
  }

  void EStateTransferFunctions::writeToReferenceMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
    writeToMemoryLocation(lab,pstate,referedMemLoc,newValue);
  }

  AbstractValue EStateTransferFunctions::readFromAnyMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc) {
    SAWYER_MESG(logger[TRACE])<<"readFromAnyMemoryLocation: "<<memLoc.toString()<<"=>"<<memLoc.isReferenceVariableAddress()<<endl;
    if(memLoc.isReferenceVariableAddress())
      return readFromReferenceMemoryLocation(lab,pstate,memLoc);
    else
      return readFromMemoryLocation(lab,pstate,memLoc);
  }

  void EStateTransferFunctions::writeToAnyMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    if(memLoc.isReferenceVariableAddress())
      writeToReferenceMemoryLocation(lab,pstate,memLoc,newValue);
    else
      writeToMemoryLocation(lab,pstate,memLoc,newValue);
  }

  void EStateTransferFunctions::initializeMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
    if(memLoc.isBot()) {
      return;
    }
    // in case of intra-procedural analysis all global addresses are
    // modeled as summaries (to enusure only weak updates happen and
    // no definitive errors are reported as any function may modify
    // those shared variables
    if(_analyzer->getOptionsRef().getIntraProceduralFlag()) {
      if(isGlobalAddress(memLoc)) {
        memLoc.setAbstractFlag(true);
      }
    }
    SAWYER_MESG(logger[TRACE])<<"initializeMemoryLocation: "<<memLoc.toString()<<" := "<<newValue.toString()<<endl;
    memLoc=conditionallyApplyArrayAbstraction(memLoc);
    reserveMemoryLocation(lab,pstate,memLoc);
    writeToMemoryLocation(lab,pstate,memLoc,newValue);
    SAWYER_MESG(logger[TRACE])<<"initializeMemoryLocation: done: "<<memLoc.toString()<<endl;
  }

  void EStateTransferFunctions::reserveMemoryLocation(Label lab, PStatePtr pstate, AbstractValue memLoc) {
    if(memLoc.isBot()) {
      return;
    }
    writeUndefToMemoryLocation(lab,pstate,memLoc);
  }

  void EStateTransferFunctions::writeUndefToMemoryLocation(Label /*lab*/, PStatePtr pstate, AbstractValue memLoc) {
    if(memLoc.isBot()) {
      return;
    }

    if(_analyzer->getOptionsRef().getIntraProceduralFlag()) {
      if(isGlobalAddress(memLoc)) {
        memLoc.setAbstractFlag(true);
      }
    }

    memLoc=conditionallyApplyArrayAbstraction(memLoc);
    pstate->writeUndefToMemoryLocation(memLoc);
  }

  void EStateTransferFunctions::writeUndefToMemoryLocation(PState* pstate, AbstractValue memLoc) {
    if(memLoc.isBot()) {
      return;
    }
    memLoc=conditionallyApplyArrayAbstraction(memLoc);
    pstate->writeUndefToMemoryLocation(memLoc);
  }

  void EStateTransferFunctions::printLoggerWarning(EStatePtr estate) {
    Label lab=estate->label();
    if(_analyzer) {
      SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": this pointer set to top."<<endl;
    } else {
      SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<": this pointer set to top."<<endl;
    }
  }

  size_t EStateTransferFunctions::numberOfReadWriteListeners() {
    return _readWriteListenerMap.size();
  }

  void EStateTransferFunctions::registerReadWriteListener(ReadWriteListener* rwl, std::string name) {
    ROSE_ASSERT(_readWriteListenerMap.find(name)==_readWriteListenerMap.end());
    _readWriteListenerMap[name]=rwl;
  }

  ReadWriteListener* EStateTransferFunctions::getReadWriteListener(std::string name) {
    auto iter=_readWriteListenerMap.find(name);
    if(iter==_readWriteListenerMap.end()) {
      return nullptr;
    } else {
      return _readWriteListenerMap.at(name);
    }
  }

  bool EStateTransferFunctions::isFunctionCallWithAssignment(Label lab,VariableId* varIdPtr){
    //return _labeler->isFunctionCallWithAssignment(lab,varIdPtr);
    SgNode* node=getLabeler()->getNode(lab);
    if(getLabeler()->isFunctionCallLabel(lab)) {
      std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
      if(p.first) {
        if(varIdPtr) {
          *varIdPtr=getVariableIdMapping()->variableId(p.first);
        }
        return true;
      }
    }
    return false;
  }

  // wrapper function for reusing exprAnalyzer's function. This function
  // is only relevant for external functions (when the implementation
  // does not exist in input program and the semantics are available in
  // the analyzer (e.g. malloc, strlen, etc.))
  list<EStatePtr> EStateTransferFunctions::evaluateFunctionCallArguments(Edge edge, SgFunctionCallExp* funCall, EStatePtr currentEState, bool /*useConstraints*/) {
    CallString cs=currentEState->getCallString();
    SAWYER_MESG(logger[TRACE]) <<"evaluating arguments of function call:"<<funCall->unparseToString()<<endl;
    SingleEvalResult evalResult=evalFunctionCallArguments(funCall, currentEState);
    PStatePtr newPState=evalResult.estate->pstate();
    return elistify(reInitEState(currentEState,edge.target(),cs,newPState));
  }

  void EStateTransferFunctions::initializeStringLiteralInState(Label lab, PStatePtr initialPState,SgStringVal* stringValNode, VariableId stringVarId) {
    SAWYER_MESG(logger[TRACE])<<"initializeStringLiteralInState: "<<stringValNode->unparseToString()<<endl;
    string theString=stringValNode->get_value();
    size_t pos;
    for(pos=0;pos<theString.size();pos++) {
      AbstractValue character(theString[pos]);
      AbstractValue address=AbstractValue::createAddressOfArrayElement(stringVarId,pos);
      reserveMemoryLocation(lab, initialPState, address);
      writeToMemoryLocation(lab, initialPState, address,character);
    }
    // add terminating 0 to string in state
    reserveMemoryLocation(lab, initialPState, AbstractValue::createAddressOfArrayElement(stringVarId,pos));
    writeToMemoryLocation(lab, initialPState, AbstractValue::createAddressOfArrayElement(stringVarId,pos),AbstractValue(0));
  }

  void EStateTransferFunctions::initializeStringLiteralsInState(Label lab, PStatePtr initialPState) {
    ROSE_ASSERT(getVariableIdMapping());
    std::map<SgStringVal*,VariableId>* map=getVariableIdMapping()->getStringLiteralsToVariableIdMapping();
    SAWYER_MESG(logger[INFO])<<"Creating "<<map->size()<<" string literals in state."<<endl;
    for(auto iter=map->begin();iter!=map->end();++iter) {
      auto dataPair=*iter;
      SgStringVal* stringValNode=dataPair.first;
      VariableId stringVarId=dataPair.second;
      initializeStringLiteralInState(lab,initialPState,stringValNode,stringVarId);
    }
  }

  void EStateTransferFunctions::initializeCommandLineArgumentsInState(Label lab, PStatePtr initialPState) {
    // SgFunctionDefinition* startFunRoot: node of function
    SgNode* startFunRoot=_analyzer->getStartFunRoot();
    if(startFunRoot==0) {
      // no main function, therefore nothing to initialize
      return;
    }
    string functionName=SgNodeHelper::getFunctionName(startFunRoot);
    SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(startFunRoot);
    VariableId argcVarId;
    VariableId argvVarId;
    size_t mainFunArgNr=0;
    for(SgInitializedNamePtrList::iterator i=initNamePtrList.begin();i!=initNamePtrList.end();++i) {
      VariableId varId=getVariableIdMapping()->variableId(*i);
      if(functionName=="main" && initNamePtrList.size()==2) {
        switch(mainFunArgNr) {
        case 0:
          argcVarId=varId;
          SAWYER_MESG(logger[TRACE])<<"INIT CLARGS: found argc in main function."<<endl;
          break;
        case 1:
          argvVarId=varId;
          SAWYER_MESG(logger[TRACE])<<"INIT CLARGS: found argv in main function."<<endl;
          break;
        default:
          throw CodeThorn::Exception("Error: main function has more than 2 parameters.");
        }
        mainFunArgNr++;
      }
      ROSE_ASSERT(varId.isValid());
    }
    // if function main exists and has 2 arguments then argcvarid and argvvarid have valid ids now.
    // otherwise the command line options are ignored (because this is the correct behaviour)
    CodeThorn::TypeSize ptrSize=getVariableIdMapping()->getBuiltInTypeSize(CodeThorn::BuiltInType::BITYPE_POINTER);
    if(argcVarId.isValid() && argvVarId.isValid()) {
      auto commandLineOptions=_analyzer->getCommandLineOptions();
      // argv is an array of pointers to memory regions
      if(commandLineOptions.size()>0) {
        // create command line option array argv and argc in initial pstate if argv and argc exist in the program
        int argc=0;
        VariableId argvArrayMemoryId=getVariableIdMapping()->createAndRegisterNewMemoryRegion("$argvmem",(int)commandLineOptions.size());
        getVariableIdMapping()->setElementSize(argvArrayMemoryId,ptrSize);
        AbstractValue argvArrayAddress=AbstractValue::createAddressOfArray(argvArrayMemoryId);
        AbstractValue argvElementAddress=argvArrayAddress; // element [0]
        initializeMemoryLocation(lab,initialPState,AbstractValue::createAddressOfVariable(argvVarId),argvArrayAddress); // argv
        for (auto argvElem:commandLineOptions) {
          SAWYER_MESG(logger[TRACE])<<"INIT: Initial state: "
                                    <<getVariableIdMapping()->variableName(argvVarId)<<"["<<argc+1<<"]: "
                                    <<argvElem<<endl;
          int stringLen=(int)string(argvElem).size();
          SAWYER_MESG(logger[TRACE])<<"argv["<<argc+1<<"] size (num elements): "<<stringLen<<endl;
          stringstream memRegionName;
          memRegionName<<"$argv"<<argc<<"mem";
          VariableId stringMemoryId_i=getVariableIdMapping()->createAndRegisterNewMemoryRegion(memRegionName.str(),stringLen+1);
          getVariableIdMapping()->setElementSize(stringMemoryId_i,1);
          AbstractValue stringAddress=AbstractValue::createAddressOfArray(stringMemoryId_i);
          // argv[i]=&string_i[0]
          initializeMemoryLocation(lab,initialPState,argvElementAddress,stringAddress); // argv[i]=&string_i[0]
          // copy concrete command line argument strings char by char to State
          int j; // also used after loop
          AbstractValue stringCharAddress=stringAddress;
          AbstractValue plusOne(1);
          for(j=0;commandLineOptions[argc][j]!=0;j++) {
            SAWYER_MESG(logger[TRACE])<<"INIT: Copying: @argc="<<argc<<" char: "<<commandLineOptions[argc][j]<<endl;
            initializeMemoryLocation(lab,initialPState,stringCharAddress,AbstractValue(commandLineOptions[argc][j]));
            stringCharAddress=AbstractValue::operatorAdd(stringCharAddress,plusOne,AbstractValue(1));
          }
          // write terminating 0
          initializeMemoryLocation(lab,initialPState,stringCharAddress,AbstractValue(0));
          argvElementAddress=AbstractValue::operatorAdd(argvElementAddress,plusOne,AbstractValue(ptrSize)); // argv++;
          argc++;
        }
        // this also covers the case that no command line options were provided. In this case argc==0. argv is non initialized.
        SAWYER_MESG(logger[TRACE])<<"INIT: Initial state argc:"<<argc<<endl;
        AbstractValue abstractValueArgc(argc);
        initializeMemoryLocation(lab,initialPState,argcVarId,abstractValueArgc);
      } else {
        // argc and argv present in program but no command line arguments provided
        SAWYER_MESG(logger[TRACE])<<"INIT: no command line arguments provided. Initializing argc=0."<<endl;
        AbstractValue abstractValueArgc(0);
        initializeMemoryLocation(lab,initialPState,argcVarId,abstractValueArgc);
      }
    } else {
      // argv and argc not present in program. argv and argc are not added to initialPState.
      // in this case it is irrelevant whether command line arguments were provided (correct behaviour)
      // nothing to do.
    }
  }

  // only used in analyzeDeclaration
  int EStateTransferFunctions::computeNumberOfElements(SgVariableDeclaration* decl) {
    SgNode* initName0=decl->get_traversalSuccessorByIndex(1);
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      SgInitializer* initializer=initName->get_initializer();
      if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
        SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type());
        ROSE_ASSERT(arrayType);
        //SgType* arrayElementType=arrayType->get_base_type();
        SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
        SgExpressionPtrList& initList=initListObjPtr->get_expressions();
        // TODO: nested initializers, currently only outermost elements: {{1,2,3},{1,2,3}} evaluates to 2.
        SAWYER_MESG(logger[TRACE])<<"computeNumberOfElements returns "<<initList.size()<<": case : SgAggregateInitializer"<<aggregateInitializer->unparseToString()<<endl;
        return initList.size();
      } else if(isSgAssignInitializer(initializer)) {
        SAWYER_MESG(logger[TRACE])<<"computeNumberOfElements returns 0: case SgAssignInitializer: "<<initializer->unparseToString()<<endl;
        return 0; // MS 3/5/2019: changed from 1 to 0
      }
    }
    return 0;
  }

} // end of namespace
